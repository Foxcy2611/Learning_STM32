#include "flash_logger.h"

// Vi tri ghi tiep theo va tong so mau hien co
static volatile uint16_t g_log_write_index = 0;
static volatile uint16_t g_log_total_count = 0;

// Khoi tao Flash: quet tim vi tri o nho trong dau tien (0xFFFF) sau khi reset
void FLASH_Logger_Init(void)
{
    uint16_t i;
    uint16_t read_val;

    g_log_write_index = 0;
    g_log_total_count = 0;

    // Tim o nho chua ghi (0xFFFF)
    for (i = 0; i < FLASH_LOG_MAX_SAMPLES; i++)
    {
        read_val = *(__IO uint16_t*)(FLASH_LOG_PAGE_ADDR + (i * 2));
        if (read_val == FLASH_EMPTY_VALUE)
        {
            g_log_write_index = i;
            g_log_total_count = i;
            return;
        }
    }

    // Neu da ghi du 50 mau
    g_log_write_index = FLASH_LOG_MAX_SAMPLES;
    g_log_total_count = FLASH_LOG_MAX_SAMPLES;
}

// Ghi 1 mau vao Flash (neu day 50 mau thi xoa page de ghi lai tu dau)
uint8_t FLASH_Logger_WriteSample(uint16_t adc_val)
{
    FLASH_Status status;
    uint32_t target_addr;

    // Neu da day 50 mau -> xoa page 63
    if (g_log_write_index >= FLASH_LOG_MAX_SAMPLES)
    {
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        
        status = FLASH_ErasePage(FLASH_LOG_PAGE_ADDR);
        FLASH_Lock();

        if (status != FLASH_COMPLETE)
        {
            return 0; // Xoa that bai
        }

        g_log_write_index = 0;
        g_log_total_count = 0;
    }

    target_addr = FLASH_LOG_PAGE_ADDR + (g_log_write_index * 2);

    // Mo khoa va ghi 2 byte vao Flash
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    
    status = FLASH_ProgramHalfWord(target_addr, adc_val);
    FLASH_Lock();

    if (status == FLASH_COMPLETE)
    {
        g_log_write_index++;
        g_log_total_count = g_log_write_index;
        return 1;
    }

    return 0;
}

// Doc 1 mau tai vi tri index
uint16_t FLASH_Logger_ReadSample(uint16_t index)
{
    if (index >= FLASH_LOG_MAX_SAMPLES)
    {
        return FLASH_EMPTY_VALUE;
    }
    return *(__IO uint16_t*)(FLASH_LOG_PAGE_ADDR + (index * 2));
}

// Lay so luong mau hien co
uint16_t FLASH_Logger_GetSampleCount(void)
{
    return g_log_total_count;
}

// Xoa sach page 63
void FLASH_Logger_ClearAll(void)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(FLASH_LOG_PAGE_ADDR);
    FLASH_Lock();

    g_log_write_index = 0;
    g_log_total_count = 0;
}
