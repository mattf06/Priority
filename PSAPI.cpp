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
#include <VersionHelpers.h>

#define	MAX_HANDLE_COUNT	1024

#pragma comment(lib, "psapi.lib")

//////////////////////////////////////////////////////////////////////
// CPSAPI Construction/Destruction

CPSAPI::CPSAPI()
{
}

BOOL CPSAPI::Initialize(void)
{
	return TRUE;
}

CPSAPI::~CPSAPI()
{
}


BOOL CPSAPI::EnumDeviceDrivers(void)
{
	return FALSE;
}

BOOL CPSAPI::EnumProcesses(void)
{

	DWORD aProcesses[MAX_HANDLE_COUNT];
	DWORD cbNeeded, cProcesses;

	if (!::EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return FALSE;

	cProcesses = cbNeeded / sizeof(DWORD);

	for (unsigned int i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, aProcesses[i]);

			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				if (::EnumProcessModules(hProcess, &hMod, sizeof(hMod),
					&cbNeeded))
				{
					GetModuleBaseName(hProcess, hMod, szProcessName,
						sizeof(szProcessName) / sizeof(TCHAR));

					if (OnProcess(szProcessName, aProcesses[i]) == FALSE)
					{
						CloseHandle(hProcess);
						return FALSE;
					}

				}

				CloseHandle(hProcess);
			}

		}
	}

	return TRUE;
}

BOOL CPSAPI::EnumProcessModules(DWORD dwProcessId)
{
	return FALSE;
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
