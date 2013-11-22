#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include "BackupSocket.h"
#include "RestoreSocket.h"
#include "Spiral.h"
#include "vdierror.h"   // error constants

void printError(HRESULT code);
//
// main function
//
int main(int argc, char *argv[])
{
	HRESULT h;
	BackupSocket* b = new BackupSocket();
	RestoreSocket* r = new RestoreSocket();
	Spiral* s = new Spiral();

	printError(b->Initialize());
	printError(r->Initialize());
	printError(s->Initialize());

	std::thread backup = b->StartOperation(NULL, "pubs"); 
	std::thread restore = r->StartOperation(NULL, "pubs2");
	
	printError(s->Transfer());
	backup.join();
	restore.join();

	int x;

	delete b, r, s;
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