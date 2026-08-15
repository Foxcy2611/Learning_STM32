#include "stm32f10x.h"
#include "bkp.h"
#include "flash.h"

int main(void) {
	BKP_WriteData(BKP_DR1, 0xABCD);
	uint16_t my_bkp_data = BKP_ReadData(BKP_DR1);
	
	FLASH_WriteHalfWord(FINAL_ADDRESS_FLASH, 2026);
	uint16_t my_flash_data = FLASH_ReadHalfWord(FINAL_ADDRESS_FLASH);
	
	while(1){
		// Su dung cac bien tranh warning
		if(my_bkp_data > 0 || my_flash_data > 0){
			
		}
	}
}
