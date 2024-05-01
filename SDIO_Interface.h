/**
 ******************************************************************************
 * @file SDIO_Interface.h
 * @brief SDIO interface implementation file
 *        Manage communication with SD card
 *
 * @creation 2024/04/06
 * @edition 2024/04/11
 * 
 * @author Guillaume Dauguen
 *
 ******************************************************************************
 */
#ifndef __SDIO_INTERFACE_H__
#define __SDIO_INTERFACE_H__

#include "fatfs.h"
#include "string.h"
#include "stdio.h"

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
FRESULT SDIO_Interface_MountSD();
FRESULT SDIO_Interface_UnmountSD();
FRESULT SDIO_Interface_ScanFiles(char* pat);
FRESULT SDIO_Interface_OpenFile(char* name);
FRESULT SDIO_Interface_ReadFile(uint8_t* buf, uint32_t );
FRESULT SDIO_Interface_CloseFile();
FRESULT SDIO_Interface_CheckSD(uint32_t* total, uint32_t* free_space);
FRESULT SDIO_Interface_CheckFile(char* name);

#endif /* __SDIO_INTERFACE_H__ */
