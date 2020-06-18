// ProcessList.h: interface for the CProcessList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSLIST_H__3A77A369_CD09_4A33_9CF2_BC09501ACCCF__INCLUDED_)
#define AFX_PROCESSLIST_H__3A77A369_CD09_4A33_9CF2_BC09501ACCCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wchar.h>
#include <stdio.h>
#include "PSAPI.h"


class CProcessList : public CPSAPI
{
public:
	LPCWSTR GetExeName(void) { return m_szExeName; }
	BOOL OnProcess(LPCWSTR lpszFileName, DWORD ProcessID);
	CProcessList(DWORD dwProcessID = 0);
	virtual ~CProcessList();

protected:
	WCHAR m_szExeName[MAX_PATH];
	DWORD m_dwProcessID;
	BOOL OnModule(HMODULE hModule, LPCWSTR lpszModuleName, LPCWSTR lpszPathName);
	void SetPriority(DWORD dwID, DWORD dwPriority);
};

#endif // !defined(AFX_PROCESSLIST_H__3A77A369_CD09_4A33_9CF2_BC09501ACCCF__INCLUDED_)
