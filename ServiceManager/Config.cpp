#include "Config.h"
#include <locale>
#include <codecvt>

KeyValue::KeyValue() {
}

KeyValue::KeyValue(std::wstring Key, std::wstring Value) {
    this->Key = Key;
    this->Value = Value;
}

void KeyValue::setKey(std::wstring Key) {
    this->Key = Key;
}

void KeyValue::setValue(std::wstring Value) {
    this->Value = Value;
}

std::wstring KeyValue::GetValue() {
    return this->Value;
}

std::wstring KeyValue::GetKeyValue() {
    std::wstring ret = this->Key + L":" + this->Value;
    return ret;
}


// Going to write to config file in ASCII
void ConfigFile(std::vector<KeyValue> kvs) {
    std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converter;

    std::string toWrite;

    int i = 0;
    for (KeyValue kv : kvs) {
        if (i > 0)
            toWrite += '\n';

        toWrite += converter.to_bytes(kv.GetKeyValue());

        i++;
    }

    toWrite += '\n';

    TCHAR szPath[MAX_PATH];

    HRESULT hResult = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);

    if (S_OK != hResult) {
        return;
    }

    PathAppend(szPath, TEXT("config.txt"));

    HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        return;
    }

    DWORD bWritten;

    const char* buf = toWrite.c_str();

    if (!WriteFile(hFile, buf, strlen(buf), &bWritten, NULL)) {
        CloseHandle(hFile);
        return;
    }

    CloseHandle(hFile);
}
//TODO: error checking
std::vector<KeyValue> GetConfig() {

    std::vector<KeyValue> kvs;

    TCHAR szPath[MAX_PATH];

    HRESULT hResult = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath);

    if (S_OK != hResult) {
        return kvs;
    }

    PathAppend(szPath, TEXT("config.txt"));

    HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        return kvs;
    }

    char buf[1024];
    DWORD dwBytesRead;

    ReadFile(hFile, buf, 1024, &dwBytesRead, NULL);

    CloseHandle(hFile);


    std::string config = buf;

    size_t pos = 0;
    size_t startPos = pos;

    pos = config.find('\n', pos + 1);
    do {

        std::string line = config.substr(startPos, (pos - startPos));

        startPos = pos + 1;

        int line_sep = line.find(':', 0);

        std::string key = line.substr(0, line_sep);
        std::string value = line.substr(line_sep + 1);

        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

        std::wstring wKey = converter.from_bytes(key);
        std::wstring wValue = converter.from_bytes(value);

        KeyValue kv(wKey, wValue);

        kvs.push_back(kv);

        startPos = pos + 1;

        pos = config.find('\n', pos + 1);

    } while (pos != std::string::npos);

    return kvs;


}