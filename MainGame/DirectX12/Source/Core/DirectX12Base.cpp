//////////////////////////////////////////////////////////////////////////////////
///             @file    DirectX12Base.cpp
///             @brief   DirectX12 Initialize ～ BackGround
///             @author  Toide Yutaro
///             @date    2021_01_30
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include <DirectXColors.h>
#include <vector>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                            Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        void DirectX12::Initialize(void)
*  @brief     Initialize Back Screen
*  @param[in] HWND hwnd
*  @return 　　void
*****************************************************************************/
void DirectX12::Initialize(HWND hwnd)
{
	SetHWND(hwnd);
	LoadPipeLine();
	LoadAssets();
}

/****************************************************************************
*							OnResize
*************************************************************************//**
*  @fn        void DirectX12::OnResize(void)
*  @brief     Use when doing screen resizing 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::OnResize()
{
	/*-------------------------------------------------------------------
	-              Nullptr check
	---------------------------------------------------------------------*/
	assert(_device);
	assert(_swapchain);
	assert(_commandAllocator);

	/*-------------------------------------------------------------------
	-              Flush before chainging any resources
	---------------------------------------------------------------------*/
	FlushCommandQueue();
	//ResetCommandList();
	ThrowIfFailed(_commandList->Reset(_commandAllocator[_currentFrameIndex].Get(), nullptr));
	
	/*-------------------------------------------------------------------
	-              Release the previous resources
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		_renderTargetList[i].Reset();
	}
	_depthStencilBuffer.Reset();

	/*-------------------------------------------------------------------
	-                   Resize Swapchain
	---------------------------------------------------------------------*/
	ThrowIfFailed(_swapchain->ResizeBuffers(
		FRAME_BUFFER_COUNT,
		_screen.GetScreenWidth(),
		_screen.GetScreenHeight(),
		_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));	
	_currentFrameIndex = 0;

	BuildRenderTargetView();
	BuildDepthStencilView();
	CompleteInitialize();
	FlushCommandQueue();
	CreateViewport();
}

/****************************************************************************
*                     ClearScreen
*************************************************************************//**
*  @fn        void DirectX12::ClearScreen(void)
*  @brief     Clear Screen. Describe in the first of Draw functions for each scene.
*             The background color is SteelBlue.
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::ClearScreen()
{
	// Reset Commmand 
	ResetCommandList();

	// Indicate a state transition (Present -> Render Target)
	auto barrier = BARRIER::Transition(GetCurrentRenderTarget(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	_commandList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect. 
	// This needs to be reset whenever the command list is reset.
	_commandList->RSSetViewports(1, &_screenViewport);
	_commandList->RSSetScissorRects(1, &_scissorRect);

	// Clear the back buffer and depth buffer.
	_commandList->ClearRenderTargetView(_rtvAllocator.GetCPUDescHandler(_currentFrameIndex), DirectX::Colors::SteelBlue, 0, nullptr);
	_commandList->ClearDepthStencilView(_dsvAllocator.GetCPUDescHandler(0), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Set Render Target 
	auto rtv = _rtvAllocator.GetCPUDescHandler(_currentFrameIndex);
	auto dsv = _dsvAllocator.GetCPUDescHandler(0);
	_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
	ID3D12DescriptorHeap* heapList[] = { _cbvSrvUavHeap.Get() };
	_commandList->SetDescriptorHeaps(_countof(heapList), heapList);
}

/****************************************************************************
*                     CompleteRendering
*************************************************************************//**
*  @fn        void DirectX12::CompleteRendering(void)
*  @brief     End of the drawing process (describe in the last of
*             Draw functions for each scene)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::CompleteRendering()
{
	/*-------------------------------------------------------------------
	-      // Indicate a state transition (Render Target -> Present)
	---------------------------------------------------------------------*/
	auto barrier = BARRIER::Transition(GetCurrentRenderTarget(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	_commandList->ResourceBarrier(1, &barrier);
	
	ThrowIfFailed(_commandList->Close());

	/*-------------------------------------------------------------------
	-          Add command list to the queue for execution
	---------------------------------------------------------------------*/
	ID3D12CommandList* commandLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	/*-------------------------------------------------------------------
	-						Flip screen
	---------------------------------------------------------------------*/
	ThrowIfFailed(_swapchain->Present(VSYNC, 0));
	_currentFrameIndex = (_currentFrameIndex + 1) % FRAME_BUFFER_COUNT;

	FlushCommandQueue();
}


#pragma endregion Public Function

#pragma region Private Function 
/****************************************************************************
*                     LoadPipeLine
*************************************************************************//**
*  @fn        void DirectX12::LoadPipeLine(void)
*  @brief     Load DirectX Basic PipeLine (for initialize) 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::LoadPipeLine()
{
	/*-------------------------------------------------------------------
	-                   Set Debug Layer
	---------------------------------------------------------------------*/
#if _DEBUG
	EnabledDebugLayer();
#endif
	/*-------------------------------------------------------------------
	-					 Create Device
	---------------------------------------------------------------------*/
	CreateDevice();
	/*-------------------------------------------------------------------
	-                   Log Adapters (for debug) 
	---------------------------------------------------------------------*/
#if _DEBUG
	LogAdapters();
#endif
	/*-------------------------------------------------------------------
	-				       Set 4xMsaa
	---------------------------------------------------------------------*/
	CheckMultiSampleQualityLevels();
	/*-------------------------------------------------------------------
	-				       Set 4xMsaa
	---------------------------------------------------------------------*/
	_isHDRSupport = CheckHDRDisplaySupport();
	/*-------------------------------------------------------------------
	-                     Create Fence
	---------------------------------------------------------------------*/
	ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i) { _currentFenceValue[i] = 0; }
	/*-------------------------------------------------------------------
	-                   Create Command Object
	---------------------------------------------------------------------*/
	CreateCommandObject();
	/*-------------------------------------------------------------------
	-                   Create Swapchain
	---------------------------------------------------------------------*/
	CreateSwapChain();
	/*-------------------------------------------------------------------
	-                 Create Descriptor Heap
	---------------------------------------------------------------------*/
	CreateDescriptorHeap();
	BuildResourceAllocator();
	/*-------------------------------------------------------------------
	-                 Create Default PSO
	---------------------------------------------------------------------*/
	CreateDefaultPSO();

}

