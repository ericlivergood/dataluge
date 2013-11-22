#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


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
}

HRESULT Spiral::Initialize()
{
	return 0;
}

HRESULT Spiral::Transfer()
{
	return 0;
}