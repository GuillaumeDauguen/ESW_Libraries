/**
 ******************************************************************************
 * @file SDIO_Interface.c
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
#include "SDIO_Interface.h"
#include "Shell.h"

// ------------------------------------------------------------------------
// -------------------------- STATIC PROTOTYPES ---------------------------
// ------------------------------------------------------------------------
static FATFS fs;				// file system
static FIL fil;					// file
static FILINFO fno;
static UINT br, bw;			// file read / write count

// ------------------------------------------------------------------------
// ------------------- PUBLIC FUNCTIONS IMPLEMENTATION --------------------
// ------------------------------------------------------------------------
FRESULT SDIO_Interface_MountSD() {
	return f_mount(&fs, "/", 1);
	/*if (fresult != FR_OK) Shell_PrintString("error in mounting SD CARD...\r\n ");
	else Shell_PrintString("SD CARD mounted successfully...\r\n ");*/
}

FRESULT SDIO_Interface_UnmountSD() {
	return f_mount(NULL, "/", 1);
	/*if (fresult == FR_OK) Shell_PrintString("SD CARD UNMOUNTED successfully...\r\n ");
	else Shell_PrintString("error!!! in UNMOUNTING SD CARD\r\n ");*/
}

/* Start node to be scanned (***also used as work area***) */
FRESULT SDIO_Interface_ScanFiles(char* pat) {
	char result_string[128];
	FRESULT fresult;
	DIR dir;
	UINT i;

	char path[20];
	sprintf(path, "%s", pat);

	fresult = f_opendir(&dir, path);												/* Open the directory */
	if (fresult == FR_OK) {
		for (;;) {
			fresult = f_readdir(&dir, &fno);										/* Read a directory item */
			if (fresult != FR_OK || fno.fname[0] == 0) break;		/* Break on error or end of dir */
			if (fno.fattrib & AM_DIR) {													/* It is a directory */
				if (!(strcmp ("SYSTEM~1", fno.fname))) continue;
				sprintf(result_string, "Dir: %s", fno.fname);
				Shell_PrintString(result_string);
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				fresult = SDIO_Interface_ScanFiles(path);												/* Enter the directory */
				if (fresult != FR_OK) break;
				path[i] = 0;
			}
			else {																							/* It is a file. */
				 sprintf(result_string, "File: %s/%s\r\n ", path, fno.fname);
				 Shell_PrintString(result_string);
			}
		}
		f_closedir(&dir);
	}
	return fresult;
}

FRESULT SDIO_Interface_OpenFile(char* name) {
	FRESULT fresult;
	
	/**** check whether the file exists or not ****/
	fresult = f_stat(name, &fno);
	if (fresult != FR_OK) {
		return fresult;					//"*%s* does not exists\n", name
	}
	/* Open file to read */
	return f_open(&fil, name, FA_READ);
	//	if (fresult != FR_OK)
	//		"error no %d in opening file\n", fresult
}

FRESULT SDIO_Interface_CloseFile() {
	return f_close(&fil);
//	if (fresult != FR_OK)
//		"error no %d in closing file\n", fresult
}


FRESULT SDIO_Interface_ReadFile(uint8_t* buf, uint32_t length) {
	return f_read(&fil, buf, length, &br);
//	if (fresult != FR_OK)
//		"error no %d in reading file\n", fresult
}

FRESULT SDIO_Interface_CheckSD(uint32_t* total, uint32_t* free_space) {
	/**** capacity related *****/
	FATFS* pfs;
	DWORD fre_clust;
	
	/* Check free space */
	FRESULT fresult = f_getfree("", &fre_clust, &pfs);
	if (fresult != FR_OK) {
		*total = 0;
		*free_space = 0;
		return fresult;
	}
	
	*total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	*free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
	
	return FR_OK;
}

FRESULT SDIO_Interface_CheckFile(char* name) {
	char result_string[128];
	FRESULT fresult;
	
	fresult = f_stat(name, &fno);
	if (fresult != FR_OK) {
		return fresult;
		/*
		case FR_NO_FILE:
			"*%s* does not exist.\r\n "
		default:
			"An error occurred. (%d)\r\n "
		*/
	}
	sprintf(result_string, "Below are the details of the *%s* \nSize: %lu\r\n ",name, fno.fsize);
	Shell_PrintString(result_string);
	sprintf(result_string, "Timestamp: %u/%02u/%02u, %02u:%02u\r\n " ,
				 (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,
				 fno.ftime >> 11, fno.ftime >> 5 & 63);
	Shell_PrintString(result_string);
	sprintf(result_string, "Attributes: %c%c%c%c%c\r\n ",
				 (fno.fattrib & AM_DIR) ? 'D' : '-',
				 (fno.fattrib & AM_RDO) ? 'R' : '-',
				 (fno.fattrib & AM_HID) ? 'H' : '-',
				 (fno.fattrib & AM_SYS) ? 'S' : '-',
				 (fno.fattrib & AM_ARC) ? 'A' : '-');
	Shell_PrintString(result_string);
	
	return FR_OK;
}


