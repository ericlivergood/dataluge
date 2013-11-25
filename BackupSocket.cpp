#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#include "BackupSocket.h"
#include <process.h> 
#include <thread>

BackupSocket::BackupSocket(void) 
	: VDISocket("9001")
{
}


BackupSocket::~BackupSocket(void)
{
}


bool BackupSocket::CanStartOperation(void)
{
	return true;
}

void BackupSocket::PerformOperation(std::string instanceName, std::string databaseName)
{
	std::thread o(&BackupSocket::OpenDevice, this);
	std::thread r(&BackupSocket::RunBackup, this, instanceName, databaseName);
	std::thread w(&BackupSocket::TransferData, this);
	o.join();
	r.join();
	w.join();
}

void BackupSocket::TransferData(void)
{
	VDC_Command* cmd;
	DWORD bytes;
	HRESULT r;

	while(device == NULL)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	while(SUCCEEDED(r=device->GetCommand(INFINITE, &cmd)))
	{
		switch(cmd->commandCode)
		{
			case VDC_Write:
				send(luge, (const char*)cmd->buffer, cmd->size, 0);
				bytes = cmd->size;
				device->CompleteCommand(cmd, ERROR_SUCCESS, bytes, 0);
				printf("Transferred %u bytes\n", bytes);
				break;
			case VDC_Flush:
				//we're done here.
				device->CompleteCommand(cmd, ERROR_SUCCESS, bytes, 0);
				break;
			default:
				device->CompleteCommand(cmd, ERROR_NOT_SUPPORTED, 0, 0);

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

void BackupSocket::RunBackup(std::string instanceName, std::string databaseName)
{
	char sqlCommand[1024];

	sprintf_s(sqlCommand, 1024, "-Q \"BACKUP DATABASE pubs TO VIRTUAL_DEVICE='%ls'\"", deviceName);
	
	int rc;
	printf ("running osql to execute: %s\n", sqlCommand);

    // this is probably going to be gross if the osql call fails...
	rc = _spawnlp( _P_WAIT, "osql", "osql", "-E", "-b", sqlCommand, NULL);

    if (rc == -1)
    {
        printf ("Spawn failed with error: %d\n", errno);
    }
}