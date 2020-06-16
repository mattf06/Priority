#include <windows.h>
#include "Priority.h"
#include "ProcessList.h"

#include <stdio.h>


CPriority::CPriority()
	: CNTService(TEXT("Priority"), TEXT("Digital Design Priority for NT/2000"))
	, m_hStop(0)
{
	m_dwControlsAccepted = 0;
	m_dwControlsAccepted |= SERVICE_ACCEPT_STOP;
}

void CPriority::Run(DWORD dwArgc, LPTSTR * ppszArgv) {
	// report to the SCM that we're about to start
	ReportStatus(SERVICE_START_PENDING);

	m_hStop = ::CreateEvent(0, TRUE, FALSE, 0);

	// TODO: You might do some initialization here.
	//		 Parameter processing for instance ...
	//		 If this initialization takes a long time,
	//		 don't forget to call "ReportStatus()"
	//		 frequently or adjust the number of milliseconds
	//		 in the "ReportStatus()" above.
	
	// report SERVICE_RUNNING immediately before you enter the main-loop
	// DON'T FORGET THIS!
	ReportStatus(SERVICE_RUNNING);

#ifdef _DEBUG
	int iCount = 0;
#endif

	// main-loop
	// If the Stop() method sets the event, then we will break out of
	// this loop.
	while( ::WaitForSingleObject(m_hStop, 10) != WAIT_OBJECT_0 ) 
	{
		CProcessList pl;
		pl.Initialize();
		pl.EnumProcesses();
		Sleep(2000);
	}

	if( m_hStop )
		::CloseHandle(m_hStop);
}


void CPriority::Stop() {
	// report to the SCM that we're about to stop

	// TODO: Adjust the number of milliseconds you think
	//		 the stop-operation may take.
	ReportStatus(SERVICE_STOP_PENDING, 5000);

	if( m_hStop )
		::SetEvent(m_hStop);
}

