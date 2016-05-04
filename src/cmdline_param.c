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
		if( !strcmp(argv[i], "--upload") ) {     // --upload
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_UPLOAD_FIRMWARE;
		}
		else if( !strcmp(argv[i], "--version") ) {    // --version
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_FETCH_VERSIONS;
		}
		else if( !strcmp(argv[i], "--device-id") ) {    // --device-id
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_FETCH_DEV_ID;
		}
		else if(!strcmp(argv[i], "--credential")) {    // --credential
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CHECK_CREDENTIAL;
		}
		else if(!strcmp(argv[i], "--scan-ap")) {    // --scan-ap
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_SCAN_AP;
		}
		else if(!strcmp(argv[i], "--config-ap")) {    // --config-ap
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CONFIG_AP;
		}
		else if(!strcmp(argv[i], "--connect-ap")) {    // --connect-ap
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CONNECT_AP;
		}
		else if(!strcmp(argv[i], "--public-key")) {    // --public-key
			if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_PUBLIC_KEY;
		}
		else if( (!strcmp(argv[i], "--verbose")) || (!strcmp(argv[i], "-v")) ) {    // --verbose
			cmdline_params.verbose = 1;
		}
		else if( (!strcmp(argv[i], "--file")) || (!strcmp(argv[i], "-f")) ) {    // --file
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
		else if( (!strcmp(argv[i], "--region")) || (!strcmp(argv[i], "-r")) ) {    // --region
			i++;
			if(i < arg) {
				if(argv[i][0]>='1' && argv[i][0]<='8') {
					cmdline_params.region = argv[i][0] - 48;
				}
				else {
					printf("\nERROR: The parameter specified by \"--region\" should be 1 ~ 7 or 8!\n\n");
					return -1;
				}
			}
			else {
				printf("\nERROR: The region is not specified by parameter \"--region\"!\n\n");
				PrintHelpMessage();
				return -1;
			}
		}
		else if( (!strcmp(argv[i], "--safe")) || (!strcmp(argv[i], "-s")) ) {    // --safe
			cmdline_params.safe = 1;
		}
		else if( (!strcmp(argv[i], "--leave")) || (!strcmp(argv[i], "-l")) ) {    // --leave
			cmdline_params.leave = 1;
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
	printf("    --upload          Upload firmware to Duo\n");
	printf("    --version         Fetch the firmware versions\n");
	printf("    --device-id       Fetch the MCU's unique 12-bytes device ID\n");
	printf("    --credential      Check if Duo has stored credentials or not\n");
	printf("    --scan-ap         Scan the nearby Wi-Fi Access Points\n");
	printf("    --config-ap       Config an AP for Duo to connect\n");
	printf("    --connect-ap      Leave listening mode and try connecting the stored AP\n");
	printf("                      It will enter listening mode again if no valid AP stored\n");
	printf("    --public-key      Fetch the device public key that stored in Duo\n\n");
	
	printf("parameters: \n");
	printf("    --verbose,-v      Print additional message during executing this programm.\n");
	printf("    --file,-f <file>  Used with --upload option. Specify the binary file\n");
	printf("                      (.bin) to be uploaded. The file will be stored from the\n");
	printf("                      beginning of the OTA region by default, if no \"--region\"\n");
	printf("                      is present.\n");
	printf("    --region,-r <n>   Used with --upload option. Specify the region of \n");
	printf("                      the external flash to store the file. Without this\n");
	printf("                      parameter, the file is stored from the beginning of\n");
	printf("                      the OTA region. Otherwise, if n=[1...7], the file\n");
	printf("                      is stored from the offset of the OTA region, where\n"); 
	printf("                      the offset is n*64KB. if n=8, the file is stored\n");
	printf("                      at the Factory Reset region\n");
	printf("    --safe,-s         Used with --upload option. Invalid user part so that\n");
	printf("                      Duo enter safe mode after firmware update.\n");
	printf("    --leave,-l        Used with --upload option. Leave listening mode when\n");
	printf("                      uploading firmware completed.\n\n");
}


