#include "Controller.hpp"
#include <psapi.h>

//TODO: Make routine to Create thread in remote process 
//TODO: Make routine to write memory in any process for injecting a DLL (write memory, call PsCreateThread which calls LoadLibraryA)

DWORD DriverController::FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

UINT64 DriverController::GetBaseAddress(const IN wchar_t* imgName) //needs to be in the driver for protected apps
{
	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, DriverController::FindProcessId(imgName));
	HMODULE hMods[1024];
	DWORD cbNeeded;

	if (hProc)
	{
		EnumProcessModules(hProc, hMods, sizeof(hMods), &cbNeeded);

		for (int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProc, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.
				if (wcsstr(szModName, imgName) != NULL)
				{
					printf("Found module..\n");

					//get base addr
					MODULEINFO lpm;
					if (GetModuleInformation(hProc, hMods[i], &lpm, sizeof(lpm)))
					{
						printf("Address: %llX\n", (UINT64)lpm.lpBaseOfDll);
						printf("EntryPoint: %llX\n", (UINT64)lpm.EntryPoint);
						printf("SizeOfImage: %d\n", lpm.SizeOfImage);
						return (UINT64)lpm.lpBaseOfDll;
					}

				}
				wprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}

		CloseHandle(hProc);
	}

	return 0;
}

bool DriverController::ReadMemory(DWORD ProcessId, UINT64 Address, LPBYTE Buffer, INT Size, const char* OptFileName) //todo: change this to return # bytes read if possible
{
	HANDLE driver = CreateFileW(DRIVER_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	DRIVER_COPY_MEMORY copy = {};
	copy.ProcessId = ProcessId;
	copy.Source = (unsigned long long)Buffer;
	copy.Target = (unsigned long long)Address;
	copy.Size = Size;
	copy.Write = FALSE;
	std::ofstream binFile;

	if(OptFileName != NULL)
		binFile = std::ofstream(OptFileName, std::ios::app);

	//printf("Reading memory..\n");

	if (DeviceIoControl(driver, IOCTL_DRIVER_COPY_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), 0, 0))
	{
		for (int i = 0; i < Size; i++)
		{
			if (OptFileName != NULL)
				binFile << Buffer[i];
			
			printf("%x ", Buffer[i]);
		}

		//printf("\n");
		if (OptFileName != NULL)
			binFile.close();	
	}
	else
	{
		printf("Sending driver control failed: %d\n", GetLastError());
		return false;
	}

	return true;
}

bool DriverController::WriteMemory(IN DWORD ProcessId, IN UINT64 Address, IN LPBYTE Buffer, IN INT Size)
{
	HANDLE driver = CreateFileW(DRIVER_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	DRIVER_COPY_MEMORY copy = {};
	copy.ProcessId = ProcessId;
	copy.Source = (unsigned long long)Buffer;
	copy.Target = (unsigned long long)Address;
	copy.Size = Size;
	copy.Write = TRUE;

	if (DeviceIoControl(driver, IOCTL_DRIVER_COPY_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), 0, 0))
	{
		printf("Wrote memory...\n");
	}
	else
	{
		printf("Sending driver control failed: %d\n", GetLastError());
		return false;
	}

	return true;
}

bool DriverController::WriteAndExecuteShell(IN DWORD ProcessId, IN UINT64 Address, CONST IN LPBYTE Buffer, IN INT Size, PVOID OptionalParameter)
{
	HANDLE driver = CreateFileW(DRIVER_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

	DRIVER_COPY_MEMORY shell = {};
	shell.ProcessId = ProcessId;
	shell.Source = (unsigned long long)Buffer;
	shell.Target = (unsigned long long)Address;
	shell.Size = Size;
	shell.OptionalParameter = OptionalParameter;
	shell.Write = TRUE;

	if (DeviceIoControl(driver, IOCTL_DRIVER_EXECUTE_SHELL, &shell, sizeof(shell), &shell, sizeof(shell), 0, 0))
	{
		printf("Sent shell write + execute control successfully...\n");
	}
	else
	{
		printf("Sending driver control failed: %d\n", GetLastError());
		return false;
	}

	return true;
}
