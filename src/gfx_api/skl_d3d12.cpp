#include "skl_d3d12.h"

#include <D3d12.h>
#include <D3d12SDKLayers.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include <wrl.h>

#include "defines.h"

using namespace Microsoft::WRL;

// TODO: learn about difference between DXGI and D3d12

void hresult_log(HRESULT result) {
  switch (result) {
    case DXGI_ERROR_INVALID_CALL:
      SKL_LOG("DXGI_ERROR_INVALID_CALL");
      break;
    default:
      SKL_LOG("unknown HRESULT");
  }
}

void initD3D12(HWND hwnd) {

  ComPtr<IDXGIFactory4> dxgiFactory;
  if (S_OK != CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))) {
  // if (S_OK != CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory))) {
    SKL_LOG("dxgi factory could not be created");
    return;
  }

  ComPtr<ID3D12Debug> debugController;
  if (S_OK != D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))) {
    SKL_LOG("could not create debug interface");
    return;
  }
  debugController->EnableDebugLayer();

  ComPtr<ID3D12Device> device;
  if (S_OK != D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))) {
    SKL_LOG("could not create device");
    return;
  }

  D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
  commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

  ComPtr<ID3D12CommandQueue> commandQueue;
  if (S_OK != device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue))) {
    SKL_LOG("command queue is not created");
    return;
  } 

#if true
  {
    ComPtr<IDXGISwapChain1> swapChain;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

    RECT rect = {};
    GetWindowRect(hwnd, &rect);
    swapChainDesc.Width = rect.right - rect.left;
    swapChainDesc.Height = rect.bottom - rect.top;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags = 0;

    HRESULT res = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, NULL, NULL, &swapChain);
    if (S_OK != res) {
      SKL_LOG("could not create swap chain");
      hresult_log(res);
      return;
    }
  }
#else
  DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
  swapChainDesc.BufferCount = 2;
  RECT rect = {};
  GetWindowRect(hwnd, &rect);
  swapChainDesc.BufferDesc.Width = rect.right - rect.left;
  swapChainDesc.BufferDesc.Height = rect.bottom - rect.top;
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.OutputWindow = hwnd;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.Windowed = TRUE;

  ComPtr<IDXGISwapChain> swapChain;
  dxgiFactory->CreateSwapChain(
    commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
    &swapChainDesc,
    &swapChain
  );
#endif

  SKL_LOG("successfully initialized D3d12");
}
