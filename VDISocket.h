#include <string>
#include <objbase.h>
#include "vdi.h"

using namespace std;

#pragma once
class VDISocket
{
public:
	VDISocket(LPCSTR portNumber);
	~VDISocket(void);
	
	HRESULT	Initialize();
	int		StartOperation(std::string instanceName, std::string databaseName);
	
	virtual bool	CanStartOperation(void) = 0;
	
	LPCSTR Port;

protected:
	HRESULT	OpenDevice(void);
	
	virtual	void	PerformOperation(void) = 0;
	virtual	void	TransferData(void) = 0;

	IClientVirtualDevice*	device;
	WCHAR					deviceName [50];
	SOCKET					luge;
	SOCKET					listener;

private:
	HRESULT	CleanupNetworking(void); 
	int		CreateVirtualDevice(std::string instanceName);
	int		DestroyVirtualDevice(void);
	HRESULT	InitializeNetworking(void);
	HRESULT	Listen(void);

};

