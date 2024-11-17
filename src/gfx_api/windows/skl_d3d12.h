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

struct skl_pos_t {
  float x = 0;
  float y = 0;
  float z = 0;

#if false
  skl_pos_t() = default;

  skl_pos_t(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
  }
#endif
};

struct skl_color_t {
  float r = 0;
  float g = 0;
  float b = 0;
  float a = 1.0f;

#if false
  skl_color_t() = default;

  skl_color_t(float _r, float _g, float _b, float _a) {
    r = _r;
    g = _g;
    b = _b;
    a = _a;
  }
#endif
};

struct skl_vert_t {
  skl_pos_t pos;
  skl_color_t color;

#if false
  skl_vert_t() = default;

  skl_vert_t(skl_pos_t _pos, skl_color_t _col) {
    pos = _pos;
    color = _col;
  }
#endif
};


struct cbv_data_t {
  float r = 0;
  float g = 0;
  float b = 0;
};

struct d3d_ctx_t {
  int fence_val = 0;
  HANDLE evt_handle;
  ComPtr<ID3D12Fence> fence;

  ComPtr<ID3D12CommandQueue> cmd_queue;
  ComPtr<ID3D12GraphicsCommandList> cmd_list;
  ComPtr<ID3D12CommandAllocator> cmd_alloc;
  ComPtr<ID3D12PipelineState> pipeline_state;
  ComPtr<ID3D12RootSignature> root_sig;
  ComPtr<ID3D12Resource> render_targets[2];
  ComPtr<ID3D12DescriptorHeap> rtv_desc_heap;
  ComPtr<ID3D12Device> device;
  D3D12_VERTEX_BUFFER_VIEW vert_buffer_view;
  ComPtr<ID3D12Resource> vert_buffer;

  // D3D12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle;
  // WARNING: need to create 2 of these in practice because we don't want to modify constant buffer data for renders that might be in flight
  ComPtr<ID3D12DescriptorHeap> cbv_desc_heap;
  ComPtr<ID3D12Resource> cbv_resource;

  D3D12_VIEWPORT viewport;
  D3D12_RECT scissor_rect;

  unsigned int frame_idx = 0;
  ComPtr<IDXGISwapChain3> swap_chain3;

  bool valid_context = false;

#if defined(_DEBUG)
  ComPtr<ID3D12Debug> debug_controller;
#endif
};

void init_d3d12(HWND hwnd);
void render_d3d12_frame();
void sync(d3d_ctx_t& context);
