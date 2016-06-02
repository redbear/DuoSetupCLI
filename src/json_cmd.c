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
#include <stdlib.h>
#include "cmdline_param.h"
#include "communication.h"

#define MAX_JSON_CMD_LENGTH    256

static void __itoa_(uint32_t num, char *str);

void AssembleOtaCmdString(char *js, uint32_t file_len, uint32_t chunk_address, uint16_t chunk_size, uint8_t file_store) {
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
    
    strcat(json_param, ",\"file_store\":");
    __itoa_(file_store, temp_str);
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

void AssembleRstCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "finish-update\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssembleVerCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "version\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssembleDevidCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "device-id\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssembleChkCredentCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "check-credential\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssembleScanApCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "scan-ap\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssembleConfigApCmdString(char *js, char *ssid, int32_t security, char *password) {
    char temp_str[10];
    char json_param[MAX_JSON_CMD_LENGTH] = {'\0'};
    uint16_t json_param_len = 0;    
         
    strcpy(json_param, "{\"idx\":");
    __itoa_(0, temp_str);
    strcat(json_param, temp_str);
    
    strcat(json_param, ",\"ssid\":\"");
    strcat(json_param, ssid);
    strcat(json_param, "\"");
    
    strcat(json_param, ",\"pwd\":\"");
    strcat(json_param, password);
    strcat(json_param, "\"");
    
    strcat(json_param, ",\"ch\":");
    __itoa_(0, temp_str);
    strcat(json_param, temp_str);
    
    strcat(json_param, ",\"sec\":");
    __itoa_(security, temp_str);
    strcat(json_param, temp_str);
    
    strcat(json_param, "}");
    
    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "configure-ap\n");
    
    json_param_len = strlen(json_param);
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
    strcat(js, json_param);
}

void AssembleConnectApCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "connect-ap\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssemblePublicKeyCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "public-key\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

void AssembleInvalidCmdString(char *js) {
    char temp_str[10];
    uint16_t json_param_len = 0;

    memset(js, '\0', MAX_JSON_CMD_LENGTH);
    strcpy(js, "invalid-user\n");
    
    json_param_len = 0;
    __itoa_(json_param_len, temp_str);
    strcat(js, temp_str);
    
    strcat(js, "\n\n");
}

int SendJSONCmd(char *js, char *rsp, uint16_t rsp_len) {
    int timeout = 5000; // In milliseconds
    
    SOCKET sclient;
    if ( ConnectToClient(&sclient, DUO_SERVER_IP_ADDRESS, DUO_SERVER_CMD_PORT) < 0 ) {
        return -1;
    }
    
    setsockopt(sclient,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    setsockopt(sclient,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
    
    // Send request
    if (cmdline_params.verbose) printf("\nSend json request command:\n\n%s\n\n", js);
    
    if (send(sclient, js, strlen(js), 0) == SOCKET_ERROR) {
        printf("\nERROR: Sent request command failed!\n");
        closesocket(sclient);
        WSACleanup();
        return -1;
    }

    // Receive respond
    int ret = recv(sclient, rsp, rsp_len, 0);
    rsp[ret] = '\0';
    if (ret <= 0) {
        printf("\nERROR: Receive respond timeout!\n");
        closesocket(sclient);
        WSACleanup();
        return -1;
    }
    if (cmdline_params.verbose) printf("\nReceived: %s\n", rsp);
    
    closesocket(sclient);
    WSACleanup();
    
    return 0;
}

static void __itoa_(uint32_t num, char *str) {
    char index[] = "0123456789";
    int i = 0, j;

    do {
        str[i++] = index[ num % 10 ];
        num /= 10;
    }while (num);

    char temp;
    for (j = 0; j <= (i-1)/2; j++) {
        temp = str[j];
        str[j] = str[i-1-j];
        str[i-1-j] = temp;
    }
    str[i]='\0';
    
    return;
}