/****************************************************************************
*                     LoadAssets
*************************************************************************//**
*  @fn        void DirectX12::LoadAssets(void)
*  @brief     Load assets
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::LoadAssets()
{
	OnResize();

}

/****************************************************************************
*                     FlushCommandQueue
*************************************************************************//**
*  @fn        void DirectX12::FlushCommandQueue(void)
*  @brief     Flush Command Queue
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::FlushCommandQueue()
{
	++_currentFenceValue[_currentFrameIndex];
	ThrowIfFailed(_commandQueue->Signal(_fence.Get(), _currentFenceValue[_currentFrameIndex]));

	/*-------------------------------------------------------------------
	-   Wait until the GPU has completed commands up to this fence point
	---------------------------------------------------------------------*/
	if (_fence->GetCompletedValue() < _currentFenceValue[_currentFrameIndex])
	{
		_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(_fence->SetEventOnCompletion(_currentFenceValue[_currentFrameIndex], _fenceEvent));

		if (_fenceEvent != 0)
		{
			WaitForSingleObject(_fenceEvent, INFINITE);
			CloseHandle(_fenceEvent);
		}
	
	}
	_currentFrameIndex = _swapchain->GetCurrentBackBufferIndex();
}

#pragma region Initialize
/****************************************************************************
*                     CreateDevice
*************************************************************************//**
*  @fn        void DirectX12::CreateDevice(void)
*  @brief     Create DirectX12 Device
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::CreateDevice()
{
#ifdef _DEBUG
	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&_dxgiFactory)));
#else
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory)));
#endif

	/*-------------------------------------------------------------------
	-                   Search Hardware Adapter
	---------------------------------------------------------------------*/
	AdapterComPtr adapter;
	for (int i = 0; _dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) { continue; }
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) { break; };
	}
	_useAdapter = adapter.Detach();

	/*-------------------------------------------------------------------
	-                   Create Hardware Device
	---------------------------------------------------------------------*/
	HRESULT hardwareResult = D3D12CreateDevice(
		_useAdapter.Get(),                // default adapter
		D3D_FEATURE_LEVEL_11_0, // minimum feature level
		IID_PPV_ARGS(&_device));

	/*-------------------------------------------------------------------
	-                  Fallback to WARP Device 
	-    (WARP: hight speed fully conformant software rasterizer)
	---------------------------------------------------------------------*/
	if (FAILED(hardwareResult))
	{
		AdapterComPtr warpAdapter;
		ThrowIfFailed(_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&_device)));
		_isWarpAdapter = true;
	}

}

/****************************************************************************
*							CreateCommandObject
*************************************************************************//**
*  @fn        void DirectX12::CreateCommandObject(void)
*  @brief     Create command object (command queue, command allocator, and command list)@n 
*             for initialize.
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::CreateCommandObject()
{
	/*-------------------------------------------------------------------
	-                   Create Command Queue
	---------------------------------------------------------------------*/
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	cmdQDesc.NodeMask = 0;  // No Adapter Mask
	cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; 
	cmdQDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;// enable to execute all command 
	cmdQDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(_device->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(&_commandQueue)));

	/*-------------------------------------------------------------------
	-                   Create Command Allocator
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		ThrowIfFailed(_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&_commandAllocator[i])));
	}
	

	/*-------------------------------------------------------------------
	-                   Create Command List
	---------------------------------------------------------------------*/

	ThrowIfFailed(_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_commandAllocator[_currentFrameIndex].Get(), // Associated command allocator
		nullptr,                 // Initial PipeLine State Object
		IID_PPV_ARGS(_commandList.GetAddressOf())));

	// Start off in a closed state.
	// This is because the first time we refer to the command list when it is reset.
	_commandList->Close();
}

