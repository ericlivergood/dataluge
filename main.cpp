#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "BackupSocket.h"
#include "RestoreSocket.h"
#include "vdierror.h"   // error constants

void printError(HRESULT code);
//
// main function
//
int main(int argc, char *argv[])
{
	HRESULT h;
	BackupSocket* b = new BackupSocket();
	//RestoreSocket* r = new RestoreSocket();

	h = b->Initialize();
	printError(h);
	h = b->StartOperation("", "pub");
	printError(h);


	int x;
	cin >> x;
}

void printError(HRESULT code)
{
	switch(code){
	case 0:
		printf("success\n");
		break;
	case VD_E_TIMEOUT:
		printf("Connection timed out\n");
		break;
	default:
		printf("Exit code: x%X\n", code);
		break;
	}

}