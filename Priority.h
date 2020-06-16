#if !defined(AFX_PRIORITY_H__92FAFA5A_8A9F_48C2_9038_8060F2E4B969__INCLUDED_)
#define AFX_PRIORITY_H__92FAFA5A_8A9F_48C2_9038_8060F2E4B969__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include "NTService.h"



class CPriority : public CNTService {
	public:	// construction
		CPriority();

	public:	// overridables
		virtual void	Run(DWORD, LPTSTR *);
		virtual void	Stop();

	private:
		HANDLE	m_hStop;
protected:
	static BOOL WINAPI ControlHandle(DWORD CtrlType);
	static DWORD m_dwGuiThreadId;
};

#endif // !defined(AFX_PRIORITY_H__92FAFA5A_8A9F_48C2_9038_8060F2E4B969__INCLUDED_)
