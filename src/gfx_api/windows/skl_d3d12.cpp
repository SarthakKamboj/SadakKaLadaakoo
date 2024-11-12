#include "skl_d3d12.h"

#include "app_state.h"
#include "defines.h"

#define NUM_CONSTS 7

D3DContext d3dContext;

extern AppState g_appState;

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

void InitD3D12(HWND hwnd) {

  // create the dxgi factory (which seems to be the graphics separate logic responsible for churning out things to the OS (like how to present swap chain attachments))
  ComPtr<IDXGIFactory4> dxgiFactory;
  if (S_OK != CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))) {
    SKL_LOG("dxgi factory could not be created");
    return;
  }

  // enable debug layer
  ComPtr<ID3D12Debug> debugController;
  if (S_OK != D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))) {
    SKL_LOG("could not create debug interface");
    return;
  }
  debugController->EnableDebugLayer();

  // create the device, which is an instance of the D3d12 api
  ComPtr<IDXGIAdapter1> hardwareAdapter;
  // GetHardwareAdapter(dxgiFactory.Get(), hardwareAdapter.GetAddressOf());
  // GetHardwareAdapter(dxgiFactory.Get(), &hardwareAdapter);
  if (S_OK != D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dContext.device))) {
    SKL_LOG("could not create device");
    return;
  }

  // create the command queue on the device
  D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
  commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

  if (S_OK != d3dContext.device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&d3dContext.commandQueue))) {
    SKL_LOG("command queue is not created");
    return;
  } 

  // create the swap chain with the factory (probably because this is more on the side of communicating with the OS)
  ComPtr<IDXGISwapChain1> swapChain;
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

  RECT rect = {};
  GetClientRect(hwnd, &rect);
  SKL_LOG("rect top: %i right: %i bottom: %i left: %i ", 
        rect.top, rect.right, rect.bottom, rect.left);
  swapChainDesc.Width = rect.right;
  swapChainDesc.Height = rect.bottom;
  SKL_LOG("swapchain width: %i height: %i ", swapChainDesc.Width, swapChainDesc.Height);
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

  if (S_OK != dxgiFactory->CreateSwapChainForHwnd(d3dContext.commandQueue.Get(), hwnd, &swapChainDesc, NULL, NULL, &swapChain)) {
    SKL_LOG("could not create swap chain");
    return;
  }

  if (S_OK != swapChain.As(&d3dContext.swapChain3)) {
    SKL_LOG("swap chain 3 could not be created");
    return;
  }
  d3dContext.frameIndex = d3dContext.swapChain3->GetCurrentBackBufferIndex();

  // descriptors/views describe Id3d resource objects (in this case a render target) 
  // a render target view is a type of descriptor for the render target
  D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
  heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  heapDesc.NumDescriptors = 2;
  heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  heapDesc.NodeMask = 0;

  if (S_OK != d3dContext.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&d3dContext.descriptorHeap))) {
    SKL_LOG("could not make render target view descriptor heap");
    return;
  }

  // for each of those render target view descriptors, convert the buffer from the swap chain backbuffer into a render target view and associate it with the render target view descriptor
  int rtvDescHandleSize = d3dContext.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(d3dContext.descriptorHeap->GetCPUDescriptorHandleForHeapStart());
  for (int i = 0; i < heapDesc.NumDescriptors; i++) {
    if (S_OK != d3dContext.swapChain3->GetBuffer(i, IID_PPV_ARGS(&d3dContext.renderTargets[i]))) {
      SKL_LOG("could not get back buffer index %i from swapchain", i);
      return;
    }

    // create a render target out of the swap chain buffer and associate it with the render target view
    d3dContext.device->CreateRenderTargetView(d3dContext.renderTargets[i].Get(), NULL, rtvHandle);
    rtvHandle.Offset(1, rtvDescHandleSize);
  }

  // create the command allocator for this device that will allocate the command lists that will contain the actual commands
  if (S_OK != d3dContext.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3dContext.commandAllocator))) {
    SKL_LOG("could not create command allocator");
    return;
  }

  D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};

  D3D12_ROOT_PARAMETER rootParameter = {};
  rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
  rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
  rootParameter.Constants.Num32BitValues = NUM_CONSTS;
  rootParameter.Constants.ShaderRegister = 0;
  rootParameter.Constants.RegisterSpace = 0;

  rootSigDesc.NumParameters = 1;
  rootSigDesc.pParameters = &rootParameter;
  rootSigDesc.NumStaticSamplers = 0;
  rootSigDesc.pStaticSamplers = NULL;
  rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

  ComPtr<ID3DBlob> serializedSig;
  ComPtr<ID3DBlob> error;
  if (S_OK != D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedSig, &error)) {
    SKL_LOG("could not serialize root signature");
    return;
  }
  if (S_OK != d3dContext.device->CreateRootSignature(0, serializedSig->GetBufferPointer(), serializedSig->GetBufferSize(), IID_PPV_ARGS(&d3dContext.rootSig))) {
    SKL_LOG("could not create root signature on device");
    return;
  }

  ComPtr<ID3DBlob> vertexShader;
  ComPtr<ID3DBlob> vertShaderError;
  ComPtr<ID3DBlob> pixelShader;
  ComPtr<ID3DBlob> pixelShaderError;
  const wchar_t* fileName = L"src\\shaders\\windows\\shaders.glsl";
  if (S_OK != D3DCompileFromFile(fileName, NULL, NULL, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShader, &vertShaderError)) {
    char* error_msg = (char*)malloc(vertShaderError->GetBufferSize()+1);
    error_msg[vertShaderError->GetBufferSize()] = 0;
    memcpy(error_msg, vertShaderError->GetBufferPointer(), vertShaderError->GetBufferSize());
    SKL_LOG(error_msg);
    SKL_LOG("could not compile vertex shader");
    return;
  }
  

  if (S_OK != D3DCompileFromFile(fileName, NULL, NULL, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelShader, &pixelShaderError)) {
    char* error_msg = (char*)malloc(pixelShaderError->GetBufferSize()+1);
    error_msg[pixelShaderError->GetBufferSize()] = 0;
    memcpy(error_msg, pixelShaderError->GetBufferPointer(), pixelShaderError->GetBufferSize());
    SKL_LOG("could not compile fragment shader");
    return;
  }

  // seems like graphics pipeline state is very much tied to a vertex and index buffer layout
  D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};
  pipelineStateDesc.pRootSignature = d3dContext.rootSig.Get();
  pipelineStateDesc.VS.pShaderBytecode = vertexShader->GetBufferPointer();
  pipelineStateDesc.VS.BytecodeLength = vertexShader->GetBufferSize();
  pipelineStateDesc.PS.pShaderBytecode = pixelShader->GetBufferPointer();
  pipelineStateDesc.PS.BytecodeLength = pixelShader->GetBufferSize();
  pipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
  pipelineStateDesc.SampleMask = UINT_MAX;
  pipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
  pipelineStateDesc.DepthStencilState.DepthEnable = false;
  pipelineStateDesc.DepthStencilState.StencilEnable = false;

  D3D12_INPUT_ELEMENT_DESC inputAssemElemDesc[2] = {};
  inputAssemElemDesc[0].SemanticName = "POSITION";
  inputAssemElemDesc[0].SemanticIndex = 0;
  inputAssemElemDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  inputAssemElemDesc[0].InputSlot = 0;
  inputAssemElemDesc[0].AlignedByteOffset = 0;
  inputAssemElemDesc[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
  inputAssemElemDesc[0].InstanceDataStepRate = 0;

  inputAssemElemDesc[1].SemanticName = "COLOR";
  inputAssemElemDesc[1].SemanticIndex = 0;
  inputAssemElemDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT ;
  inputAssemElemDesc[1].InputSlot = 0;
  inputAssemElemDesc[1].AlignedByteOffset = sizeof(SKL_Position);
  inputAssemElemDesc[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
  inputAssemElemDesc[1].InstanceDataStepRate = 0;

  pipelineStateDesc.InputLayout.pInputElementDescs = inputAssemElemDesc;
  pipelineStateDesc.InputLayout.NumElements = _countof(inputAssemElemDesc);

  pipelineStateDesc.SampleDesc.Count = 1;
  pipelineStateDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
  pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
  pipelineStateDesc.NumRenderTargets = 1;
  pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

  if (S_OK != d3dContext.device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&d3dContext.pipelineState))) {
    SKL_LOG("could not create graphics pipeline state object");
    return;
  }

  if (S_OK != d3dContext.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dContext.commandAllocator.Get(), d3dContext.pipelineState.Get(), IID_PPV_ARGS(&d3dContext.commandList))) {
    SKL_LOG("could not create command list");
    return;
  }

  if (S_OK != d3dContext.commandList->Close()) {
    SKL_LOG("could not close command list");
    return;
  }

