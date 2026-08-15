#include "bkp.h"

// Ham cap xung nhip va mo khoa vung BKP
void BKP_Init_Access(void){
	// 1. Cap clock cho khoi Power (PWR) va Backup (BKP)
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	// 2. Mo chot an toan (DBP bit) cho phep CPU ghi vao vung Backup
	PWR_BackupAccessCmd(ENABLE);
}

// Ham ghi 16-bit vao BKP (Ghi nhu RAM, khong can xoa)
void BKP_WriteData(uint16_t bkp_register, uint16_t data) {
	// Goi ham mo khoa truoc khi ghi de chac chan
	BKP_Init_Access();
	
	// Ghi truc tiep data vao thanh ghi BKP tuong ung (vd: BKP_DR1)
	BKP_WriteBackupRegister(bkp_register, data);
	
	// Khoa lai sau khi ghi xong de bao ve
	// PWR_BackupAccessCmd(DISABLE); 
}

// Ham doc du lieu tu BKP
uint16_t BKP_ReadData(uint16_t bkp_register) {
	// BKP doc thoai mai khong can mo khoa PWR_BackupAccess
	// Nhung van phai dam bao da cap Clock cho khoi BKP
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	return BKP_ReadBackupRegister(bkp_register);
}
