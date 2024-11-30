#ifndef TRAYICON_H
#define TRAYICON_H

#include <windows.h>
#include <shellapi.h>
#include <gdiplus.h>

class TrayIcon
{
public: 
	TrayIcon(HINSTANCE hInstance);
	~TrayIcon();
	void Create();
	void UpdateVolumeIcon(int volume, BOOL isMuted);
	void ShowContextMenu();

private:
	HWND hwnd;
	NOTIFYICONDATA nid;
	HMENU hMenu;
	HICON hCurrentIcon;

	HICON CreateIconWithText(int volume, BOOL isMuted);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lPAram);
};

#endif // TRAYICON_H
