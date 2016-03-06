// Including files
#include "platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json_cmd.h"
#include "upload.h"
#include "cmdline_param.h"
#include "cJSON.h"

#define MAX_SCAN_RECORD	   20

#define SHARED_ENABLED     0x00008000
#define WPA_SECURITY       0x00200000
#define WPA2_SECURITY      0x00400000
#define ENTERPRISE_ENABLED 0x02000000
#define WPS_ENABLED        0x10000000
#define IBSS_ENABLED       0x20000000

#define WEP_ENABLED                 0x0001
#define TKIP_ENABLED                0x0002
#define AES_ENABLED                 0x0004

typedef enum
{
    WICED_SECURITY_OPEN           = 0,                                                                   /**< Open security                                         */
    WICED_SECURITY_WEP_PSK        = WEP_ENABLED,                                                         /**< WEP PSK Security with open authentication             */
    WICED_SECURITY_WEP_SHARED     = ( WEP_ENABLED   | SHARED_ENABLED ),                                  /**< WEP PSK Security with shared authentication           */
    WICED_SECURITY_WPA_TKIP_PSK   = ( WPA_SECURITY  | TKIP_ENABLED ),                                    /**< WPA PSK Security with TKIP                            */
    WICED_SECURITY_WPA_AES_PSK    = ( WPA_SECURITY  | AES_ENABLED ),                                     /**< WPA PSK Security with AES                             */
    WICED_SECURITY_WPA_MIXED_PSK  = ( WPA_SECURITY  | AES_ENABLED | TKIP_ENABLED ),                      /**< WPA PSK Security with AES & TKIP                      */
    WICED_SECURITY_WPA2_AES_PSK   = ( WPA2_SECURITY | AES_ENABLED ),                                     /**< WPA2 PSK Security with AES                            */
    WICED_SECURITY_WPA2_TKIP_PSK  = ( WPA2_SECURITY | TKIP_ENABLED ),                                    /**< WPA2 PSK Security with TKIP                           */
    WICED_SECURITY_WPA2_MIXED_PSK = ( WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED ),                      /**< WPA2 PSK Security with AES & TKIP                     */

    WICED_SECURITY_WPA_TKIP_ENT   = ( ENTERPRISE_ENABLED | WPA_SECURITY  | TKIP_ENABLED ),               /**< WPA Enterprise Security with TKIP                     */
    WICED_SECURITY_WPA_AES_ENT    = ( ENTERPRISE_ENABLED | WPA_SECURITY  | AES_ENABLED ),                /**< WPA Enterprise Security with AES                      */
    WICED_SECURITY_WPA_MIXED_ENT  = ( ENTERPRISE_ENABLED | WPA_SECURITY  | AES_ENABLED | TKIP_ENABLED ), /**< WPA Enterprise Security with AES & TKIP               */
    WICED_SECURITY_WPA2_TKIP_ENT  = ( ENTERPRISE_ENABLED | WPA2_SECURITY | TKIP_ENABLED ),               /**< WPA2 Enterprise Security with TKIP                    */
    WICED_SECURITY_WPA2_AES_ENT   = ( ENTERPRISE_ENABLED | WPA2_SECURITY | AES_ENABLED ),                /**< WPA2 Enterprise Security with AES                     */
    WICED_SECURITY_WPA2_MIXED_ENT = ( ENTERPRISE_ENABLED | WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED ), /**< WPA2 Enterprise Security with AES & TKIP              */

    WICED_SECURITY_IBSS_OPEN      = ( IBSS_ENABLED ),                                                    /**< Open security on IBSS ad-hoc network                  */
    WICED_SECURITY_WPS_OPEN       = ( WPS_ENABLED ),                                                     /**< WPS with open security                                */
    WICED_SECURITY_WPS_SECURE     = ( WPS_ENABLED | AES_ENABLED),                                        /**< WPS with AES security                                 */

    WICED_SECURITY_UNKNOWN        = -1,                                                                  /**< May be returned by scan function if security is unknown. Do not pass this to the join function! */

    WICED_SECURITY_FORCE_32_BIT   = 0x7fffffff                                                           /**< Exists only to force wiced_security_t type to 32 bits */
} wiced_security_t;

typedef struct {
	char ssid[21];
	int rssi;
	int32_t sec;
	uint8_t ch;
	uint32_t mdr;
}Scan_result_t;

