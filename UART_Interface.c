/**
 ******************************************************************************
 * @file UART_Interface.c
 * @brief UART interface implementation file
 *        Manage UART communication
 *
 * @creation 2024/04/10
 * @edition 2024/04/12
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#include "UART_Interface.h"
#include "RingBuffer.h"
#include "Shell.h"

#include <string.h>

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------
static UART_HandleTypeDef* uart;
static RingBuffer* _rx_buffer;
static RingBuffer* _tx_buffer;

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void UART_Interface_Init(UART_HandleTypeDef* huart, RingBuffer* rx_buffer, RingBuffer* tx_buffer) {
	uart = huart;
  _rx_buffer = rx_buffer;
  _tx_buffer = tx_buffer;
  
  __HAL_UART_ENABLE_IT(uart, UART_IT_ERR);		// Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
  __HAL_UART_ENABLE_IT(uart, UART_IT_RXNE);		// Enable the UART Data Register not empty Interrupt
}

void UART_Interface_Run() {
	uint32_t isrflags   = READ_REG(uart->Instance->SR);
	uint32_t cr1its     = READ_REG(uart->Instance->CR1);

	/* if DR is not empty and the Rx Int is enabled */
	if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
		/******************
							*  @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
							*          error) and IDLE (Idle line detected) flags are cleared by software
							*          sequence: a read operation to USART_SR register followed by a read
							*          operation to USART_DR register.
							* @note   RXNE flag can be also cleared by a read to the USART_DR register.
							* @note   TC flag can be also cleared by software sequence: a read operation to
							*          USART_SR register followed by a write operation to USART_DR register.
							* @note   TXE flag is cleared only by a write to the USART_DR register.

		*********************/
		uart->Instance->SR;                       /* Read status register */
		unsigned char c = uart->Instance->DR;     /* Read data register */
		RingBuffer_Put(_rx_buffer, c);  // store data in buffer
		Shell_PrintLetter(c);
		return;
	}

	/*If interrupt is caused due to Transmit Data Register Empty */
	if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET)) {
		if(RingBuffer_IsEmpty(_tx_buffer)) {
				// Buffer empty, so disable interrupts
				__HAL_UART_DISABLE_IT(uart, UART_IT_TXE);
		}

		else {
			// There is more data in the output buffer. Send the next byte
			uint8_t c;
			RingBuffer_Get(_tx_buffer, &c);

			/******************
			*  @note   PE (Parity error), FE (Framing error), NE (Noise error), ORE (Overrun
			*          error) and IDLE (Idle line detected) flags are cleared by software
			*          sequence: a read operation to USART_SR register followed by a read
			*          operation to USART_DR register.
			* @note   RXNE flag can be also cleared by a read to the USART_DR register.
			* @note   TC flag can be also cleared by software sequence: a read operation to
			*          USART_SR register followed by a write operation to USART_DR register.
			* @note   TXE flag is cleared only by a write to the USART_DR register.

			*********************/

			uart->Instance->SR;
			uart->Instance->DR = c;
		}
	return;
	}
}

void UART_Interface_EnableIT() {
	__HAL_UART_ENABLE_IT(uart, UART_IT_TXE); // Enable UART transmission interrupt
}


