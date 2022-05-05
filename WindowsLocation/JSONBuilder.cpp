#include "JSONBuilder.h"


	JSONBuilder::JSONBuilder() {
		hasValue = 0;
		JSONString = L"";

	}

	void JSONBuilder::StartJson() {
		hasValue = 0;
		JSONString = L"";
		JSONString += L'{';
	}

	void JSONBuilder::EndJson() {
		hasValue = 0;
		JSONString += L'}';
	}

	void JSONBuilder::StartObject() { //This is probally wrong
		if (hasValue)
			JSONString += L',';

		JSONString += L'{';
	}

	void JSONBuilder::EndObject() {
		hasValue = 0;  
		JSONString += L'}';
	}

	void JSONBuilder::AddValue(const std::wstring& Key, double Value) {
		if (hasValue)
			JSONString += L',';
		JSONString += (L"\"" + Key + L"\"" + L':' + std::to_wstring(Value) + L'\n');
		hasValue++;
	}

	void JSONBuilder::AddValue(const std::wstring& Key, const std::wstring& Value) {
		if (hasValue)
			JSONString += L',\n';
		JSONString += (L"\"" + Key + L"\"" + L':' + L"\"" + Value + L"\"");
		hasValue++;
	}
	void JSONBuilder::AddArray(const std::wstring& Key, const std::wstring& jsonArray) {
		if(hasValue)
			JSONString += L',\n';

		JSONString += L"\"" + Key + L"\"" + L':' + L'[' + jsonArray + L']';
		hasValue++;
	}

	void JSONBuilder::AddObject(const std::wstring& Key, const std::wstring& jsonObject) { // jsonObject should already contain 
		if (hasValue)
			JSONString += L',\n';
		JSONString += (L"\"" + Key + L"\"" + L':'  + jsonObject);
		hasValue++;
	}

	void JSONBuilder::AddJSON(const std::wstring& json) {
		JSONString += json;
	}

	std::wstring JSONBuilder::GetJson() {
		return JSONString;
	}
