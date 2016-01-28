// Compile: g++ main.c -lws2_32 -static -o DuoSetup.exe

#ifdef __cplusplus
extern "C" {
#endif

// Including files
#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>

#pragma  comment(lib,"ws2_32.lib")

// Macro definitions
#define DUO_SERVER_IP_ADDRESS         "192.168.0.1"
#define DUO_SERVER_OTA_PORT           50007
#define DUO_SERVER_CMD_PORT           5609

#define MAX_JSON_CMD_LENGTH           256
#define OTA_CHUNK_SIZE                128
#define OTA_CHUNK_ADDR                0
#define MAX_FILE_LENGTH               (512*1024)
#define MAX_CHUNK_SIZE                1024

#define APP_START_MASK                ((uint32_t)0x2FF10000)
#define INTERNAL_FLASH_START_ADDR     0x08000000
#define INTERNAL_FLASH_END_ADDR       0x08100000

#define PROGRESS_BAR_WIDTH            25

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

// Type definitions
typedef struct module_dependency_t {
    uint8_t module_function;        // module function, lowest 4 bits
    uint8_t module_index;           // moudle index, lowest 4 bits.
    uint16_t module_version;        // version/release number of the module.
} module_dependency_t;

typedef struct module_info_t {
    uint32_t module_start_address;   /* the first byte of this module in flash */
    uint32_t module_end_address;     /* the last byte (exclusive) of this smodule in flash. 4 byte crc starts here. */
    uint8_t reserved;
    uint8_t reserved2;
    uint16_t module_version;            /* 16 bit version */
    uint16_t platform_id;               /* The platform this module was compiled for. */
    uint8_t  module_function;           /* The module function */
    uint8_t  module_index;
    module_dependency_t dependency;
    uint32_t reserved3;
} module_info_t;

// Variables
const char *module_func[7] = {
	"MOD_FUNC_NONE",
	"MOD_FUNC_RESOURCE",
	"MOD_FUNC_BOOTLOADER",
	"MOD_FUNC_MONO_FIRMWARE",
	"MOD_FUNC_SYSTEM_PART",
	"MOD_FUNC_USER_PART",
	"MOD_FUNC_SETTINGS"
};

CmdlineOptions_t cmdline_option;
CmdLineParams_t cmdline_params;

// Functions declaration
static int UploadFirmware(char *file_name, uint8_t reset);
static int FetchFirmwareVersion(void);
static int FetchDeviceID(void);
static int CheckCredential(void);
static int ScanNetworks(void);

static void AssembleOtaCmdString(char *js, uint32_t file_len, uint32_t chunk_address, uint16_t chunk_size);
static void AssembleRstCmdString(char *js);
static void AssembleVerCmdString(char *js);
static void AssembleDevidCmdString(char *js);
static void AssembleChkCredentCmdString(char *js);
static void AssembleScanApCmdString(char *js);
static int SendJSONCmd(char *js, char *rsp, uint16_t rsp_len);
static int  OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize);
static int  ConnectToClient(SOCKET *sclient, const char *ipAddr, uint16_t port);
static int  CheckFileValidity(uint8_t *firmware);
static void PrintModuleInfo(module_info_t *module_info);
static void __itoa_(uint32_t num, char *str);
static void ProgressBar(const char *desc, unsigned long long curr, unsigned long long max);
static void PrintHelpMessage(void);

// Functions achievement
static void PrintHelpMessage(void){
	printf("\nUsage   : %s <option> [parameters]\n\n", "DuoSetup");
	
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

int main(int arg, char *argv[]){
	int result = -1;

	/* 
	 * Checks the input arguments 
	 */
	printf("Copyright (c) 2016 redbear.cc\n");
	printf("RedBear Duo Setup CLI version 1.0.0\n");
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
					return -2;  
				}
				format[3] = argv[i][fLen-1];
				format[2] = argv[i][fLen-2];
				format[1] = argv[i][fLen-3];
				format[0] = argv[i][fLen-4];	
				if(0 != strcmp(form, format)){
					printf("\nERROR: Selected file \"%s\" format must be .bin!\n\n", argv[i]);
					return -3;
				}
				
				cmdline_params.file_set = 1;
				cmdline_params.file_name = argv[i];
			}
			else {
				printf("\nERROR: File is not specified by parameter \"--file\"!\n\n");
				PrintHelpMessage();
				return -4;
			}
		}
		else if(!strcmp(argv[i], "--reset")) {    // --reset
			cmdline_params.reset_set = 1;
		}
		else {
			printf("\nERROR: Unknown option or parameter \"%s\" .\n", argv[i]);
			PrintHelpMessage();
			return -4;
		}
	}
	
	switch(cmdline_option) {
		case OPTION_UPLOAD_FIRMWARE:
			if(cmdline_params.file_set) {
				printf("Upload firmware to Duo.\n");
				result = UploadFirmware(cmdline_params.file_name, cmdline_params.reset_set);	
			}
			else {
				printf("\nERROR: The file to be uploaded is not specified yet!\n");
				PrintHelpMessage();
				result = -5;
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
			result = -6;
			break;
	}
	
	return result;
}

