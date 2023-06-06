#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

void menuLoadDataFromMemoryToWorkingRAM(int program);
void menuSaveDataFromWorkingRAMToMemory(int program);
void moveCursorLR();
void moveCursorUD();
void menuShow();
void menuShowStatus(char status);
void menuShowFrequency(int new_frequency);

#endif /* MENU_H_INCLUDED */
