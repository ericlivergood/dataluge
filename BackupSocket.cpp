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

void BackupSocket::PerformOperation(void)
{
	std::thread r(&BackupSocket::RunBackup, this);
	OpenDevice();
	std::thread w(&BackupSocket::TransferData, this);
	r.join();
	w.join();
}

void BackupSocket::TransferData(void)
{
	VDC_Command* cmd;
	DWORD bytes;
	HRESULT r;

	while(SUCCEEDED(r=device->GetCommand(INFINITE, &cmd)))
	{
		if(cmd->commandCode != VDC_Write)
		{
			printf("wrong command type? %X\n", cmd->commandCode);
		}
		else
		{
			//take the buffer and push it into the socket here.
			bytes = cmd->size;
			device->CompleteCommand(cmd, ERROR_SUCCESS, bytes, 0);
			printf("Transferred %u bytes\n", bytes);
		}
	}

}

void BackupSocket::RunBackup(void)
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