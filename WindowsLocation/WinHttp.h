#pragma once
#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include "JSONBuilder.h"
#include "JSON.h"
std::string SendAPIRequest(const wchar_t* data, const wchar_t* url, const wchar_t* path, const int port, bool bSecure);
bool GetWindowsLocationAndSendRequest(std::wstring user_id, std::wstring api_key);