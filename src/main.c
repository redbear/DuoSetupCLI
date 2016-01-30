// Including files
#include "platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json_cmd.h"
#include "upload.h"
#include "cmdline_param.h"


// Functions declaration
static int UploadFirmware(char *file_name, uint8_t reset);
static int FetchFirmwareVersion(void);
static int FetchDeviceID(void);
static int CheckCredential(void);
static int ScanNetworks(void);

// Functions achievement
int main(int arg, char *argv[]){
	int result = -1;

	printf("Copyright (c) 2016 redbear.cc\n");
	printf("RedBear Duo Setup CLI version 1.0.0 ");
	
#if defined WINDOWS
	printf("for Windows\n");
#elif defined LINUX
	printf("for Linux\n");
#elif defined MACOSX
	printf("for Mac OS X\n");
#endif

	if(ParseCmdlineParameters(arg, argv) < 0)
		return -1;
	
	switch(cmdline_option) {
		case OPTION_UPLOAD_FIRMWARE:
			if(cmdline_params.file_set) {
				printf("Upload firmware to Duo.\n");
				result = UploadFirmware(cmdline_params.file_name, cmdline_params.reset_set);	
			}
			else {
				printf("\nERROR: The file to be uploaded is not specified yet!\n");
				PrintHelpMessage();
				result = -1;
			}
			break;
			
		case OPTION_FETCH_VERSIONS:
			printf("Fetch firmware versions.\n");
			result = FetchFirmwareVersion();
			break;
			
		case OPTION_FETCH_DEV_ID:
			printf("Fetch device ID.\n");
			result = FetchDeviceID();
			break;
			
		case OPTION_CHECK_CREDENTIAL:
			printf("Check credential.\n");
			result = CheckCredential();
			break;
			
		case OPTION_SCAN_AP:
			printf("Scan Access Points.\n");
			result = ScanNetworks();
			break;
			
		default:
			printf("\nERROR: No option is selected!\n");
			PrintHelpMessage();
			result = -1;
			break;
	}
	
	return result;
}

static int UploadFirmware(char *file_name, uint8_t reset) {
	char jsonString[256];
	char respond[64];
	uint8_t chunk_addr = 0;
	uint8_t chunk_size = 128;
	uint8_t fileData[MAX_FILE_LENGTH];
	uint32_t fileLength = 0;
	
	if(PrepareUpload(file_name, fileData, &fileLength) < 0)
		return -1;
	
	AssembleOtaCmdString(jsonString, fileLength, chunk_addr, chunk_size);
	uint8_t i;
	for(i=0; i<3; i++) {
		if(i > 0) printf("\nRetrying...\n");
		if(SendJSONCmd(jsonString, respond, sizeof(respond)) < 0) continue;
		if(cmdline_params.verbose) printf("\nJSON request command is sent successfully.\n");
		if( OTAUploadFirmware(fileData, fileLength, chunk_size) < 0 ) continue;
		break;
	}
	if(reset && i<3) {
		printf("\nReset to restart...\n");
		AssembleRstCmdString(jsonString);
		SendJSONCmd(jsonString, respond, sizeof(respond));
	}
	
	return 0;
}

static int FetchFirmwareVersion(void) {
	char jsonString[256];
	char respond[128];
	
	AssembleVerCmdString(jsonString);
	SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(!cmdline_params.verbose) {
		printf("%s\n", respond);
	}
	
	return 0;
}

static int FetchDeviceID(void) {
	char jsonString[256];
	char respond[128];
	
	AssembleDevidCmdString(jsonString);
	SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(!cmdline_params.verbose) {
		printf("%s\n", respond);
	}
	
	return 0;
}

static int CheckCredential(void) {
	char jsonString[256];
	char respond[64];
	
	AssembleChkCredentCmdString(jsonString);
	SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(!cmdline_params.verbose) {
		printf("%s\n", respond);
	}
	
	return 0;
}

static int ScanNetworks(void) {
	char jsonString[256];
	char respond[10240];
	
	AssembleScanApCmdString(jsonString);
	SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(!cmdline_params.verbose) {
		printf("%s\n", respond);
	}
	
	return 0;
}

