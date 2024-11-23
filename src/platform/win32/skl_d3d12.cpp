#include "skl_d3d12.h"

#include "platform/app.h"

#define NUM_CONSTS 7

static d3d_ctx_t d3d_ctx;

extern app_state_t g_app_state;

// TODO: learn about difference between DXGI and D3d12

void hresult_log(HRESULT result) {
  switch (result) {
    case DXGI_ERROR_INVALID_CALL:
      SKL_LOG("DXGI_ERROR_INVALID_CALL");
      break;
    case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
      SKL_LOG("D3D12_ERROR_DRIVER_VERSION_MISMATCH");
      break;
    case DXGI_ERROR_WAS_STILL_DRAWING:
      SKL_LOG("DXGI_ERROR_WAS_STILL_DRAWING");
      break;
    case E_FAIL:
      SKL_LOG("E_FAIL");
      break;
    case E_INVALIDARG:
      SKL_LOG("E_INVALIDARG");
      break;
    case E_OUTOFMEMORY:
      SKL_LOG("E_OUTOFMEMORY");
      break;
    case E_NOTIMPL:
      SKL_LOG("E_NOTIMPL");
      break;
    case S_FALSE:
      SKL_LOG("S_FALSE");
      break;
    case S_OK:
      SKL_LOG("S_OK");
      break;
    default:
      SKL_LOG("unknown HRESULT");
  }
}

void init_d3d12(HWND hwnd) {

  // create the dxgi factory (which seems to be the graphics separate logic responsible for churning out things to the OS (like how to present swap chain attachments))
  ComPtr<IDXGIFactory4> dxgi_factory;
  if (S_OK != CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory))) {
    SKL_LOG("dxgi factory could not be created");
    return;
  }

#if defined(_DEBUG)
  // enable debug layer
  if (S_OK != D3D12GetDebugInterface(IID_PPV_ARGS(&d3d_ctx.debug_controller))) {
    SKL_LOG("could not create d3d12 debug interface");
    return;
  }
  d3d_ctx.debug_controller->EnableDebugLayer();
  SKL_LOG("enabled d3d12 debug layer");

#if false
  ComPtr<IDXGIDebug1> dxgiDebug;
  if (S_OK != DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))) {
    SKL_LOG("could not create dxgi debug interface");
    return;
  }
  dxgiDebug->EnableLeakTrackingForThread();