// Functions declaration
static int UploadFirmware(char *file_name, uint8_t area, uint8_t leave, uint8_t safe);
static int FetchFirmwareVersion(void);
static int FetchDeviceID(void);
static int CheckCredential(void);
static int ScanNetworks(Scan_result_t *scan_result);

Scan_result_t scan_result[MAX_SCAN_RECORD];

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
				result = UploadFirmware(cmdline_params.file_name, cmdline_params.region, cmdline_params.leave, cmdline_params.safe);	
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
			for(uint8_t i=0; i<MAX_SCAN_RECORD; i++) {
				memset(&scan_result[i], 0x00, sizeof(Scan_result_t));
			}
			result = ScanNetworks(scan_result);
			break;
			
		default:
			printf("\nERROR: No option is selected!\n");
			PrintHelpMessage();
			result = -1;
			break;
	}
	
	return result;
}

static int UploadFirmware(char *file_name, uint8_t region, uint8_t leave, uint8_t safe) {
	char jsonString[256];
	char respond[64];
	uint32_t chunk_addr = 0x0;
	uint16_t chunk_size = 128;
	uint8_t file_store;
	uint8_t fileData[MAX_FILE_LENGTH];
	uint32_t fileLength = 0;
	uint32_t bounds[9]={0x0, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000, 0x70000,  // The offset of the OTA region
						0x140000  // The address of the Factory Reset region
						};
	
	if(PrepareUpload(file_name, fileData, &fileLength) < 0)
		return -1;
	
	chunk_addr = bounds[region];
	
	if(region < 8) {
		printf("File will be stored from offset 0x%02x of the OTA region.\n", chunk_addr);
		file_store = 0;		// FIRMWARE
	}
	else {
		printf("File will be stored from 0x%02x of the Factory Reset region.\n", chunk_addr);
		file_store = 1;		// SYSTEM
	}
	
	AssembleOtaCmdString(jsonString, fileLength, chunk_addr, chunk_size, file_store);
	uint8_t i;
	for(i=0; i<3; i++) {
		if(i > 0) printf("\nRetrying...\n");
		if(SendJSONCmd(jsonString, respond, sizeof(respond)) < 0) continue;
		if(cmdline_params.verbose) printf("\nJSON request command is sent successfully.\n");
		if( OTAUploadFirmware(fileData, fileLength, chunk_size) < 0 ) continue;
		break;
	}
	
	if(safe && i<3) {
		printf("\nInvalid user part...\n");
		AssembleInvalidCmdString(jsonString);
		SendJSONCmd(jsonString, respond, sizeof(respond));
	}
	
	if(leave && i<3) {
		printf("\nLeave listening mode...\n");
		AssembleRstCmdString(jsonString);
		SendJSONCmd(jsonString, respond, sizeof(respond));
	}
	
	return 0;
}

static int FetchFirmwareVersion(void) {
	char jsonString[256];
	char respond[128];
	cJSON *json;
	int result = -1;
	
	AssembleVerCmdString(jsonString);
	result = SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(result == 0) {
		json=cJSON_Parse((const char *)respond);
		if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		else {
			printf("\n");
			printf("    Release      : %s\n", cJSON_GetObjectItem(json, "release string")->valuestring);
			printf("    Bootloader   : %d\n", cJSON_GetObjectItem(json, "bootloader")->valueint);
			printf("    System part1 : %d\n", cJSON_GetObjectItem(json, "system part1")->valueint);
			printf("    System part2 : %d\n", cJSON_GetObjectItem(json, "system part2")->valueint);
			printf("    User part    : %d\n", cJSON_GetObjectItem(json, "user part")->valueint);
			
			cJSON_Delete(json);
		}
	}
	
	return 0;
}

static int FetchDeviceID(void) {
	char jsonString[256];
	char respond[128];
	cJSON *json;
	int result = -1;
	
	AssembleDevidCmdString(jsonString);
	result = SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(result == 0) {
		json=cJSON_Parse((const char *)respond);
		if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		else {
			printf("\n");
			printf("    Device ID : %s\n", cJSON_GetObjectItem(json, "id")->valuestring);
			if( !strcmp("1", cJSON_GetObjectItem(json, "c")->valuestring) ) 
				printf("    Claimed   : YES\n");
			else
				printf("    Claimed   : NO\n");
			
			cJSON_Delete(json);
		}
	}
	
	return 0;
}