/****************************************************************************
*							CreateDescriptorHeap
*************************************************************************//**
*  @fn        void DirectX12::CreateDescriptorHeap(void)
*  @brief     Create RTV and DSV descriptor heap
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::CreateDescriptorHeap()
{
	/*-------------------------------------------------------------------
	-                   Get Descriptor Inclement Size
	---------------------------------------------------------------------*/
	_rtvDescriptorSize       = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	_dsvDescriptorSize       = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	_cbvSrvUavDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	/*-------------------------------------------------------------------
	-			     Set RTV Heap
	---------------------------------------------------------------------*/
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = RTV_DESC_COUNT;
	rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask       = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));

	/*-------------------------------------------------------------------
	-			     Set DSV Heap 
	---------------------------------------------------------------------*/
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = DSV_DESC_COUNT;
	dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask       = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.GetAddressOf())));

	/*-------------------------------------------------------------------
	-			     Set CBV, SRV, UAV Heap 
	---------------------------------------------------------------------*/
	D3D12_DESCRIPTOR_HEAP_DESC csuHeapDesc;
	csuHeapDesc.NumDescriptors = CBV_DESC_COUNT + SRV_DESC_COUNT + UAV_DESC_COUNT;
	csuHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	csuHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	csuHeapDesc.NodeMask       = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(&csuHeapDesc, IID_PPV_ARGS(&_cbvSrvUavHeap)));
}

/****************************************************************************
*							BuildResourceAllocator
*************************************************************************//**
*  @fn        void DirectX12::BuildResourceAllocator()
*  @brief     Build Resource Allocator
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::BuildResourceAllocator()
{
	/*-------------------------------------------------------------------
	-			     Set RTV Heap
	---------------------------------------------------------------------*/
	auto rtvCpuHandler = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	auto rtvGpuHandler = _rtvHeap->GetGPUDescriptorHandleForHeapStart();
	_rtvAllocator.SetResourceAllocator(RTV_DESC_COUNT, _rtvDescriptorSize, rtvCpuHandler, rtvGpuHandler);

	/*-------------------------------------------------------------------
	-			     Set DSV Heap
	---------------------------------------------------------------------*/
	auto dsvCpuHandler = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvGpuHandler = _dsvHeap->GetGPUDescriptorHandleForHeapStart();
	_dsvAllocator.SetResourceAllocator(DSV_DESC_COUNT, _dsvDescriptorSize, dsvCpuHandler, dsvGpuHandler);

	/*-------------------------------------------------------------------
	-			     Set CBV Heap
	---------------------------------------------------------------------*/
	auto cbvCpuHandler = _cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
	auto cbvGpuHandler = _cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
	_cbvAllocator.SetResourceAllocator(CBV_DESC_COUNT, _cbvSrvUavDescriptorSize, cbvCpuHandler, cbvGpuHandler);

	/*-------------------------------------------------------------------
	-			     Set SRV Heap
	---------------------------------------------------------------------*/
	auto srvCpuHandler = _cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
	srvCpuHandler.ptr += (UINT64)CBV_DESC_COUNT * _cbvSrvUavDescriptorSize;
	auto srvGpuHandler = _cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
	srvGpuHandler.ptr += (UINT64)CBV_DESC_COUNT * _cbvSrvUavDescriptorSize;
	_srvAllocator.SetResourceAllocator(SRV_DESC_COUNT, _cbvSrvUavDescriptorSize, srvCpuHandler, srvGpuHandler);

	/*-------------------------------------------------------------------
	-			     Set UAV Heap
	---------------------------------------------------------------------*/
	auto uavCpuHandler = _cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
	uavCpuHandler.ptr += (UINT64)(CBV_DESC_COUNT + SRV_DESC_COUNT) * _cbvSrvUavDescriptorSize;
	auto uavGpuHandler = _cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
	uavGpuHandler.ptr += (UINT64)(CBV_DESC_COUNT + SRV_DESC_COUNT) * _cbvSrvUavDescriptorSize;
	_srvAllocator.SetResourceAllocator(SRV_DESC_COUNT, _cbvSrvUavDescriptorSize, uavCpuHandler, uavGpuHandler);

}

/****************************************************************************
*							BuildRenderTargetView
*************************************************************************//**
*  @fn        void DirectX12::BuildRenderTargetView(void)
*  @brief     Create Render Target View
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::BuildRenderTargetView()
{
	CPU_DESC_HANDLER rtvHeapHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		ThrowIfFailed(_swapchain->GetBuffer(i, IID_PPV_ARGS(&_renderTargetList[i])));
		auto rtvID = IssueViewID(HeapType::RTV);
		_device->CreateRenderTargetView(_renderTargetList[i].Get(), nullptr, _rtvAllocator.GetCPUDescHandler(rtvID));
	}
}

/****************************************************************************
*							BuildDepthStencilView
*************************************************************************//**
*  @fn        void DirectX12::BuildDepthStencilView(void)
*  @brief     Create Depth / Stencil View
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::BuildDepthStencilView()
{
	/*-------------------------------------------------------------------
	-				 Set Depth / Stencil Descriptor
	---------------------------------------------------------------------*/
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment          = 0;
	depthStencilDesc.Width              = _screen.GetScreenWidth();
	depthStencilDesc.Height             = _screen.GetScreenHeight();
	depthStencilDesc.DepthOrArraySize   = 1;
	depthStencilDesc.MipLevels          = 1;
	depthStencilDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count   = _4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	/*-------------------------------------------------------------------
	-					Set Clear Value
	---------------------------------------------------------------------*/
	D3D12_CLEAR_VALUE optClear;
	optClear.Format               = _depthStencilFormat;
	optClear.DepthStencil.Depth   = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	auto heapProp = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(_depthStencilBuffer.GetAddressOf())));

	/*-------------------------------------------------------------------
	-				Create Depth Stencil View
	---------------------------------------------------------------------*/
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags              = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format             = _depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	auto dsvID = IssueViewID(HeapType::DSV);
	_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, _dsvAllocator.GetCPUDescHandler(dsvID));

	/*-------------------------------------------------------------------
	- Transition the resource from its initial state to be used as a depth buffer.
	---------------------------------------------------------------------*/
	auto barrier = BARRIER::Transition(_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	_commandList->ResourceBarrier(1, &barrier);
}



