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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json_cmd.h"
#include "upload.h"
#include "cmdline_param.h"
#include "cJSON.h"

#define MAX_SCAN_RECORD    20
#define MAX_SSID_LENGTH    33
#define MAX_SEC_KEY_LENGTH 64

#define SHARED_ENABLED     0x00008000
#define WPA_SECURITY       0x00200000
#define WPA2_SECURITY      0x00400000
#define ENTERPRISE_ENABLED 0x02000000
#define WPS_ENABLED        0x10000000
#define IBSS_ENABLED       0x20000000

#define WEP_ENABLED        0x0001
#define TKIP_ENABLED       0x0002
#define AES_ENABLED        0x0004

typedef enum {
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
    char ssid[MAX_SSID_LENGTH];
    int rssi;
    int32_t sec;
    uint8_t ch;
    uint32_t mdr;
} Scan_result_t;

Scan_result_t scan_result[MAX_SCAN_RECORD];
uint8_t scan_result_cnt = 0;

uint8_t file_idx = 0;
uint32_t curr_addr = 0;
uint32_t file_len = 0;
uint8_t region_idx = 0;

// Functions declaration
static uint32_t UploadFirmware(char *file_name, uint32_t chunk_addr, uint8_t file_store);
static int FetchFirmwareVersion(void);
static int FetchDeviceID(void);
static int CheckCredential(void);
static int ScanNetworks(Scan_result_t *scan_result, uint8_t *scan_result_cnt);
static int ConfigAP(void);
static int ConnectAP(void);
static int FetchDevicePublicKey(void);
static int InvalidUserPart(void);
static int LeaveListeningMode(void);

