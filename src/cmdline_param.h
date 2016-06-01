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
}CmdlineOptions_t;    

typedef struct {
    uint8_t file_num;
    char *file_name[MAX_OTA_FILE_NUM]; 
    uint8_t fac_set;
    char *fac_name;
	uint8_t safe;
	uint8_t leave;
	uint8_t verbose;
}CmdLineParams_t;        

extern CmdlineOptions_t cmdline_option;
extern CmdLineParams_t cmdline_params;

int ParseCmdlineParameters(int arg, char *argv[]);
void PrintHelpMessage(void);


#endif

