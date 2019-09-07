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

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
	if (pDecoder == NULL) {
		return;
	}

	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
	(void)pInput;
}

ma_device device;
ma_decoder decoder;


void audio_manager_stop()
{
	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
}

int audio_manager_play(const char* path)
{
	audio_manager_stop();

	ma_result result;
	ma_device_config deviceConfig;

	result = ma_decoder_init_file(path, NULL, &decoder);
	if (result != MA_SUCCESS) {
		return -2;
	}

	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = decoder.outputFormat;
	deviceConfig.playback.channels = decoder.outputChannels;
	deviceConfig.sampleRate = decoder.outputSampleRate;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = &decoder;

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



void luaopen_audio_manager(lua_State* L_machine)
{
	script_system_register_function(L_machine, audio_manager_play);
	script_system_register_function(L_machine, audio_manager_stop);
}
