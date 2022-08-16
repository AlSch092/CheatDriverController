#pragma once
#include <Windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <fstream>
#include "../src/driver_config.h" //these are from 'cheat-driver' project repo
#include "../src/driver_codes.h"

namespace DriverController
{
	DWORD FindProcessId(const std::wstring& processName);
  UINT64 GetBaseAddress(const IN wchar_t* imgName);
  
	bool ReadMemory(DWORD ProcessId, UINT64 Address, LPBYTE Buffer, INT Size, const char* OptFileName);
	bool WriteMemory(IN DWORD ProcessId, IN UINT64 Address, IN LPBYTE Buffer, IN INT Size);
	bool WriteAndExecuteShell(IN DWORD ProcessId, IN UINT64 Address, CONST IN LPBYTE Buffer, IN INT Size, PVOID OptionalParameter);

}
