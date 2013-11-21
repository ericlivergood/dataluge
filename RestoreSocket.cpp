#include "RestoreSocket.h"
#include <process.h> 
#include <thread>


RestoreSocket::RestoreSocket(void)
	: VDISocket(9002)
{
}


RestoreSocket::~RestoreSocket(void)
{
}


bool RestoreSocket::CanStartOperation(void)
{
	return true;
}

void RestoreSocket::RunRestore(void)
{
	char sqlCommand[1024];

	sprintf_s(sqlCommand, 1024, "-Q \"RESTORE DATABASE pubs FROM VIRTUAL_DEVICE='%ls'\"", deviceName);
	
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

	while(SUCCEEDED(r=device->GetCommand(INFINITE, &cmd)))
	{
		if(cmd->commandCode != VDC_Read)
		{
			printf("wrong command type? %X\n", cmd->commandCode);
		}
		else
		{
			//fill the buffer by reading from the socket here.
			bytes = cmd->size;
			device->CompleteCommand(cmd, ERROR_SUCCESS, bytes, 0);
			printf("Transferred %u bytes\n", bytes);
		}
	}
}

void RestoreSocket::PerformOperation(void)
{
	std::thread r(&RestoreSocket::RunRestore, this);
	OpenDevice();
	std::thread w(&RestoreSocket::TransferData, this);
	r.join();
	w.join();
}