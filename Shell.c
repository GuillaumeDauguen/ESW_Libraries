/**
 ******************************************************************************
 * @file Shell.c
 * @brief Shell implementation file
 *        Bridge between user and UART interface
 *
 * @creation 2024/04/10
 * @edition 2024/04/11
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#include "Shell.h"
#include "RingBuffer.h"
#include "UART_Interface.h"

#include <string.h>

// ------------------------------------------------------------------------
// -------------------------------- MACROS --------------------------------
// ------------------------------------------------------------------------
#define _RX_BUFFER_LENGTH (512)
#define _TX_BUFFER_LENGTH (1024)

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------
static uint8_t s_uart_Rx_raw_buf[_RX_BUFFER_LENGTH];
static uint8_t s_uart_Tx_raw_buf[_TX_BUFFER_LENGTH];
static RingBuffer s_uart_Rx_buf, s_uart_Tx_buf;

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void Shell_Init(UART_HandleTypeDef* huart) {
	RingBuffer_Init(&s_uart_Rx_buf, s_uart_Rx_raw_buf, sizeof(s_uart_Rx_raw_buf));
	RingBuffer_Init(&s_uart_Tx_buf, s_uart_Tx_raw_buf, sizeof(s_uart_Tx_raw_buf));
	UART_Interface_Init(huart, &s_uart_Rx_buf, &s_uart_Tx_buf);
}

bool Shell_IsNotEmpty() {
	return RingBuffer_IsNotEmpty(&s_uart_Rx_buf);
}

char Shell_ReadLetter() {
	uint8_t letter;
	
	RingBuffer_Get(&s_uart_Rx_buf, &letter);
	return letter;
}

void Shell_PrintLetter(uint8_t letter) {
	RingBuffer_Put(&s_uart_Tx_buf, letter);
	UART_Interface_EnableIT();
}

void Shell_PrintString(const char* string) {
	RingBuffer_PutSeveral(&s_uart_Tx_buf, (uint8_t*) string, strlen(string));
	UART_Interface_EnableIT();
}

void Shell_ClearBuf(char *buf, uint16_t word_length) {
	for(int i=0; i < word_length; i++) {
		buf[i] = 0;
	}
}

/*
void Uart_printbase (long n, uint8_t base)
{
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  char *s = &buf[sizeof(buf) - 1];

  *s = '\0';

  // prevent crash if called with base == 1
  if (base < 2) base = 10;

  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    *--s = c < 10 ? c + '0' : c + 'A' - 10;
  } while(n);

  while(*s) Uart_write(*s++);
}

void Get_string (char *buffer)
{
	int index=0;

	while (_rx_buffer->tail>_rx_buffer->head)
	{
		if ((_rx_buffer->buffer[_rx_buffer->head-1] == '\n')||((_rx_buffer->head == 0) && (_rx_buffer->buffer[UART_BUFFER_SIZE-1] == '\n')))
			{
				buffer[index] = Uart_read();
				index++;
			}
	}
	unsigned int start = _rx_buffer->tail;
	unsigned int end = (_rx_buffer->head);
	if ((_rx_buffer->buffer[end-1] == '\n'))
	{

		for (unsigned int i=start; i<end; i++)
		{
			buffer[index] = Uart_read();
			index++;
		}
	}
}
*/