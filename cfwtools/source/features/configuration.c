#include "configuration.h"
#include "common.h"
#include "filepack.h"
#include "fs.h"
#include "ff.h"
#include "console.h"
#include "draw.h"
#include "hid.h"
#include "ncch.h"
#include "crypto.h"
#include "aes.h"
#include "cfw.h"
#include "downgradeapp.h"
#include "stdio.h"

#define DATAFOLDER "tools/data"
#define VERSTRING "tools/firm/ver.txt"
#define WORKBUF (u8*)0x21000000
#define NAT_SIZE 0xEBC00
#define AGB_SIZE 0xD9C00
#define TWL_SIZE 0x1A1C00

char tmpstr[256] = {0};
File tempfile;
UINT tmpu32;
int verexist = 1;
u8 fvers[5];
u8 fvernew[1];

int InstallData(char* drive){
	FIL firmfile;
	
	//Create the workdir
	sprintf(tmpstr, "%s:%s", drive, DATAFOLDER);
	f_mkdir(tmpstr);
	f_chmod(tmpstr, AM_HID, AM_HID);
	
	//Read firmware data
	if(f_open(&firmfile, "tools/firm/firmware.bin", FA_READ | FA_OPEN_EXISTING) == FR_OK){
		//... We'll see
	}else return CONF_NOFIRMBIN;
	DrawString(TOP_SCREEN, "O", 75, SCREEN_HEIGHT - 11, WHITE, BLACK);
	
	//Create patched native_firm
	f_read(&firmfile, WORKBUF, NAT_SIZE, &tmpu32);
	u8* n_firm = decryptFirmTitle(WORKBUF, NAT_SIZE, 0x00000002);
	u8* n_firm_patch = GetFilePack("nat_patch.bin");
	applyPatch(n_firm, n_firm_patch);
	u8 keyx[16] = { 0xCE, 0xE7, 0xD8, 0xAB, 0x30, 0xC0, 0x0D, 0xAE, 0x85, 0x0E, 0xF5, 0xE3, 0x82, 0xAC, 0x5A, 0xF3 };
	DrawString(TOP_SCREEN, "O", 83, SCREEN_HEIGHT - 11, WHITE, BLACK);
	for(int i = 0; i < NAT_SIZE; i+=0x4){
		if(!strcmp((char*)n_firm + i, "CuCkD")){
			if (verexist == 0 || fvernew[0] == 0x30){
				if (1){
					memcpy((char*)n_firm + i, cfwmode_label, 5);
				}
			}else{
				if (1){
					memcpy((char*)n_firm + i, fvers, 5);			//ver.txt stuff
				}
			}
		}
		if(!strcmp((char*)n_firm + i, "InsertKeyXHere!") && keyx[0] != 0){
			memcpy(n_firm + i, keyx, 16);
		}
		if(*((unsigned int*)(n_firm + i)) == 0xAAAABBBB){
			*((unsigned int*)(n_firm + i)) = (checkEmuNAND() / 0x200) - 1;
		}
	}
	DrawString(TOP_SCREEN, "O", 91, SCREEN_HEIGHT - 11, WHITE, BLACK);
	sprintf(tmpstr, "%s:%s/0004013800000002.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, n_firm, NAT_SIZE, 0);
		FileClose(&tempfile);
		//FileCopy("0004013800000002.bin", tmpstr);
	}else return CONF_ERRNFIRM;
	DrawString(TOP_SCREEN, "O", 99, SCREEN_HEIGHT - 11, WHITE, BLACK);
	
	//Create AGB patched firmware
	f_read(&firmfile, WORKBUF, AGB_SIZE, &tmpu32);
	u8* a_firm = decryptFirmTitle(WORKBUF, AGB_SIZE, 0x00000202);
	u8* a_firm_patch = GetFilePack("agb_patch.bin");
	if(a_firm){
		applyPatch(a_firm, a_firm_patch);
		sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
		if(FileOpen(&tempfile, tmpstr, 1)){
			FileWrite(&tempfile, a_firm, AGB_SIZE, 0);
			FileClose(&tempfile);
		}else return CONF_ERRNFIRM;
	DrawString(TOP_SCREEN, "O", 107, SCREEN_HEIGHT - 11, WHITE, BLACK);
	}else{	
		//If we cannot decrypt it from firmware.bin becouse of titlekey messed up, it probably means that AGB has been modified in some way.
		//So we read it from his installed ncch...
		FindApp(0x00040138, 0x00000202, 1);
		char* path = getContentAppPath();
		FileOpen(&tempfile, path, 0);
		FileRead(&tempfile, WORKBUF, AGB_SIZE, 0);
		FileClose(&tempfile);
		a_firm = decryptFirmTitleNcch(WORKBUF, AGB_SIZE);
		if(a_firm){
			applyPatch(a_firm, a_firm_patch);
			sprintf(tmpstr, "%s:%s/0004013800000202.bin", drive, DATAFOLDER);
			if(FileOpen(&tempfile, tmpstr, 1)){
				FileWrite(&tempfile, a_firm, AGB_SIZE, 0);
				FileClose(&tempfile);
			}else return CONF_ERRNFIRM;
				DrawString(TOP_SCREEN, "O", 115, SCREEN_HEIGHT - 11, WHITE, BLACK);
		}else{
				DrawString(TOP_SCREEN, "X", 115, SCREEN_HEIGHT - 11, RED, BLACK); //If we get here, then we'll play without AGB, lol
		}
	}
	
	//Create TWL patched firmware
	f_read(&firmfile, WORKBUF, TWL_SIZE, &tmpu32);
	u8* t_firm = decryptFirmTitle(WORKBUF, TWL_SIZE, 0x00000102);
	u8* t_firm_patch = GetFilePack("twl_patch.bin");
	if(t_firm){
		applyPatch(t_firm, t_firm_patch);
		sprintf(tmpstr, "%s:%s/0004013800000102.bin", drive, DATAFOLDER);
		if(FileOpen(&tempfile, tmpstr, 1)){
			FileWrite(&tempfile, t_firm, TWL_SIZE, 0);
			FileClose(&tempfile);
			//FileCopy("0004013800000102.bin", tmpstr);
		}else return CONF_ERRNFIRM;
			DrawString(TOP_SCREEN, "O", 123, SCREEN_HEIGHT - 11, WHITE, BLACK);
	}else{
			DrawString(TOP_SCREEN, "X", 123, SCREEN_HEIGHT - 11, RED, BLACK);
	}
	
	sprintf(tmpstr, "%s:%s/data.bin", drive, DATAFOLDER);
	if(FileOpen(&tempfile, tmpstr, 1)){
		FileWrite(&tempfile, __DATE__, 12, 0);
		FileWrite(&tempfile, __TIME__, 9, 12);
		FileClose(&tempfile);
	}else return CONF_CANTOPENFILE;
	DrawString(TOP_SCREEN, "O", 131, SCREEN_HEIGHT - 11, WHITE, BLACK);
	
	f_close(&firmfile);
	return 0;
}

