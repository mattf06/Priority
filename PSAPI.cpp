/*
 * C++ Windows process, module enumeration class
 *
 * Written by Lee, JaeKil
 *        mailto:juria@gtech.co.kr
 *
 * Copyright (c) 1998-1999.
 *
 * This code may be used in compiled form in any way you desire. This
 * file may be redistributed unmodified by any means PROVIDING it is 
 * not sold for profit without the authors written consent, and 
 * providing that this notice and the authors name and all copyright 
 * notices remains intact. If the source code in this file is used in 
 * any  commercial application then a statement along the lines of 
 * "Portions copyright (c) Chris Maunder, 1998" must be included in 
 * the startup banner, "About" box or printed documentation. An email 
 * letting me know that you are using it would be nice as well. That's 
 * not much to ask considering the amount of work that went into this.
 *
 * This file is provided "as is" with no expressed or implied warranty.
 * The author accepts no liability for any damage/loss of business that
 * this product may cause.
 *
 * Expect bugs!
 * 
 * Please use and enjoy, and let me know of any bugs/mods/improvements 
 * that you have found/implemented and I will fix/incorporate them into 
 * this file. 
 */

// PSAPI.cpp: implementation of the CPSAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PSAPI.h"
#include <tlhelp32.h>
#include <vdmdbg.h>
#include <tchar.h>

#define	MAX_HANDLE_COUNT	1024

//////////////////////////////////////////////////////////////////////
// CPSAPI Construction/Destruction

CPSAPI::CPSAPI()
{
	OSVERSIONINFO ovi;
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	// PSAPI is for Windows NT verison 4.0
	m_bNeedPSAPI = ovi.dwPlatformId == VER_PLATFORM_WIN32_NT && ovi.dwMajorVersion <= 4;
}

BOOL CPSAPI::Initialize(void)
{
	if ( m_bNeedPSAPI )
	{
		m_modPSAPI = LoadLibrary(_T("PSAPI.DLL"));
		m_modVDMDBG = LoadLibrary(_T("VDMDBG.DLL"));

		if ( m_modPSAPI && m_modVDMDBG )
			return TRUE;
		return FALSE;
	}
	else
	{
	}

	return TRUE;
}

CPSAPI::~CPSAPI()
{
	if ( m_bNeedPSAPI )
	{
		FreeLibrary((HINSTANCE)m_modVDMDBG);
		FreeLibrary((HINSTANCE)m_modPSAPI);
	}
	else
	{
	}
}

//Windows NT Functions
typedef BOOL (WINAPI *ENUMPROCESSES)(
  DWORD * lpidProcess,  // array to receive the process identifiers
  DWORD cb,             // size of the array
  DWORD * cbNeeded      // receives the number of bytes returned
);

typedef BOOL (WINAPI *ENUMPROCESSMODULES)(
  HANDLE hProcess,      // handle to the process
  HMODULE * lphModule,  // array to receive the module handles
  DWORD cb,             // size of the array
  LPDWORD lpcbNeeded    // receives the number of bytes returned
);

typedef DWORD (WINAPI *GETMODULEFILENAMEA)( 
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPSTR lpstrFileName,	// array to receive filename
  DWORD nSize			// size of filename array.
);

typedef DWORD (WINAPI *GETMODULEFILENAMEW)( 
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPWSTR lpstrFileName,	// array to receive filename
  DWORD nSize			// size of filename array.
);

#ifdef	UNICODE
#define	GETMODULEFILENAME	GETMODULEFILENAMEW
#else
#define	GETMODULEFILENAME	GETMODULEFILENAMEA
#endif

typedef DWORD (WINAPI *GETMODULEBASENAMEA)( 
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPTSTR lpstrFileName,	// array to receive base name of module
  DWORD nSize			// size of module name array.
);

typedef DWORD (WINAPI *GETMODULEBASENAMEW)( 
  HANDLE hProcess,		// handle to the process
  HMODULE hModule,		// handle to the module
  LPTSTR lpstrFileName,	// array to receive base name of module
  DWORD nSize			// size of module name array.
);

#ifdef	UNICODE
#define	GETMODULEBASENAME	GETMODULEBASENAMEW
#else
#define	GETMODULEBASENAME	GETMODULEBASENAMEA
#endif

typedef INT (WINAPI *VDMENUMTASKWOWEX)(
  DWORD dwProcessId,	// ID of NTVDM process
  TASKENUMPROCEX fp,	// address of our callback function
  LPARAM lparam);		// anything we want to pass to the callback function.



// Win95 functions
typedef HANDLE (WINAPI *CREATESNAPSHOT)(
    DWORD dwFlags, 
    DWORD th32ProcessID
); 

typedef BOOL (WINAPI *PROCESSWALK)(
    HANDLE hSnapshot,    
    LPPROCESSENTRY32 lppe
);
 
