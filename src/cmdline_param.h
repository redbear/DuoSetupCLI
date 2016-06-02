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
 
#ifndef _CMDLINE_PARAM_H_
#define _CMDLINE_PARAM_H_

#define MAX_OTA_FILE_NUM    3 // Be capable of storing 3 images simultaneously in OTA region

typedef enum {
    OPTION_NOT_SPECIFIED,
    OPTION_UPLOAD_FIRMWARE,
    OPTION_FETCH_VERSIONS,
    OPTION_CHECK_CREDENTIAL,
    OPTION_FETCH_DEV_ID,
    OPTION_SCAN_AP,   
    OPTION_CONFIG_AP,
    OPTION_CONNECT_AP,
    OPTION_PUBLIC_KEY,
} CmdlineOptions_t;    

typedef struct {
    uint8_t file_num;
    char *file_name[MAX_OTA_FILE_NUM]; 
    uint8_t fac_set;
    char *fac_name;
    uint8_t safe;
    uint8_t leave;
    uint8_t verbose;
} CmdLineParams_t;        

extern CmdlineOptions_t cmdline_option;
extern CmdLineParams_t cmdline_params;

int ParseCmdlineParameters(int arg, char *argv[]);
void PrintHelpMessage(void);


#endif

