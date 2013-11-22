#include "VDISocket.h"

#pragma once
class BackupSocket :
	public VDISocket
{
public:
	BackupSocket(void);
	~BackupSocket(void);
	virtual bool CanStartOperation(void);

protected:
	virtual void PerformOperation(std::string instanceName, std::string databaseName);
	virtual void TransferData(void);

private:
	void RunBackup(std::string instanceName, std::string databaseName);
};
