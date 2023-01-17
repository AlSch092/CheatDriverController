// CheatDriverController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include "FindPattern.hpp"

int main(int argc, char** argv)
{
	std::string procName;
	UINT64 Address = 0;
	int bytesToReadWrite = 0;
	char ReadWrite = 'R';
		
	BYTE buff[28] = { 0 }; //read memory buffer
	BYTE ret[] = { 0xC3 }; //bytes which will be written to the remote process and then executed as a system thread. be careful for BSODs!

	if (argc > 2)
	{
		procName = argv[1];
		Address = _atoi64(argv[2]);
		bytesToReadWrite = atoi(argv[3]);
		ReadWrite = argv[4]
			
		//need to convert procName to a wide string. . .
		std::wstring wProcName;
		
		if(ReadWrite == 'R')
		{
			if (DriverController::ReadMemory(DriverController::FindProcessId(wProcName.c_str()), Address, buff, bytesToReadWrite, "./result.out"))
			{
				printf("Read memory!\n");
			}
		}
		else if(ReadWrite == 'W')
		{
		    if (DriverController::WriteMemory(DriverController::FindProcessId(wProcName.c_str()), Address, (const LPBYTE)ret, 1, NULL)) //will finish 'write' a bit later
	            {
		        printf("Wrote memory!\n");
	            }
		}
	}
	else
	{
		if (DriverController::WriteAndExecuteShell(DriverController::FindProcessId(L"notepad.exe"), 0x7ff787d23000, (const LPBYTE)ret, 1, NULL))
		{
			printf("Wrote shell and executed!\n");
		}
	
		if (DriverController::ReadMemory(DriverController::FindProcessId(L"notepad.exe"), 0x7ff787d23000, buff, 1, "./result.out"))
		{
			printf("Read memory!\n");
		}	
	}

	
         //we can also turn out driver into a pattern scanner! The process below is normally protected by an anti-cheat driver, which we can get past now
         //unsigned long long addr = Pattern::FindPattern(L"Albion-Online.exe", GameObjectManagerPattern, "xxx??xxxx??xxxx??xxx??xxxxxx", 0, DriverController::GetBaseAddress(L"Albion-Online.exe"), FALSE); 

	return 0;
}
