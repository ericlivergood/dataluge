#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#include "Spiral.h"

Spiral::Spiral(void)
{
	SourceAddr = "127.0.0.1";
	SourcePort = "9001";
	DestAddr = "127.0.0.1";
	DestPort = "9002";
}


Spiral::~Spiral(void)
{
	closesocket(Source);
	closesocket(Dest);
	WSACleanup();
}

HRESULT Spiral::Initialize()
{
	ConnectTo(&Source, SourceAddr, SourcePort);
	ConnectTo(&Dest, DestAddr, DestPort);
	return 0;
}

HRESULT Spiral::ConnectTo(SOCKET* Socket, LPCSTR Address, LPCSTR Port)
{
	SOCKET ConnectSocket = INVALID_SOCKET;
	WSADATA wsaData;
    
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

	int r;
	
    // Initialize Winsock
    r = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (r != 0) {
        printf("WSAStartup failed with error: %d\n", r);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
	r = getaddrinfo(Address, Port, &hints, &result);
    if ( r != 0 ) {
        printf("getaddrinfo failed with error: %d\n", r);
        WSACleanup();
        return 1;
    }

	Socket = &ConnectSocket;
	return 0;
}

HRESULT Spiral::Transfer()
{
	HRESULT r, s;
    char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	do{
		r = recv(Source, recvbuf, recvbuflen, 0);

		if(r > 0)
		{
			s = send(Dest, recvbuf, recvbuflen, 0);
			if(s != r)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				NetworkingCleanup();
				return 1;
			}
			else
			{
				printf("Pushed %d bytes\n", s);
			}
		}
		else if(r == 0)
		{
			printf("Connection Closed.\n", s);
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
		}

	} while( r > 0 );

	NetworkingCleanup();
	return 0;
}

HRESULT Spiral::NetworkingCleanup(void)
{
	closesocket(Source);
	closesocket(Dest);
	WSACleanup();
	return 0;
}