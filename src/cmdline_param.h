#ifndef _CMDLINE_PARAM_H_
#define _CMDLINE_PARAM_H_

typedef enum {
	OPTION_NOT_SPECIFIED,
	OPTION_UPLOAD_FIRMWARE,
	OPTION_FETCH_VERSIONS,
	OPTION_CHECK_CREDENTIAL,
	OPTION_FETCH_DEV_ID,
	OPTION_SCAN_AP,   
}CmdlineOptions_t;    

typedef struct {
	uint8_t file_set;
	char *file_name;
	uint8_t reset_set;
	uint8_t verbose;
}CmdLineParams_t;        

extern CmdlineOptions_t cmdline_option;
extern CmdLineParams_t cmdline_params;

int ParseCmdlineParameters(int arg, char *argv[]);
void PrintHelpMessage(void);


#endif

