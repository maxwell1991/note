/*
 * iap.h
 *
 *  Created on: 2017Äê8ÔÂ9ÈÕ
 *      Author: JT
 */

#ifndef BOOTLOADER_IAP_H_
#define BOOTLOADER_IAP_H_

void System_Reboot(void);
static uint8_t IapUpg_Checksum(uint8_t *buf, uint16_t len);
static uint8_t IapUpg_GetChar(uint8_t *c);
static uint8_t IapUpg_PutChar(uint8_t c);
static uint8_t IapUpg_RomFlashUnlock(void);
static uint8_t IapUpg_RomFlashLock(void);
static uint8_t IapUpg_RomFlashErase(void);
static uint8_t IapUpg_RomFlashWrDWord(uint32_t offset, uint8_t * data);
void IapUpg_Proc(void);
uint8_t t_IapUpg_RomFlashErase(void);
uint8_t t_IapUpg_RomFlashWrDWord(uint32_t offset, uint8_t * data);
uint8_t SrvUpg_CheckData(void);




#endif /* BOOTLOADER_IAP_H_ */
