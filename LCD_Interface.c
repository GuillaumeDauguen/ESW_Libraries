/**
 ******************************************************************************
 * @file LCD_Interface.c
 * @brief "file" implementation file
 *        Do things
 *
 * @creation 2024/04/24
 * @edition 2024/04/24
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#include <LCD_Interface.h>
#include "main.h"

// ------------------------------------------------------------------------
// -------------------------------- MACROS --------------------------------
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// ----------------------------- STATIC TYPES -----------------------------
// ------------------------------------------------------------------------
typedef enum {
	INSTRUCTION = GPIO_PIN_RESET,
	DATA = GPIO_PIN_SET,
}register_select;

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// ---------------------- STATIC FUCTIONS PROTOTYPES ----------------------
// ------------------------------------------------------------------------
static void _SendByte(register_select RS, uint8_t data);
static void _SendHalfByte(register_select RS, uint8_t data);

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void LCD_Interface_Init() {
	HAL_Delay(50);
	_SendHalfByte(INSTRUCTION, 0b0011);	//
	_SendHalfByte(INSTRUCTION, 0b0011);	// Manual Reset
	_SendHalfByte(INSTRUCTION, 0b0011);	//

	_SendHalfByte(INSTRUCTION, 0b0010);	// 4-bit comm
	_SendByte(INSTRUCTION, 0b00101000);	// 2 lines
	_SendByte(INSTRUCTION, 0b00000001);	// clear screen
	_SendByte(INSTRUCTION, 0b00000010);	// return home
	_SendByte(INSTRUCTION, 0b00001110);
	_SendByte(INSTRUCTION, 0b00000110);
}

void LCD_Interface_Run() {
	char hello[] = "Hello, world !";
	for (int i = 0; i < 14; ++i) {
		_SendByte(DATA, hello[i]);
	}
	_SendByte(INSTRUCTION, 0b00000010);	// return home
}

// ------------------------------------------------------------------------
// ------------------- STATIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void _SendByte(register_select RS, uint8_t data) {
	_SendHalfByte(RS, data >> 4);
	_SendHalfByte(RS, data & 0x0F);
}

void _SendHalfByte(register_select RS, uint8_t data) {
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, RS);
	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data & (1 << 3)));
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data & (1 << 2)));
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data & (1 << 1)));
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data & (1 << 0)));

	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_Enable_GPIO_Port, LCD_Enable_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_Enable_GPIO_Port, LCD_Enable_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
}


