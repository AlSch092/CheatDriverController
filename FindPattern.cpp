#include "FindPattern.hpp"

BOOL Pattern::Check(const BYTE* pData, const BYTE* bMask, const char* szMask) {

	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask != '?' && *pData != *bMask)
			return false;

	return (*szMask) == NULL;
}

//todo: convert this into kernel driver function and do all of this inside the driver to acces ZwOpenProcess
unsigned long long Pattern::FindPattern(CONST IN wchar_t* processName, CONST BYTE* bMask, CONST char* szMask, UINT64 dwOffset, UINT64 dwStartAddress, BOOL DumpBytes)
{
	unsigned long long dwLen = 0x10000000000;
	int bufferLength = strlen(szMask);

	if (!bufferLength)
		return 0;

	DWORD ProcessId = DriverController::FindProcessId(processName);

	printf("buffer length: %d\n", bufferLength);

	for (unsigned long long i = 0; i < dwLen; i++)
	{		
		BYTE* readBytes = new BYTE[bufferLength];

		if(DumpBytes)
			DriverController::ReadMemory(ProcessId, dwStartAddress + i, readBytes, bufferLength, "./outBytes.pattern"); //todo: modify this to use larger buffer length instead of 1 to increase performance
		else
			DriverController::ReadMemory(ProcessId, dwStartAddress + i, readBytes, bufferLength, NULL);

		if (Check(readBytes, bMask, szMask))
		{
			UINT64 FoundAddress = (UINT64)(dwStartAddress + i + dwOffset);
			printf("[PATTERN] Found: %llX\n", FoundAddress);
			std::ofstream binFile("./Pattern.out.bin", std::ios::app);
			binFile << FoundAddress;
			binFile.close();
			return FoundAddress;
		}

	}

	return 0;
}
