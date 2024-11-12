#pragma once

#include <windows.h>
#include <d3dx12.h>
#include <D3d12.h>
#include <D3d12SDKLayers.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include "defines.h"
#include "../renderer.h"
#include "../windowing.h"

using namespace Microsoft::WRL;

struct SKL_Position {
  float x = 0;
  float y = 0;
  float z = 0;

#if false
  SKL_Position() = default;

  SKL_Position(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
  }
#endif
};

struct SKL_Color {
  float r = 0;
  float g = 0;
  float b = 0;
  float a = 1.0f;

#if false
  SKL_Color() = default;

  SKL_Color(float _r, float _g, float _b, float _a) {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
  }
#endif
};

struct SKL_Vertex {
  SKL_Position pos;
  SKL_Color color;

#if false
  SKL_Vertex() = default;

  SKL_Vertex(SKL_Position _pos, SKL_Color _col) {
    pos = _pos;
    color = _col;
  }
#endif
};


struct D3DContext {
  int fenceValue = 0;
  HANDLE eventHandle;
  ComPtr<ID3D12Fence> fence;

  ComPtr<ID3D12CommandQueue> commandQueue;
  ComPtr<ID3D12GraphicsCommandList> commandList;
  ComPtr<ID3D12CommandAllocator> commandAllocator;
  ComPtr<ID3D12PipelineState> pipelineState;
  ComPtr<ID3D12RootSignature> rootSig;
  ComPtr<ID3D12Resource> renderTargets[2];
  ComPtr<ID3D12DescriptorHeap> descriptorHeap;
  ComPtr<ID3D12Device> device;
  D3D12_VERTEX_BUFFER_VIEW vertBufferView;
  ComPtr<ID3D12Resource> vertexBuffer;

  D3D12_VIEWPORT viewport;
  D3D12_RECT scissorRect;

  unsigned int frameIndex = 0;
  ComPtr<IDXGISwapChain3> swapChain3;

  bool valid_context = false;

};

void InitD3D12(HWND hwnd);
void RenderD3D12Frame();
void sync(D3DContext& context);
