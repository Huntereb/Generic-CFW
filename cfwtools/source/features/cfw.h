#ifndef CFW_H
#define CFW_H

#include "common.h"

void DevMode();
void cfwModeSys();
void cfwModeEmu();
void cfwModeQuickBoot();
u8* decryptFirmTitleNcch(u8* title, unsigned int size);
u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid, int drive);
void applyPatch(unsigned char* file, unsigned char* patch);

#endif
