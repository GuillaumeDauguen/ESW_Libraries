/**
 ******************************************************************************
 * @file UART_Interface.h
 * @brief UART interface implementation file
 *        Manage UART communication
 *
 * @creation 2024/04/10
 * @edition 2024/04/11
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 * @setup the following function must be put in the USART IRQ Handler
 * 			UART_Interface_Run();
 *
 ******************************************************************************
 */
#ifndef __UART_INTERFACE_H__
#define __UART_INTERFACE_H__

#include "usart.h"
#include "RingBuffer.h"

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void UART_Interface_Init(UART_HandleTypeDef* huart, RingBuffer* rx_buffer, RingBuffer* tx_buffer);
void UART_Interface_Run();
void UART_Interface_EnableIT();

#endif /* __UART_INTERFACE_H__ */
