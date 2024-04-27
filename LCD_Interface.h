/**
 ******************************************************************************
 * @file LCD_Interface.h
 * @brief LCD Interface implementation file
 *        Do things
 *
 * @creation 2024/04/24
 * @edition 2024/04/27
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#ifndef __1602_INTERFACE_H__
#define __1602_INTERFACE_H__

#include "main.h"

#include <stdint.h>

// ------------------------------------------------------------------------
// ----------------------------- PUBLIC TYPES -----------------------------
// ------------------------------------------------------------------------
typedef struct {
		GPIO_TypeDef *Port;
		uint16_t Pin;
} GPIO;

typedef struct {
		uint8_t rows;
		uint8_t display_cols;
		uint8_t memory_cols;

		uint8_t cursor_row;
		uint8_t cursor_position;

		GPIO RS;
		GPIO RW;
		GPIO EN;
		GPIO D7;
		GPIO D6;
		GPIO D5;
		GPIO D4;
} LCD_data;

typedef enum {
	CURSOR = 0,
	DISPLAY = 1,
} LCD_ELEMENT;

typedef enum {
	LEFT = 0,
	RIGHT = 1,
} LCD_DIRECTION;

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void LCD_Interface_Init(LCD_data *LCD);
void LCD_Interface_Reset();
void LCD_Interface_Home();
void LCD_Interface_SetCursorPos(uint8_t row, uint8_t col);
void LCD_Interface_SendInstruction(uint8_t data);
void LCD_Interface_SendData(uint8_t data);
void LCD_Interface_PrintChar(uint8_t data);
void LCD_Interface_PrintString(uint8_t *data, uint8_t len);
void LCD_Interface_Shift(LCD_ELEMENT element, LCD_DIRECTION dir);
void LCD_Interface_StorePattern(uint8_t CGRAM_addr, uint8_t *pattern);
void LCD_Interface_Run();

#endif /* __1602_INTERFACE_H__ */
