#include "ServiceControls.h"

SC_HANDLE serviceManager;
SC_HANDLE serviceHandle;

LPCWSTR ServiceName = L"LOCATION_HISTORY_SERVICE";

void __stdcall Cleanup() {
	CloseServiceHandle(serviceHandle);
	CloseServiceHandle(serviceManager);
}

INT __stdcall Startup(DWORD dwDesiredAccess) {

	serviceManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == serviceManager) {
		printf("OpenSCManager failed: %i\n", GetLastError());
		return 0;
	}

	serviceHandle = OpenService(serviceManager, ServiceName, dwDesiredAccess);

	if (NULL == serviceHandle) { //TODO: handling for already deleted?
		CloseServiceHandle(serviceManager);
		printf("OpenService failed: %i\n", GetLastError());
		return 0;
	}

	return 1;
}


INT __stdcall DeleteService() {
	DWORD dwError;
	if (!Startup(DELETE)) {
		return 0;
	}

	if (!DeleteService(serviceHandle)) {
		dwError = GetLastError();

		Cleanup();
		return 0;
	}

	Cleanup();

	return 1;
}

INT __stdcall StopService() {
	DWORD dwError;
	SERVICE_STATUS ss;
	ULONGLONG StartTime = GetTickCount64();
	DWORD dwTimeout = 30000;

	if (!Startup(SERVICE_ALL_ACCESS)) {
		return 0;
	}

	if (!QueryServiceStatus(serviceHandle, &ss)) {
		dwError = GetLastError();
		Cleanup();
		return 0;
	}

	if (ss.dwCurrentState == SERVICE_STOPPED) {
		//printf("Service already stopped\n");
		Cleanup();
		return 1;
	}

	if (ss.dwCurrentState == SERVICE_STOP_PENDING) {
		//printf("Service stopping...\n");
		Cleanup();
		return 1;
	}

	if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &ss)) {
		dwError = GetLastError();
		Cleanup();
		return 0;
	}

	while (ss.dwCurrentState != SERVICE_STOPPED) {
		Sleep(ss.dwWaitHint);

		if (!QueryServiceStatus(serviceHandle, &ss)) {
			dwError = GetLastError();
			Cleanup();
			return 0;
		}

		if (ss.dwCurrentState == SERVICE_STOPPED) {
			break;
		}

		if (GetTickCount64() - StartTime > dwTimeout) {
			//printf("Timed out...\n");
			Cleanup();
			return 0;
		}

	}

	//printf("Stopped service...\n");
	Cleanup();

	return 1;

}

INT __stdcall doStartService(DWORD dwNumServiceArgs, LPCWSTR* lpServiceArgsVectors) {
	DWORD dwError;
	SERVICE_STATUS ss;

	if (!Startup(SERVICE_ALL_ACCESS)) {
		return 0;
	}

	if (!QueryServiceStatus(serviceHandle, &ss)) {
		dwError = GetLastError();
		Cleanup();
		return 0;

	}

	if (ss.dwCurrentState != SERVICE_STOPPED) {
		//printf("Cannot start the service, service is not stopped\n");
		Cleanup();
		return 0;
	}

	if (!StartService(serviceHandle, dwNumServiceArgs, lpServiceArgsVectors)) {
		dwError = GetLastError();
		Cleanup();
		return 0;
	}

	//TODO: service start running handling

	if (!QueryServiceStatus(serviceHandle, &ss)) {
		dwError = GetLastError();
		Cleanup();
		return 0;

	}

	if (ss.dwCurrentState == SERVICE_RUNNING || ss.dwCurrentState == SERVICE_START_PENDING) {
		//printf("Service Running...\n");
		Cleanup();
		return 1;
	}

	//printf("Service is not running...\n");

	Cleanup();

	return 0;


}

ServiceErrors isServiceInstalled() {

	if (!Startup(SC_MANAGER_ALL_ACCESS)) {
		DWORD dwError = GetLastError();
		if (dwError == ERROR_SERVICE_DOES_NOT_EXIST)
			return SERVICE_NOT_INSTALLED;
		else
			return OTHER_ERROR;
	}
	Cleanup();

	return SERVICE_INSTALLED;

}

ServiceStatus __stdcall GetServiceStatus() {
	DWORD dwError;
	SERVICE_STATUS ss;

	if (!Startup(SERVICE_ALL_ACCESS)) {
		return ERROR_GETTING_STATUS;
	}

	if (!QueryServiceStatus(serviceHandle, &ss)) {
		dwError = GetLastError();
		Cleanup();
		return ERROR_GETTING_STATUS;
	}
	Cleanup();

	switch (ss.dwCurrentState) {
	case SERVICE_STOPPED:
		return STOPPED;
	case SERVICE_RUNNING:
		return RUNNING;
	case SERVICE_START_PENDING:
		return START_PENDING;
	case SERVICE_STOP_PENDING:
		return STOP_PENDING;
	default:
		return UNKNOWN_STATUS;
	}

}


INT InstallService(LPCWSTR szPath) {
	DWORD dwError;
	/*if (!GetModuleFileName((HMODULE)"", szPath, MAX_PATH)) {
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}*/

	serviceManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (NULL == serviceManager) {
		dwError = GetLastError();
		return 0;
	}

	serviceHandle = CreateService(serviceManager, ServiceName, ServiceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, szPath, NULL, NULL, NULL, NULL, NULL);

	if (serviceHandle == NULL) {
		dwError = GetLastError();
		CloseServiceHandle(serviceManager);
		return 0;
	}


	Cleanup();

	return 1;
}