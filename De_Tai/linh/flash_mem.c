#include "flash_mem.h"

void Flash_WriteThreshold(float value) {
    uint32_t data_as_int = *((uint32_t*)&value); 

    FLASH_Unlock(); 
    
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    FLASH_ErasePage(FLASH_USER_START_ADDR);
    
    FLASH_ProgramWord(FLASH_USER_START_ADDR, data_as_int);
    
    FLASH_Lock(); 
}

float Flash_ReadThreshold(void) {
    uint32_t data_as_int = *(__IO uint32_t*)FLASH_USER_START_ADDR;
    
    // trang trong
    if (data_as_int == 0xFFFFFFFF) {
        return -1.0; // 
    }
    
    float value = *((float*)&data_as_int); 
    return value;
}