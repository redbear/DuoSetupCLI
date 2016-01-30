// Including files
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "upload.h"
#include "cmdline_param.h"
#include "communication.h"

// Macro definitions
#define MAX_CHUNK_SIZE                1024

#define APP_START_MASK                ((uint32_t)0x2FF10000)
#define INTERNAL_FLASH_START_ADDR     0x08000000
#define INTERNAL_FLASH_END_ADDR       0x08100000

#define PROGRESS_BAR_WIDTH            25


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

static int CheckFileValidity(uint8_t *firmware);
static void PrintModuleInfo(module_info_t *module_info);
static void ProgressBar(const char *desc, unsigned long long curr, unsigned long long max);

// Functions achievement
int PrepareUpload(char *file_name, uint8_t *fileData, uint32_t *fileLength){
	FILE *pFile = NULL;
	
	pFile = fopen(file_name, "rb");      // read only
	if(NULL == pFile){
		printf("\nERROR: File \"%s\" doesn't exist or is being occupied!\n", file_name);
        return -1;
    }
	printf("Open file \"%s\"\n", file_name);
	fseek(pFile,0,SEEK_SET);
    fseek(pFile,0,SEEK_END);
    *fileLength = ftell(pFile);
    fseek(pFile,0,SEEK_SET);   
	if(*fileLength > MAX_FILE_LENGTH){
		printf("\nERROR: Selected file is too large!\n");
		return -1;
	}
	if(1 != fread(fileData, *fileLength, 1, pFile)){
		printf("\nERROR: Read file failed!\n");
		fclose(pFile);
        return -1;
	}
	fclose(pFile);
	
	printf("Check file validity:\n");
	if( CheckFileValidity(fileData) < 0 ){
		return -1;
	}
	printf("File length : %d\n", *fileLength);
}

int OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize){
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
			return -1;
		}
		
		char recData[255];
		int ret = recv(sclient, recData, 255, 0);
		if(ret < 0)
		{
			printf("\nERROR: Receive respond timeout!\n");
			closesocket(sclient);
			WSACleanup();
			return -1;
		}
		
		recData[ret] = '\0';
		if( !strcmp((const char *)recData, respond[2]) ) // OTA server not init
		{
			printf("\nERROR: OTA server not init!\n");
			closesocket(sclient);
			WSACleanup();
			return -1;
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
				return -1;
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
			return -1;
		}
	}
	
	closesocket(sclient);
	WSACleanup();
	
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
		return -1;
	}
	
	if(module_info.dependency.module_function > 6 || module_info.dependency.module_index > 2){
		printf("ERROR: Module dependency is invalid!\n");
		return -1;
	}
	
	if(module_info.module_start_address < INTERNAL_FLASH_START_ADDR || module_info.module_start_address > INTERNAL_FLASH_END_ADDR || \
	   module_info.module_end_address < INTERNAL_FLASH_START_ADDR || module_info.module_end_address > INTERNAL_FLASH_END_ADDR){
		printf("ERROR: Module address is out of range!\n");
		return -1;
	}
	
	if((module_info.module_end_address - module_info.module_start_address) <= sizeof(module_info_t)){
		printf("ERROR: Module length is invalid!\n");
		return -1;
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

static void ProgressBar(const char *desc, unsigned long long curr, unsigned long long max){
	static char buf[PROGRESS_BAR_WIDTH + 1];
	static unsigned long long last_progress = -1;
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
	if (progress == last_progress)
		return;
	last_progress = progress;

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

