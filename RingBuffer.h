/**
 ******************************************************************************
 * @file RingBuffer.h
 * @brief Ring buffer implementation file
 *        Manage a ring buffer for communications
 *
 * @creation 2024/04/06
 * @edition 2024/04/10
 *
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
 
/* Define to prevent recursive inclusion ---------------------------------*/
#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>

// ------------------------------------------------------------------------
// ----------------------------- PUBLIC TYPES -----------------------------
// ------------------------------------------------------------------------
typedef struct {
	uint32_t size;
	uint32_t max_size;
	uint32_t ptr_read;
	uint32_t ptr_write;
	uint8_t* buf;
} RingBuffer;

typedef enum {
	RB_OK = 0,
	RB_BUFFER_FULL,
	RB_NOT_ENOUGH_SPACE,
	RB_NOT_ENOUGH_DATA,
	RB_BUFFER_EMPTY,
} RBRESULT;

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void 		 RingBuffer_Init(RingBuffer* buf, uint8_t* raw_buf, uint32_t max_size);
uint32_t RingBuffer_GetSize(const RingBuffer* buf);
uint32_t RingBuffer_GetRemainingSize(const RingBuffer* buf);
uint8_t  RingBuffer_IsEmpty(const RingBuffer* buf);
uint8_t  RingBuffer_IsNotEmpty(const RingBuffer* buf);
RBRESULT RingBuffer_Put(RingBuffer* buf, const uint8_t byte);
RBRESULT RingBuffer_PutSeveral(RingBuffer* buf, const uint8_t* data, const uint32_t length);
RBRESULT RingBuffer_Get(RingBuffer* buf, uint8_t* byte);
RBRESULT RingBuffer_GetSeveral(RingBuffer* buf, uint8_t* data, const uint32_t length);
RBRESULT RingBuffer_GetAll(RingBuffer* buf, uint8_t* data);
RBRESULT RingBuffer_IgnoreSeveral(RingBuffer* buf, const uint32_t length);

#endif /* __RING_BUFFER_H__ */
