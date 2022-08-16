// CheatDriverController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include "FindPattern.hpp"

int main(int argc, char** argv)
{
    std::cout << "Hello World!\n";

	if (argc > 2) //will finish cmd line args a bit later
	{
		std::string procName = argv[2];
		UINT64 Address = _atoi64(argv[3]);
		//convert ascii bytes to LPBYTE
	}

	//const BYTE GameObjectManagerPattern[] = { 0x89,0x5C,0x24,'?','?',0x48,0x89,0x74,0x24,'?','?',0x57,0x48,0x83,0xEC,'?','?',0x48,0x8D,0x59,'?','?',0x48,0x8B,0xF9,0x48,0x8B,0x0B };

  BYTE ret[] = { 0xC3 }; //bytes which will be written to the remote process and then executed as a system thread. be careful for BSODs!

	if (DriverController::WriteAndExecuteShell(DriverController::FindProcessId(L"notepad.exe"), 0x7ff787d23000, (const LPBYTE)ret, 1, NULL))
	{
		printf("Wrote shell and executed!\n");
	}

	BYTE buff[28] = { 0 };
	if (DriverController::ReadMemory(DriverController::FindProcessId(L"notepad.exe"), 0x7ff787d23000, buff, 1, "./result.out"))
	{
		printf("Read memory!\n");
	}
	
  //we can also turn out driver into a pattern scanner
  //unsigned long long addr = Pattern::FindPattern(L"Albion-Online.exe", GameObjectManagerPattern, "xxx??xxxx??xxxx??xxx??xxxxxx", 0, DriverController::GetBaseAddress(L"Albion-Online.exe"), FALSE); 
	
	//if(addr != 0)
	//	printf("Addr: %llX\n", addr);

	system("pause");
	return 0;
}
