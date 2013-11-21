#include <string>
#include <objbase.h>
#include "vdi.h"

using namespace std;
#ifndef VDISocket_H
#define VDISocket_H
class VDISocket
{
public:
	VDISocket(int portNumber);
	~VDISocket(void);

	HRESULT Initialize();
	int StartOperation(std::string instanceName, std::string databaseName);
	virtual bool CanStartOperation(void) = 0;

protected:
	HRESULT OpenDevice(void);
	virtual void PerformOperation(void) = 0;
	virtual void TransferData(void) = 0;

	WCHAR deviceName [50];
	IClientVirtualDevice* device;
private:
	int CreateVirtualDevice(std::string instanceName);
	int DestroyVirtualDevice(void);
	void performTransfer(IClientVirtualDevice* vd, int backup);
};
#endif