int CheckInstallationData(){
	File file;
	char str[32];
	if(!FileOpen(&file, "tools/data/0004013800000002.bin", 0)) return -1;
	FileClose(&file);
	if(!FileOpen(&file, "tools/data/0004013800000202.bin", 0)) return -2;
	FileClose(&file);
	if(!FileOpen(&file, "tools/data/0004013800000102.bin", 0)) return -3;
	FileClose(&file);
	if(!FileOpen(&file, "tools/data/data.bin", 0)) return -4;
	FileRead(&file, str, 32, 0);
	FileClose(&file);
	if(memcmp(str, __DATE__, 11)) return -5;
	if(memcmp(&str[12], __TIME__, 8)) return -5;
	return 0;
}

void InstallConfigData(){

	File verfile2;

	//Read fver data
	if (FileOpen(&verfile2, VERSTRING, 0)){
		FileRead(&verfile2, &fvernew[0], 1, 0x0);
		FileRead(&verfile2, &fvers[0], 5, 0x3);
		FileClose(&verfile2);
	}else{
		verexist = 0;
	}

	//If it exists with a 1, write a 0 to the top of it and apply
	if (verexist != 0 && fvernew[0] == 0x31){
		u8 fverold[1] = { 0x30 };
		FileOpen(&verfile2, VERSTRING, 0);
		FileWrite(&verfile2, &fverold[0], 1, 0);
		FileClose(&verfile2);
		goto skipcheck;
	}

	//checks for ver.txt if it's gone, reapplies default file
	if (verexist == 0){
		u8 vertext[8] = { 0x30, 0x0D, 0x0A, 0x41, 0x42, 0x43, 0x44, 0x45, };

		FileOpen(&verfile2, VERSTRING, 1);
		FileWrite(&verfile2, &vertext, 15, 0);
		FileClose(&verfile2);

		goto skipcheck;
	}

	if(CheckInstallationData() == 0) return;

	skipcheck:

	InitScreen();
	DrawString(TOP_SCREEN, "[", 67, SCREEN_HEIGHT - 11, WHITE, BLACK);
	DrawString(TOP_SCREEN, "]", 139, SCREEN_HEIGHT - 11, WHITE, BLACK);
	int res = InstallData("0");	//SD Card
	if (!res == 0){
		DrawString(TOP_SCREEN, "FAIL! ERR %d\n", 75, SCREEN_HEIGHT - 11, RED, WHITE);
	}
}
