
#include "WindowsLocation.h"
#include <locationapi.h>
#include <SensorsApi.h>
#include <wlanapi.h>
#include <vector>
#include <set>
#include <algorithm>
#include <sstream>

#pragma comment(lib, "Wlanapi.lib")

class GoogleAPIWifi {

	JSONBuilder json;

public:
	std::wstring ssid;
	std::wstring macAddress;
	int signalStrength;

	//Create a JSON object of the wifi network for use in Google's Geolocation API
	std::wstring jsonOutput() 
	{
		json.StartJson();
		json.AddValue(L"macAddress", macAddress);
		json.AddValue(L"signalStrength", signalStrength);
		json.EndJson();

		return json.GetJson();

	}

	bool operator< (const GoogleAPIWifi& right) const
	{
		return signalStrength < right.signalStrength;
	}
	bool operator> (const GoogleAPIWifi& right) const
	{
		return signalStrength > right.signalStrength;
	}
};

DWORD clientVersion = 2;
DWORD dwNegotiatedVersion = 0;
HANDLE clientHandle = NULL;

_GUID guid;

std::set<GoogleAPIWifi, std::greater<GoogleAPIWifi>> api_list;
const auto MAX_NUM_OF_WIFIS = 5;

// Linear Interpolation
int lerp(int wlan) {
	int x0 = 0;
	int dbm0 = -100;

	int x1 = 100;
	int dbm1 = -50;
	 
	int dbm = 0;
	dbm = (((dbm0 * (x1 - wlan)) + (dbm1 * (wlan - x0))) / (x1 - x0));
	return dbm;
}


//Get the MAC address of the wifi network
std::wstring bss(PDOT11_SSID ssid, BOOL securityEnabled) {

	PWLAN_BSS_LIST WlanBssList;

	if (ERROR_SUCCESS != WlanGetNetworkBssList(clientHandle, &guid, ssid, dot11_BSS_type_infrastructure, securityEnabled, NULL, &WlanBssList)) {
		std::cout << "Error with WlanGetNetworkBssList\n";
	}

	WLAN_BSS_ENTRY bssEntry = WlanBssList->wlanBssEntries[0];

	char mac[500];

	sprintf_s(mac,500, "%02x:%02x:%02x:%02x:%02x:%02x",
		bssEntry.dot11Bssid[0],
		bssEntry.dot11Bssid[1],
		bssEntry.dot11Bssid[2],
		bssEntry.dot11Bssid[3],
		bssEntry.dot11Bssid[4],
		bssEntry.dot11Bssid[5]);

	std::string mac_s = mac;

	std::wstring mac_ws(mac_s.begin(),mac_s.end());

	if (WlanBssList)
		WlanFreeMemory(WlanBssList);

	return mac_ws;
}

void GetWifiAccessPoints() {

	PWLAN_INTERFACE_INFO_LIST list;

	if (ERROR_SUCCESS != WlanEnumInterfaces(clientHandle, NULL, &list)) {
		std::cout << "Error with WlanEnumInterfaces\n";
	}

	for (int i = 0; i < list->dwNumberOfItems; i++) {
		WLAN_INTERFACE_INFO info = list->InterfaceInfo[i];
		guid = info.InterfaceGuid;
	}

	if (list)
		WlanFreeMemory(list);
	
}

void GetNetworkList() {

	WLAN_AVAILABLE_NETWORK_LIST * list;

	WlanGetAvailableNetworkList(clientHandle, &guid, WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES, NULL, &list);

	for (int i = 0; i < list->dwNumberOfItems; i++) {
		WLAN_AVAILABLE_NETWORK info = list->Network[i];
		
		GoogleAPIWifi g;

		std::string temp = std::string(info.dot11Ssid.ucSSID, info.dot11Ssid.ucSSID + sizeof info.dot11Ssid.ucSSID / sizeof info.dot11Ssid.ucSSID[0]);

		g.ssid = std::wstring(temp.begin(), temp.end());

		g.macAddress = bss(&info.dot11Ssid, info.bSecurityEnabled);
		g.signalStrength = lerp(info.wlanSignalQuality);
		
		api_list.insert(g);
		
	}
	
	if (list)
		WlanFreeMemory(list);
}


std::wstring GetWifiList() {
	std::wstring json_output = L"";

	if (ERROR_SUCCESS != WlanOpenHandle(clientVersion, NULL, &dwNegotiatedVersion, &clientHandle)) {
		return L"ERROR";
	}

	GetWifiAccessPoints();
	GetNetworkList();

	int index = 0;

	for (auto itr : api_list) {

		if (index == MAX_NUM_OF_WIFIS) {
			break;
		}

		if (index > 0) {
			json_output += L",\n";

		}
		json_output += itr.jsonOutput();
		
		index++;
		
	}

	return json_output;

	WlanCloseHandle(clientHandle, NULL);

}
