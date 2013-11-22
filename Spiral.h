#include <objbase.h>

#pragma once
//The "spiral" is the track on which one luges.  Kinda looks like a pipe.
//This class is intended to facilitate to transfer between two of our VDISockets, or a pipe.
//Super simple implementation for now:
//	No buffering, no name resolution; just connect to both sides call recv then send
class Spiral
{
public:
	Spiral(void);
	~Spiral(void);
	HRESULT Initialize();
	HRESULT Transfer();

private:
	HRESULT NetworkingCleanup(void);
	
	static HRESULT ConnectTo(SOCKET* Socket, LPCSTR Address, LPCSTR Port);

	LPCSTR SourcePort;
	LPCSTR SourceAddr;
	LPCSTR DestPort;
	LPCSTR DestAddr;
	SOCKET Source;
	SOCKET Dest;
};