typedef BOOL (WINAPI *MODULEWALK)(
	HANDLE hSnapshot,
	LPMODULEENTRY32 lpme
);
 


BOOL CPSAPI::EnumDeviceDrivers(void)
{
	return FALSE;
}

BOOL CPSAPI::EnumProcesses(void)
{
	if ( m_bNeedPSAPI )
	{
		if ( NULL == m_modPSAPI || NULL == m_modVDMDBG )
			return FALSE;

		ENUMPROCESSES       pEnumProcesses;
		GETMODULEFILENAME   pGetModuleFileName;
		ENUMPROCESSMODULES  pEnumProcessModules;  
		VDMENUMTASKWOWEX	pVDMEnumTaskWOWEx;
		GETMODULEBASENAME	pGetModuleBaseName;

		pVDMEnumTaskWOWEx = (VDMENUMTASKWOWEX)GetProcAddress(m_modVDMDBG, "VDMEnumTaskWOWEx");
		pEnumProcesses = (ENUMPROCESSES)GetProcAddress(m_modPSAPI, "EnumProcesses");
#ifdef	UNICODE
		pGetModuleFileName = (GETMODULEFILENAME)GetProcAddress(m_modPSAPI, "GetModuleFileNameExW");
		pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(m_modPSAPI, "GetModuleBaseNameW");
#else
		pGetModuleFileName = (GETMODULEFILENAME)GetProcAddress(m_modPSAPI, "GetModuleFileNameExA");
		pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(m_modPSAPI, "GetModuleBaseNameA");
#endif
		pEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(m_modPSAPI, "EnumProcessModules");

		if (
			NULL == pVDMEnumTaskWOWEx	||
			NULL == pEnumProcesses		|| 
			NULL == pGetModuleFileName	|| 
			NULL == pGetModuleBaseName	||
			NULL == pEnumProcessModules  )
			return FALSE;

		DWORD nProcessIDs[MAX_HANDLE_COUNT];
		DWORD nProcessNo;

		BOOL bSuccess = pEnumProcesses(nProcessIDs, sizeof(nProcessIDs), &nProcessNo);
		nProcessNo /= sizeof(nProcessIDs[0]);

		if ( !bSuccess )
		{
			return FALSE;
		}  

		for ( unsigned i=0; i<nProcessNo; i++)
		{
			HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
								FALSE, nProcessIDs[i]);

			HMODULE hModules[MAX_HANDLE_COUNT];
			DWORD nModuleNo;
			TCHAR szFileName[MAX_PATH];

			pEnumProcessModules(process, hModules, sizeof(hModules), &nModuleNo);

			nModuleNo /= sizeof(hModules[0]);

			if ( pGetModuleFileName(process, hModules[0], szFileName, sizeof(szFileName)) )
			{
				if ( OnProcess(szFileName, nProcessIDs[i]) == FALSE )
					break;

				pGetModuleBaseName(process, hModules[0], szFileName, sizeof(szFileName));

				if ( 0 == _tcsicmp(szFileName, _T("NTVDM.EXE")) )
				{
					// We've got an NT VDM -- enumerate the processes
					// it contains.
//					pVDMEnumTaskWOWEx(nProcessIDs[i], show_task, (long)&disp);
				}
			}
			CloseHandle(process);
		}

		return TRUE;
	}
	else
	{
		HINSTANCE modKERNEL = GetModuleHandle(_T("KERNEL32.DLL"));

		CREATESNAPSHOT CreateToolhelp32Snapshot; 
		PROCESSWALK pProcess32First; 
		PROCESSWALK pProcess32Next; 

		CreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(modKERNEL, "CreateToolhelp32Snapshot"); 
		pProcess32First = (PROCESSWALK)GetProcAddress(modKERNEL, "Process32First"); 
		pProcess32Next  = (PROCESSWALK)GetProcAddress(modKERNEL, "Process32Next"); 

		if (
			NULL == CreateToolhelp32Snapshot	|| 
			NULL == pProcess32First				||
			NULL == pProcess32Next)
			return FALSE;

		PROCESSENTRY32 proc;
		ZeroMemory(&proc, sizeof(PROCESSENTRY32));
		proc.dwSize = sizeof(proc);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		/* Now that we have a snapshot of the system state, we simply
		* walk the list it represents by calling Process32First once,
		* then call Proces32Next repeatedly until we get to the end 
		* of the list.
		*/
		pProcess32First(snapshot, &proc);
		if ( OnProcess(proc.szExeFile, proc.th32ProcessID) == FALSE )
		{
			CloseHandle(snapshot);
			return TRUE;
		}

		while ( pProcess32Next(snapshot, &proc) )
			if ( OnProcess(proc.szExeFile, proc.th32ProcessID) == FALSE )
				break;

		/* This should happen automatically when we terminate, but it never
		* hurts to clean up after ourselves.
		*/
		CloseHandle(snapshot);

		return TRUE;
	}
}

