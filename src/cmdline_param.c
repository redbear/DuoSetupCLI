/*
 * Copyright (c) 2016 RedBear
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
// Including files
#include "platform.h"
#include <stdio.h>
#include <string.h>
#if defined WINDOWS
#include <windows.h>
#endif
#include <stdlib.h>
#include "cmdline_param.h"

CmdlineOptions_t cmdline_option;
CmdLineParams_t cmdline_params;

// Functions achievement
int ParseCmdlineParameters(int arg, char *argv[]) {

    if (arg < 2) {
        printf("\nError: Invalid parameters!\n\n");
        PrintHelpMessage();
        return -1;
    }
    
    memset(&cmdline_params, 0x00, sizeof(CmdLineParams_t));
    
    for (uint8_t i = 1; i < arg; i++) {
        if ( !strcmp(argv[i], "--upload") ) {     // --upload
            if (cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_UPLOAD_FIRMWARE;
        }
        else if ( !strcmp(argv[i], "--version") ) {    // --version
            if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_FETCH_VERSIONS;
        }
        else if ( !strcmp(argv[i], "--device-id") ) {    // --device-id
            if (cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_FETCH_DEV_ID;
        }
        else if (!strcmp(argv[i], "--credential")) {    // --credential
            if (cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CHECK_CREDENTIAL;
        }
        else if (!strcmp(argv[i], "--scan-ap")) {    // --scan-ap
            if (cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_SCAN_AP;
        }
        else if(!strcmp(argv[i], "--config-ap")) {    // --config-ap
            if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CONFIG_AP;
        }
        else if (!strcmp(argv[i], "--connect-ap")) {    // --connect-ap
            if (cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_CONNECT_AP;
        }
        else if(!strcmp(argv[i], "--public-key")) {    // --public-key
            if(cmdline_option == OPTION_NOT_SPECIFIED) cmdline_option = OPTION_PUBLIC_KEY;
        }
        else if ( (!strcmp(argv[i], "--verbose")) || (!strcmp(argv[i], "-v")) ) {    // --verbose
            cmdline_params.verbose = 1;
        }
        else if ( (!strcmp(argv[i], "--file")) || (!strcmp(argv[i], "-f")) ) {    // --file
            i++;
            if (i < arg) {
                if (cmdline_params.file_num >= MAX_OTA_FILE_NUM) {
                    printf("\nERROR: Too many file to be uploaded while the OTA region\n");
                    printf("       is only capable of storing %d images simultaneously!\n\n", cmdline_params.file_num);
                    return -1;
                }
                char form[5] = ".bin";
                char format[5] = "    ";
                int fLen = strlen(argv[i]);
                if (fLen < 5) {
                    printf("\nERROR: The selcted file \"%s\" is invalid!\n\n", argv[i]);
                    return -1;  
                }
                format[3] = argv[i][fLen-1];
                format[2] = argv[i][fLen-2];
                format[1] = argv[i][fLen-3];
                format[0] = argv[i][fLen-4];
                if (0 != strcmp(form, format)) {
                    printf("\nERROR: Selected file \"%s\" format must be .bin!\n\n", argv[i]);
                    return -1;
                }
                
                cmdline_params.file_name[cmdline_params.file_num] = argv[i];
                cmdline_params.file_num++;
            }
            else {
                printf("\nERROR: File is not specified by parameter \"--file\"!\n\n");
                PrintHelpMessage();
                return -1;
            }
        }
        else if ( !strcmp(argv[i], "-fac") ) {    // -fac
            i++;
            if (i < arg) {
                char form[5] = ".bin";
                char format[5] = "    ";
                int fLen = strlen(argv[i]);
                if (fLen < 5) {
                    printf("\nERROR: The selcted file \"%s\" is invalid!\n\n", argv[i]);
                    return -1;  
                }
                format[3] = argv[i][fLen-1];
                format[2] = argv[i][fLen-2];
                format[1] = argv[i][fLen-3];
                format[0] = argv[i][fLen-4];    
                if (0 != strcmp(form, format)) {
                    printf("\nERROR: Selected file \"%s\" format must be .bin!\n\n", argv[i]);
                    return -1;
                }
                
                cmdline_params.fac_set = 1;
                cmdline_params.fac_name = argv[i];
            }
            else {
                printf("\nERROR: File is not specified by parameter \"-fac\"!\n\n");
                PrintHelpMessage();
                return -1;
            }
        }
        else if ( (!strcmp(argv[i], "--safe")) || (!strcmp(argv[i], "-s")) ) {    // --safe
            cmdline_params.safe = 1;
        }
        else if ( (!strcmp(argv[i], "--leave")) || (!strcmp(argv[i], "-l")) ) {    // --leave
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

void PrintHelpMessage(void) {
    printf("\nUsage   : %s <option> [parameters]\n\n", "DuoSetupCLI");
    
    printf("options: \n");
    printf("    --upload          Upload firmware to Duo. At least one of the --file and -fac\n");
    printf("                      parameters must be presented. It's OK if both are presented.\n");
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
    printf("                      (.bin) to be uploaded. The first file specified by --file,\n");
    printf("                      will be stored from the beginning of the OTA region. If\n");
    printf("                      several --file are presented, the rest files will be stored\n");
    printf("                      in the OTA region successively\n");
    printf("    -fac <file>       Used with --upload option. Specify the binary file\n");
    printf("                      (.bin) to be uploaded. The file will be stored from the\n");
    printf("                      beginning of the FAC region\n");
    printf("    --safe,-s         Used with --upload option. Invalid user part so that\n");
    printf("                      Duo enter safe mode after firmware update.\n");
    printf("    --leave,-l        Used with --upload option. Leave listening mode when\n");
    printf("                      uploading firmware completed.\n\n");
}


