#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#if defined WINDOWS // Windows

    #include <winsock2.h>


#else // Linux and MacOSX

    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>

    #define SOCKET            int
    #define SOCKET_ERROR      (-1)
	#define INVALID_SOCKET    (-1)
    #define closesocket       close
    #define WSACleanup()  

#endif

#endif

