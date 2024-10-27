#include "skl_d3d12.h"

#include <D3d12.h>
#include <d3dx12.h>
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

  if (S_OK != dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, NULL, NULL, &swapChain)) {
    SKL_LOG("could not create swap chain");
    return;
  }

  ComPtr<IDXGISwapChain3> swapChain3;
  if (S_OK != swapChain.As(&swapChain3)) {
    SKL_LOG("swap chain 3 could not be created");
    return;
  }
  int frameIndex = swapChain3->GetCurrentBackBufferIndex();

  ComPtr<ID3D12DescriptorHeap> descriptorHeap;
  D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
  heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  heapDesc.NumDescriptors = 2;
  heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  heapDesc.NodeMask = 0;

  if (S_OK != device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap))) {
    SKL_LOG("could not make render target view descriptor heap");
    return;
  }

#ifdef D3D12_RESOURCE_DESC1
  SKL_LOG("D3D12_RESOURCE_DESC1 defined");
#else
  SKL_LOG("D3D12_RESOURCE_DESC1 not defined");
#endif

#if true
  int rtvDescHandleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
  ComPtr<ID3D12Resource> renderTargets[2];
  for (int i = 0; i < heapDesc.NumDescriptors; i++) {
    if (S_OK != swapChain3->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]))) {
      SKL_LOG("could not get back buffer index %i from swapchain", i);
      return;
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    device->CreateRenderTargetView(renderTargets[i].Get(), NULL, rtvHandle);
    rtvHandle.Offset(1, rtvDescHandleSize);
  }

  ComPtr<ID3D12CommandAllocator> commandAllocator;
  if (S_OK != device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))) {
    SKL_LOG("could not create command allocator");
    return;
  }
#endif


  SKL_LOG("successfully initialized D3d12");
}
