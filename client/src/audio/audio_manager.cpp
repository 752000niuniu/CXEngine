#include "audio_manager.h"
#include "lua_bind.h"

#include <extras/stb_vorbis.c>
#define  DR_FLAC_IMPLEMENTATION
#include <extras/dr_flac.h>  /* enables flac decoding. */
#define DR_MP3_IMPLEMENTATION 
#include <extras/dr_mp3.h>   /* enables mp3 decoding. */
#define DR_WAV_IMPLEMENTATION 
#define DR_WAVE_FORMAT_GSM_IMPLEMENTATION
#include <extras/dr_wav.h>   /* enables wav decoding. */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

struct AudioFile {
	CXString path;
	bool loop;
	ma_device device;
	ma_decoder decoder;
	bool erase;
	bool start;
};

std::deque<AudioFile*> g_AudioFiles;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	AudioFile* audioFile = (AudioFile*)pDevice->pUserData;
	if (audioFile == NULL) {
		return;
	}

	if (!audioFile->erase) {
		ma_decoder* pDecoder = &audioFile->decoder;
		ma_uint64 read = ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
		if (read < frameCount) {
			if (audioFile->loop) {
				ma_decoder_seek_to_pcm_frame(pDecoder, 0);
			}
			else {
				pDevice->pUserData = nullptr;
				audioFile->erase = true;
			}
		}
	}

	(void)pInput;
}

void audio_manager_stop(const char* path) {
	for (auto it = g_AudioFiles.begin(); it != g_AudioFiles.end(); it++) {
		if ((*it)->path == path) {
			(*it)->erase = true;
		}
	}
}

void audio_manager_clear() {
	for (auto& it : g_AudioFiles) {
		it->erase = true;
	}
}

int audio_manager_play(const char* path, bool loop)
{
	if (g_AudioFiles.size() > 0) {
		for (auto it = g_AudioFiles.begin(); it != g_AudioFiles.end();) {
			if ((*it)->erase) {
				ma_device_uninit(&(*it)->device);
				ma_decoder_uninit(&(*it)->decoder);
				delete (*it);
				it = g_AudioFiles.erase(it);
			}
			else {
				it++;
			}
		}
	}

	AudioFile* file = new AudioFile();
	file->erase = false;
	file->loop = loop;
	file->start = false;
	file->path = path;
	g_AudioFiles.push_back(file);

	ma_device& device = file->device;
	ma_decoder& decoder = file->decoder;

	ma_result result;
	result = ma_decoder_init_file(path, NULL, &decoder);
	if (result != MA_SUCCESS) {
		return -2;
	}
	ma_device_config deviceConfig;
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = decoder.outputFormat;
	deviceConfig.playback.channels = decoder.outputChannels;
	deviceConfig.sampleRate = decoder.outputSampleRate;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = file;

	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
		printf("Failed to open playback device.\n");
		ma_decoder_uninit(&decoder);
		return -3;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		printf("Failed to start playback device.\n");
		ma_device_uninit(&device);
		ma_decoder_uninit(&decoder);
		return -4;
	}

	return 0;
};

#define L
#undef L
int lua_audio_manager_play(lua_State* L){
	const char* path = lua_tostring(L, 1);
	bool loop = lua_toboolean(L, 2);
	audio_manager_play(path, loop);
	return 0;
}

void luaopen_audio_manager(lua_State* L)
{
	script_system_register_luac_function_with_name(L,"audio_manager_play", lua_audio_manager_play);
	script_system_register_function(L, audio_manager_stop);
	script_system_register_function(L, audio_manager_clear);
}
