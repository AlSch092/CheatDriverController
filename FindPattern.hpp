#pragma once
#include "Controller.hpp"

namespace Pattern
{
	BOOL Check(const BYTE* pData, const BYTE* bMask, const char* szMask);
	unsigned long long FindPattern(CONST IN wchar_t* processName, CONST BYTE* bMask, CONST char* szMask, unsigned long long dwOffset, unsigned long long dwStartAddress, BOOL DumpBytes);
}