/****************************************************************************
*							CreateSwapChain
*************************************************************************//**
*  @fn        void DirectX12::CreateSwapChain(void)
*  @brief     Create SwapChain
*  @param[in] void
*  @return 　　void 
*****************************************************************************/
void DirectX12::CreateSwapChain()
{
	/*-------------------------------------------------------------------
	-        Release the previous swapchain we will be recreating
	---------------------------------------------------------------------*/
	_swapchain.Reset();

	/*-------------------------------------------------------------------
	-                   Create Swapchain Descriptor
	---------------------------------------------------------------------*/
	DXGI_SWAP_CHAIN_DESC1 sd;

	sd.BufferCount = FRAME_BUFFER_COUNT;                       // Current: Double Buffer
	sd.Width       = _screen.GetScreenWidth();               // Window Size Width
	sd.Height      = _screen.GetScreenHeight();              // Window Size Height 
	sd.Format      = _backBufferFormat;                      // Back Buffer Format 
	sd.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;            // Alpha Mode => transparency behavior is not specified
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;        // Use the surface or resource as an output render target
	sd.Flags       = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allow Resize Window
	sd.Scaling     = DXGI_SCALING_STRETCH;                   // scaling: stretch
	sd.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;          // bit-block transfer model
	sd.Stereo      = false;
	sd.SampleDesc.Count   = _4xMsaaState ? 4 : 1;                     // MSAA: Anti-Alias
	sd.SampleDesc.Quality = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;  // MSAA: Anti-Alias
	

	/*-------------------------------------------------------------------
	-                   Create Swapchain for hwnd
	---------------------------------------------------------------------*/
	ThrowIfFailed(_dxgiFactory->CreateSwapChainForHwnd(
		_commandQueue.Get(),
		_hwnd,
		&sd,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(_swapchain.GetAddressOf())
	));

	/*-------------------------------------------------------------------
	-                   Set HDR
	---------------------------------------------------------------------*/
#ifdef USE_HDR
	EnsureSwapChainColorSpace();
	SetHDRMetaData();
#endif
}

/****************************************************************************
*							CompleteInitialize
*************************************************************************//**
*  @fn        void DirectX12::CompleteInitialize(void)
*  @brief     Finish Initialize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::CompleteInitialize()
{
	ThrowIfFailed(_commandList->Close());

	/*-------------------------------------------------------------------
	-          Add command list to the queue for execution
	---------------------------------------------------------------------*/
	ID3D12CommandList* commandLists[] = { _commandList.Get() };
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	/*-------------------------------------------------------------------
	-						Flip screen
	---------------------------------------------------------------------*/
	ThrowIfFailed(_swapchain->Present(VSYNC, 0));
	_currentFrameIndex = (_currentFrameIndex + 1) % FRAME_BUFFER_COUNT;

}

/****************************************************************************
*                     CreateViewport
*************************************************************************//**
*  @fn        void DirectX12::CreateViewport(void)
*  @brief     Create default viewport
*  @param[in] void
*  @return 　　void @n
*  @details   viewport: we use the viewport when we want to draw the 3D scene @n
              into a subrectangle of the back buffer.
*****************************************************************************/
void DirectX12::CreateViewport()
{
	_screenViewport.TopLeftX = 0;
	_screenViewport.TopLeftY = 0;
	_screenViewport.Width    = static_cast<float>(_screen.GetScreenWidth());
	_screenViewport.Height   = static_cast<float>(_screen.GetScreenHeight());
	_screenViewport.MinDepth = 0.0f;
	_screenViewport.MaxDepth = 1.0f;

	_scissorRect = { 0,0, _screen.GetScreenWidth(), _screen.GetScreenHeight() };
}

