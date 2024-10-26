#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
      {
        if (MessageBox(hwnd, L"You sure you wanna quit brody?", L"caption", MB_OKCANCEL)) {
          DestroyWindow(hwnd);
        } else {
          break;
        }
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
        printf("x_pos: %i y_pos: %i\n", x_pos, y_pos);
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
    fprintf(newFile, "created window successfully\n");
    bool success = AllocConsole();
    // reroutes stdout FILE* stream to console "file"
    freopen("CON", "w", stdout);
    if (!success) {
      fprintf(newFile, "could not create console");
    } else {
      printf("created console");
      fprintf(newFile, "created console");
    }
    ShowWindow(hwd, nShowState);
  }
  fclose(newFile);

  MSG msg{};
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
