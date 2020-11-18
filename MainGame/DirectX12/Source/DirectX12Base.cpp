//////////////////////////////////////////////////////////////////////////////////
///             @file    DirectX12Base.cpp
///             @brief   DirectX12 Initialize ～ BackGround
///             @author  Toide Yutaro
///             @date    2020_11_
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12Base.hpp"
#include "DirectX12/Include/DirectX12Config.hpp"
#include "DirectX12/Include/DirectX12BaseStruct.hpp"
#include "GameCore/Include/Screen.hpp"
#include <vector>
#include <string>
#include <cassert>
#include <DirectXColors.h>
#include <iostream>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

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
	ThrowIfFailed(_commandList->Reset(_commandAllocator.Get(), nullptr));
	
	/*-------------------------------------------------------------------
	-              Release the previous resources
	---------------------------------------------------------------------*/
	for (int i = 0; i < SWAPCHAIN_BUFFER; ++i)
	{
		_swapchainBuffer[i].Reset();
	}
	_depthStencilBuffer.Reset();

	/*-------------------------------------------------------------------
	-                   Resize Swapchain
	---------------------------------------------------------------------*/
	ThrowIfFailed(_swapchain->ResizeBuffers(
		SWAPCHAIN_BUFFER,
		_screen.GetScreenWidth(),
		_screen.GetScreenHeight(),
		_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));	
	_currentBackBuffer = 0;

	BuildRenderTargetView();
	BuildDepthStencilView();
	CompleteRendering();
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
	_commandList->ResourceBarrier(1, &BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Set the viewport and scissor rect. 
	// This needs to be reset whenever the command list is reset.
	_commandList->RSSetViewports(1, &_screenViewport);
	_commandList->RSSetScissorRects(1, &_scissorRect);

	// Clear the back buffer and depth buffer.
	_commandList->ClearRenderTargetView(GetCurrentBackBufferView(), DirectX::Colors::SteelBlue, 0, nullptr);
	_commandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Set Render Target 
	_commandList->OMSetRenderTargets(1, &GetCurrentBackBufferView(), true, &GetDepthStencilView());

	// Indicate a state transition (Render Target -> Present)
	_commandList->ResourceBarrier(1, &BARRIER::Transition(GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
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
	-                   Done recording commands
	---------------------------------------------------------------------*/
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
	_currentBackBuffer = (_currentBackBuffer + 1) % SWAPCHAIN_BUFFER;

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
	-                     Create Fence
	---------------------------------------------------------------------*/
	ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
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
	FlushCommandQueue();
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
	_currentFence++;
	ThrowIfFailed(_commandQueue->Signal(_fence.Get(), _currentFence));

	/*-------------------------------------------------------------------
	-   Wait until the GPU has completed commands up to this fence point
	---------------------------------------------------------------------*/
	if (_fence->GetCompletedValue() < _currentFence)
	{
		_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(_fence->SetEventOnCompletion(_currentFence, _fenceEvent));

		WaitForSingleObject(_fenceEvent, INFINITE);
		CloseHandle(_fenceEvent);
	}
	_currentBackBuffer = _swapchain->GetCurrentBackBufferIndex();
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
	/*-------------------------------------------------------------------
	-                   Create Hardware Device
	---------------------------------------------------------------------*/
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,                // default adapter
		D3D_FEATURE_LEVEL_11_0, // minimum feature level
		IID_PPV_ARGS(&_device));

#ifdef _DEBUG
	ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&_dxgiFactory)));
#else
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory)));
#endif
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
	cmdQDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(_device->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(&_commandQueue)));

	/*-------------------------------------------------------------------
	-                   Create Command Allocator
	---------------------------------------------------------------------*/
	ThrowIfFailed(_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_commandAllocator.GetAddressOf())));

	/*-------------------------------------------------------------------
	-                   Create Command List
	---------------------------------------------------------------------*/

	ThrowIfFailed(_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		_commandAllocator.Get(), // Associated command allocator
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
	-			     Set RTV Heap Descriptor Struct
	---------------------------------------------------------------------*/
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SWAPCHAIN_BUFFER;
	rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask       = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));

	/*-------------------------------------------------------------------
	-			     Set DSV Heap Descriptror Struct
	---------------------------------------------------------------------*/
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask       = 0;
	ThrowIfFailed(_device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.GetAddressOf())));
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
	for (UINT i = 0; i < SWAPCHAIN_BUFFER; ++i)
	{
		ThrowIfFailed(_swapchain->GetBuffer(i, IID_PPV_ARGS(&_swapchainBuffer[i])));
		_device->CreateRenderTargetView(_swapchainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, _rtvDescriptorSize);
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
	ThrowIfFailed(_device->CreateCommittedResource(
		&HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT),
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
	_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

	/*-------------------------------------------------------------------
	- Transition the resource from its initial state to be used as a depth buffer.
	---------------------------------------------------------------------*/
	_commandList->ResourceBarrier(1, &BARRIER::Transition(_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
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

	sd.BufferCount = SWAPCHAIN_BUFFER;                       // Current: Double Buffer
	sd.Width       = _screen.GetScreenWidth();                // Window Size Width
	sd.Height      = _screen.GetScreenHeight();               // Window Size Height 
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

/****************************************************************************
*                        GetCurrentBackBuffer
*************************************************************************//**
*  @fn        Resource* DirectX12::GetCurrentBackBuffer(void)
*  @brief     Get current frame back buffer pointer
*  @param[in] void
*  @return 　　Resource*
*****************************************************************************/
Resource* DirectX12::GetCurrentBackBuffer() const
{
	return _swapchainBuffer[_currentBackBuffer].Get();
}

/****************************************************************************
*                        GetCurrentBackBuffer
*************************************************************************//**
*  @fn        D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCurrentBackBufferView(void)
*  @brief     Get current frame back buffer view pointer
*  @param[in] void
*  @return 　　D3D12_CPU_DESCRIPTOR_HANDLE
*****************************************************************************/
D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetCurrentBackBufferView() const
{
	// We need to write it this way because the back buffer is an array
	return CPU_DESC_HANDLER(
		_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		_currentBackBuffer,
		_rtvDescriptorSize);
}

/****************************************************************************
*                        GetDepthStencilView
*************************************************************************//**
*  @fn        D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetDepthStencilView(void)
*  @brief     Get current frame depth/stencil view pointer
*  @param[in] void
*  @return 　　D3D12_CPU_DESCRIPTOR_HANDLE
*****************************************************************************/
D3D12_CPU_DESCRIPTOR_HANDLE DirectX12::GetDepthStencilView() const
{
	return _dsvHeap->GetCPUDescriptorHandleForHeapStart();
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
		//OnResize();
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

