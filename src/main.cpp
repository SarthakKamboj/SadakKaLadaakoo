#include <cstdio>
#include <stdio.h>

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <WindowsX.h>
#include <D3d12.h>
#include <D3d12SDKLayers.h>
#include <dxgi1_4.h>
#include <wrl.h>

#define SKL_LOG(test, ...) printf("\n%s", test, ##__VA_ARGS__)

using namespace Microsoft::WRL;

void initD3D12() {
  ComPtr<ID3D12Device> device;
  SKL_LOG("about to call D3D12CreateDevice");
  HRESULT res = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
  SKL_LOG("res is success: %i for S_OK and %i for S_FALSE", res == S_OK, res == S_FALSE);
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
    
    ShowWindow(hwd, nShowState);
  }

  initD3D12();

  MSG msg{};
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}
