#pragma once

#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>

#include "skl_d3d12.h"
#include "defines.h"
#include "app_state.h"
#include "../windowing.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct Win32InitContext {
  HINSTANCE hInstance;
  PWSTR pCmdLineArgs;
  int nShowState;
};

struct Win32Context {
  HWND hwd;
};

void ConvertCharToWChar(const char* input, int inputLen, __SKL_OUT__ wchar_t* output);
void InitPlatformSpecific(WindowInfo& windowInfo, Win32InitContext& context);
