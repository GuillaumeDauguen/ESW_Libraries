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
#ifndef __SHELL_H__
#define __SHELL_H__

#include "usart.h"

#include <stdbool.h>

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void Shell_Init(UART_HandleTypeDef* huart);
bool Shell_IsNotEmpty();
char Shell_ReadLetter();
void Shell_PrintLetter(uint8_t letter);
void Shell_PrintString(const char* string);
void Shell_ClearBuf(char *buf, uint16_t word_length);

#endif /* __SHELL_H__ */