// Functions achievement
int main(int arg, char *argv[]) {
    int16_t result = -1;

    printf("Copyright (c) 2016 redbear.cc\n");
    printf("RedBear Duo Setup CLI version 1.0.0 ");
    
#if defined WINDOWS
    printf("for Windows\n");
#elif defined LINUX
    printf("for Linux\n");
#elif defined MACOSX
    printf("for Mac OS X\n");
#endif

    cmdline_params.file_num = 0;

    if (ParseCmdlineParameters(arg, argv) < 0)
        return -1;
    
    switch (cmdline_option) {
        case OPTION_UPLOAD_FIRMWARE:
            if (cmdline_params.file_num != 0 || cmdline_params.fac_set) {
                printf("Upload firmware to Duo.\n");
                
                // Upload firmware
                while (file_idx < cmdline_params.file_num) { 
                    if (region_idx >= OTA_REGION_UNIT_CNT) {
                        printf("\nERROR: No OTA region available!\n");
                        break;
                    }
                    else {
                        curr_addr = region_idx * OTA_REGION_UNIT_SIZE;
                        printf("\nUpload image %s to 0x%x of the OTA region.\n", cmdline_params.file_name[file_idx], curr_addr);
                        file_len = UploadFirmware(cmdline_params.file_name[file_idx], curr_addr, 0); // 0: FIRMWARE
                        if (file_len != 0) {
                            curr_addr += file_len;
                            if (curr_addr % OTA_REGION_UNIT_SIZE == 0) region_idx = curr_addr / OTA_REGION_UNIT_SIZE;
                            else region_idx = (curr_addr / OTA_REGION_UNIT_SIZE) + 1;
                        }
                        else {
                            printf("\nERROR: Upload file failed!\n");
                            break;
                        }
                    }
                    file_idx++;
                }
                
                // Upload factory reset firmware
                if (cmdline_params.fac_set) {
                    printf("Upload factory reset image %s to the FAC region.\n", cmdline_params.fac_name);
                    file_len = UploadFirmware(cmdline_params.fac_name, FAC_REGION_ADDR, 1); // 1: SYSTEM
                    if (file_len == 0) {
                        printf("\nERROR: Upload file failed!\n");
                    }
                }
                
                if (cmdline_params.safe) {
                    printf("\nInvalid user part...\n");
                    InvalidUserPart();
                }
                if (cmdline_params.leave) {
                    printf("\nLeave listening mode...\n");
                    LeaveListeningMode();
                }
            }
            else {
                printf("\nERROR: Neither --file nor -fac is presented when using --upload option!\n");
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
            for (uint8_t i = 0; i < MAX_SCAN_RECORD; i++) {
                memset(&scan_result[i], 0x00, sizeof(Scan_result_t));
            }
            result = ScanNetworks(scan_result, &scan_result_cnt);
            break;
            
        case OPTION_CONFIG_AP:
            printf("Config Access Points.\n");
            result = ConfigAP();
            break;
            
        case OPTION_CONNECT_AP:
            printf("Connect to Access Points.\n");
            result = ConnectAP();
            break;
            
        case OPTION_PUBLIC_KEY:
            printf("Fetch device public key.\n");
            result = FetchDevicePublicKey();
            break;
            
        default:
            printf("\nERROR: No option is selected!\n");
            PrintHelpMessage();
            result = -1;
            break;
    }
    
    return result;
}

static uint32_t UploadFirmware(char *file_name, uint32_t chunk_addr, uint8_t file_store) {
    char jsonString[256];
    char respond[64];
    uint16_t chunk_size = 128;
    uint8_t fileData[MAX_FILE_LENGTH];
    uint32_t fileLength = 0;
    
    if (PrepareUpload(file_name, fileData, &fileLength) < 0)
        return 0;
    
    if (file_store == 0) {
        if ((chunk_addr + fileLength) > MAX_OTA_REGION_ADDR) printf("\nERROR: Can not fill the image to the rest memory of OTA region!\n");
    }
    else {
        if ((chunk_addr + fileLength) > MAX_FAC_REGION_ADDR) printf("\nERROR: Can not fill the image to the rest memory of FAC region!\n");
    }
    
    AssembleOtaCmdString(jsonString, fileLength, chunk_addr, chunk_size, file_store);

    if (SendJSONCmd(jsonString, respond, sizeof(respond)) < 0) return 0;
    if (cmdline_params.verbose) printf("\nJSON request command is sent successfully.\n");
    
    if ( OTAUploadFirmware(fileData, fileLength, chunk_size) < 0 ) return 0;

    return fileLength;
}

static int InvalidUserPart(void) {
    char jsonString[256];
    char respond[128];
    int result = -1;
    
    AssembleInvalidCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    return result;
}

static int LeaveListeningMode(void) {
    char jsonString[256];
    char respond[128];
    int result = -1;
    
    AssembleRstCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    return result;
}

static int FetchFirmwareVersion(void) {
    char jsonString[256];
    char respond[128];
    cJSON *json;
    int result = -1;
    
    AssembleVerCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    if (result == 0) {
        json = cJSON_Parse((const char *)respond);
        if (!json) {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
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
    
    return result;
}

static int FetchDeviceID(void) {
    char jsonString[256];
    char respond[128];
    cJSON *json;
    int result = -1;
    
    AssembleDevidCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    if (result == 0) {
        json = cJSON_Parse((const char *)respond);
        if (!json) {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
        else {
            printf("\n");
            printf("    Device ID : %s\n", cJSON_GetObjectItem(json, "id")->valuestring);
            if ( !strcmp("1", cJSON_GetObjectItem(json, "c")->valuestring) ) 
                printf("    Claimed   : YES\n");
            else
                printf("    Claimed   : NO\n");
            
            cJSON_Delete(json);
        }
    }
    
    return result;
}

static int CheckCredential(void) {
    char jsonString[256];
    char respond[64];
    cJSON *json;
    int result = -1;
    
    AssembleChkCredentCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    if (result == 0) {
        json = cJSON_Parse((const char *)respond);
        if (!json) {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
        else {
            printf("\n");
            if ( 1 == cJSON_GetObjectItem(json, "has credentials")->valueint ) 
                printf("    Has credentials : YES\n");
            else
                printf("    Has credentials : NO\n");
            
            cJSON_Delete(json);
        }
    }
    
    return result;
}

static int ScanNetworks(Scan_result_t *scan_result, uint8_t *scan_result_cnt) {
    char jsonString[256];
    char respond[10240];
    cJSON *json, *array, *object;
    int result = -1;
    
    AssembleScanApCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    if (result == 0) {
        json = cJSON_Parse((const char *)respond);
        if (!json) {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
        else {
            array = cJSON_GetObjectItem(json, "scans");
            *scan_result_cnt = cJSON_GetArraySize( array );

            printf( "\n    SSID                             RSSI    Security       Channel  MDR");
            printf( "\n    ----                             ----    --------       -------  ---\n");
            
            for (uint8_t i = 0; i < *scan_result_cnt; i++) {
                object = cJSON_GetArrayItem(array, i);
                
                char *ssid = cJSON_GetObjectItem(object, "ssid")->valuestring;
                memcpy(scan_result[i].ssid, ssid, strlen(ssid));
                scan_result[i].ssid[strlen(ssid)] = '\0';
                printf("%02d. ", i+1);
                uint8_t ssid_len = strlen(scan_result[i].ssid);
                for (uint8_t j = 0; j < MAX_SSID_LENGTH; j++) {
                    if (j < ssid_len)
                        printf("%c", scan_result[i].ssid[j]);
                    else
                        printf(" ");
                }
                
                scan_result[i].rssi = cJSON_GetObjectItem(object, "rssi")->valueint;
                printf("%ddBm", scan_result[i].rssi);
                if (scan_result[i].rssi > -10) printf("   ");
                else if (scan_result[i].rssi > -100) printf("  ");
                else printf(" ");
                
                char sec_str[18];
                scan_result[i].sec = cJSON_GetObjectItem(object, "sec")->valueint;
                switch (scan_result[i].sec) {
                    case WICED_SECURITY_OPEN:           strcpy(sec_str, "OPEN"); break;
                    case WICED_SECURITY_WEP_PSK:        strcpy(sec_str, "WEP_PSK"); break;
                    case WICED_SECURITY_WEP_SHARED:     strcpy(sec_str, "WEP_SHARED"); break;
                    case WICED_SECURITY_WPA_TKIP_PSK:   strcpy(sec_str, "WPA_TKIP_PSK"); break;
                    case WICED_SECURITY_WPA_AES_PSK:    strcpy(sec_str, "WPA_AES_PSK"); break;
                    case WICED_SECURITY_WPA_MIXED_PSK:  strcpy(sec_str, "WPA_MIXED_PSK"); break;
                    case WICED_SECURITY_WPA2_AES_PSK:   strcpy(sec_str, "WPA2_AES_PSK"); break;
                    case WICED_SECURITY_WPA2_TKIP_PSK:  strcpy(sec_str, "WPA2_TKIP_PSK"); break;
                    case WICED_SECURITY_WPA2_MIXED_PSK: strcpy(sec_str, "WPA2_MIXED_PSK"); break;
                    case WICED_SECURITY_WPA_TKIP_ENT:   strcpy(sec_str, "WPA_TKIP_ENT"); break;
                    case WICED_SECURITY_WPA_AES_ENT:    strcpy(sec_str, "WPA_AES_ENT"); break;
                    case WICED_SECURITY_WPA_MIXED_ENT:  strcpy(sec_str, "WPA_MIXED_ENT"); break;
                    case WICED_SECURITY_WPA2_TKIP_ENT:  strcpy(sec_str, "WPA2_TKIP_ENT"); break;
                    case WICED_SECURITY_WPA2_AES_ENT:   strcpy(sec_str, "WPA2_AES_ENT"); break;
                    case WICED_SECURITY_WPA2_MIXED_ENT: strcpy(sec_str, "WPA2_MIXED_ENT"); break;
                    case WICED_SECURITY_IBSS_OPEN:      strcpy(sec_str, "IBSS_OPEN"); break;
                    case WICED_SECURITY_WPS_OPEN:       strcpy(sec_str, "WPS_OPEN"); break;
                    case WICED_SECURITY_WPS_SECURE:     strcpy(sec_str, "WPS_SECURE"); break;
                    default:                            strcpy(sec_str, "UNKNOWN"); break;
                }
                for (uint8_t j = 0; j < 18; j++) {
                    if (j < strlen(sec_str))
                        printf("%c", sec_str[j]);
                    else
                        printf(" ");
                }
                
                scan_result[i].ch = cJSON_GetObjectItem(object, "ch")->valueint;
                printf("%d", scan_result[i].ch);
                if (scan_result[i].ch < 10) printf("     ");
                else if (scan_result[i].ch < 100) printf("    ");
                else printf("   ");
                
                scan_result[i].mdr = cJSON_GetObjectItem(object, "mdr")->valueint / 1000;
                printf("%dKB/s\n", scan_result[i].mdr);
            }
            
            cJSON_Delete(json);
        }
    }
    
    return result;
}

static int ConfigAP(void) {
    char jsonString[256];
    char respond[10240];
    cJSON *json;
    int result = -1;
    
    for (uint8_t i = 0; i < MAX_SCAN_RECORD; i++) {
        memset(&scan_result[i], 0x00, sizeof(Scan_result_t));
    }
    char scan_again = 'n';
    do {
        printf("Scanning networks ...\n");
        result = ScanNetworks(scan_result, &scan_result_cnt);
        if (result == 0) {
            printf("\nScan again?(y/n):");
            scan_again = getchar();
            while (getchar() != '\n'); // Clear the stdin
        }
    }while (scan_again == 'y' && result == 0);
    
    if (result == 0) {
        uint8_t idx = 0;
        char ssid[MAX_SSID_LENGTH];
        int32_t security = WICED_SECURITY_OPEN;
        uint8_t cipher;
        char password[MAX_SEC_KEY_LENGTH];
        
        printf("\nPlease input the index of AP in the above scanned result lists, \n");
        printf("or input '0' to manually config the AP that not in the list: ");

        scanf("%d", &idx);
        while (getchar() != '\n'); // Clear the stdin
        
        if (idx > 0 && idx <= scan_result_cnt) {
            memcpy(ssid, scan_result[idx].ssid, sizeof(ssid));
            security = scan_result[idx].sec;
        }
        else {
            printf("\nThe AP you chose is not in the list, please config AP manually.\n");
            printf("SSID: ");
            char c, num = 0;
            do {
                c = getchar();
                if (c != '\n') {
                    ssid[num] = c;
                    num++;
                }
            }while (c != '\n');
            ssid[num] = '\0';
            
            printf("Security 0=unsecured, 1=WEP, 2=WPA, 3=WPA2: ");
            scanf("%d", &security);
            if (security == 0) 
                security = WICED_SECURITY_OPEN;
            else if (security == 1) {
                security = WICED_SECURITY_WEP_PSK;
            }
            else if (security == 2 || security == 3) {
                if (security == 2) 
                    security = WPA_SECURITY;
                else 
                    security = WPA2_SECURITY;
                printf("Security Cipher 1=AES, 2=TKIP, 3=AES+TKIP: ");
                scanf("%d", &cipher);
                if (cipher == 1) 
                    security |= AES_ENABLED;
                else if (cipher == 2) 
                    security |= TKIP_ENABLED;
                else 
                    security |= AES_ENABLED|TKIP_ENABLED;
            }
        }
        if (security != WICED_SECURITY_OPEN) {
            printf("Password: ");
            scanf("%s", password);
        }
        
        AssembleConfigApCmdString(jsonString, ssid, security, password);
        result = SendJSONCmd(jsonString, respond, sizeof(respond));
        
        if (result == 0) {
            json = cJSON_Parse((const char *)respond);
            if (!json) {
                printf("Error before: [%s]\n", cJSON_GetErrorPtr());
            }
            else {
                printf("\n");
                printf("    Result : %d\n", cJSON_GetObjectItem(json, "r")->valueint);
                cJSON_Delete(json);
            }
        }
    }
    
    return result;
}

static int ConnectAP(void) {
    char jsonString[256];
    char respond[128];
    cJSON *json;
    int result = -1;
    
    AssembleConnectApCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    if (result == 0) {
        json = cJSON_Parse((const char *)respond);
        if (!json) {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
        else {
            printf("\n");
            printf("    Result : %d\n", cJSON_GetObjectItem(json, "r")->valueint);
            cJSON_Delete(json);
        }
    }
    
    return result;
}

static int FetchDevicePublicKey(void) {
    char jsonString[256];
    char respond[1024];
    cJSON *json;
    int result = -1;
    
    AssemblePublicKeyCmdString(jsonString);
    result = SendJSONCmd(jsonString, respond, sizeof(respond));
    
    if (result == 0) {
        json = cJSON_Parse((const char *)respond);
        if (!json) {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        }
        else {
            printf("\n");
            if (cJSON_GetObjectItem(json, "r")->valueint != 1) 
                printf("    Device public key : %s\n", cJSON_GetObjectItem(json, "b")->valuestring);
            else 
                printf("Fetch device public key failed!\n");
            cJSON_Delete(json);
        }
    }
    
    return result;
}

