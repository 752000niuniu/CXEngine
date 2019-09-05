#include "audio_manager.h"

#define  DR_FLAC_IMPLEMENTATION
#include "audio/extras/dr_flac.h"  /* enables flac decoding. */
#define DR_MP3_IMPLEMENTATION 
#include "audio/extras/dr_mp3.h"   /* enables mp3 decoding. */
#define DR_WAV_IMPLEMENTATION 
#include "audio/extras/dr_wav.h"   /* enables wav decoding. */

#define MINIAUDIO_IMPLEMENTATION
#include "audio/miniaudio.h"

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
	if (pDecoder == NULL) {
		return;
	}

	ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
	(void)pInput;
}

int audio_manager_play(const char* path)
{
	ma_result result;
	ma_decoder decoder;
	ma_device_config deviceConfig;
	ma_device device;

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

	printf("Press Enter to quit...");
	getchar();

	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
	return 0;
};
