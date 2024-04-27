/**
 ******************************************************************************
 * @file LCD_Interface.c
 * @brief LCD Interface implementation file
 *        Do things
 *
 * @creation 2024/04/24
 * @edition 2024/04/27
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 * @todo read busy flag
 * @todo switch to interrupt mode
 ******************************************************************************
 */
#include "../ESW_Libraries/LCD_Interface.h"

// ------------------------------------------------------------------------
// ----------------------------- STATIC TYPES -----------------------------
// ------------------------------------------------------------------------
typedef enum {
	INSTRUCTION = GPIO_PIN_RESET,
	DATA = GPIO_PIN_SET,
} register_select;

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------
static LCD_data *s_LCD;

// ------------------------------------------------------------------------
// ---------------------- STATIC FUCTIONS PROTOTYPES ----------------------
// ------------------------------------------------------------------------
static void _SendByte(register_select RS, uint8_t data);
static void _SendHalfByte(register_select RS, uint8_t data);
static void _SetPinState(GPIO gpio, GPIO_PinState PinState);

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void LCD_Interface_Init(LCD_data *LCD) {
	s_LCD = LCD;
	s_LCD->cursor_row = 0;
	s_LCD->cursor_position = 0;

	HAL_Delay(50);
	LCD_Interface_Reset();

	_SendHalfByte(INSTRUCTION, 0b0010);	// 4-bit comm
	LCD_Interface_SendInstruction(0b00101000);	// Function Set: set Data length (4-bit bus), line number (2 lines), font (5x8 dots)
	LCD_Interface_SendInstruction(0b00000001);	// Clear display
	LCD_Interface_SendInstruction(0b00000010);	// Return home
	LCD_Interface_SendInstruction(0b00001100);	// Display ON/OFF: set display (on), cursor (off), cursor blink (off)
	LCD_Interface_SendInstruction(0b00000110);	// Entry Mode Set: set the moving direction of cursor (right), display (no shift)
}

void LCD_Interface_Reset() {
	// 40ms min delay after power up
	_SendHalfByte(INSTRUCTION, 0b0011);
	// 4.1ms min delay
	_SendHalfByte(INSTRUCTION, 0b0011);
	// 100us min delay
	_SendHalfByte(INSTRUCTION, 0b0011);
}

void LCD_Interface_Home() {
	LCD_Interface_SendInstruction(0b00000010);	// return home
	s_LCD->cursor_row = 0;
	s_LCD->cursor_position = 0;
}

void LCD_Interface_SetCursorPos(uint8_t row, uint8_t col) {
	LCD_Interface_SendInstruction(0x80 | (row << 6) | col);
	s_LCD->cursor_row = row;
	s_LCD->cursor_position = col;
}

void LCD_Interface_PrintString(uint8_t *data, uint8_t len) {
	for (uint8_t i = 0; i < len; i++) {
		LCD_Interface_PrintChar(data[i]);
	}
}

void LCD_Interface_PrintChar(uint8_t data) {
	LCD_Interface_SendData(data);
	s_LCD->cursor_position++;
	if (s_LCD->cursor_position >= s_LCD->memory_cols) {
		s_LCD->cursor_position %= s_LCD->memory_cols;
		LCD_Interface_SetCursorPos(s_LCD->cursor_row, s_LCD->cursor_position);
	}
}

void LCD_Interface_SendData(uint8_t data) {
	_SendByte(DATA, data);
}

void LCD_Interface_SendInstruction(uint8_t data) {
	_SendByte(INSTRUCTION, data);
}

/*
 * CGRAM addr [0:7]
 * Need a 8 bytes array
 */
void LCD_Interface_StorePattern(uint8_t CGRAL_addr, uint8_t *pattern) {
	LCD_Interface_SendInstruction(0x40 | ((CGRAL_addr & 0x07) << 3));
	for (uint8_t i = 0; i < 8; i++) {
		_SendByte(DATA, pattern[i]);
	}
	LCD_Interface_SetCursorPos(s_LCD->cursor_row, s_LCD->cursor_position);
}

void LCD_Interface_Shift(LCD_ELEMENT element, LCD_DIRECTION dir) {
	_SendByte(INSTRUCTION, (0x10 | (element << 3) | (dir << 2)));
}

// ------------------------------------------------------------------------
// ------------------- STATIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void _SendByte(register_select RS, uint8_t data) {
	_SendHalfByte(RS, data >> 4);
	_SendHalfByte(RS, data & 0x0F);
}

void _SendHalfByte(register_select RS, uint8_t data) {
	_SetPinState(s_LCD->RS, RS);
	_SetPinState(s_LCD->RW, GPIO_PIN_RESET);

	_SetPinState(s_LCD->D7, (data & (1 << 3)));
	_SetPinState(s_LCD->D6, (data & (1 << 2)));
	_SetPinState(s_LCD->D5, (data & (1 << 1)));
	_SetPinState(s_LCD->D4, (data & (1 << 0)));

	HAL_Delay(1);
	_SetPinState(s_LCD->EN, GPIO_PIN_SET);
	HAL_Delay(1);
	_SetPinState(s_LCD->EN, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void _SetPinState(GPIO gpio, GPIO_PinState PinState) {
	HAL_GPIO_WritePin(gpio.Port, gpio.Pin, PinState);
}


