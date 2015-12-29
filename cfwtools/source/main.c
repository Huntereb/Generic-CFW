#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "MainMenu.h"
#include "crypto.h"
#include "fs.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "filepack.h"
#include "cfw.h"
#include "configuration.h"

int firmexist = 1;

void Initialize(){
	DrawString(TOP_SCREEN,  "", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	if(FSInit()){
		DrawString(TOP_SCREEN,  "", 0, SCREEN_HEIGHT-FONT_SIZE, WHITE, BLACK);
	}else{
		DrawString(TOP_SCREEN,  " ERROR!        ", 0, SCREEN_HEIGHT-FONT_SIZE, RED, BLACK);
	}
	LoadPack();
	//Console Stuff
	memset(TOP_SCREEN, 0x00, SCREEN_SIZE);
	memset(TOP_SCREEN2, 0x00, SCREEN_SIZE);
	ConsoleSetXY(15, 15);
	ConsoleSetWH(SCREEN_WIDTH-30, SCREEN_HEIGHT-80);
	ConsoleSetBorderColor(WHITE);
	ConsoleSetTextColor(WHITE);
	ConsoleSetBackgroundColor(BLACK);
	ConsoleSetSpecialColor(WHITE);
	ConsoleSetSpacing(2);
	ConsoleSetBorderWidth(3);
	//Check that the data is installed
	f_mkdir ("tools");
	f_mkdir ("tools/nand");
	f_mkdir ("tools/firm");
	//Check if firmware.bin exists, and if it doesn't, skip everything
	File FirmFile1;
	if (FileOpen(&FirmFile1, "tools/firm/firmware.bin", 0)){
		FileClose(&FirmFile1);
	}else{
		firmexist = 0;
		goto Tools_boot;
	}

	InstallConfigData();
	
	SplashScreen();
	for(int i = 0; i < 0x333333*2; i++){
		u32 pad = GetInput();
		if(pad & BUTTON_R1 && i > 0x333333) goto Tools_boot;
	}
	cfwModeQuickBoot();
	Tools_boot:
	memset(TOP_SCREEN, 0x00, SCREEN_SIZE);
	memset(TOP_SCREEN2, 0x00, SCREEN_SIZE);
}

int main(){
	Initialize();

	if (firmexist == 0){
		DrawString(TOP_SCREEN, "FIRMWARE.BIN NOT FOUND IN /TOOLS/FIRM!", 75, SCREEN_HEIGHT - 10, RED, BLACK);
	}

	u8 keyX[16] = { 0xCE, 0xE7, 0xD8, 0xAB, 0x30, 0xC0, 0x0D, 0xAE, 0x85, 0x0E, 0xF5, 0xE3, 0x82, 0xAC, 0x5A, 0xF3 };
	setup_aeskeyX(0x25, keyX);
		
	DrawString(TOP_SCREEN, " EmuNAND ", 0, SCREEN_HEIGHT-FONT_SIZE*2, checkEmuNAND() ? GREEN : RED, BLACK);

	//That's the Main Menu initialization, easy and cool
	MenuInit(&MainMenu);
	MenuShow();

	while (true) {
		DrawString(TOP_SCREEN,  "[START]  Shutdown", SCREEN_WIDTH-51-18*FONT_SIZE, SCREEN_HEIGHT-59-24-FONT_SIZE, RED, BLACK);
		DrawString(TOP_SCREEN,  "[SELECT] Reboot", SCREEN_WIDTH-51-18*FONT_SIZE, SCREEN_HEIGHT-59-24, RED, BLACK);
		u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) 	MenuNextSelection();
		if(pad_state & BUTTON_UP)   	MenuPrevSelection();
		if(pad_state & BUTTON_A)    	MenuSelect();
		if(pad_state & BUTTON_SELECT)	returnHomeMenu();
		if(pad_state & BUTTON_START)	ShutDown();
		MenuShow();
	}

	FSDeInit();
	return 0;
}
