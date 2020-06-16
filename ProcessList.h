// ProcessList.h: interface for the CProcessList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSLIST_H__3A77A369_CD09_4A33_9CF2_BC09501ACCCF__INCLUDED_)
#define AFX_PROCESSLIST_H__3A77A369_CD09_4A33_9CF2_BC09501ACCCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PSAPI.h"

class CProcessList : public CPSAPI  
{
public:
	LPCSTR GetExeName(void) { return m_szExeName; }
	BOOL OnProcess(LPCTSTR lpszFileName, DWORD ProcessID);
	CProcessList(DWORD dwProcessID = 0);
	virtual ~CProcessList();

protected:
	char m_szExeName[MAX_PATH];
	DWORD m_dwProcessID;
	BOOL OnModule(HMODULE hModule, LPCTSTR lpszModuleName, LPCTSTR lpszPathName);
	void SetPriority(DWORD dwID, DWORD dwPriority);
};

#endif // !defined(AFX_PROCESSLIST_H__3A77A369_CD09_4A33_9CF2_BC09501ACCCF__INCLUDED_)
