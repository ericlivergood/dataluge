#pragma once
#ifndef RestoreSocket_H
#define RestoreSocket_H
#include "VDISocket.h"
class RestoreSocket :
	public VDISocket
{
public:
	RestoreSocket(void);
	~RestoreSocket(void);
	virtual bool CanStartOperation(void);

protected:
	virtual void PerformOperation(void);
	virtual void TransferData(void);

private:
	void RunRestore(void);
};
#endif

