#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "advapi32.lib")
enum ServiceErrors {

	SERVICE_NOT_INSTALLED,
	SERVICE_INSTALLED,
	OTHER_ERROR

};

enum ServiceStatus {
	ERROR_GETTING_STATUS,
	RUNNING,
	STOPPED,
	START_PENDING,
	STOP_PENDING,
	UNKNOWN_STATUS
};

INT __stdcall DeleteService();
INT __stdcall StopService();
INT __stdcall doStartService(DWORD dwNumServiceArgs, LPCWSTR* lpServiceArgsVectors);
INT InstallService(LPCWSTR szPath);

ServiceErrors isServiceInstalled();
ServiceStatus __stdcall GetServiceStatus();

