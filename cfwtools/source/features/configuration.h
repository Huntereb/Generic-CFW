#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define cfwmode_label " CFW "

enum {
	CONF_NOKEYX = -1,
	CONF_NOFIRMBIN = -2,
	CONF_ERRNFIRM = -3,
	CONF_CANTOPENFILE = -4,
};

int InstallData(char* drive);
void InstallConfigData();

#endif