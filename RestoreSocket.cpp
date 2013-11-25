#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <process.h> 
#include <thread>

#include "RestoreSocket.h"


RestoreSocket::RestoreSocket(void)
	: VDISocket("9002")
{
}


RestoreSocket::~RestoreSocket(void)
{
}


bool RestoreSocket::CanStartOperation(void)
{
	return true;
}

void RestoreSocket::RunRestore(std::string instanceName, std::string databaseName)
{
	char sqlCommand[1024];

	sprintf_s(sqlCommand, 1024, "-Q \"RESTORE DATABASE pubs2 FROM VIRTUAL_DEVICE='%ls'\"", deviceName);
	
	int rc;
	printf ("running osql to execute: %s\n", sqlCommand);

	rc = _spawnlp( _P_WAIT, "osql", "osql", "-E", "-b", sqlCommand, NULL);

    if (rc == -1)
    {
        printf ("Spawn failed with error: %d\n", errno);
    }
}

void RestoreSocket::TransferData(void)
{
	VDC_Command* cmd;
	DWORD bytes;
	HRESULT r;

	if(device == NULL)
	{
		printf("restore device not initialized");
		return;
	}

	while(SUCCEEDED(r=device->GetCommand(INFINITE, &cmd)))
	{
		switch(cmd->commandCode) 
		{
			case VDC_Read:
				recv(luge, (char*) cmd->buffer, cmd->size, 0);
				bytes = cmd->size;
				device->CompleteCommand(cmd, ERROR_SUCCESS, bytes, 0);
				printf("Transferred %u bytes\n", bytes);
				break;
			case VDC_Flush:
				device->CompleteCommand(cmd, ERROR_SUCCESS, bytes, 0);
			default:
				device->CompleteCommand(cmd, ERROR_NOT_SUPPORTED, bytes, 0);
		}
	}
    // shutdown the connection since we're done
    r = shutdown(luge, SD_SEND);
    if (r == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(luge);
        WSACleanup();
    }
}

void RestoreSocket::PerformOperation(std::string instanceName, std::string databaseName)
{
	std::thread o(&RestoreSocket::OpenDevice, this);
	std::thread r(&RestoreSocket::RunRestore, this, instanceName, databaseName);
	std::thread w(&RestoreSocket::TransferData, this);
	o.join();
	r.join();
	w.join();
}