/****************************************************************************
*                     CreateDefaultPSO
*************************************************************************//**
*  @fn        void DirectX12::CreateDefaultPSO(void)
*  @brief     Create default pipeline default object
*  @param[in] void
*  @return 　　void @n
*****************************************************************************/
void DirectX12::CreateDefaultPSO()
{
	VertexPositionNormalColorTexture vertex;

	ZeroMemory(&_defaultPSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	_defaultPSODesc.InputLayout           = vertex.InputLayout;
	_defaultPSODesc.pRootSignature        = nullptr;
	_defaultPSODesc.RasterizerState       = RASTERIZER_DESC(D3D12_DEFAULT);
	_defaultPSODesc.BlendState            = BLEND_DESC(D3D12_DEFAULT);
	_defaultPSODesc.SampleMask            = D3D12_DEFAULT_SAMPLE_MASK;
	_defaultPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	_defaultPSODesc.NumRenderTargets      = 1;
	_defaultPSODesc.RTVFormats[0]         = _backBufferFormat;
	_defaultPSODesc.SampleDesc.Count      = _4xMsaaState ? 4 : 1;
	_defaultPSODesc.SampleDesc.Quality    = _4xMsaaState ? (_4xMsaaQuality - 1) : 0;
	_defaultPSODesc.DSVFormat             = _depthStencilFormat;
	if (_isWarpAdapter)
	{
		_defaultPSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
	}
}


/****************************************************************************
*                     MultiSampleQualityLevels
*************************************************************************//**
*  @fn        void DirectX12::CheckMultiSampleQualityLevels(void)
*  @brief     Multi Sample Quality Levels for 4xMsaa (Anti-Alias)
*  @param[in] void
*  @return 　　void 
*****************************************************************************/
void DirectX12::CheckMultiSampleQualityLevels()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format           = _backBufferFormat;
	msQualityLevels.SampleCount      = 4;
	msQualityLevels.Flags            = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

}

/****************************************************************************
*                     EnsureSwapChainColorSpace
*************************************************************************//**
*  @fn        void DirectX12::EnsureSwapChainColorSpace()
*  @brief     Check SwapChain Color Space
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::EnsureSwapChainColorSpace()
{
	DXGI_SWAP_CHAIN_DESC1 desc;
	ThrowIfFailed(_swapchain->GetDesc1(&desc));

	DXGI_COLOR_SPACE_TYPE colorSpace;
	switch (desc.Format)
	{
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
			break;
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
			break;
		default:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
			break;
	}
	
	_swapchain->SetColorSpace1(colorSpace);
}

/****************************************************************************
*                     SetHDRMetaData
*************************************************************************//**
*  @fn        void DirectX12::SetHDRMetaData()
*  @brief     Set HDR Meta Data
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void DirectX12::SetHDRMetaData()
{
	DXGI_SWAP_CHAIN_DESC1 desc;
	ThrowIfFailed(_swapchain->GetDesc1(&desc));

	/*-------------------------------------------------------------------
	-          In case False (isHDRSupport)
	---------------------------------------------------------------------*/
	if (!_isHDRSupport)
	{
		ThrowIfFailed(_swapchain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_NONE, 0, nullptr));
		return;
	}

	/*-------------------------------------------------------------------
	-          Define Display Chromacity
	---------------------------------------------------------------------*/
	struct DisplayChromacities
	{
		float RedX  ; float RedY  ;
		float GreenX; float GreenY;
		float BlueX ; float BlueY ;
		float WhiteX; float WhiteY;
	};

	static const DisplayChromacities DisplayChromacityList[] =
	{
		 { 0.64000f, 0.33000f, 0.30000f, 0.60000f, 0.15000f, 0.06000f, 0.31270f, 0.32900f }, // Display Gamut Rec709 
		 { 0.70800f, 0.29200f, 0.17000f, 0.79700f, 0.13100f, 0.04600f, 0.31270f, 0.32900f }  // Display Gamut Rec2020
	};

	/*-------------------------------------------------------------------
	-          Select Chroma Format
	---------------------------------------------------------------------*/
	int selectedChroma = 0;
	if (desc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT)
	{
		selectedChroma = 1;
	}

	/*-------------------------------------------------------------------
	-          Set HDR10Meta Data
	---------------------------------------------------------------------*/
	const auto& chroma = DisplayChromacityList[selectedChroma];
	DXGI_HDR_METADATA_HDR10 HDR10MetaData;
	HDR10MetaData.RedPrimary[0]             = UINT16(chroma.RedX   * 50000.0f);
	HDR10MetaData.RedPrimary[1]             = UINT16(chroma.RedY   * 50000.0f);
	HDR10MetaData.GreenPrimary[0]           = UINT16(chroma.GreenX * 50000.0f);
	HDR10MetaData.GreenPrimary[1]           = UINT16(chroma.GreenY * 50000.0f);
	HDR10MetaData.BluePrimary[0]            = UINT16(chroma.BlueX  * 50000.0f);
	HDR10MetaData.BluePrimary[1]            = UINT16(chroma.BlueY  * 50000.0f);
	HDR10MetaData.WhitePoint[0]             = UINT16(chroma.WhiteX * 50000.0f);
	HDR10MetaData.WhitePoint[1]             = UINT16(chroma.WhiteY * 50000.0f);
	HDR10MetaData.MaxMasteringLuminance     = UINT(1000.0f * 10000.0f);
	HDR10MetaData.MinMasteringLuminance     = UINT(0.001f  * 10000.0f);
	HDR10MetaData.MaxContentLightLevel      = UINT16(2000.0f);
	HDR10MetaData.MaxFrameAverageLightLevel = UINT16(500.0f);

	/*-------------------------------------------------------------------
	-          Set HDRMetaData
	---------------------------------------------------------------------*/
	_swapchain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(HDR10MetaData), &HDR10MetaData);
}

