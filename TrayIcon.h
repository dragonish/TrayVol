#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>
#include <shellapi.h>
#include <gdiplus.h>
#include "StartupRegistry.h"

class TrayIcon
{
public: 
	TrayIcon(HINSTANCE hInstance);
	~TrayIcon();
	void Create();
	void UpdateVolumeIcon(int volume, BOOL isMuted);
	void ShowContextMenu();
	void ChangeStartup();

private:
	HWND hwnd;
	NOTIFYICONDATA nid;
	HMENU hMenu;
	HICON hCurrentIcon;
	StartupRegistry* sr;

	HICON CreateIconWithText(int volume, BOOL isMuted);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lPAram);
};

#endif // TRAYICON_H
