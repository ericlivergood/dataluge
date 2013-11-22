#include "VDISocket.h"

#pragma once
class RestoreSocket :
	public VDISocket
{
public:
	RestoreSocket(void);
	~RestoreSocket(void);
	virtual bool CanStartOperation(void);

protected:
	virtual void PerformOperation(std::string instanceName, std::string databaseName);
	virtual void TransferData(void);

private:
	void RunRestore(std::string instanceName, std::string databaseName);
};