/****************************************************************************
*                     CheckHDRDisplaySupport
*************************************************************************//**
*  @fn        bool DirectX12::CheckHDRDisplaySupport()
*  @brief     CheckHDRDisplaySupport()
*  @param[in] void
*  @return 　　void
*****************************************************************************/
bool DirectX12::CheckHDRDisplaySupport()
{
	_backBufferFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	bool isEnabledHDR =
		_backBufferFormat == DXGI_FORMAT_R16G16B16A16_FLOAT ||
		_backBufferFormat == DXGI_FORMAT_R10G10B10A2_UNORM;

	if (isEnabledHDR)
	{
		bool isDisplayHDR10     = false;
		UINT index              = 0;
		
		ComPtr<IDXGIOutput> currentOutput;
		while (_useAdapter->EnumOutputs(index, &currentOutput) != DXGI_ERROR_NOT_FOUND)
		{
			OutputComPtr output;
			currentOutput.As(&output);

			DXGI_OUTPUT_DESC1 desc;
			output->GetDesc1(&desc);
			
			isDisplayHDR10 |= desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
			++index;
		}

		if (!isDisplayHDR10)
		{
			_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			isEnabledHDR = false;
		}
	}

	return isEnabledHDR;
		
}
#pragma endregion Initialize

//////////////////////////////////////////////////////////////////////////////////
//                            Debug
//////////////////////////////////////////////////////////////////////////////////
#pragma region Debug
/****************************************************************************
*                     EnabledDebugLayer
*************************************************************************//**
*  @fn        void DirectX12::EnabledDebugLayer(void)
*  @brief     Enabled CPU debug layer
*  @param[in] void
*  @return 　　void @n
*  @details   it must be called before the D3D12 device is created. 
*****************************************************************************/
void DirectX12::EnabledDebugLayer()
{
	DebugComPtr debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
}

/****************************************************************************
*                   EnabledGPUBasedValiation
*************************************************************************//**
*  @fn        void DirectX12::EnabledGPUBasedValidation(void)
*  @brief     Enabled GPU debugger 
*  @param[in] void
*  @return 　　void
*  @details   GPU-based valiation helps to identify the following errors.@n
*             1. Use of uninitialized or incompatible descriptors in a shader.@n
*             2. Use of descriptors referencing deleted Resources in a shader.@n
*             3. Validation of promoted resource states and resource state decay.@n
*             4. Indexing beyond the end of the descriptor heap in a shader.@n
*             5. Shader accesses of resources in incompatible state.@n
*             6. Use of uninitialized or incompatible Samplers in a shader.@n    
*****************************************************************************/
void DirectX12::EnabledGPUBasedValidation()
{
	DebugComPtr debugController;
	ComPtr<ID3D12Debug3> debug3;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	ThrowIfFailed(debugController->QueryInterface(IID_PPV_ARGS(&debug3)));
	debug3->SetEnableGPUBasedValidation(true);
}

/****************************************************************************
*                     LogAdapters
*************************************************************************//**
*  @fn        void DirectX12::LogAdapters(void)
*  @brief     Show all adapter name (GeForce... )
*  @param[in] Adapter* adapter
*  @return 　　void
*****************************************************************************/
void DirectX12::LogAdapters()
{
	UINT i = 0;
	Adapter* adapter = nullptr;
	std::vector<Adapter*> adapterList;

	/*-------------------------------------------------------------------
	-                  Enum Adapter List
	---------------------------------------------------------------------*/
	while (_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	/*-------------------------------------------------------------------
	-                  Show Adapter List
	---------------------------------------------------------------------*/
	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		SAFE_RELEASE(adapterList[i]);
	}
}

/****************************************************************************
*                     LogAdapterOutputs
*************************************************************************//**
*  @fn        void DirectX12::LogAdapterOutputs(Adapter* adapter)
*  @brief     Show all display output name
*  @param[in] Adapter* adapter
*  @return 　　void
*****************************************************************************/
void DirectX12::LogAdapterOutputs(Adapter* adapter)
{
	/*-------------------------------------------------------------------
	-                     Initialize
	---------------------------------------------------------------------*/
	UINT i = 0;
	Output* output = nullptr;

	/*-------------------------------------------------------------------
	-                Show all adapter name
	---------------------------------------------------------------------*/
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, _backBufferFormat);
		
		SAFE_RELEASE(output);

		++i;
	}
}

/****************************************************************************
*                     LogOutputDisplayModes
*************************************************************************//**
*  @fn        void DirectX12::LogOutputDisplayModes(Output* output, DXGI_FORMAT format)
*  @brief     Show display modes (output screen width, height and refresh rates)
*  @param[in] Output* output: for display modelist
*  @param[in] DXGI_FORMAT   : format
*  @return 　　void
*****************************************************************************/
void DirectX12::LogOutputDisplayModes(Output* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	/*-------------------------------------------------------------------
	-             Call with nullptr to get list count
	---------------------------------------------------------------------*/
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	/*-------------------------------------------------------------------
	-                 Show display modes
	---------------------------------------------------------------------*/
	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}

}
#pragma endregion Debug
#pragma endregion Private Function

