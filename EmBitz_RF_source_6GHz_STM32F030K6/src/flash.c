#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include <stdio.h>
#include <stdlib.h>

extern const int program_flash_memory[10][3];
extern int memory[10][3];
extern char status;

void flashUnlock(void){
    // set latency
    FLASH->ACR |= FLASH_ACR_LATENCY; // more than 24 MHz
    while((FLASH->SR & FLASH_SR_BSY) != 0); // wait for all memory operation to finish
    if ((FLASH->CR & FLASH_CR_LOCK) != 0){ // flash unlock
        FLASH->KEYR = FLASH_FKEY1;
        FLASH->KEYR = FLASH_FKEY2;
    }
}

void flashLock(void){
    FLASH->CR |= FLASH_CR_LOCK; // flash lock
}

void flashPageErase(int page_addr){
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Wait for BSY bit to be cleared
    FLASH->CR |= FLASH_CR_PER; // Enable page erase
    FLASH->AR = page_addr; // Set page address
    FLASH->CR |= FLASH_CR_STRT; // Begin erase
    __NOP(); // One clock cycle delay
    while ((FLASH->SR & FLASH_SR_BSY) != 0);
    if(FLASH->SR & FLASH_SR_EOP){
        FLASH->SR = FLASH_SR_EOP; // Clear EOP flag (write 1)
    }
    FLASH->CR &= ~FLASH_CR_PER; // Set to read mode
}

void flashWrite(uint32_t flash_address, uint32_t flash_data){
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Wait for BSY bit to be cleared
    FLASH->CR |= FLASH_CR_PG; // Write enable
    // 1/2
    *(__IO uint16_t*)(flash_address) = (uint16_t)flash_data; // Write half-word on desired address
    __NOP(); // One clock cycle delay
    while((FLASH->SR & FLASH_SR_BSY) != 0);
    if(FLASH->SR & FLASH_SR_EOP){
        FLASH->SR = FLASH_SR_EOP; // Clear EOP flag (write 1)
    }
    // 2/2
    int temp = flash_address + 2;
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Wait for BSY bit to be cleared
    *(__IO uint16_t*)(temp) = (uint16_t)(flash_data >> 16); // Write half-word on desired address
    __NOP(); // One clock cycle delay
    while((FLASH->SR & FLASH_SR_BSY) != 0);
    while ((FLASH->SR & FLASH_SR_EOP) == 0); // Wait for the operation to finish
    if(FLASH->SR & FLASH_SR_EOP){
        FLASH->SR = FLASH_SR_EOP; // Clear EOP flag (write 1)
    }
    FLASH->CR &= ~FLASH_CR_PG; // Write disable
}

char flashWriteChanges(void){
    // Write data into flash but only if there were some changes
    // Trying to minimize flash degradation as much as possible (max ~1000 write cycles)
    int i, j;
    int changes = 0;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 3; j++){
            if(memory[i][j] != program_flash_memory[i][j]) changes++;
        }
    }
    if(changes == 0){
        return 0; // no changes
    }

    // write changes
    flashUnlock(); // unlock flash
    flashPageErase(0x08007C00); // delete whole page

    //const int *first_flash_address = &(program_flash_memory[0][0]);
    int first_flash_address = 0x08007C00;
    int *data = &(memory[0][0]); // create pointer to memory address

    for(i = 0; i < (sizeof(memory)/sizeof(memory[0][0])); i++, data++){
        flashWrite(first_flash_address, *data);
        first_flash_address += 4;
    }

    flashLock();
    return 1;
}

void flashCopyFromFlashToRAM(void){
    // copies values from Flash to RAM memory
    int i,j;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 3; j++){
            memory[i][j] = program_flash_memory[i][j];
        }
    }
}
