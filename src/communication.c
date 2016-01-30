// Including files
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmdline_param.h"
#include "communication.h"


int ConnectToClient(SOCKET *sclient, const char *ipAddr, uint16_t port){
	
#if defined WINDOWS
	WSADATA data; 
    if(WSAStartup(MAKEWORD(2,2), &data) != 0){
		printf("\nERROR: WSAStartup failed!\n");
        return -1;
    }
#endif
	
	*sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(*sclient == INVALID_SOCKET){
		printf("\nERROR: Create socket failed!\n");
        return -1;
    }
	
	sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
#if defined WINDOWS
    serAddr.sin_addr.S_un.S_addr = inet_addr(ipAddr); 
#else
	serAddr.sin_addr.s_addr = inet_addr(ipAddr); 
#endif
	if(cmdline_params.verbose) printf("Connecting to server %s on port %d ...", ipAddr, port);
    if(connect(*sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
        closesocket(*sclient);
		printf("\n");
		printf("\nERROR: Connect to server failed!\n");
        return -1;
    }
	if(cmdline_params.verbose) printf("Connected!\n");
	
	return 0;
}