//////////////////////////////////////////////////////////////////////////////////
//                            Property
//////////////////////////////////////////////////////////////////////////////////
#pragma region Property
/****************************************************************************
*                          GetDevice
*************************************************************************//**
*  @fn        Device* DirectX12::GetDevice(void)
*  @brief     Get device pointer
*  @param[in] void
*  @return 　　Device*
*****************************************************************************/
Device* DirectX12::GetDevice() const
{
	return _device.Get();
}

/****************************************************************************
*                         GetCommandList
*************************************************************************//**
*  @fn        CommandList* DirectX12::GetCommandList(void)
*  @brief     Get command list pointer
*  @param[in] void
*  @return 　　CommandList*
*****************************************************************************/
CommandList* DirectX12::GetCommandList() const
{
	return _commandList.Get();
}

/****************************************************************************
*                          GetCommandQueue
*************************************************************************//**
*  @fn        CommandQueue* DirectX12::GetCommandQueue(void)
*  @brief     Get command queue pointer
*  @param[in] void
*  @return 　　CommandQueue*
*****************************************************************************/
CommandQueue* DirectX12::GetCommandQueue() const
{
	return _commandQueue.Get();
}

CommandAllocator* DirectX12::GetCommandAllocator() const
{
	return _commandAllocator[_currentFrameIndex].Get();
}
/****************************************************************************
*                        GetCurrentBackBuffer
*************************************************************************//**
*  @fn        Resource* DirectX12::GetCurrentBackBuffer(void)
*  @brief     Get current frame back buffer pointer
*  @param[in] void
*  @return 　　Resource*
*****************************************************************************/
Resource* DirectX12::GetCurrentRenderTarget() const
{
	return _renderTargetList[_currentFrameIndex].Get();
}


/****************************************************************************
*                        GetCPUCbvSrvUavHeapStart
*************************************************************************//**
*  @fn        D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCPUCbvSrvUavHeapStart() const
*  @brief     Get current frame CBV / SRV / UAV pointer
*  @param[in] void
*  @return 　　D3D12_CPU_DESCRIPTOR_HANDLE
*****************************************************************************/
D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCPUCbvSrvUavHeapStart() const
{
	return _cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
}

/****************************************************************************
*                        GetGPUCbvSrvUavHeapStart
*************************************************************************//**
*  @fn        D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetGPUCbvSrvUavHeapStart() const
*  @brief     Get current frame CBV / SRV / UAV pointer
*  @param[in] void
*  @return 　　D3D12_CPU_DESCRIPTOR_HANDLE
*****************************************************************************/
D3D12_GPU_DESCRIPTOR_HANDLE DirectX12::GetGPUCbvSrvUavHeapStart() const
{
	return _cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart();
}

/****************************************************************************
*                        DirectX12::GetCbvSrvUavHeap
*************************************************************************//**
ID3D12DescriptorHeap* DirectX12::GetCbvSrvUavHeap() const
*  @brief     Get current frame CBV / SRV / UAV pointer
*  @param[in] void
*  @return 　　D3D12_CPU_DESCRIPTOR_HANDLE
*****************************************************************************/
ID3D12DescriptorHeap* DirectX12::GetCbvSrvUavHeap() const
{
	return  _cbvSrvUavHeap.Get();
}

/****************************************************************************
*                        GetCPUResourceView
*************************************************************************//**
*  @fn        D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCPUResourceView(HeapType heapType, int offsetIndex) const
*  @brief     Get current frame constant buffer view pointer
*  @param[in] int offsetIndex
*  @return 　　D3D12_CPU_DESCRIPTOR_HANDLE
*****************************************************************************/
D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCPUResourceView(HeapType heapType, int offsetIndex) const
{
	switch (heapType)
	{
	case HeapType::RTV:
		return _rtvAllocator.GetCPUDescHandler(offsetIndex);
	case HeapType::DSV:
		return _dsvAllocator.GetCPUDescHandler(offsetIndex);
	case HeapType::CBV:
		return _cbvAllocator.GetCPUDescHandler(offsetIndex);
	case HeapType::SRV:
		return _srvAllocator.GetCPUDescHandler(offsetIndex);
	default:
		return _uavAllocator.GetCPUDescHandler(offsetIndex);
	}
}

