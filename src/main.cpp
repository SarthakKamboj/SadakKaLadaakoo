#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>

#include "gfx_api/windows/skl_win32.h"
#include "gfx_api/windowing.h"
#include "defines.h"
#include "app_state.h"

AppState g_appState;

#ifdef SKL_WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE disregard, PWSTR pCmdLineArgs, int nShowState) {
#endif

  WindowInfo windowInfo{};
  windowInfo.width = 800;
  windowInfo.height = 450;
  const char* name = "SadakKaLadaakoo";
  memcpy(windowInfo.name, name, strlen(name)+1);

#ifdef SKL_WINDOWS
  Win32InitContext win32InitContext{};
  win32InitContext.hInstance = hInstance;
  win32InitContext.pCmdLineArgs = pCmdLineArgs;
  win32InitContext.nShowState = nShowState;

  InitPlatformSpecific(windowInfo, win32InitContext);
#endif

  RenderContext renderContext;

  g_appState.running = true;
  while (g_appState.running) {
    PollEvents();
    RenderFrame(renderContext);
  }

  return 0;
}