#endif
#endif

  // create the device, which is an instance of the D3d12 api
  ComPtr<IDXGIAdapter1> hardware_adapter;
  // GetHardwareAdapter(dxgi_factory.Get(), hardware_adapter.GetAddressOf());
  // GetHardwareAdapter(dxgi_factory.Get(), &hardware_adapter);
  if (S_OK != D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d_ctx.device))) {
    SKL_LOG("could not create device");
    return;
  }

  // create the command queue on the device
  D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
  cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

  if (S_OK != d3d_ctx.device->CreateCommandQueue(&cmd_queue_desc, IID_PPV_ARGS(&d3d_ctx.cmd_queue))) {
    SKL_LOG("command queue is not created");
    return;
  } 

  // create the swap chain with the factory (probably because this is more on the side of communicating with the OS)
  ComPtr<IDXGISwapChain1> swap_chain;
  DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};

  RECT rect = {};
  GetClientRect(hwnd, &rect);
  SKL_LOG("rect top: %i right: %i bottom: %i left: %i ", 
        rect.top, rect.right, rect.bottom, rect.left);
  swapchain_desc.Width = rect.right;
  swapchain_desc.Height = rect.bottom;
  SKL_LOG("swapchain width: %i height: %i ", swapchain_desc.Width, swapchain_desc.Height);
  swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapchain_desc.Stereo = false;
  swapchain_desc.SampleDesc.Count = 1;
  swapchain_desc.SampleDesc.Quality = 0;
  swapchain_desc.Scaling = DXGI_SCALING_NONE;
  swapchain_desc.BufferCount = 2;
  swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  swapchain_desc.Flags = 0;

  if (S_OK != dxgi_factory->CreateSwapChainForHwnd(d3d_ctx.cmd_queue.Get(), hwnd, &swapchain_desc, NULL, NULL, &swap_chain)) {
    SKL_LOG("could not create swap chain");
    return;
  }

  if (S_OK != swap_chain.As(&d3d_ctx.swap_chain3)) {
    SKL_LOG("swap chain 3 could not be created");
    return;
  }
  d3d_ctx.frame_idx = d3d_ctx.swap_chain3->GetCurrentBackBufferIndex();

  // RENDER TARGET DESCRIPTOR HEAP + DESCRIPTOR CREATION

  // descriptors/views describe Id3d resource objects (in this case a render target) 
  // a render target view is a type of descriptor for the render target
  D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
  rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  rtv_heap_desc.NumDescriptors = 2;
  rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  rtv_heap_desc.NodeMask = 0;

  if (S_OK != d3d_ctx.device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&d3d_ctx.rtv_desc_heap))) {
    SKL_LOG("could not make render target view descriptor heap");
    return;
  }

  // for each of those render target view descriptors, convert the buffer from the swap chain backbuffer into a render target view and associate it with the render target view descriptor
  int rtvDescHandleSize = d3d_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(d3d_ctx.rtv_desc_heap->GetCPUDescriptorHandleForHeapStart());
  for (int i = 0; i < rtv_heap_desc.NumDescriptors; i++) {
    if (S_OK != d3d_ctx.swap_chain3->GetBuffer(i, IID_PPV_ARGS(&d3d_ctx.render_targets[i]))) {
      SKL_LOG("could not get back buffer index %i from swapchain", i);
      return;
    }

    // create a render target out of the swap chain buffer and associate it with the render target view
    d3d_ctx.device->CreateRenderTargetView(d3d_ctx.render_targets[i].Get(), NULL, rtv_handle);
    rtv_handle.Offset(1, rtvDescHandleSize);
  }

  // CONSTANT BUFFER VIEW/DESCRIPTOR CREATION

  D3D12_DESCRIPTOR_HEAP_DESC cbv_desc_heap = {};
  cbv_desc_heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
  cbv_desc_heap.NumDescriptors = 1;
  cbv_desc_heap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

  if (S_OK != d3d_ctx.device->CreateDescriptorHeap(&cbv_desc_heap, IID_PPV_ARGS(&d3d_ctx.cbv_desc_heap))) {
    SKL_LOG("could not create descriptor heap for cbv");
    return;
  }

  D3D12_DESCRIPTOR_HEAP_DESC desc = d3d_ctx.cbv_desc_heap->GetDesc();
  SKL_LOG("desc info: type: %i, num desc: %i, flags: %i, node mask: %i", desc.Type, desc.NumDescriptors, desc.Flags, desc.NodeMask);

  D3D12_HEAP_PROPERTIES cbvHeapProps{};
  cbvHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
  cbvHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  cbvHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
  cbvHeapProps.CreationNodeMask = 0;
  cbvHeapProps.VisibleNodeMask = 0;

  D3D12_RESOURCE_DESC cbvResourceDesc{};
  cbvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  cbvResourceDesc.Alignment = 0;
  cbvResourceDesc.Width = sizeof(cbv_data_t);
  cbvResourceDesc.Height = 1;
  cbvResourceDesc.DepthOrArraySize = 1;
  cbvResourceDesc.MipLevels = 1;
  cbvResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
  cbvResourceDesc.SampleDesc.Count = 1;
  cbvResourceDesc.SampleDesc.Quality = 0;
  cbvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  cbvResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

  if (S_OK != d3d_ctx.device->CreateCommittedResource(&cbvHeapProps, D3D12_HEAP_FLAG_NONE, &cbvResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&d3d_ctx.cbv_resource))) {
    SKL_LOG("could not create committed resource for cbv");
    return;
  }

  D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc = {};
  cbv_desc.BufferLocation = d3d_ctx.cbv_resource->GetGPUVirtualAddress();
  cbv_desc.SizeInBytes = (sizeof(cbv_data_t) + 255) & ~255; // must be 256 byte aligned

  CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(d3d_ctx.cbv_desc_heap->GetCPUDescriptorHandleForHeapStart());

  SKL_LOG("buffer gpu BufferLoc: %llu, buffer size in bytes: %i, address of 1st descriptor in cbv descriptor heap: %zu", cbv_desc.BufferLocation, cbv_desc.SizeInBytes, cbvCpuHandle.ptr);

  d3d_ctx.device->CreateConstantBufferView(&cbv_desc, d3d_ctx.cbv_desc_heap->GetCPUDescriptorHandleForHeapStart());

  cbv_data_t data{};
  data.r = 0.0f;
  data.g = 0.0f;
  data.b = 1.0f;

  void* cbv_data_ptr = NULL;
  D3D12_RANGE cbv_rd_range = {0,0};
  HRESULT res = d3d_ctx.cbv_resource->Map(0, &cbv_rd_range, &cbv_data_ptr);
  if (S_OK != res) {
    hresult_log(res);
    SKL_LOG("could not populate cbv resource with data");
    return;
  }
  memcpy(cbv_data_ptr, &data, sizeof(cbv_data_t));
  SKL_LOG("cbv_data_ptr is %p");
  d3d_ctx.cbv_resource->Unmap(0, &cbv_rd_range);
 

  // create the command allocator for this device that will allocate the command lists that will contain the actual commands
  if (S_OK != d3d_ctx.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d_ctx.cmd_alloc))) {
    SKL_LOG("could not create command allocator");
    return;
  }

  D3D12_ROOT_SIGNATURE_DESC root_sig_desc{};

  D3D12_ROOT_PARAMETER root_param[1]{};

  // root constants
  root_param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
  root_param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
  root_param[0].Constants.Num32BitValues = NUM_CONSTS;
  root_param[0].Constants.ShaderRegister = 0;
  root_param[0].Constants.RegisterSpace = 0;