/****************************************************************************
*                        GetGPUResourceView
*************************************************************************//**
*  @fn        D3D12_GPU_DESCRIPTOR_HANDLE DirectX12::GetGPUResourceView(HeapType heapType, int offsetIndex) const
*  @brief     Get current frame constant buffer view pointer
*  @param[in] int offsetIndex
*  @return 　　D3D12_GPU_DESCRIPTOR_HANDLE
*****************************************************************************/
D3D12_GPU_DESCRIPTOR_HANDLE DirectX12::GetGPUResourceView(HeapType heapType, int offsetIndex) const
{
	switch (heapType)
	{
	case HeapType::RTV:
		return _rtvAllocator.GetGPUDescHandler(offsetIndex);
	case HeapType::DSV:
		return _dsvAllocator.GetGPUDescHandler(offsetIndex);
	case HeapType::CBV:
		return _cbvAllocator.GetGPUDescHandler(offsetIndex);
	case HeapType::SRV:
		return _srvAllocator.GetGPUDescHandler(offsetIndex);
	default:
		return _uavAllocator.GetGPUDescHandler(offsetIndex);
	}
}
/****************************************************************************
*                        GetCbvSrvUavDescriptorHeapSize
*************************************************************************//**
*  @fn        INT DirectX12::GetCbvSrvUavDescriptorHeapSize() const
*  @brief     GetCbvSrvUav Descriptor Heap Size
*  @param[in] void
*  @return 　　INT
*****************************************************************************/
INT DirectX12::GetCbvSrvUavDescriptorHeapSize() const
{
	return _cbvSrvUavDescriptorSize;
}
/****************************************************************************
*                           GetDefaultPSOConfig
*************************************************************************//**
*  @fn        D3D12_GRAPHICS_PIPELINE_STATE_DESC DirectX12::GetDefaultPSOConfig() const
*  @brief     Get default pso config
*  @param[in] void
*  @return 　　D3D12_GRAPHICS_PIPELINE_STATE_DESC
*****************************************************************************/
D3D12_GRAPHICS_PIPELINE_STATE_DESC DirectX12::GetDefaultPSOConfig() const
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = _defaultPSODesc;
	return psoDesc;
}

/****************************************************************************
*                        IssueViewID
*************************************************************************//**
*  @fn        UINT DirectX12::IssueViewID(HeapType heapType)
*  @brief     Issue View ID
*  @param[in] void
*  @return 　　UINT
*****************************************************************************/
UINT DirectX12::IssueViewID(HeapType heapType)
{
	switch (heapType)
	{
		case HeapType::RTV:
			return _rtvAllocator.IssueID();
		case HeapType::DSV:
			return _dsvAllocator.IssueID();
		case HeapType::CBV:
			return _cbvAllocator.IssueID();
		case HeapType::SRV:
			return _srvAllocator.IssueID();
		default:
			return _uavAllocator.IssueID();
	}
}
/****************************************************************************
*                           Get4xMsaaState
*************************************************************************//**
*  @fn        bool DirectX12::Get4xMsaaState(void)
*  @brief     Whether to use 4xMsaa (Multi-Sampled Anti-Alias)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool DirectX12::Get4xMsaaState() const
{
	return _4xMsaaState;
}

/****************************************************************************
*                           Set4xMsaaState
*************************************************************************//**
*  @fn        void DirectX12::Get4xMsaaState(bool value)
*  @brief     Set 4xMsaa (Multi-Sampled Anti-Alias)
*  @param[in] bool whether to use 4xMsaa
*  @return 　　void
*****************************************************************************/
void DirectX12::Set4xMsaaState(bool value)
{
	if (_4xMsaaState != value)
	{
		_4xMsaaState = value;

		// Recreate the swapchain and buffers with new multisample setting
		CreateSwapChain();
		OnResize();
	}
}

/****************************************************************************
*                        GetViewport
*************************************************************************//**
*  @fn        D3D12_VIEWPORT DirectX12::GetViewport() const
*  @brief     Get Default viewport
*  @param[in] void
*  @return 　　INT
*****************************************************************************/
D3D12_VIEWPORT DirectX12::GetViewport() const 
{
	return this->_screenViewport;
}

/****************************************************************************
*                        GetScissorRect
*************************************************************************//**
*  @fn        D3D12_RECT DirectX12::GetScissorRect() const
*  @brief     Get Current render target frame index
*  @param[in] void
*  @return 　　D3D12_RECT
*****************************************************************************/
D3D12_RECT DirectX12::GetScissorRect() const
{
	return this->_scissorRect;
}

/****************************************************************************
*                        GetCurrentFrameIndex
*************************************************************************//**
*  @fn        INT DirectX12::GetCurrentFrameIndex() const
*  @brief     Get Current render target frame index
*  @param[in] void
*  @return 　　INT
*****************************************************************************/
INT DirectX12::GetCurrentFrameIndex() const
{
	return this->_currentFrameIndex;
}

/****************************************************************************
*                        GetCurrentBackBufferIndex
*************************************************************************//**
*  @fn        INT DirectX12::GetCurrentBackBufferIndex() const
*  @brief     Get Current BackBufferIndex
*  @param[in] void
*  @return 　　INT
*****************************************************************************/
INT DirectX12::GetCurrentBackBufferIndex() const
{
	if (_currentFrameIndex != FRAME_BUFFER_COUNT - 1)
	{
		return _currentFrameIndex + 1; // next Buffer
	}
	else
	{
		return 0; // 初期フレーム
	}
}
/****************************************************************************
*                           SetHWND
*************************************************************************//**
*  @fn        void DirectX12::SetHWND(HWND hwnd)
*  @brief     Set HWND (Window)
*  @param[in] HWND window
*  @return 　　void
*****************************************************************************/
void DirectX12::SetHWND(HWND hwnd)
{
	assert(hwnd != nullptr);
	_hwnd = hwnd;
}

/****************************************************************************
*                           GetHWND
*************************************************************************//**
*  @fn        HWND DirectX12::GetHWND(void)
*  @brief     Get HWND (Window)
*  @param[in] void
*  @return 　　HWND
*****************************************************************************/
HWND DirectX12::GetHWND() const
{
	return _hwnd;
}
#pragma endregion Property

