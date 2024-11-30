#include "TrayIcon.h"
#include <strsafe.h>

TrayIcon::TrayIcon(HINSTANCE hInstance) {
	// Registration window class
	const wchar_t CLASS_NAME[] = L"TrayIconWindowClass";

	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Tray Icon Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	// Set user data to access TrayIcon instance in WindowProc
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	ShowWindow(hwnd, SW_HIDE); // Hide Window

	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_APP + 1; // Set a custom message ID for tray notifications

	// Create context menu
	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, 2, L"GitHub");
	AppendMenu(hMenu, MF_GRAYED, 3, L"Version: 1.0.0");
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, 1, L"Exit");
}

TrayIcon::~TrayIcon() {
	if (hCurrentIcon) {
		DestroyIcon(hCurrentIcon);  // Release current icon
	}
	Shell_NotifyIcon(NIM_DELETE, &nid); // Delete tray icon
}

void TrayIcon::Create() {
	HRESULT result = StringCchCopyW(nid.szTip, ARRAYSIZE(nid.szTip), L"Volume: Loading...");
	if (FAILED(result)) {
		MessageBox(NULL, L"Failed to copy string to szTip", L"Error", MB_OK);
		return;
	}

	hCurrentIcon = CreateIconWithText(-1, false);
	nid.hIcon = hCurrentIcon;

	// Add to tray
	if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
		MessageBox(NULL, L"Failed to add tray icon", L"Error", MB_OK);
	}
}

void TrayIcon::UpdateVolumeIcon(int volume, BOOL isMuted) {
	// Update prompt text
	wchar_t tipText[64];
	if (isMuted) {
		swprintf(tipText, 64, L"Volume: Muted");
	}
	else {
		swprintf(tipText, 64, L"Volume: %d%%", volume);
	}
	StringCchCopyW(nid.szTip, ARRAYSIZE(nid.szTip), tipText);

	if (hCurrentIcon) {
		DestroyIcon(hCurrentIcon); // Release current icon
	}

	// Update tray icon
	hCurrentIcon = CreateIconWithText(volume, isMuted);
	nid.hIcon = hCurrentIcon;

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

HICON TrayIcon::CreateIconWithText(int volume, BOOL isMuted) {
	HDC hdcScreen = GetDC(NULL);
	HDC hdc = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, 32, 32);
	SelectObject(hdc, hBitmap);

	// Fill background
	RECT rect = { 0, 0, 32, 32 };
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hdc, &rect, hBrush);

	// Set text format
	wchar_t text[10];
	if (volume >= 100) {
		swprintf(text, sizeof(text) / sizeof(wchar_t), L"%s", L"F");
	}
	else if (volume < 0) {
		swprintf(text, sizeof(text) / sizeof(wchar_t), L"%s", L"N");
	}
	else {
		swprintf(text, sizeof(text) / sizeof(wchar_t), L"%d", volume);
	}

	// Create font
	HFONT hFont = CreateFont(32, 0, 0, 0,
		FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		VARIABLE_PITCH, L"Arial");
	SelectObject(hdc, hFont);

	// Get text size
	SIZE textSize;
	GetTextExtentPoint32(hdc, text, wcslen(text), &textSize);
	int x = (32 - textSize.cx) / 2;
	int y = (32 - textSize.cy) / 2;

	// Draw text
	if (isMuted) {
		SetTextColor(hdc, RGB(201, 42, 42));
	}
	else {
		SetTextColor(hdc, RGB(0, 0, 0));
	}
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, x, y, text, wcslen(text));

	// Create icon
	ICONINFO iconInfo;
	iconInfo.fIcon = TRUE;
	iconInfo.xHotspot = 0;
	iconInfo.yHotspot = 0;
	iconInfo.hbmMask = hBitmap;
	iconInfo.hbmColor = hBitmap;

	HICON hIcon = CreateIconIndirect(&iconInfo);

	DeleteObject(hBrush);
	DeleteObject(hFont);
	DeleteObject(hBitmap);
	DeleteDC(hdc);

	ReleaseDC(NULL, hdcScreen);

	return hIcon;
}

void TrayIcon::ShowContextMenu() {
	POINT pt;
	GetCursorPos(&pt);
	SetForegroundWindow(nid.hWnd); // Ensure window is in front
	TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, nid.hWnd, NULL);
}

LRESULT CALLBACK TrayIcon::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_APP + 1: // Custom message for tray icon notifications
		if (lParam == WM_RBUTTONUP) { // Right-click event
			TrayIcon* trayIcon = reinterpret_cast<TrayIcon*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (trayIcon) {
				trayIcon->ShowContextMenu(); // Show context menu
			}
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1: // Exit
			PostQuitMessage(0);
			break;
		case 2:
			ShellExecute(NULL, L"open", L"https://github.com/dragonish/TrayIcon", NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