BOOL CPSAPI::EnumProcessModules(DWORD dwProcessId)
{
	if ( m_bNeedPSAPI )
	{
		if ( NULL == m_modPSAPI || NULL == m_modVDMDBG )
			return FALSE;

		ENUMPROCESSES       pEnumProcesses;
		GETMODULEFILENAME   pGetModuleFileName;
		ENUMPROCESSMODULES  pEnumProcessModules;  
		VDMENUMTASKWOWEX	pVDMEnumTaskWOWEx;
		GETMODULEBASENAME	pGetModuleBaseName;

		pVDMEnumTaskWOWEx = (VDMENUMTASKWOWEX)GetProcAddress(m_modVDMDBG, "VDMEnumTaskWOWEx");
		pEnumProcesses = (ENUMPROCESSES)GetProcAddress(m_modPSAPI, "EnumProcesses");
#ifdef	UNICODE
		pGetModuleFileName = (GETMODULEFILENAME)GetProcAddress(m_modPSAPI, "GetModuleFileNameExW");
		pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(m_modPSAPI, "GetModuleBaseNameW");
#else
		pGetModuleFileName = (GETMODULEFILENAME)GetProcAddress(m_modPSAPI, "GetModuleFileNameExA");
		pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(m_modPSAPI, "GetModuleBaseNameA");
#endif
		pEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(m_modPSAPI, "EnumProcessModules");

		if ( NULL == pGetModuleFileName ||
			NULL == pEnumProcessModules  )
			return FALSE;

		HMODULE hModules[MAX_HANDLE_COUNT];
		DWORD nModuleNo;

		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
		if ( process == NULL )
			return FALSE;

		BOOL bSuccess = pEnumProcessModules(process, hModules, sizeof(hModules), &nModuleNo);
		nModuleNo /= sizeof(hModules[0]);

		if ( !bSuccess )
		{
			CloseHandle(process);
			return FALSE;
		}

		TCHAR szFileName[MAX_PATH];
		for ( unsigned i=0; i<nModuleNo; i++)
		{
			if ( pGetModuleFileName(process, hModules[i], szFileName, sizeof(szFileName)) == FALSE )
			{
				CloseHandle(process);
				return FALSE;
			}

			LPTSTR szSep = _tcsrchr(szFileName, '\\');
			if ( szSep )
				szSep++;

			if ( OnModule(hModules[i], szFileName, szSep ) == FALSE )
				break;
		}

		CloseHandle(process);
		return TRUE;
	}
	else
	{
		HINSTANCE modKERNEL = GetModuleHandle(_T("KERNEL32.DLL"));

		CREATESNAPSHOT CreateToolhelp32Snapshot; 
		MODULEWALK pModule32First; 
		MODULEWALK pModule32Next; 

		CreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(modKERNEL, "CreateToolhelp32Snapshot"); 
		pModule32First = (MODULEWALK)GetProcAddress(modKERNEL, "Module32First"); 
		pModule32Next  = (MODULEWALK)GetProcAddress(modKERNEL, "Module32Next"); 

		if (
			NULL == CreateToolhelp32Snapshot	|| 
			NULL == pModule32First				||
			NULL == pModule32Next)
			return FALSE;

		MODULEENTRY32 module;
		ZeroMemory(&module, sizeof(MODULEENTRY32));
		module.dwSize = sizeof(module);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
		/* Now that we have a snapshot of the system state, we simply
		* walk the list it represents by calling Process32First once,
		* then call Proces32Next repeatedly until we get to the end 
		* of the list.
		*/
		pModule32First(snapshot, &module);
		if ( OnModule(module.hModule, module.szExePath, module.szModule) == FALSE )
		{
			CloseHandle(snapshot);
			return TRUE;
		}

		while ( pModule32Next(snapshot, &module) )
			if ( OnModule(module.hModule, module.szExePath, module.szModule) == FALSE )
				break;

		/* This should happen automatically when we terminate, but it never
		* hurts to clean up after ourselves.
		*/
		CloseHandle(snapshot);
		return TRUE;
	}
}


//////////////////////////////////////////////////////////////////////
// CPSAPI Overridables

BOOL CPSAPI::OnDeviceDriver(LPVOID lpImageBase)
{
	return TRUE;
}

BOOL CPSAPI::OnProcess(LPCTSTR lpszFileName, DWORD ProcessID)
{
	return TRUE;
}

BOOL CPSAPI::OnModule(HMODULE hModule, LPCTSTR lpszModuleName, LPCTSTR lpszPathName)
{
	return TRUE;
}
