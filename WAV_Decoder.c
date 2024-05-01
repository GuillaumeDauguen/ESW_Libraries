/**
 ******************************************************************************
 * @file WAW_Decoder.c
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
 *
 * @todo see why there is few dac buffer loop at the and of file reading
 * @todo move end of music condition/actions
 * @todo manage SDIO_Interface errors
 ******************************************************************************
 */
#include "WAV_Decoder.h"
#include "RingBuffer.h"
#include "SDIO_Interface.h"
#include "Shell.h"

// ------------------------------------------------------------------------
// -------------------------------- MACROS --------------------------------
// ------------------------------------------------------------------------
#define WAV_BUFFER_SIZE (4095)

// ------------------------------------------------------------------------
// ----------------------------- STATIC TYPES -----------------------------
// ------------------------------------------------------------------------
typedef enum {
	BI_UNKNOWN,
	BI_RIFF,
	BI_FMT_,
	BI_DATA,
} BLOCK_ID;

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------
static WAV_parameters hwav;

static RingBuffer wav_buffer;
static uint8_t _buffer[WAV_BUFFER_SIZE];
static uint8_t read_file_buffer[WAV_BUFFER_SIZE];

// ------------------------------------------------------------------------
// ---------------------- STATIC FUCTIONS PROTOTYPES ----------------------
// ------------------------------------------------------------------------
static void 	_ReadHeader();
static void 	_ReadBlock(BLOCK_ID block_id);
static BLOCK_ID _ReadBlockID();
static uint32_t _ReadBlockData(const uint8_t nb_bytes);
static uint8_t 	_StrCmp(uint8_t* data, char* block_id);

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void WavDecoder_Init() {
	RingBuffer_Init(&wav_buffer, _buffer, WAV_BUFFER_SIZE);
}

void WavDecoder_OpenFile(char *name) {
	SDIO_Interface_OpenFile(name);
	_ReadHeader();
}

WAV_parameters* WavDecoder_GetMusicData() {
	return &hwav;
}

void WavDecoder_FeedDacBuffer() {
	if (hwav.remaining_data) {
	
		uint32_t buffer_remaining_size = RingBuffer_GetRemainingSize(&wav_buffer);
		uint32_t buffer_size           = RingBuffer_GetSize(&wav_buffer);
		
		uint32_t bytes_to_read = (hwav.remaining_data > buffer_remaining_size) ? buffer_remaining_size : hwav.remaining_data;
		
		if(buffer_remaining_size > buffer_size) {
			SDIO_Interface_ReadFile(read_file_buffer, bytes_to_read);
			RingBuffer_PutSeveral(&wav_buffer, read_file_buffer, bytes_to_read);
			hwav.remaining_data -= bytes_to_read;
		}
	}
}

uint16_t WavDecoder_GetDacValue() { // return ok/error, param in: *dac_value
	if (RingBuffer_IsEmpty(&wav_buffer)) {
		if (hwav.remaining_data == 0) {
			//playing_wav_flag = 0;
			//__HAL_TIM_SET_AUTORELOAD(WAV_HTIM, (TIM_FREQ / 42000) - 1); // 42000 denined in music.c for hardcoded music
			SDIO_Interface_CloseFile();
		}
		return 0;
	}
	
	uint8_t data[hwav.byte_per_block];
	uint32_t value = 0;
	
	RingBuffer_GetSeveral(&wav_buffer, data, hwav.byte_per_block);
	
	switch (hwav.byte_per_block) {
		case 1:
			value = data[0] << 4;
			break;

		case 2:
			value = (data[0] + data[1]) << 4;
			break;

		case 4:
			value = (((data[1] + data[3]) << 7) | ((data[0] + data[2]) >> 1)) >> 4;
			value &= 0b0000111111111111;
	}
	
	return value >> 5;
}

// ------------------------------------------------------------------------
// -------------------- STATIC FUCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void _ReadHeader() {
	SDIO_Interface_ReadFile(read_file_buffer, WAV_BUFFER_SIZE);
	RingBuffer_PutSeveral(&wav_buffer, read_file_buffer, WAV_BUFFER_SIZE);
	
	BLOCK_ID block_id = -1;
	while (block_id != BI_DATA) {		// infinite loop... bof
		block_id = _ReadBlockID();
		_ReadBlock(block_id);
	}
}

void _ReadBlock(BLOCK_ID block_id) {
	switch (block_id) {
		case BI_UNKNOWN: {		// brackets to declare variable in switch case
			uint32_t bytes_to_skip = _ReadBlockData(4);						// get the number of bytes in the block
			RingBuffer_IgnoreSeveral(&wav_buffer, bytes_to_skip);	// ignore entire block
			break;
		}
		
		case BI_RIFF:
			hwav.file_size = _ReadBlockData(4);
			RingBuffer_IgnoreSeveral(&wav_buffer, 4);		// ingnore file format (WAVE)
			break;
		
		case BI_FMT_:
			RingBuffer_IgnoreSeveral(&wav_buffer, 4);		// ignore block size (16)
			hwav.audio_format 		= _ReadBlockData(2);
			hwav.nb_channels 		= _ReadBlockData(2);
			hwav.sample_rate 		= _ReadBlockData(4);
			hwav.byte_per_sec 		= _ReadBlockData(4);
			hwav.byte_per_block 	= _ReadBlockData(2);
			hwav.bits_per_sample	= _ReadBlockData(2);
			break;
		
		case BI_DATA:
			hwav.remaining_data = _ReadBlockData(4);
			break;
	}
}

BLOCK_ID _ReadBlockID() {
	uint8_t data[4];
	RingBuffer_GetSeveral(&wav_buffer, data, 4);
	
	if (_StrCmp(data, "RIFF")) {
		return BI_RIFF;
	}
	if (_StrCmp(data, "fmt ")) {
		return BI_FMT_;
	}
	if (_StrCmp(data, "data")) {
		return BI_DATA;
	}
	return BI_UNKNOWN;
}

uint32_t _ReadBlockData(const uint8_t nb_bytes) {
	uint8_t data[nb_bytes];
	RingBuffer_GetSeveral(&wav_buffer, data, nb_bytes);
	
	uint64_t value = 0;
	
	for(uint8_t cpt = 0; cpt < nb_bytes; cpt++) {
		value += data[cpt] << (8 * cpt);
	}
	return value;
}

uint8_t _StrCmp(uint8_t* data, char* block_id) {
	for (uint8_t cpt = 0; cpt < 4; cpt++) {
		if (data[cpt] != block_id[cpt]) {
			return 0;
		}
	}
	return 1;
}