#if 1
  SKL_Vertex verts[3] = {
    { { 0.0f, 0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { { 0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.25f, -0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
  };
#else
  SKL_Vertex verts[3] = {
    { { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { { 0.25f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.25f, -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
  };
#endif

  int verticesSize = sizeof(SKL_Vertex) * 3;

  D3D12_HEAP_PROPERTIES vertexBuffHeapProps = {};
  vertexBuffHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
  vertexBuffHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
  vertexBuffHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
  vertexBuffHeapProps.CreationNodeMask = 0;

  D3D12_RESOURCE_DESC vertBuffResourceDesc = {};
  vertBuffResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
  // NOTE: why should alignment be 0 instead of 1?
  vertBuffResourceDesc.Alignment = 0;
  vertBuffResourceDesc.Width = verticesSize;
  vertBuffResourceDesc.Height = 1;
  vertBuffResourceDesc.DepthOrArraySize = 1;
  vertBuffResourceDesc.MipLevels = 1;
  vertBuffResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
  vertBuffResourceDesc.SampleDesc.Count = 1;
  vertBuffResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR ;
  vertBuffResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

  if (S_OK != d3dContext.device->CreateCommittedResource(&vertexBuffHeapProps, D3D12_HEAP_FLAG_NONE, &vertBuffResourceDesc, 
                                                D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&d3dContext.vertexBuffer))) {
    SKL_LOG("could not create the vertex buffer");
    return;
  }

  void* vertexBufferDataPtr = NULL;
  D3D12_RANGE readRange = {0,0};
  if (S_OK != d3dContext.vertexBuffer->Map(0, &readRange, &vertexBufferDataPtr)) {
    SKL_LOG("could not get the data ptr of the data referenced by the vertex buffer");
    return;
  }
  memcpy(vertexBufferDataPtr, verts, verticesSize);
  d3dContext.vertexBuffer->Unmap(0, &readRange);

  d3dContext.vertBufferView.BufferLocation = d3dContext.vertexBuffer->GetGPUVirtualAddress();
  d3dContext.vertBufferView.SizeInBytes = verticesSize;
  d3dContext.vertBufferView.StrideInBytes = sizeof(SKL_Vertex);

  if (S_OK != d3dContext.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3dContext.fence))) {
    SKL_LOG("could not create fence");
    return;
  }
  d3dContext.fenceValue = 1;

  d3dContext.eventHandle = CreateEventA(NULL, false, false, NULL);
  if (d3dContext.eventHandle == NULL) {
    SKL_LOG("could not create event");
    return;
  }

  d3dContext.viewport.TopLeftX = 0;
  d3dContext.viewport.TopLeftY = 0;
  d3dContext.viewport.Width = swapChainDesc.Width;
  d3dContext.viewport.Height = swapChainDesc.Height;
  d3dContext.viewport.MinDepth = 0;
  d3dContext.viewport.MaxDepth = 0;

  d3dContext.scissorRect.top = 0;
  d3dContext.scissorRect.bottom = swapChainDesc.Height;
  d3dContext.scissorRect.left = 0;
  d3dContext.scissorRect.right = swapChainDesc.Width;

  sync(d3dContext);
  d3dContext.valid_context = true;

  SKL_LOG("successfully initialized D3d12");

}

void RenderD3D12Frame() {

  if (S_OK != d3dContext.commandAllocator->Reset()) {
    SKL_LOG("could not reset allocator");
    return;
  }

  if (S_OK != d3dContext.commandList->Reset(d3dContext.commandAllocator.Get(), d3dContext.pipelineState.Get())) {
    SKL_LOG("could not reset command list");
    return;
  }

  if (!d3dContext.valid_context) {
    SKL_LOG("not valid context");
  }

  d3dContext.commandList->SetGraphicsRootSignature(d3dContext.rootSig.Get());
  float mouse_pos[2] = {g_appState.mouse_x * 2 - 1, (1-g_appState.mouse_y) * 2 - 1};
  float color[3] = {1.0f, 0, 1.0f};
  float constants[NUM_CONSTS] = {
    mouse_pos[0], mouse_pos[1],
    0,0,
    color[0], color[1], color[2]
  };
  d3dContext.commandList->SetGraphicsRoot32BitConstants(0, NUM_CONSTS, constants, 0);

  d3dContext.commandList->RSSetViewports(1, &d3dContext.viewport);
  d3dContext.commandList->RSSetScissorRects(1, &d3dContext.scissorRect);

  // set to render target
  {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = d3dContext.renderTargets[d3dContext.frameIndex].Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    d3dContext.commandList->ResourceBarrier(1, &barrier);
  }

  CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(d3dContext.descriptorHeap->GetCPUDescriptorHandleForHeapStart());
  UINT rtvHandleSize = d3dContext.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
  rtvHandle.Offset(d3dContext.frameIndex, rtvHandleSize);

  static float clearColor[4] = {0,0,0,1};
  d3dContext.commandList->OMSetRenderTargets(1, &rtvHandle, false, NULL);
  d3dContext.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
  d3dContext.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  
  d3dContext.commandList->IASetVertexBuffers(0, 1, &d3dContext.vertBufferView);
  d3dContext.commandList->DrawInstanced(3, 1, 0, 0);

  // setting to present
  {
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = d3dContext.renderTargets[d3dContext.frameIndex].Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    d3dContext.commandList->ResourceBarrier(1, &barrier);
  }

  d3dContext.commandList->Close();

  ID3D12CommandList* commandListsToExec[] = { d3dContext.commandList.Get() };
  d3dContext.commandQueue->ExecuteCommandLists(1, commandListsToExec);
  d3dContext.swapChain3->Present(1,0);

  sync(d3dContext);
}

void sync(D3DContext& context) {
  unsigned int signalValue = context.fenceValue;
  if (S_OK != context.commandQueue->Signal(context.fence.Get(), signalValue)) {
    SKL_LOG("could not configure GPU to signal fence with signalValue: %i", signalValue);
    return;
  }
  context.fenceValue++;

  if (context.fence->GetCompletedValue() != signalValue) {
    context.fence->SetEventOnCompletion(signalValue, context.eventHandle);
    WaitForSingleObject(context.eventHandle, INFINITE);
  }

  context.frameIndex = context.swapChain3->GetCurrentBackBufferIndex();
}

void RenderFrame(RenderContext& renderContext) {
  RenderD3D12Frame();
}
