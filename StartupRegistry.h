#pragma once
#include "RegistryHandler.h"

class StartupRegistry : protected RegistryHandler
{
public:
	StartupRegistry(wstring pName, wstring pPath);
	bool SetStartup();
	bool GetStartupState();
	bool RemoveStartup();

private:
	const wstring programName;
	const wstring programPath;
};

