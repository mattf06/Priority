// ProcessList.cpp: implementation of the CProcessList class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#include "ProcessList.h"
#include <tchar.h>
#include <atlstr.h>

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
	WCHAR szBuffer[1024] = L"";

	if (ProcessID < 10)
		return TRUE;


	WCHAR lpszWorkingDir[MAX_PATH];
	GetCurrentDirectory(sizeof(lpszWorkingDir), lpszWorkingDir);

	WCHAR lpszModuleName[MAX_PATH];

	lstrcpy(lpszModuleName, lpszFileName);

	LPWSTR lpszPtr = lpszModuleName + lstrlen(lpszModuleName) - 1;
	while ((lpszPtr != lpszModuleName) && (*lpszPtr != '.'))
	{
		*lpszPtr = 0;
		lpszPtr--;
	}

	if (*lpszPtr == '.')
		*lpszPtr = 0;

	WCHAR * ptr = wcsrchr(lpszModuleName, '\\');
	if (ptr != NULL) {
		wcscpy_s(lpszModuleName, MAX_PATH, ptr + 1);
	}

	CString dir(lpszWorkingDir);

	dir += L"\\priority.ini";

#ifdef _DEBUG
	//_tprintf(TEXT("PID=%u > Filename = %s | ModuleName = %s\n"), ProcessID, LPCWSTR(lpszFileName), LPCWSTR(lpszModuleName));
	_tprintf(TEXT("INI File: %s\n"), LPCWSTR(dir));
#endif

	GetPrivateProfileString(lpszModuleName, L"priority", L"", szBuffer, sizeof(szBuffer), dir);
#ifdef _DEBUG
	if (lstrlen(szBuffer) > 0)
	{
		_tprintf(TEXT("PID=%u > Process: %s | New priority: %s\n"), ProcessID, LPCWSTR(lpszModuleName), LPCWSTR(szBuffer));
	}
#endif
	if (_wcsnicmp(szBuffer, L"NORMAL", 6) == 0)
	{
		SetPriority(ProcessID, NORMAL_PRIORITY_CLASS);
	}
	else if (_wcsnicmp(szBuffer, L"BELOW", 5) == 0)
	{
		SetPriority(ProcessID, BELOW_NORMAL_PRIORITY_CLASS);
	}
	else if (_wcsnicmp(szBuffer, L"LOW", 3) == 0)
	{
		SetPriority(ProcessID, IDLE_PRIORITY_CLASS);
	}
	else if (_wcsnicmp(szBuffer, L"ABOVE", 5) == 0)
	{
		SetPriority(ProcessID, ABOVE_NORMAL_PRIORITY_CLASS);
	}
	else if (_wcsnicmp(szBuffer, L"HIGH", 4) == 0)
	{
		SetPriority(ProcessID, HIGH_PRIORITY_CLASS);
	}
	else if (_wcsnicmp(szBuffer, L"REALTIME", 8) == 0)
	{
		SetPriority(ProcessID, REALTIME_PRIORITY_CLASS);
	}
	else if (_wcsnicmp(szBuffer, L"OFF", 3) == 0)
	{
		HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, ProcessID);
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
	HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, dwID);

	if (hProcess)
	{
#ifdef _DEBUG
		_tprintf(TEXT("PID=%u > set priority to %u\n"), dwID, dwPriority);
#endif
		BOOL bRet = SetPriorityClass(hProcess, dwPriority);

#ifdef _DEBUG
		if (!bRet) {
			_tprintf(TEXT("/!\\ Can't adjust priority, LastError: %u\n"), GetLastError());
		}
#endif


		CloseHandle(hProcess);
	}
	else
	{
#ifdef _DEBUG
		_tprintf(TEXT("Can't OpenProcess PID=%u, LastError = %u\n"), dwID, GetLastError());
#endif

	}
}

BOOL CProcessList::OnModule(HMODULE hModule, LPCWSTR lpszModuleName, LPCWSTR lpszPathName)
{
	if (hModule)
		lstrcpy(m_szExeName, lpszPathName);

	return FALSE;
}