#if false
  // root constant description
  root_param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
  rootParamaters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
  root_param[1].Descriptor.ShaderRegister = 1;
  root_param[1].Descriptor.RegisterSpace = 0;
#endif

  root_sig_desc.NumParameters = sizeof(root_param) / sizeof(D3D12_ROOT_PARAMETER);
  root_sig_desc.pParameters = root_param;
  root_sig_desc.NumStaticSamplers = 0;
  root_sig_desc.pStaticSamplers = NULL;
  root_sig_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

  ComPtr<ID3DBlob> serialized_sig;
  ComPtr<ID3DBlob> error;
  if (S_OK != D3D12SerializeRootSignature(&root_sig_desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized_sig, &error)) {
    SKL_LOG("could not serialize root signature");
    return;
  }
  if (S_OK != d3d_ctx.device->CreateRootSignature(0, serialized_sig->GetBufferPointer(), serialized_sig->GetBufferSize(), IID_PPV_ARGS(&d3d_ctx.root_sig))) {
    SKL_LOG("could not create root signature on device");
    return;
  }

  ComPtr<ID3DBlob> vert_shader;
  ComPtr<ID3DBlob> vert_shader_err;
  ComPtr<ID3DBlob> pixel_shader;
  ComPtr<ID3DBlob> pixel_shader_err;
  const wchar_t* glsl_file_name = L"src\\shaders\\windows\\shaders.glsl";
  if (S_OK != D3DCompileFromFile(glsl_file_name, NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vert_shader, &vert_shader_err)) {
    char* error_msg = (char*)malloc(vert_shader_err->GetBufferSize()+1);
    error_msg[vert_shader_err->GetBufferSize()] = 0;
    memcpy(error_msg, vert_shader_err->GetBufferPointer(), vert_shader_err->GetBufferSize());
    SKL_LOG(error_msg);
    SKL_LOG("could not compile vertex shader");
    return;
  }
  

  if (S_OK != D3DCompileFromFile(glsl_file_name, NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixel_shader, &pixel_shader_err)) {
    char* error_msg = (char*)malloc(pixel_shader_err->GetBufferSize()+1);
    error_msg[pixel_shader_err->GetBufferSize()] = 0;
    memcpy(error_msg, pixel_shader_err->GetBufferPointer(), pixel_shader_err->GetBufferSize());
    SKL_LOG("could not compile fragment shader");
    return;
  }

  // seems like graphics pipeline state is very much tied to a vertex and index buffer layout
  D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc = {};
  pipeline_state_desc.pRootSignature = d3d_ctx.root_sig.Get();
  pipeline_state_desc.VS.pShaderBytecode = vert_shader->GetBufferPointer();
  pipeline_state_desc.VS.BytecodeLength = vert_shader->GetBufferSize();
  pipeline_state_desc.PS.pShaderBytecode = pixel_shader->GetBufferPointer();
  pipeline_state_desc.PS.BytecodeLength = pixel_shader->GetBufferSize();
  pipeline_state_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  pipeline_state_desc.SampleMask = UINT_MAX;
  pipeline_state_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  pipeline_state_desc.DepthStencilState.DepthEnable = false;
  pipeline_state_desc.DepthStencilState.StencilEnable = false;

  D3D12_INPUT_ELEMENT_DESC input_assem_elem_desc[2] = {};
  input_assem_elem_desc[0].SemanticName = "POSITION";
  input_assem_elem_desc[0].SemanticIndex = 0;
  input_assem_elem_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  input_assem_elem_desc[0].InputSlot = 0;
  input_assem_elem_desc[0].AlignedByteOffset = 0;
  input_assem_elem_desc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
  input_assem_elem_desc[0].InstanceDataStepRate = 0;

  input_assem_elem_desc[1].SemanticName = "COLOR";
  input_assem_elem_desc[1].SemanticIndex = 0;
  input_assem_elem_desc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT ;
  input_assem_elem_desc[1].InputSlot = 0;
  input_assem_elem_desc[1].AlignedByteOffset = sizeof(skl_pos_t);
  input_assem_elem_desc[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
  input_assem_elem_desc[1].InstanceDataStepRate = 0;

  pipeline_state_desc.InputLayout.pInputElementDescs = input_assem_elem_desc;
  pipeline_state_desc.InputLayout.NumElements = _countof(input_assem_elem_desc);

  pipeline_state_desc.SampleDesc.Count = 1;
  pipeline_state_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
  pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  pipeline_state_desc.NumRenderTargets = 1;
  pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

  if (S_OK != d3d_ctx.device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(&d3d_ctx.pipeline_state))) {
    SKL_LOG("could not create graphics pipeline state object");
    return;
  }

  if (S_OK != d3d_ctx.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d_ctx.cmd_alloc.Get(), d3d_ctx.pipeline_state.Get(), IID_PPV_ARGS(&d3d_ctx.cmd_list))) {
    SKL_LOG("could not create command list");
    return;
  }

  if (S_OK != d3d_ctx.cmd_list->Close()) {
    SKL_LOG("could not close command list");
    return;
  }

