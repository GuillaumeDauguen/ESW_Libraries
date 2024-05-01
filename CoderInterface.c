/**
 ******************************************************************************
 * @file CoderInterface.c
 * @brief "file" implementation file
 *        Do things
 *
 * @creation 2024/04/14
 * @edition 2024/04/14
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#include "CoderInterface.h"
#include "Shell.h"

#include <stdint.h>
#include <stdio.h>

// ------------------------------------------------------------------------
// -------------------------------- MACROS --------------------------------
// ------------------------------------------------------------------------
#define CODER_NB_STEPS (30)

#define CODER_TIM (&htim9)
#define TIM_CHANNEL HAL_TIM_ACTIVE_CHANNEL_1

#define TIMCLOCK (168000000)
#define PRESCALAR (42000)
#define COUNTER_PERIOD (2000)

#define WIDTH_BUF_LENGTH (30)

// ------------------------------------------------------------------------
// ----------------------------- STATIC TYPES -----------------------------
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------
static uint8_t timeout_flag = false;

static bool 	width_buf_full = false;
static uint8_t 	width_index = 0;
static uint32_t width_buf[WIDTH_BUF_LENGTH];
static uint32_t width_sum = 0;
static uint32_t width_mean = 0;

// ------------------------------------------------------------------------
// ---------------------- STATIC FUCTIONS PROTOTYPES ----------------------
// ------------------------------------------------------------------------
void _UpdateWidth(uint32_t width);

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
TIM_HandleTypeDef* CoderInterface_GetTim() {
	return CODER_TIM;
}

void CoderInterface_TimIRQ(TIM_HandleTypeDef *htim) {
	if (htim->Channel == TIM_CHANNEL)	{

		if (timeout_flag) {
			timeout_flag = false;
			return;
		}
		else {
			uint32_t counter = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);	// read second value

			float refClock = TIMCLOCK / PRESCALAR;
			float mFactor = 1000000 / refClock;

			_UpdateWidth(counter * mFactor);
		}
		__HAL_TIM_SET_COUNTER(htim, 0);		// reset the counter
	}
	
}

void CoderInterface_TimeOut() {
	if (CODER_TIM->Instance->SR & TIM_FLAG_UPDATE) {
		//Shell_PrintString("timeout\n");
		timeout_flag = true;
	}
}

float CoderInterface_GetSpeedRatio() {
	if (timeout_flag || !width_mean) return 0;
	float ratio = (1000000.0f / (CODER_NB_STEPS * width_mean));
	ratio = (ratio - 1) * 0.2f + 1;
	return ratio;
}

// ------------------------------------------------------------------------
// ------------------- STATIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
void _UpdateWidth(uint32_t width) {
	if (width_buf_full) {
		width_sum -= width_buf[width_index];
		width_buf[width_index] = width;
		width_sum += width;
		
		width_mean = width_sum / WIDTH_BUF_LENGTH;
		
		width_index++;
		width_index %= WIDTH_BUF_LENGTH;
	}
	else {
		width_buf[width_index] = width;
		width_sum += width;
		
		width_index++;
		width_mean = width_sum / width_index;
		
		if (width_index >= WIDTH_BUF_LENGTH) {
			width_buf_full = true;
			width_index = 0;
		}
	}
}


