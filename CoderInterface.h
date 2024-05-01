/**
 ******************************************************************************
 * @file CoderInterface
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
#ifndef __CODER_INTERFACE_H__
#define __CODER_INTERFACE_H__

#include "tim.h"
// ------------------------------------------------------------------------
// ----------------------------- PUBLIC TYPES -----------------------------
// ------------------------------------------------------------------------
TIM_HandleTypeDef* CoderInterface_GetTim();
void CoderInterface_TimIRQ(TIM_HandleTypeDef *htim);
void CoderInterface_TimeOut();
float CoderInterface_GetSpeedRatio();
// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------

#endif /* __CODER_INTERFACE_H__ */
