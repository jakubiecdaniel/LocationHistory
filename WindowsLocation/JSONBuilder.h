#pragma once
#include <iostream>
#include <string>



class JSONBuilder {
	std::wstring JSONString;
	bool hasValue;
public:
	JSONBuilder();
	void StartJson();
	void EndJson();
	void StartObject();
	void EndObject();
	void AddValue(const std::wstring& Key, double Value);
	void AddValue(const std::wstring& Key, const std::wstring& Value);
	void AddJSON(const std::wstring& json);
	void AddArray(const std::wstring& Key, const std::wstring& jsonArray);
	void AddObject(const std::wstring& Key, const std::wstring& jsonObject);
	std::wstring GetJson();

};