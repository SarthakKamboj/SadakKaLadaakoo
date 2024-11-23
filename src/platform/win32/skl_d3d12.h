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
#include "geometry.h"
#include "platform/app.h"
#include "entities.h"

using namespace Microsoft::WRL;

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
void render_d3d12_frame(transform_t* transform, render_options_t* render_options);
void sync(d3d_ctx_t& context);
