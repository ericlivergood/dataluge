#pragma once
#ifndef BackupSocket_H
#define BackupSocket_H
#include "VDISocket.h"

class BackupSocket :
	public VDISocket
{
public:
	BackupSocket(void);
	~BackupSocket(void);
	virtual bool CanStartOperation(void);

protected:
	virtual void PerformOperation(void);
	virtual void TransferData(void);

private:
	void RunBackup(void);
};
#endif
