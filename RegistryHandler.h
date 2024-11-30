#pragma once
#include <string>
#include <windows.h>

using std::wstring;

class RegistryHandler
{
public:
	RegistryHandler(wstring path);

protected:
	wstring rootKeyPath;

	bool SetValue(wstring keyName, wstring value);
	bool QueryValue(wstring keyName, wstring& outValue);
	bool RemoveValue(wstring keyName);
};

