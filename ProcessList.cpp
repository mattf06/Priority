// ProcessList.cpp: implementation of the CProcessList class.
//
//////////////////////////////////////////////////////////////////////

#include "ProcessList.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessList::CProcessList(DWORD dwProcessID)
{
	m_dwProcessID = dwProcessID;
	ZeroMemory(m_szExeName, sizeof(m_szExeName));
}

CProcessList::~CProcessList()
{

}

BOOL CProcessList::OnProcess(LPCTSTR lpszFileName, DWORD ProcessID)
{
	char szBuffer[256];

	if (ProcessID < 10)
		return TRUE;

	/*
	CProcessList pl(ProcessID);
	pl.EnumProcessModules(ProcessID);

	if (!lstrlen(pl.GetExeName()))
		return TRUE;

	*/

	TCHAR lpszModuleName[MAX_PATH];

	lstrcpy(lpszModuleName, lpszFileName);

	LPSTR lpszPtr = lpszModuleName + lstrlen(lpszModuleName) - 1;
	while ((lpszPtr != lpszModuleName) && (*lpszPtr != '.'))
	{
		*lpszPtr = 0;
		lpszPtr--;
	}

	if (*lpszPtr == '.')
		*lpszPtr = 0;

	char * ptr = strrchr(lpszModuleName, '\\');
	if (ptr != NULL)
		strncpy_s(lpszModuleName,ptr+1, MAX_PATH);


#ifdef _DEBUG
	printf("%u > %s : %s\n", ProcessID, lpszFileName, lpszModuleName);
#endif

	GetPrivateProfileString(lpszModuleName, "PRIORITY", "ERROR", szBuffer, sizeof(szBuffer), "priority.ini");
	if (_strnicmp(szBuffer, "NORMAL", 6) == 0)
	{
		SetPriority(ProcessID, NORMAL_PRIORITY_CLASS);
	}
	else if (_strnicmp(szBuffer, "BELOW", 5) == 0)
	{
		SetPriority(ProcessID, BELOW_NORMAL_PRIORITY_CLASS);
	}
	else if (_strnicmp(szBuffer, "LOW", 3) == 0)
	{
		SetPriority(ProcessID, IDLE_PRIORITY_CLASS);
	}
	else if (_strnicmp(szBuffer, "ABOVE", 5) == 0)
	{
		SetPriority(ProcessID, ABOVE_NORMAL_PRIORITY_CLASS);
	}
	else if (_strnicmp(szBuffer, "HIGH", 4) == 0)
	{
		SetPriority(ProcessID, HIGH_PRIORITY_CLASS);
	}
	else if (_strnicmp(szBuffer, "REALTIME", 8) == 0)
	{
		SetPriority(ProcessID, REALTIME_PRIORITY_CLASS);
	}
	else if (_strnicmp(szBuffer, "OFF", 3) == 0)
	{
		HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, ProcessID);
		if (hProcess != NULL)
		{
			TerminateProcess(hProcess, 0); 
			CloseHandle(hProcess);
		}
	}

	return TRUE;
}

void CProcessList::SetPriority(DWORD dwID, DWORD dwPriority)
{
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION,FALSE, dwID);

	if (hProcess)
	{
		SetPriorityClass(hProcess, dwPriority);		
		CloseHandle(hProcess);
	}
}

BOOL CProcessList::OnModule(HMODULE hModule, LPCTSTR lpszModuleName, LPCTSTR lpszPathName)
{
	if (hModule)
		lstrcpy(m_szExeName, lpszPathName);
		
	return FALSE;
}
