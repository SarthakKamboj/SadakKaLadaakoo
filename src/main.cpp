#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

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
  wc.hInstance = hInstance;
  wc.lpszClassName = name;
  wc.lpfnWndProc = WindowProc;

  RegisterClass(&wc);

  HWND hwd = CreateWindowEx(
    0, 
    name,
    L"SadakKaLadaakoo Game",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL,
    NULL,
    hInstance,
    NULL
  );

  FILE* newFile = fopen("./output.txt", "w");
  if (hwd == NULL) {
    fprintf(newFile, "could not create window");
  } else {
    fprintf(newFile, "created window successfully");
    ShowWindow(hwd, nShowState);
  }
  fclose(newFile);
  return 0;
}
