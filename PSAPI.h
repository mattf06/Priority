// PSAPI.h: interface for the CPSAPI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__PSAPI_H__)
#define __PSAPI_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <Psapi.h>

class CPSAPI
{
public:
	CPSAPI();
	virtual ~CPSAPI();

	BOOL Initialize(void);

	BOOL EnumDeviceDrivers(void);
	BOOL EnumProcesses(void);
	BOOL EnumProcessModules(DWORD dwProcessId);
	//	BOOL EnumModuleFileName(HANDLE hProcess);

	virtual BOOL OnDeviceDriver(LPVOID lpImageBase);
	virtual BOOL OnProcess(LPCTSTR lpszFileName, DWORD ProcessID);
	virtual BOOL OnModule(HMODULE hModule, LPCTSTR lpszModuleName, LPCTSTR lpszPathName);

};

#endif // !defined(__PSAPI_H__)
