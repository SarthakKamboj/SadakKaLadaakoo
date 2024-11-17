#include "skl_win32.h"

static win32_ctx_t i_win32Context;

extern app_state_t g_app_state;

void convert_char_to_wchar(const char* input, __SKL_OUT__ wchar_t* output) {
  mbstowcs(output, input, strlen(input)+1);
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        g_app_state.mouse_x = ((float)x_pos) / clientRect.right;
        g_app_state.mouse_y = ((float)y_pos) / clientRect.bottom;
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

void init_platform_specific(const window_info_t& window_info, const win32_init_ctx_t& win32_init_ctx) {

  WNDCLASS wc = {};
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.hInstance = win32_init_ctx.h_instance;
  wchar_t wWindowInfoName[256]{};
  convert_char_to_wchar(window_info.name, wWindowInfoName);
  wc.lpszClassName = wWindowInfoName;
  wc.lpfnWndProc = window_procedure;

  char windowName[256]{};
  sprintf(windowName, "%s Game", window_info.name);
  wchar_t wWindowName[256]{};
  convert_char_to_wchar(windowName, wWindowName);

  RegisterClass(&wc);

  HWND hwd = CreateWindowEx(
    0, 
    wWindowInfoName,
    wWindowName,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, window_info.width, window_info.height,
    NULL,
    NULL,
    win32_init_ctx.h_instance,
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

  init_d3d12(hwd);

  ShowWindow(hwd, win32_init_ctx.n_show_state);

  i_win32Context.hwd = hwd;
}

void poll_events() {
  MSG msg{};
  while (PeekMessage(&msg, i_win32Context.hwd, 0, 0, PM_NOREMOVE)) {
    bool quitMsg = GetMessage(&msg, NULL, 0, 0) == 0;
    if (quitMsg) {
      g_app_state.running = false;
      break;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}
