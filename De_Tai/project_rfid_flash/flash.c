#include "flash.h"
#include "stm32f10x_flash.h"

void Flash_SaveUIDs(uint8_t uids[][4], uint8_t count) {
    if (count > MAX_UIDS) {
        count = MAX_UIDS;
    }
    
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    
    FLASH_Status status = FLASH_ErasePage(FLASH_UID_PAGE);
    if (status == FLASH_COMPLETE) {
        // Ghi số lượng UID đã lưu (count)
        FLASH_ProgramWord(FLASH_UID_PAGE, count);
        
        // Ghi các UID
        for (uint8_t i = 0; i < count; i++) {
            uint32_t uid_val = 0;
            uid_val |= (uint32_t)uids[i][0];
            uid_val |= (uint32_t)uids[i][1] << 8;
            uid_val |= (uint32_t)uids[i][2] << 16;
            uid_val |= (uint32_t)uids[i][3] << 24;
            
            FLASH_ProgramWord(FLASH_UID_PAGE + 4 + i * 4, uid_val);
        }
    }
    
    FLASH_Lock();
}

void Flash_ReadUID(uint8_t index, uint8_t *uid) {
    if (index >= MAX_UIDS) return;
    
    uint32_t address = FLASH_UID_PAGE + 4 + index * 4;
    uint32_t uid_val = *(volatile uint32_t*)address;
    
    uid[0] = uid_val & 0xFF;
    uid[1] = (uid_val >> 8) & 0xFF;
    uid[2] = (uid_val >> 16) & 0xFF;
    uid[3] = (uid_val >> 24) & 0xFF;
}

int Flash_FindUID(uint8_t *uid) {
    uint32_t count = *(volatile uint32_t*)FLASH_UID_PAGE;
    if (count == 0xFFFFFFFF || count == 0) {
        return -1;
    }
    if (count > MAX_UIDS) {
        count = MAX_UIDS;
    }
    
    for (uint8_t i = 0; i < count; i++) {
        uint8_t stored_uid[4];
        Flash_ReadUID(i, stored_uid);
        if (stored_uid[0] == uid[0] &&
            stored_uid[1] == uid[1] &&
            stored_uid[2] == uid[2] &&
            stored_uid[3] == uid[3]) {
            return i; // Tìm thấy, trả về index
        }
    }
    return -1; // Không tìm thấy
}

void Flash_ClearUIDs(void) {
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(FLASH_UID_PAGE);
    FLASH_Lock();
}
