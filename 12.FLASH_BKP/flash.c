#include "flash.h"

// Ham ghi 1 Half-word (16-bit) vao Flash
void FLASH_WriteHalfWord(uint32_t address, uint16_t data) {
	// 1. Mo khoa Flash
	FLASH_Unlock();
	
	// 2. Xoa cac co bao loi neu co (tu nhung lan thao tac truoc)
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	// 3. Bat buoc phai xoa nguyen 1 Page truoc khi ghi
	FLASH_ErasePage(address);
	
	// 4. Ghi du lieu moi vao dung dia chi
	FLASH_ProgramHalfWord(address, data);
	
	// 5. Khoa Flash lai ngay lap tuc de chong con tro di lac
	FLASH_Lock();
}

// Ham doc du lieu tu Flash
// Flash nam thang tren bo nho nen doc giong y het doc bien thong thuong
uint16_t FLASH_ReadHalfWord(uint32_t address) {
	// Ep kieu dia chi ve con tro 16-bit va lay gia tri tai do (*ptr)
	return *(__IO uint16_t*)address;
}
