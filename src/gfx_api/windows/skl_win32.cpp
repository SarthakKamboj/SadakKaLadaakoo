#include "skl_win32.h"

static Win32Context i_win32Context;

extern AppState g_appState;

void ConvertCharToWChar(const char* input, __SKL_OUT__ wchar_t* output) {
  mbstowcs(output, input, strlen(input)+1);
}

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
        RECT clientRect = {};
        GetClientRect(hwnd, &clientRect);
        g_appState.mouse_x = ((float)x_pos) / clientRect.right;
        g_appState.mouse_y = ((float)y_pos) / clientRect.bottom;
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

void InitPlatformSpecific(WindowInfo& windowInfo, Win32InitContext& win32InitContext) {

  WNDCLASS wc = {};
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.hInstance = win32InitContext.hInstance;
  wchar_t wWindowInfoName[256]{};
  ConvertCharToWChar(windowInfo.name, wWindowInfoName);
  wc.lpszClassName = wWindowInfoName;
  wc.lpfnWndProc = WindowProc;

  char windowName[256]{};
  sprintf(windowName, "%s Game", windowInfo.name);
  wchar_t wWindowName[256]{};
  ConvertCharToWChar(windowName, wWindowName);

  RegisterClass(&wc);

  HWND hwd = CreateWindowEx(
    0, 
    wWindowInfoName,
    wWindowName,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, windowInfo.width, windowInfo.height,
    NULL,
    NULL,
    win32InitContext.hInstance,
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

  InitD3D12(hwd);

  ShowWindow(hwd, win32InitContext.nShowState);

  i_win32Context.hwd = hwd;
}

void PollEvents() {
  MSG msg{};
  while (PeekMessage(&msg, i_win32Context.hwd, 0, 0, PM_NOREMOVE)) {
    bool quitMsg = GetMessage(&msg, NULL, 0, 0) == 0;
    if (quitMsg) {
      g_appState.running = false;
      break;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
