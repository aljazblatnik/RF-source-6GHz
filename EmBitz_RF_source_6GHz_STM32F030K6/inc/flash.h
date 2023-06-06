#ifndef FLASH_H_INCLUDED
#define FLASH_H_INCLUDED

void flashUnlock(void);
void flashLock(void);
void flashPageErase(int page_addr);
void flashWrite(uint32_t flash_address, uint32_t flash_data);
char flashWriteChanges(void);
void flashCopyFromFlashToRAM(void);

#endif /* FLASH_H_INCLUDED */
