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
	HRESULT h;
	h = ConnectTo(&Source, SourceAddr, SourcePort);
	if(!SUCCEEDED(h))
	{
		printf("Initializing Source Socket Failed 0x%X\n", h);
		return h;
	}

	//h = ConnectTo(&Dest, DestAddr, DestPort);

	if(!SUCCEEDED(h))
	{
		printf("Initializing Source Socket Failed 0x%X\n", h);
		return h;
	}

	if(Source == INVALID_SOCKET || Dest == INVALID_SOCKET)
	{
		return 1;
	}

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

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        r = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (r == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

	*Socket = ConnectSocket;
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
			//printf("%X", recvbuf);
		}
		else if(r == 0)
		{
			printf("Connection Closed.\n", r);
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