#include "flash_logger.h"

/* Biến lưu chỉ số ghi tiếp theo (0 -> FLASH_LOG_MAX_SAMPLES) */
static volatile uint16_t g_log_write_index = 0;

/* Biến lưu tổng số lượng mẫu hợp lệ hiện có */
static volatile uint16_t g_log_total_count = 0;

/**
  * @brief  Khởi tạo module Flash Logger, tự động quét tìm ô trống tiếp theo sau khi khởi động lại
  * @param  None
  * @retval None
  */
void FLASH_Logger_Init(void)
{
    uint16_t i;
    uint16_t read_val;

    g_log_write_index = 0;
    g_log_total_count = 0;

    /* Quét lần lượt các ô nhớ từ đầu trang để tìm vị trí 0xFFFF đầu tiên */
    for (i = 0; i < FLASH_LOG_MAX_SAMPLES; i++)
    {
        read_val = *(__IO uint16_t*)(FLASH_LOG_PAGE_ADDR + (i * 2));
        if (read_val == FLASH_EMPTY_VALUE)
        {
            /* Tìm thấy vị trí ô nhớ trống chưa ghi */
            g_log_write_index = i;
            g_log_total_count = i;
            return;
        }
    }

    /* Nếu toàn bộ 50 mẫu đã được ghi đầy */
    g_log_write_index = FLASH_LOG_MAX_SAMPLES;
    g_log_total_count = FLASH_LOG_MAX_SAMPLES;
}

/**
  * @brief  Ghi 1 mẫu dữ liệu vào Flash nội theo cơ chế Circular Buffer
  * @param  adc_val: Giá trị cần ghi (16-bit)
  * @retval 1 nếu ghi thành công, 0 nếu thất bại
  */
uint8_t FLASH_Logger_WriteSample(uint16_t adc_val)
{
    FLASH_Status status;
    uint32_t target_addr;

    /* Nếu bộ đệm đã đầy 50 mẫu, tiến hành xóa trang để ghi vòng lặp mới */
    if (g_log_write_index >= FLASH_LOG_MAX_SAMPLES)
    {
        FLASH_Unlock();
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        
        status = FLASH_ErasePage(FLASH_LOG_PAGE_ADDR);
        FLASH_Lock();

        if (status != FLASH_COMPLETE)
        {
            return 0; /* Xóa trang thất bại */
        }

        g_log_write_index = 0;
        g_log_total_count = 0;
    }

    /* Tính toán địa chỉ ô nhớ cần ghi */
    target_addr = FLASH_LOG_PAGE_ADDR + (g_log_write_index * 2);

    /* Mở khóa Flash và ghi dữ liệu HalfWord */
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    
    status = FLASH_ProgramHalfWord(target_addr, adc_val);
    
    /* Khóa Flash ngay sau khi hoàn tất */
    FLASH_Lock();

    if (status == FLASH_COMPLETE)
    {
        g_log_write_index++;
        g_log_total_count = g_log_write_index;
        return 1;
    }

    return 0;
}

/**
  * @brief  Đọc giá trị mẫu tại chỉ số xác định
  * @param  index: Vị trí mẫu (0 đến TotalCount - 1)
  * @retval Giá trị mẫu lưu trong Flash (hoặc 0xFFFF nếu không hợp lệ)
  */
uint16_t FLASH_Logger_ReadSample(uint16_t index)
{
    if (index >= FLASH_LOG_MAX_SAMPLES)
    {
        return FLASH_EMPTY_VALUE;
    }
    return *(__IO uint16_t*)(FLASH_LOG_PAGE_ADDR + (index * 2));
}

/**
  * @brief  Lấy tổng số lượng mẫu hợp lệ đang lưu trong Flash
  * @param  None
  * @retval Số lượng mẫu
  */
uint16_t FLASH_Logger_GetSampleCount(void)
{
    return g_log_total_count;
}

/**
  * @brief  Xóa trắng trang log Flash Page 63
  * @param  None
  * @retval None
  */
void FLASH_Logger_ClearAll(void)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(FLASH_LOG_PAGE_ADDR);
    FLASH_Lock();

    g_log_write_index = 0;
    g_log_total_count = 0;
}
