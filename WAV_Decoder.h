/**
 ******************************************************************************
 * @file WAW_Decoder.h
 * @brief WAV decoder implementation file
 *        Decode WAV file and store in a ring buffer
 *
 * @creation 2024/04/06
 * @edition 2024/04/12
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 * @caution
 * only command one DAC (one output), see WavDecoder_GetDacValue()
 ******************************************************************************
 */
#ifndef __WAV_DECODER_H__
#define __WAV_DECODER_H__

#include <stdint.h>

// ------------------------------------------------------------------------
// ----------------------------- PUBLIC TYPES -----------------------------
// ------------------------------------------------------------------------
typedef struct {
	uint32_t file_size;
	
	uint32_t block_size;
	uint16_t audio_format;
	uint16_t nb_channels;
	uint32_t sample_rate;
	uint32_t byte_per_sec;
	uint16_t byte_per_block;
	uint16_t bits_per_sample;
	
	uint32_t data_size;
	uint32_t remaining_data;
	
	char* title;
} WAV_parameters;

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void     WavDecoder_Init();
void     WavDecoder_OpenFile(char *name);
WAV_parameters* WavDecoder_GetMusicData();
uint16_t WavDecoder_GetDacValue();
void     WavDecoder_FeedDacBuffer();

#endif /* __WAV_DECODER_H__ */
