#include "StartupRegistry.h"

StartupRegistry::StartupRegistry(wstring pName, wstring pPath) : programName(pName), programPath(pPath), RegistryHandler::RegistryHandler(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run") {}

bool StartupRegistry::SetStartup() {
	return SetValue(programName, programPath);
}

bool StartupRegistry::GetStartupState() {
	wstring readPath;
	bool state = QueryValue(programName, readPath);
	if (!state) {
		return false;
	}
	else {
		return programPath == readPath;
	}
}

bool StartupRegistry::RemoveStartup() {
	return RemoveValue(programName);
}