static int CheckCredential(void) {
	char jsonString[256];
	char respond[64];
	cJSON *json;
	int result = -1;
	
	AssembleChkCredentCmdString(jsonString);
	result = SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(result == 0) {
		json=cJSON_Parse((const char *)respond);
		if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		else {
			printf("\n");
			if( 1 == cJSON_GetObjectItem(json, "has credentials")->valueint ) 
				printf("    Has credentials : YES\n");
			else
				printf("    Has credentials : NO\n");
			
			cJSON_Delete(json);
		}
	}
	
	return 0;
}

static int ScanNetworks(Scan_result_t *scan_result) {
	char jsonString[256];
	char respond[10240];
	cJSON *json, *array, *object;
	uint8_t scan_cnt;
	int result = -1;
	
	AssembleScanApCmdString(jsonString);
	result = SendJSONCmd(jsonString, respond, sizeof(respond));
	
	if(result == 0) {
		json=cJSON_Parse((const char *)respond);
		if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		else {
			array = cJSON_GetObjectItem(json, "scans");
			scan_cnt = cJSON_GetArraySize( array );

			printf( "\n    SSID       RSSI     Security     Channel     MDR\n");
			
			for(uint8_t i=0; i<scan_cnt; i++) {
				object = cJSON_GetArrayItem(array, i);
				
				char *ssid = cJSON_GetObjectItem(object, "ssid")->valuestring;
				memcpy(scan_result[i].ssid, ssid, strlen(ssid));
				scan_result[i].ssid[strlen(ssid)] = '\0';
				printf("%02d. %s     ", i+1, ssid);
				
				scan_result[i].rssi = cJSON_GetObjectItem(object, "rssi")->valueint;
				printf("%ddBm     ", scan_result[i].rssi);
				
				scan_result[i].sec = cJSON_GetObjectItem(object, "sec")->valueint;
				switch(scan_result[i].sec) {
					case WICED_SECURITY_OPEN: printf("OPEN     "); break;
					case WICED_SECURITY_WEP_PSK: printf("WEP_PSK     "); break;
					case WICED_SECURITY_WEP_SHARED: printf("WEP_SHARED     "); break;
					case WICED_SECURITY_WPA_TKIP_PSK: printf("WPA_TKIP_PSK     "); break;
					case WICED_SECURITY_WPA_AES_PSK: printf("WPA_AES_PSK     "); break;
					case WICED_SECURITY_WPA_MIXED_PSK: printf("WPA_MIXED_PSK     "); break;
					case WICED_SECURITY_WPA2_AES_PSK: printf("WPA2_AES_PSK     "); break;
					case WICED_SECURITY_WPA2_TKIP_PSK: printf("WPA2_TKIP_PSK     "); break;
					case WICED_SECURITY_WPA2_MIXED_PSK: printf("WPA2_MIXED_PSK     "); break;
					case WICED_SECURITY_WPA_TKIP_ENT: printf("WPA_TKIP_ENT     "); break;
					case WICED_SECURITY_WPA_AES_ENT: printf("WPA_AES_ENT     "); break;
					case WICED_SECURITY_WPA_MIXED_ENT: printf("WPA_MIXED_ENT     "); break;
					case WICED_SECURITY_WPA2_TKIP_ENT: printf("WPA2_TKIP_ENT     "); break;
					case WICED_SECURITY_WPA2_AES_ENT: printf("WPA2_AES_ENT     "); break;
					case WICED_SECURITY_WPA2_MIXED_ENT: printf("WPA2_MIXED_ENT     "); break;
					case WICED_SECURITY_IBSS_OPEN: printf("IBSS_OPEN     "); break;
					case WICED_SECURITY_WPS_OPEN: printf("WPS_OPEN     "); break;
					case WICED_SECURITY_WPS_SECURE: printf("WPS_SECURE     "); break;
					default: printf("UNKNOWN     "); break;
				}
				
				scan_result[i].ch = cJSON_GetObjectItem(object, "ch")->valueint;
				printf("%d     ", scan_result[i].ch);
				
				scan_result[i].mdr = cJSON_GetObjectItem(object, "mdr")->valueint/1000;
				printf("%dKB/s\n", scan_result[i].mdr);
			}
			
			cJSON_Delete(json);
		}
	}
	
	return 0;
}

