#include <objbase.h>

#pragma once
//The "spiral" is the track on which one luges.  Kinda looks like a pipe.
//This class is intended to facilitate to transfer between two of our VDISockets, or a pipe.
//Super simple implementation for now.
class Spiral
{
public:
	Spiral(void);
	~Spiral(void);
	HRESULT Transfer();

private:
	LPCSTR SourcePort;
	LPCSTR SourceAddr;
	LPCSTR DestPort;
	LPCSTR DestAddr;
};

