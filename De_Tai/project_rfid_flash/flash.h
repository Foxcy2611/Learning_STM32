#ifndef FLASH_H
#define FLASH_H

#include "stm32f10x.h"

#define FLASH_UID_PAGE   0x0800FC00
#define MAX_UIDS         10
#define UID_LENGTH       4

void Flash_SaveUIDs(uint8_t uids[][4], uint8_t count);
void Flash_ReadUID(uint8_t index, uint8_t *uid);
int Flash_FindUID(uint8_t *uid);
void Flash_ClearUIDs(void);

#endif