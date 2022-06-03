#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <debugapi.h>
#include "WinHttp.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#define SVCNAME (LPWSTR)TEXT("LOCATION_HISTORY_SERVICE")
#define WAIT_TIME 30000


SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
 

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl) {

	switch (dwCtrl) {

	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		SetEvent(ghSvcStopEvent);

		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

		return;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;

	}

}

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {

	static DWORD dwCheckPoint = 1;

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else
		gSvcStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);

}

VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv) { // Perform Service Specific initalization and begin work to be performed by the service
	ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (ghSvcStopEvent == NULL) {
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
	//auto logger = spdlog::get("JSON Error Logs");

	auto logger = spdlog::get("Service Logs");
	
	do {
		logger->info("Sending Request...");
		logger->flush();
		GetWindowsLocationAndSendRequest(lpszArgv[1], lpszArgv[2]);
	} while (WaitForSingleObject(ghSvcStopEvent, WAIT_TIME) != WAIT_OBJECT_0);

	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
	return;

	/*while (1) {
		WaitForSingleObject(ghSvcStopEvent, INFINITE);
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}*/
}

// Arg format: USER_ID API_KEY
void WINAPI SvcMain(DWORD dwArgc, LPWSTR* lpszArgv) {
	
	gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler); // Register service handler function

	if (!gSvcStatusHandle) {
		//SvcReportEvent(((LPTSTR)"RegisterServiceCtrlHandler"));
		return;
	}

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;
	
	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	SvcInit(dwArgc, lpszArgv);

}
void SetupServiceLogger() {

	try {
		spdlog::basic_logger_mt("Service Logs", "logs/service-logs.txt");

	}
	catch (const spdlog::spdlog_ex& ex) {
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}

}
int __cdecl _tmain(int argc, TCHAR * argv[]) {
	SetupServiceLogger();
	auto logger = spdlog::get("Service Logs");
	logger->info("Service Started");
	logger->flush();
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain},
		{NULL,NULL}
	};

	if (!StartServiceCtrlDispatcher(DispatchTable)) { // first call that we need to make
		//SvcReportEvent((LPTSTR)("StartServiceCtrlDispatcher"));
		return 0;
	}

	return 0;
}