#include "TrayIcon.h"
#include "AudioSession.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	TrayIcon trayIcon(hInstance);
	AudioManager audioManager;

	trayIcon.Create(); // Create system tray icon
	trayIcon.UpdateVolumeIcon(audioManager.GetVolume(), audioManager.GetMuted());

	audioManager.SetVolumeChangeCallback([&trayIcon](int volume, BOOL isMuted) {
		trayIcon.UpdateVolumeIcon(volume, isMuted);
		});

	GdiplusShutdown(gdiplusToken);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
