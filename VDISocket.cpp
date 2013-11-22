#define _WIN32_DCOM
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>      
#include <thread>
#include "VDISocket.h"
#include "vdi.h"        // interface declaration
#include "vdierror.h"   // error constants
#include "vdiguid.h"    // define the interface identifiers.
						// IMPORTANT: vdiguid.h can only be included in one source file.
						// 

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

VDConfig config;
IClientVirtualDeviceSet2* deviceSet;
int ServerConnectTimeOut = 10000;
bool deviceOpened;
bool initialized;
struct addrinfo* addr_info = NULL;

VDISocket::VDISocket(LPCSTR portNumber)
{
	deviceSet = NULL; 
	deviceOpened = false;
	initialized = false;

	memset(&config, 0, sizeof(config));
	config.deviceCount = 1;

	GUID deviceID;
	CoCreateGuid(&deviceID);
	StringFromGUID2(deviceID, deviceName, 49);

	Port = portNumber;
}


VDISocket::~VDISocket(void)
{
	CleanupNetworking();
	DestroyVirtualDevice();
	CoUninitialize();
}

HRESULT VDISocket::Luge(std::string instanceName, std::string databaseName)
{
	HRESULT h;
	h = CreateVirtualDevice(instanceName);
	if(!SUCCEEDED(h))
		return h;
	Listen();
	PerformOperation(instanceName, databaseName);
	DestroyVirtualDevice();

	return 0;
}

std::thread VDISocket::StartOperation(std::string instanceName, std::string databaseName)
{
	if(!initialized)
	{
		//return NULL;
	}
	std::thread t(&VDISocket::Luge, this, instanceName, databaseName);
	return t;
}

HRESULT VDISocket::OpenDevice()
{
	HRESULT h;
	h = deviceSet->GetConfiguration(ServerConnectTimeOut, &config);
	if (!SUCCEEDED (h))
		return h;
	h = deviceSet->OpenDevice(deviceName, &device);
	if (!SUCCEEDED (h))
		return h;
	deviceOpened = true;
	return 0;
}

HRESULT VDISocket::Initialize()
{
	HRESULT hr;
	
	hr = CoInitializeEx (NULL, COINIT_MULTITHREADED);
	if (!SUCCEEDED (hr))
		return hr;

	hr = CoCreateInstance(CLSID_MSSQL_ClientVirtualDeviceSet, NULL, CLSCTX_INPROC_SERVER, IID_IClientVirtualDeviceSet2, (void**)&deviceSet);
	if (!SUCCEEDED (hr))
		return hr;

	InitializeNetworking();
	initialized=true;

}

HRESULT VDISocket::InitializeNetworking(void)
{
	WSADATA wsaData;
	int r;
   
    struct addrinfo hints;

    // Initialize Winsock
    r = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (r != 0) {
        printf("WSAStartup failed with error: %d\n", r);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    r = getaddrinfo(NULL, Port, &hints, &addr_info );
    if ( r != 0 ) {
        printf("getaddrinfo failed with error: %d\n", r);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    listener = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (listener == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(addr_info);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    bind( listener, addr_info->ai_addr, (int)addr_info->ai_addrlen);
    if (r == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(addr_info);
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addr_info);

	return 0;
}

HRESULT VDISocket::Listen(void)
{
	HRESULT r;
	r = listen(listener, SOMAXCONN);
    if (r == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

	luge = accept(listener, NULL, NULL);
	if(luge == INVALID_SOCKET)
	{
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;		
	}

	closesocket(listener);
	return 0;
}

int VDISocket::CreateVirtualDevice(std::string instanceName)
{
	HRESULT hr;
	std::wstring wInstanceName = std::wstring(instanceName.begin(), instanceName.end());
	
	hr = deviceSet->CreateEx(wInstanceName.c_str(), deviceName, &config);
	if (!SUCCEEDED (hr))
		return hr;
	deviceOpened = true;
	return 0;
}

HRESULT VDISocket::CleanupNetworking(void)
{
	WSACleanup();
	freeaddrinfo(addr_info);
	closesocket(luge);
	closesocket(listener);
	return 0;
}

int VDISocket::DestroyVirtualDevice(void)
{
	if(deviceSet != NULL && deviceOpened)
	{
		deviceSet->Close();
		deviceSet->Release();
	}
	deviceOpened = false;

	return 0;
}

