/**
 ******************************************************************************
 * @file RingBuffer.c
 * @brief Ring buffer implementation file
 *        Manage a ring buffer for communications
 *
 * @creation 2024/04/06
 * @edition 2024/04/07
 *
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#include "RingBuffer.h"

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void RingBuffer_Init(RingBuffer* buf, uint8_t* raw_buf, uint32_t max_size) {
	buf->size 			= 0;
	buf->max_size 	= max_size;
	buf->ptr_write 	= 0;
	buf->ptr_read 	= 0;
	buf->buf 				= raw_buf;
	
	for (uint32_t i = 0; i < buf->max_size; i++) {
		buf->buf[i] = 0;
	}
}

uint32_t RingBuffer_GetSize(const RingBuffer* buf) {
	return buf->size;
}

uint32_t RingBuffer_GetRemainingSize(const RingBuffer* buf) {
	return buf->max_size - buf->size;
}

uint8_t RingBuffer_IsEmpty(const RingBuffer* buf) {
	return !buf->size;
}

uint8_t RingBuffer_IsNotEmpty(const RingBuffer* buf) {
	return !!buf->size;
}

RBRESULT RingBuffer_Put(RingBuffer* buf, const uint8_t byte) {
	if (buf->size >= buf->max_size) 
		return RB_BUFFER_FULL;
	
	buf->buf[buf->ptr_write] = byte;
	buf->ptr_write++;
	buf->ptr_write %= buf->max_size;
	buf->size++;
	
	return RB_OK;
}

RBRESULT RingBuffer_PutSeveral(RingBuffer* buf, const uint8_t* data, const uint32_t length) {
	if (length > RingBuffer_GetRemainingSize(buf)) 
		return RB_NOT_ENOUGH_SPACE;
	
	for (uint32_t cpt = 0; cpt < length; cpt++) {
		buf->buf[buf->ptr_write++] = data[cpt];
		if (buf->ptr_write >= buf->max_size)
			buf->ptr_write = 0;
	}
	buf->size += length;
	
	return RB_OK;
}

RBRESULT RingBuffer_Get(RingBuffer* buf, uint8_t* byte) {
	if (buf->size == 0) 
		return RB_BUFFER_EMPTY;
	
	*byte = buf->buf[buf->ptr_read];
	buf->ptr_read++;
	buf->ptr_read %= buf->max_size;
	buf->size--;
	
	return RB_OK;
}

RBRESULT RingBuffer_GetSeveral(RingBuffer* buf, uint8_t* data, const uint32_t length) {
	if (length > RingBuffer_GetSize(buf)) 
		return RB_NOT_ENOUGH_DATA;
	
	for (uint32_t cpt = 0; cpt < length; cpt++) {
		data[cpt] = buf->buf[buf->ptr_read++];
		if (buf->ptr_read >= buf->max_size)
			buf->ptr_read = 0;
	}
	buf->size -= length;
	
	return RB_OK;
}

RBRESULT RingBuffer_GetAll(RingBuffer* buf, uint8_t* data) {
	return RingBuffer_GetSeveral(buf, data, buf->size);
}

RBRESULT RingBuffer_IgnoreSeveral(RingBuffer* buf, const uint32_t length) {
	if (length > RingBuffer_GetSize(buf)) 
		return RB_NOT_ENOUGH_DATA;

	buf->ptr_read += length;
	buf->ptr_read %= buf->max_size;
	buf->size -= length;
	
	return RB_OK;
}


