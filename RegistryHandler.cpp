#include "RegistryHandler.h"

RegistryHandler::RegistryHandler(wstring path): rootKeyPath(path) {}

bool RegistryHandler::SetValue(wstring keyName, wstring value) {
	if (rootKeyPath.empty()) {
		return false;
	}

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, rootKeyPath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
		RegSetValueEx(hKey, keyName.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), (lstrlen(value.c_str()) + 1) * sizeof(wchar_t));
		RegCloseKey(hKey);
		return true;
	}
	
	return false;
}

bool RegistryHandler::QueryValue(wstring keyName, wstring& outValue) {
	if (rootKeyPath.empty()) {
		return false;
	}

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, rootKeyPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD valueType;
		DWORD valueSize = 0;

		if (RegQueryValueEx(hKey, keyName.c_str(), NULL, &valueType, NULL, &valueSize) == ERROR_SUCCESS && valueType == REG_SZ) {
			wchar_t* buffer = new wchar_t[valueSize / sizeof(wchar_t)];
			if (RegQueryValueEx(hKey, keyName.c_str(), NULL, NULL, (LPBYTE)buffer, &valueSize) == ERROR_SUCCESS) {
				outValue = buffer;
				delete[] buffer;
				RegCloseKey(hKey);
				return true;
			}
			delete[] buffer;
		}
		RegCloseKey(hKey);
	}

	return false;
}

bool RegistryHandler::RemoveValue(wstring keyName) {
	if (rootKeyPath.empty()) {
		return false;
	}

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, rootKeyPath.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
		LONG result = RegDeleteValue(hKey, keyName.c_str());
		RegCloseKey(hKey);
		return result == ERROR_SUCCESS;
	}

	return false;
}