#if 1
  skl_vert_t verts[3] = {
    { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.25f, -0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
  };
#else
  skl_vert_t verts[3] = {
    { { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { { 0.25f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.25f, -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
  };
#endif

  int verticesSize = sizeof(skl_vert_t) * 3;

  D3D12_HEAP_PROPERTIES vert_buff_heap_props = {};
  vert_buff_heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
  vert_buff_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  vert_buff_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
  vert_buff_heap_props.CreationNodeMask = 0;

  D3D12_RESOURCE_DESC vert_buff_resource_desc = {};
  vert_buff_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  // NOTE: why should alignment be 0 instead of 1?
  vert_buff_resource_desc.Alignment = 0;
  vert_buff_resource_desc.Width = verticesSize;
  vert_buff_resource_desc.Height = 1;
  vert_buff_resource_desc.DepthOrArraySize = 1;
  vert_buff_resource_desc.MipLevels = 1;
  vert_buff_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
  vert_buff_resource_desc.SampleDesc.Count = 1;
  vert_buff_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
  vert_buff_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

  if (S_OK != d3d_ctx.device->CreateCommittedResource(&vert_buff_heap_props, D3D12_HEAP_FLAG_NONE, &vert_buff_resource_desc, 
                                                D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&d3d_ctx.vert_buffer))) {
    SKL_LOG("could not create the vertex buffer");
    return;
  }

  void* vert_buffer_data_ptr = NULL;
  D3D12_RANGE vert_buff_rd_range = {0,0};
  if (S_OK != d3d_ctx.vert_buffer->Map(0, &vert_buff_rd_range, &vert_buffer_data_ptr)) {
    SKL_LOG("could not get the data ptr of the data referenced by the vertex buffer");
    return;
  }
  memcpy(vert_buffer_data_ptr, verts, verticesSize);
  d3d_ctx.vert_buffer->Unmap(0, &vert_buff_rd_range);

  d3d_ctx.vert_buffer_view.BufferLocation = d3d_ctx.vert_buffer->GetGPUVirtualAddress();
  d3d_ctx.vert_buffer_view.SizeInBytes = verticesSize;
  d3d_ctx.vert_buffer_view.StrideInBytes = sizeof(skl_vert_t);

  if (S_OK != d3d_ctx.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d_ctx.fence))) {
    SKL_LOG("could not create fence");
    return;
  }
  d3d_ctx.fence_val = 1;

  d3d_ctx.evt_handle = CreateEventA(NULL, false, false, NULL);
  if (d3d_ctx.evt_handle == NULL) {
    SKL_LOG("could not create event");
    return;
  }

  d3d_ctx.viewport.TopLeftX = 0;
  d3d_ctx.viewport.TopLeftY = 0;
  d3d_ctx.viewport.Width = swapchain_desc.Width;
  d3d_ctx.viewport.Height = swapchain_desc.Height;
  d3d_ctx.viewport.MinDepth = 0;
  d3d_ctx.viewport.MaxDepth = 0;

  d3d_ctx.scissor_rect.top = 0;
  d3d_ctx.scissor_rect.bottom = swapchain_desc.Height;
  d3d_ctx.scissor_rect.left = 0;
  d3d_ctx.scissor_rect.right = swapchain_desc.Width;

  sync(d3d_ctx);
  d3d_ctx.valid_context = true;

  SKL_LOG("successfully initialized D3d12");
}

void render_d3d12_frame(transform_t* transform, render_options_t* render_options) {

  if (S_OK != d3d_ctx.cmd_alloc->Reset()) {
    SKL_LOG("could not reset allocator");
    return;
  }

  if (S_OK != d3d_ctx.cmd_list->Reset(d3d_ctx.cmd_alloc.Get(), d3d_ctx.pipeline_state.Get())) {
    SKL_LOG("could not reset command list");
    return;
  }

  if (!d3d_ctx.valid_context) {
    SKL_LOG("not valid context");
  }

  d3d_ctx.cmd_list->SetGraphicsRootSignature(d3d_ctx.root_sig.Get());

  float mouse_pos[2] = {transform->screen_x_pos * 2 - 1, (1-transform->screen_y_pos) * 2 - 1};
  float color[3] = {render_options->color[0], render_options->color[1], render_options->color[2]};
  float constants[NUM_CONSTS] = {
    color[0], color[1], color[2], 0,
    mouse_pos[0], mouse_pos[1],
  };
  d3d_ctx.cmd_list->SetGraphicsRoot32BitConstants(0, NUM_CONSTS, constants, 0);

#if false
  D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress;
  d3d_ctx.cmd_list->SetGraphicsRootConstantBufferView(1, cbvGpuAddress);
#endif

  d3d_ctx.cmd_list->RSSetViewports(1, &d3d_ctx.viewport);
  d3d_ctx.cmd_list->RSSetScissorRects(1, &d3d_ctx.scissor_rect);

  // set to render target
  {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = d3d_ctx.render_targets[d3d_ctx.frame_idx].Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    d3d_ctx.cmd_list->ResourceBarrier(1, &barrier);
  }

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(d3d_ctx.rtv_desc_heap->GetCPUDescriptorHandleForHeapStart());
  UINT rtv_handle_size = d3d_ctx.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  rtv_handle.Offset(d3d_ctx.frame_idx, rtv_handle_size);

  static float clear_color[4] = {0,0,0,1};
  d3d_ctx.cmd_list->OMSetRenderTargets(1, &rtv_handle, false, NULL);
  d3d_ctx.cmd_list->ClearRenderTargetView(rtv_handle, clear_color, 0, NULL);
  d3d_ctx.cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  
  d3d_ctx.cmd_list->IASetVertexBuffers(0, 1, &d3d_ctx.vert_buffer_view);
  d3d_ctx.cmd_list->DrawInstanced(3, 1, 0, 0);

  // setting to present
  {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = d3d_ctx.render_targets[d3d_ctx.frame_idx].Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    d3d_ctx.cmd_list->ResourceBarrier(1, &barrier);
  }

  d3d_ctx.cmd_list->Close();

  ID3D12CommandList* commandListsToExec[] = { d3d_ctx.cmd_list.Get() };
  d3d_ctx.cmd_queue->ExecuteCommandLists(1, commandListsToExec);
  d3d_ctx.swap_chain3->Present(1,0);

  sync(d3d_ctx);
}

void sync(d3d_ctx_t& context) {
  unsigned int signal_val = context.fence_val;
  if (S_OK != context.cmd_queue->Signal(context.fence.Get(), signal_val)) {
    SKL_LOG("could not configure GPU to signal fence with signal_val: %i", signal_val);
    return;
  }
  context.fence_val++;

  if (context.fence->GetCompletedValue() != signal_val) {
    context.fence->SetEventOnCompletion(signal_val, context.evt_handle);
    WaitForSingleObject(context.evt_handle, INFINITE);
  }

  context.frame_idx = context.swap_chain3->GetCurrentBackBufferIndex();
}