static int UploadFirmware(char *file_name, uint8_t reset) {
	FILE *pFile = NULL;
	uint8_t fileData[MAX_FILE_LENGTH];
	uint32_t fileLength = 0;
	char jsonString[256];
	char respond[64];
	
	pFile = fopen(file_name, "rb");      // read only
	if(NULL == pFile){
		printf("\nERROR: File \"%s\" doesn't exist or is being occupied!\n", file_name);
        return -4;
    }
	printf("Open file \"%s\"\n", file_name);
	fseek(pFile,0,SEEK_SET);
    fseek(pFile,0,SEEK_END);
    fileLength = ftell(pFile);
    fseek(pFile,0,SEEK_SET);   
	if(fileLength > MAX_FILE_LENGTH){
		printf("\nERROR: Selected file is too large!\n");
		return -5;
	}
	if(1 != fread(fileData, fileLength, 1, pFile)){
		printf("\nERROR: Read file failed!\n");
		fclose(pFile);
        return -6;
	}
	fclose(pFile);
	
	printf("Check file validity:\n");
	if( CheckFileValidity(fileData) < 0 ){
		return -7;
	}
	printf("File length : %d\n", fileLength);
	printf("Chunk size  : %d\n", (uint16_t)OTA_CHUNK_SIZE);
	
	AssembleOtaCmdString(jsonString, fileLength, OTA_CHUNK_ADDR, OTA_CHUNK_SIZE);
	uint8_t i;
	for(i=0; i<3; i++) {
		if(i > 0) printf("\nRetrying...\n");
		if(SendJSONCmd(jsonString, respond, sizeof(respond)) < 0) continue;
		if(cmdline_params.verbose) printf("\nJSON request command is sent successfully.\n");
		if( OTAUploadFirmware(fileData, fileLength, OTA_CHUNK_SIZE) < 0 ) continue;
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
	char respond[64];
	
	AssembleVerCmdString(jsonString);
	SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(!cmdline_params.verbose) {
		printf("%s\n", respond);
	}
	
	return 0;
}

static int FetchDeviceID(void) {
	char jsonString[256];
	char respond[64];
	
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

static void AssembleOtaCmdString(char *js, uint32_t file_len, uint32_t chunk_address, uint16_t chunk_size){
	char temp_str[10];
	char json_param[MAX_JSON_CMD_LENGTH] = {'\0'};
	uint16_t json_param_len = 0;
	
	strcpy(json_param, "{\"file_length\":");
	__itoa_(file_len, temp_str);
	strcat(json_param, temp_str);
	
	strcat(json_param, ",\"chunk_address\":");
	__itoa_(chunk_address, temp_str);
	strcat(json_param, temp_str);
	
	strcat(json_param, ",\"chunk_size\":");
	__itoa_(chunk_size, temp_str);
	strcat(json_param, temp_str);
	
	strcat(json_param, "}");
	
	memset(js, '\0', MAX_JSON_CMD_LENGTH);
	strcpy(js, "prepare-update\n");
	
	json_param_len = strlen(json_param);
	__itoa_(json_param_len, temp_str);
	strcat(js, temp_str);
	
	strcat(js, "\n\n");
	strcat(js, json_param);
}

static void AssembleRstCmdString(char *js){
	char temp_str[10];
	uint16_t json_param_len = 0;

	memset(js, '\0', MAX_JSON_CMD_LENGTH);
	strcpy(js, "finish-update\n");
	
	json_param_len = 0;
	__itoa_(json_param_len, temp_str);
	strcat(js, temp_str);
	
	strcat(js, "\n\n");
}

static void AssembleVerCmdString(char *js)
{
	char temp_str[10];
	uint16_t json_param_len = 0;

	memset(js, '\0', MAX_JSON_CMD_LENGTH);
	strcpy(js, "version\n");
	
	json_param_len = 0;
	__itoa_(json_param_len, temp_str);
	strcat(js, temp_str);
	
	strcat(js, "\n\n");
}

static void AssembleDevidCmdString(char *js)
{
	char temp_str[10];
	uint16_t json_param_len = 0;

	memset(js, '\0', MAX_JSON_CMD_LENGTH);
	strcpy(js, "device-id\n");
	
	json_param_len = 0;
	__itoa_(json_param_len, temp_str);
	strcat(js, temp_str);
	
	strcat(js, "\n\n");
}

static void AssembleChkCredentCmdString(char *js)
{
	char temp_str[10];
	uint16_t json_param_len = 0;

	memset(js, '\0', MAX_JSON_CMD_LENGTH);
	strcpy(js, "check-credential\n");
	
	json_param_len = 0;
	__itoa_(json_param_len, temp_str);
	strcat(js, temp_str);
	
	strcat(js, "\n\n");
}

static void AssembleScanApCmdString(char *js)
{
	char temp_str[10];
	uint16_t json_param_len = 0;

	memset(js, '\0', MAX_JSON_CMD_LENGTH);
	strcpy(js, "scan-ap\n");
	
	json_param_len = 0;
	__itoa_(json_param_len, temp_str);
	strcat(js, temp_str);
	
	strcat(js, "\n\n");
}

static int SendJSONCmd(char *js, char *rsp, uint16_t rsp_len){
	int timeout = 5000; // In milliseconds
	
	SOCKET sclient;
	if( ConnectToClient(&sclient, DUO_SERVER_IP_ADDRESS, DUO_SERVER_CMD_PORT) < 0 ){
		return -4;
	}
	
	setsockopt(sclient,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
	setsockopt(sclient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
	
	// Send request
	if(cmdline_params.verbose) printf("\nSend json request command:\n\n%s\n\n", js);
	
	if(send(sclient, js, strlen(js), 0) == SOCKET_ERROR)
	{
		printf("\nERROR: Sent request command failed!\n");
		closesocket(sclient);
        WSACleanup();
		return -5;
	}

	// Receive respond
	int ret = recv(sclient, rsp, rsp_len, 0);
	rsp[ret] = '\0';
	if(ret <= 0)
	{
		printf("\nERROR: Receive respond timeout!\n");
		closesocket(sclient);
		WSACleanup();
		return -6;
	}
	if(cmdline_params.verbose) printf("\nReceived: %s\n", rsp);
	
	closesocket(sclient);
    WSACleanup();
	
    return 0;
}

static int OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize){
	uint32_t sentBytes = 0;
	uint8_t  chunk[MAX_CHUNK_SIZE];
	uint16_t chunkLen = 0; 
	const char  *respond[3] = {"chunk saved", "file saved", "not init"};
	int timeout = 5000; // In milliseconds
	
	SOCKET sclient;
	if( ConnectToClient(&sclient, DUO_SERVER_IP_ADDRESS, DUO_SERVER_OTA_PORT) < 0 ){
		return -1;
	}
	
	setsockopt(sclient,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
	setsockopt(sclient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

	ProgressBar("Upload", 0, 1);
	
	while(sentBytes < fileLen){
		if((fileLen-sentBytes) >= chunkSize){
			chunkLen = chunkSize;
		}
		else{
			chunkLen = fileLen-sentBytes;
		}
		
		memcpy(chunk, &firmware[sentBytes], chunkLen);
		if(send(sclient, (const char *)chunk, chunkLen, 0) == SOCKET_ERROR)
		{
			printf("\nERROR: Sent chunk data failed!\n");
			closesocket(sclient);
			WSACleanup();
			return -2;
		}
		
		char recData[255];
		int ret = recv(sclient, recData, 255, 0);
		if(ret < 0)
		{
			printf("\nERROR: Receive respond timeout!\n");
			closesocket(sclient);
			WSACleanup();
			return -3;
		}
		
		recData[ret] = '\0';
		if( !strcmp((const char *)recData, respond[2]) ) // OTA server not init
		{
			printf("\nERROR: OTA server not init!\n");
			closesocket(sclient);
			WSACleanup();
			return -4;
		}
        else if( !strcmp((const char *)recData, respond[1]) ) // File saved
		{
			sentBytes += chunkLen;
			ProgressBar("Upload", sentBytes, fileLen);
			if(sentBytes == fileLen)
				printf("Selected file is successfully uploaded.\n");
			else
			{
				printf("\nERROR: OTA server finished while file isn't sent completely!\n");
				closesocket(sclient);
				WSACleanup();
				return -6;
			}
		}
		else if( !strcmp((const char *)recData, respond[0]) ) // Chunk saved
		{		
			sentBytes += chunkLen;
			ProgressBar("Upload", sentBytes, fileLen);
		}
		else
		{
			printf("\nERROR: respond mismatch! \"%s\"\n", recData);
			closesocket(sclient);
			WSACleanup();
			return -5;
		}
	}
	
	closesocket(sclient);
	WSACleanup();
	
	return 0;
}

static int ConnectToClient(SOCKET *sclient, const char *ipAddr, uint16_t port){
	WSADATA data; 
    if(WSAStartup(MAKEWORD(2,2), &data) != 0){
		printf("\nERROR: WSAStartup failed!\n");
        return -1;
    }
	
	*sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(*sclient == INVALID_SOCKET){
		printf("\nERROR: Create socket failed!\n");
        return -2;
    }
	
	sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.S_un.S_addr = inet_addr(ipAddr); 
	if(cmdline_params.verbose) printf("Connecting to server %s on port %d ...", ipAddr, port);
    if(connect(*sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
        closesocket(*sclient);
		printf("\n");
		printf("\nERROR: Connect to server failed!\n");
        return -3;
    }
	if(cmdline_params.verbose) printf("Connected!\n");
	
	return 0;
}

static int CheckFileValidity(uint8_t *firmware){
	module_info_t module_info;
	uint32_t startAddress = 0;
	memset(&module_info, 0x00, sizeof(module_info_t));
	if( (*firmware & APP_START_MASK) == 0x20000000 ){
		startAddress += 0x184; // Skip the vector table
	}

	memcpy(&module_info, &firmware[startAddress], sizeof(module_info_t));
	
	PrintModuleInfo(&module_info);
	
	if(module_info.platform_id != 88){   // Not Duo
		printf("ERROR: Selected file is not for Duo!\n");
		return -1;
	}
	
	if(module_info.module_function > 6 || module_info.module_index > 2){
		printf("ERROR: Module function is invalid!\n");
		return -2;
	}
	
	if(module_info.dependency.module_function > 6 || module_info.dependency.module_index > 2){
		printf("ERROR: Module dependency is invalid!\n");
		return -2;
	}
	
	if(module_info.module_start_address < INTERNAL_FLASH_START_ADDR || module_info.module_start_address > INTERNAL_FLASH_END_ADDR || \
	   module_info.module_end_address < INTERNAL_FLASH_START_ADDR || module_info.module_end_address > INTERNAL_FLASH_END_ADDR){
		printf("ERROR: Module address is out of range!\n");
		return -3;
	}
	
	if((module_info.module_end_address - module_info.module_start_address) <= sizeof(module_info_t)){
		printf("ERROR: Module length is invalid!\n");
		return -4;
	}
	
	return 0;
}

static void PrintModuleInfo(module_info_t *module_info){
	printf("\n\tPlatform ID             : %d\n", module_info->platform_id);
	
	if(module_info->module_function > 6)
		printf("\tModule function         : Unknown module function.\n");
	else 
		printf("\tModule function         : %s\n", module_func[module_info->module_function]);
	
	printf("\tModule index            : %d\n", module_info->module_index);
	
	if(module_info->dependency.module_function > 6)	
		printf("\tModule dependency       : Unknown module dependency.\n");
	else
		printf("\tModule dependency       : %s\n", module_func[module_info->dependency.module_function]);
	
	printf("\tModule dependency index : %d\n", module_info->dependency.module_index);
	
	printf("\tModule start address    : 0x%x\n", module_info->module_start_address);
	
	printf("\tModule end address      : 0x%x\n\n", module_info->module_end_address);
}

static void __itoa_(uint32_t num, char *str){
    char index[] = "0123456789";
    int i = 0, j;

	do{
		str[i++] = index[ num % 10 ];
		num /= 10;
	}while(num);

	char temp;
	for(j=0; j<=(i-1)/2; j++){
		temp = str[j];
		str[j] = str[i-1-j];
		str[i-1-j] = temp;
	}
	
	str[i]='\0';
	
	return;
}

static void ProgressBar(const char *desc, unsigned long long curr, unsigned long long max){
	static char buf[PROGRESS_BAR_WIDTH + 1];
	static unsigned long long last_progress = -1;
//	static time_t last_time;
//	time_t curr_time = time(NULL);
	unsigned long long progress;
	unsigned long long x;

	/* check for not known maximum */
	if (max < curr)
		max = curr + 1;
	/* make none out of none give zero */
	if (max == 0 && curr == 0)
		max = 1;

	/* compute completion */
	progress = (PROGRESS_BAR_WIDTH * curr) / max;
	if (progress > PROGRESS_BAR_WIDTH)
		progress = PROGRESS_BAR_WIDTH;
	if (progress == last_progress /*&& curr_time == last_time*/)
		return;
	last_progress = progress;
//	last_time = curr_time;

	for (x = 0; x != PROGRESS_BAR_WIDTH; x++) {
		if (x < progress)
			buf[x] = '=';
		else
			buf[x] = ' ';
	}
	buf[x] = 0;

	fflush(stdout); // Clear the line buffer 
	printf("\r%s\t[%s] %3lld%% %6lld bytes", desc, buf,
	    (100ULL * curr) / max, curr);

	if (progress == PROGRESS_BAR_WIDTH)
		printf("\n%s done.\n", desc);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

