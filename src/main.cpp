#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>

#include "gfx_api/skl_d3d12.h"
#include "defines.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
      {
        // if (MessageBox(hwnd, L"You sure you wanna quit brody?", L"caption", MB_OKCANCEL)) {
          // DestroyWindow(hwnd);
        // } else {
          // break;
        // }
      }

    case WM_DESTROY:
      {
        PostQuitMessage(0);
        return 0;
      }

    case WM_MOUSEMOVE: 
      {
        int x_pos = GET_X_LPARAM(lParam);
        int y_pos = GET_Y_LPARAM(lParam);
        SKL_LOG("x_pos: %i y_pos: %i", x_pos, y_pos);
      }


    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
        EndPaint(hwnd, &ps);
      }
      return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE disregard, PWSTR pCmdLineArgs, int nShowState) {
  const wchar_t name[] = L"SadakKaLadaakoo";

  WNDCLASS wc = {};
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.hInstance = hInstance;
  wc.lpszClassName = name;
  wc.lpfnWndProc = WindowProc;

  RegisterClass(&wc);

  HWND hwd = CreateWindowEx(
    0, 
    name,
    L"SadakKaLadaakoo Game",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 800, 450,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  bool success = AllocConsole();
  AttachConsole(GetCurrentProcessId());
  // reroutes stdout FILE* stream to console "file"
  freopen("CON", "w", stdout);
  
  if (!success) {
    SKL_LOG("could not create console");
  } else {
    SKL_LOG("created console");
  }

  if (hwd == NULL) {
    SKL_LOG("could not create window");
  } else {
    SKL_LOG("created window successfully");
    
  }

  D3DContext d3dContext;
  initD3D12(hwd, d3dContext);

  ShowWindow(hwd, nShowState);

  bool running = true;
  while (running) {
    MSG msg{};
    while (PeekMessage(&msg, hwd, 0, 0, PM_NOREMOVE)) {
      bool quitMsg = GetMessage(&msg, NULL, 0, 0) == 0;
      if (quitMsg) {
        running = false;
        break;
      }
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    renderFrame(d3dContext);
  }

  return 0;
}
