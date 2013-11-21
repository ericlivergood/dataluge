#define _WIN32_DCOM

#include "VDISocket.h"
#include <thread>
#include <stdio.h>      // for file operations
#include "vdi.h"        // interface declaration
#include "vdierror.h"   // error constants
#include "vdiguid.h"    // define the interface identifiers.
						// IMPORTANT: vdiguid.h can only be included in one source file.
						// 

VDConfig config;
IClientVirtualDeviceSet2* deviceSet;
int ServerConnectTimeOut = 10000;
bool deviceOpened;
bool initialized;

VDISocket::VDISocket(int portNumber)
{
	deviceSet = NULL; 
	deviceOpened = false;
	initialized = false;

	memset(&config, 0, sizeof(config));
	config.deviceCount = 1;

	GUID deviceID;
	CoCreateGuid(&deviceID);
	StringFromGUID2(deviceID, deviceName, 49);
}


VDISocket::~VDISocket(void)
{
	DestroyVirtualDevice();
	CoUninitialize();
}

int VDISocket::StartOperation(std::string instanceName, std::string databaseName)
{
	HRESULT h;
	if(!initialized)
	{
		return 1234;
	}

	h = CreateVirtualDevice(instanceName);
	if(!SUCCEEDED(h))
		return h;

	std::thread t(&VDISocket::PerformOperation, this);
	t.join();

	h = DestroyVirtualDevice();
	if(!SUCCEEDED(h))
		return h;

	return 0;
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

	initialized=true;
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


void VDISocket::performTransfer(IClientVirtualDevice*   vd, int backup)
{
    FILE *          fh;
    char*           fname = "superbak.dmp";
    VDC_Command *   cmd;
    DWORD           completionCode;
    DWORD           bytesTransferred;
    HRESULT         hr;

    fopen_s(&fh, fname, (backup)? "wb" : "rb");
    if (fh == NULL )
    {
        printf ("Failed to open: %s\n", fname);
        return;
    }

    while (SUCCEEDED (hr=vd->GetCommand (INFINITE, &cmd)))
    {
        bytesTransferred = 0;
        switch (cmd->commandCode)
        {
            case VDC_Read:
                bytesTransferred = fread (cmd->buffer, 1, cmd->size, fh);
                if (bytesTransferred == cmd->size)
                    completionCode = ERROR_SUCCESS;
                else
                    // assume failure is eof
                    completionCode = ERROR_HANDLE_EOF;
                break;

            case VDC_Write:
                bytesTransferred = fwrite (cmd->buffer, 1, cmd->size, fh);
                if (bytesTransferred == cmd->size )
                {
                    completionCode = ERROR_SUCCESS;
                }
                else
                    // assume failure is disk full
                    completionCode = ERROR_DISK_FULL;
                break;

            case VDC_Flush:
                fflush (fh);
                completionCode = ERROR_SUCCESS;
                break;
    
            case VDC_ClearError:
                completionCode = ERROR_SUCCESS;
                break;

            default:
                // If command is unknown...
                completionCode = ERROR_NOT_SUPPORTED;
        }

        hr = vd->CompleteCommand (cmd, completionCode, bytesTransferred, 0);
        if (!SUCCEEDED (hr))
        {
            printf ("Completion Failed: x%X\n", hr);
            break;
        }
    }

    if (hr != VD_E_CLOSE)
    {
        printf ("Unexpected termination: x%X\n", hr);
    }
    else
    {
        // As far as the data transfer is concerned, no
        // errors occurred.  The code which issues the SQL
        // must determine if the backup/restore was
        // really successful.
        //
        printf ("Successfully completed data transfer.\n");
    }

    fclose (fh);
}
