
#include "WindowsLocation.h"
#include "WinHttp.h"
#include <locale>
#include <codecvt>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#pragma comment(lib, "winhttp")

const auto API_URL = L"www.googleapis.com";

const auto API_PATH = L"/geolocation/v1/geolocate?key=";
const auto API_PORT = INTERNET_DEFAULT_HTTPS_PORT;

const auto APP_URL = L"127.0.0.1";
const auto APP_PATH = L"/LocationHistory/add/";
const auto APP_PORT = 8000;

int checkGoogleApiError(std::string response) {
	const int SUCCESS = 0;
	const int FAILURE = 1;

	JSONParser jp;
	JSON google_json;
	try {
		google_json = jp.string_to_json(response);
	}
	catch (std::runtime_error err) {
		std::cout << err.what() << "\n";
		return FAILURE;
	}

	JSON_VALUE error = google_json["error"];

	if (std::holds_alternative<std::string>(error.json_value) && std::get<std::string>(error.json_value) == "") {
		return SUCCESS;
	}
	else {
		return FAILURE;
	}

}

void SetupLogger() {

	try {
		spdlog::basic_logger_mt("JSON Logs", "logs/json-logs.txt");

	}
	catch (const spdlog::spdlog_ex& ex) {
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}

}

std::string SendAPIRequest(const wchar_t* data, const wchar_t* url, const wchar_t* path, const int port, bool bSecure) {
	HINTERNET hSession = 0, hConnect = 0, hRequest = 0;
	BOOL bRequest;
	DWORD dwSize = 0;

	std::string response;

	size_t data_size;
	size_t header_size;
	size_t total_size;

	const wchar_t* headers = L"Content-Type: application/json";

	std::vector<char> buf;

	//Going to send data over in utf8, this would lose any chars that dont fall in ASCII range.
	std::wstring data_w(data);
	std::string data_s(data_w.begin(), data_w.end());

	data_size = strlen(data_s.c_str());
	header_size = wcslen(headers);
	total_size = data_size + header_size;

	hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession) {
		printf("Error %u in WinHttpOpen.\n", GetLastError());
		goto clean_up;
	}

	hConnect = WinHttpConnect(hSession, url, port, 0);
	if (!hConnect) {
		printf("Error %u in WinHttpConnect.\n", GetLastError());
		goto clean_up;
	}

	hRequest = WinHttpOpenRequest(hConnect, L"POST", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, bSecure ? WINHTTP_FLAG_SECURE : NULL); //WINHTTP_FLAG_SECURE
	if (!hRequest) {
		printf("Error %u in WinHttpOpenRequest.\n", GetLastError());
		goto clean_up;
	}

	bRequest = WinHttpSendRequest(hRequest, headers, header_size, (LPVOID)data_s.c_str(), data_size, data_size, 0);
	if (!bRequest) {
		printf("Error %u in WinHttpSendRequest.\n", GetLastError());

		goto clean_up;
	}

	bRequest = WinHttpReceiveResponse(hRequest, NULL);

	bRequest = WinHttpQueryDataAvailable(hRequest, &dwSize);

	buf.resize(dwSize + 1); // do we need this +1?

	while (dwSize > 0) {

		bRequest = WinHttpReadData(hRequest, static_cast<LPVOID>(buf.data()), dwSize + 1, &dwSize);

		response += buf.data();

		dwSize = 0;
		bRequest = WinHttpQueryDataAvailable(hRequest, &dwSize);
		
		buf.resize(dwSize + 1);
	}

clean_up:
	if (hRequest)
		WinHttpCloseHandle(hRequest);
	if (hConnect)
		WinHttpCloseHandle(hConnect);
	if (hSession)
		WinHttpCloseHandle(hSession);

	return response;
}


bool GetWindowsLocationAndSendRequest(std::wstring user_id, std::wstring api_key) {
	SetupLogger();
	auto logger = spdlog::get("JSON Logs");
		
	
	JSONBuilder json;
	json.StartJson();
	json.AddValue(L"considerIp", L"false");
	json.AddArray(L"wifiAccessPoints", GetWifiList());
	json.EndJson();

	std::wstring full_api_path = API_PATH + api_key;

	logger->info(json.GetJson());
	logger->flush();

	std::string google_response = SendAPIRequest(json.GetJson().c_str(), API_URL, full_api_path.c_str(), API_PORT, true);
	std::wstring api_json_response = std::wstring(google_response.begin(), google_response.end());

	logger->info(google_response);

	json.StartJson();
	json.AddObject(L"location",api_json_response);
	json.AddValue(L"user", user_id);
	json.EndJson();

	std::string app_json_response = SendAPIRequest(json.GetJson().c_str(), APP_URL, APP_PATH, APP_PORT, false);

	return TRUE;

}




//int main() {
//	auto logger = spdlog::get("JSON Error Logs");
//	if(!logger)
//		SetupLogger();
//	GetWindowsLocationAndSendRequest(USER_ID,API_KEY);
//}

