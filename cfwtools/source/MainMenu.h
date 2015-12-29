#ifndef MY_MENU
#define MY_MENU

#include "hid.h"
#include "console.h"
#include "menu.h"
#include "fs.h"
#include "CTRDecryptor.h"
#include "NandDumper.h"
#include "TitleKeyDecrypt.h"
#include "padgen.h"
#include "nandtools.h"
#include "downgradeapp.h"
#include "cfw.h"
#include "i2c.h"
#include "configuration.h"

static void returnHomeMenu(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (unsigned char)(1<<2));
}

static void ShutDown(){
	i2cWriteRegister(I2C_DEV_MCU, 0x20, (u8)(1<<0));
	while(1);
}

static Menu DecryptMenu = {
	"Decryption Options",
	.Option = (MenuEntry[6]){
		{" Decrypt CTR Titles", &CTRDecryptor},
		{" Decrypt Title Keys", &DecryptTitleKeys},
		{" Decrypt encTitleKeys.bin", &DecryptTitleKeyFile},
		{" Generate Xorpads", &PadGen},
		{" Decrypt partitions", &DumpNandPartitions},
		{" Generate fat16 Xorpad", &GenerateNandXorpads},
	},
	6,
	0,
	0
};

static Menu DumpMenu = {
	"Dumping Options",
	.Option = (MenuEntry[3]){
		{" Create NAND dump", &NandDumper},
		{" Dump System Titles", &DumpNANDSystemTitles},
		{" Dump NAND Files", &dumpCoolFiles},
	},
	3,
	0,
	0
};

static Menu InjectMenu = {
	"Injection Options",
	.Option = (MenuEntry[2]){
		{" Inject EmuNAND partitions", &RebuildNand},
		{" Inject NAND Files", &restoreCoolFiles},
	},
	2,
	0,
	0
};

static Menu ExploitMenu = {
	"Other Options",
	.Option = (MenuEntry[4]){
		{" Downgrade MSET on SysNAND", &downgradeMSET},
		{" Install FBI over Health&Safety App", &installFBI},
		{" Restore original Health&Safety App", &restoreHS},
		{" DevMode (Simple Sysnand CFW)", &DevMode},
	},
	4,
	0,
	0
};

void DecryptMenuInit(){
	MenuInit(&DecryptMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		MenuShow();
    }
}

void DumpMenuInit(){
	MenuInit(&DumpMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		MenuShow();
    }
}

void InjectMenuInit(){
	MenuInit(&InjectMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		MenuShow();
    }
}

void ExploitMenuInit(){
	MenuInit(&ExploitMenu);
	MenuShow();
    while (true) {
        u32 pad_state = InputWait();
		if(pad_state & BUTTON_DOWN) MenuNextSelection();
		if(pad_state & BUTTON_UP)   MenuPrevSelection();
		if(pad_state & BUTTON_A)    MenuSelect();
		if(pad_state & BUTTON_B) 	break;
		MenuShow();
    }
}

static void Credits(){
	ConsoleInit();
	ConsoleSetTitle("Credits");
	print("Roxas75 - Original Developer\n");
	print("3dbrew - 3DS Documentation\n");
	print("GbaTemp users - Literally who\n");
	print("Brickway Team - Researchers with money\n");
	print("Archshift - Author of useful libs\n");
	print("Huntereb - Absolutely nothing\n");
	print("Rei - She muh nigguh\n");
	print("\nPress B to exit\n"); ConsoleShow();
	WaitForButton(BUTTON_B);
}

static Menu MainMenu = {
		"Generic CFW [BETA v1.0.3]",
		.Option = (MenuEntry[7]){
			{" Launch CFW on EmuNAND", &cfwModeEmu},
			{" Launch CFW on SysNAND", &cfwModeSys},
			{" Decryption Options", &DecryptMenuInit},
			{" Dumping Options", &DumpMenuInit},
			{" Injection Options", &InjectMenuInit},
			{" Other Options", &ExploitMenuInit},
			{" Credits", &Credits},
		},
		7,
		0,
		0
	};

#endif
