#include <iostream>
#include <shlobj_core.h>
#include <shlwapi.h>
#include <vector>
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

class KeyValue {

private:
    std::wstring Key;
    std::wstring Value;

public:
    KeyValue();
    KeyValue(std::wstring Key, std::wstring Value);
    void setKey(std::wstring Key);
    void setValue(std::wstring Value);
    std::wstring GetValue();
    std::wstring GetKeyValue();

};


void ConfigFile(std::vector<KeyValue> kvs);
std::vector<KeyValue> GetConfig();
