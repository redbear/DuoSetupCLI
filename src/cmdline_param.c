// Including files
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include "cmdline_param.h"

CmdlineOptions_t cmdline_option;
CmdLineParams_t cmdline_params;

// Functions achievement
int ParseCmdlineParameters(int arg, char *argv[]){

	if(arg < 2){
		printf("\nError: Invalid parameters!\n\n");
		PrintHelpMessage();
		return -1;
	}
	
	memset(&cmdline_params, 0x00, sizeof(CmdLineParams_t));
	
	for(uint8_t i=1; i<arg; i++)
	{
		if(!strcmp(argv[i], "--upload")) {     // --upload
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_UPLOAD_FIRMWARE;
		}
		else if(!strcmp(argv[i], "--version")) {    // --version
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_FETCH_VERSIONS;
		}
		else if(!strcmp(argv[i], "--device-id")) {    // --device-id
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_FETCH_DEV_ID;
		}
		else if(!strcmp(argv[i], "--credential")) {    // --credential
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CHECK_CREDENTIAL;
		}
		else if(!strcmp(argv[i], "--scan-ap")) {    // --scan-ap
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_SCAN_AP;
		}
		else if(!strcmp(argv[i], "--verbose")) {    // --verbose
			cmdline_params.verbose = 1;
		}
		else if(!strcmp(argv[i], "--file")) {    // --file
			i++;
			if(i < arg) {
				char form[5] = ".bin";
				char format[5] = "    ";
				int fLen = strlen(argv[i]);
				if(fLen < 5){
					printf("\nERROR: The selcted file \"%s\" is invalid!\n\n", argv[i]);
					return -1;  
				}
				format[3] = argv[i][fLen-1];
				format[2] = argv[i][fLen-2];
				format[1] = argv[i][fLen-3];
				format[0] = argv[i][fLen-4];	
				if(0 != strcmp(form, format)){
					printf("\nERROR: Selected file \"%s\" format must be .bin!\n\n", argv[i]);
					return -1;
				}
				
				cmdline_params.file_set = 1;
				cmdline_params.file_name = argv[i];
			}
			else {
				printf("\nERROR: File is not specified by parameter \"--file\"!\n\n");
				PrintHelpMessage();
				return -1;
			}
		}
		else if(!strcmp(argv[i], "--reset")) {    // --reset
			cmdline_params.reset_set = 1;
		}
		else {
			printf("\nERROR: Unknown option or parameter \"%s\" .\n", argv[i]);
			PrintHelpMessage();
			return -1;
		}
	}
	
	return 0;
}

void PrintHelpMessage(void){
	printf("\nUsage   : %s <option> [parameters]\n\n", "DuoSetupCLI");
	
	printf("options: \n");
	printf("    --upload       Upload firmware to Duo\n");
	printf("    --version      Fetch the firmware versions\n");
	printf("    --device-id    Fetch the MCU's unique 12-bytes device ID\n");
	printf("    --credential   Check if Duo has stored credentials or not\n");
	printf("    --scan-ap      Scan the nearby Wi-Fi Access Points for Duo to connect\n\n");
	
	printf("parameters: \n");
	printf("    --verbose      Print additional message during executing programm.\n");
	printf("    --file <file>  Used with --upload option. Specify the binary file (.bin) to be uploaded.\n");
	printf("    --reset        Used with --upload option. Make Duo perform reset when uploading firmware complete.\n\n");
}


