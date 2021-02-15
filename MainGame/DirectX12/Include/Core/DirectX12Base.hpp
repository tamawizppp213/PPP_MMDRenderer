//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Base.hpp
///             @brief  DirectX12 Initialize Å` BackGround
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BASE_HPP
#define DIRECTX12_BASE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"
#include "DirectX12Config.hpp"
#include "DirectX12/Include/DirectX12Debug.hpp"
#include "GameCore/Include/Screen.hpp"
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         DirectX12 Class
//////////////////////////////////////////////////////////////////////////////////
class DirectX12
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize(HWND hwnd);
	void OnResize();
	void ClearScreen();
	void CompleteInitialize();
	void CompleteRendering();
	void FlushCommandQueue();
	inline void ResetCommandList()
	{
		ThrowIfFailed(_commandAllocator->Reset());
		ThrowIfFailed(_commandList->Reset(_commandAllocator.Get(), nullptr));
	}
	
#pragma region Property
	Device*       GetDevice()                    const;
	CommandList*  GetCommandList()               const;
	CommandQueue* GetCommandQueue()              const;
	CommandAllocator* GetCommandAllocator()      const;
	Resource* GetCurrentBackBuffer()             const;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetDefaultPSOConfig() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const;
	bool Get4xMsaaState() const;
	void Set4xMsaaState(bool value);
	void SetHWND(HWND hwnd);
	HWND GetHWND() const;
#pragma endregion Property

	// Constructor
	//DirectX12() = default;
	//~DirectX12() = default;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void LoadPipeLine();
	void LoadAssets();
	

#pragma region View
	void BuildRenderTargetView();
	void BuildDepthStencilView();

#pragma endregion View

#pragma region Initialize
	void CreateDevice();
	void CreateCommandObject();
	void CreateSwapChain();
	void CreateDescriptorHeap();
	void CreateViewport();
	void CreateDefaultPSO();
	void CheckMultiSampleQualityLevels();
	
#pragma endregion
#pragma region Debug
	void EnabledDebugLayer();
	void EnabledGPUBasedValidation();
	
	void LogAdapters();
	void LogAdapterOutputs(Adapter* adapter);
	void LogOutputDisplayModes(Output* output, DXGI_FORMAT format);
#pragma endregion Debug
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	// Window Handler
	HWND _hwnd = nullptr;
	Screen _screen;

	// Basic DirectX12 Object
	DeviceComPtr           _device;             /// Device
	FactoryComPtr          _dxgiFactory;        /// DXGI
	SwapchainComPtr        _swapchain;          /// SwapChain
	CommandQueueComPtr     _commandQueue;       /// Command Queue (Command Execution Unit)
	CommandListComPtr      _commandList;        /// Graphics Command List
	CommandAllocatorComPtr _commandAllocator;   /// Command Memory Allocator
	DescriptorHeapComPtr   _rtvHeap;            /// Heap For Render Target View 
	DescriptorHeapComPtr   _dsvHeap;            /// Heap For Depth Stencil View
	DescriptorHeapComPtr   _cbvHeap;            /// Heao Fir Constant Buffer View
	PipelineStateComPtr    _pipelineState;      /// Graphic Pipeline State
	ResourceComPtr         _swapchainBuffer[SWAPCHAIN_BUFFER]; /// Swapchain Buffer (0:Front, 1: Back Buffer)
	ResourceComPtr         _depthStencilBuffer; /// DepthStencl Buffer   
	UINT _rtvDescriptorSize       = 0;
	UINT _dsvDescriptorSize       = 0;
	UINT _cbvSrvUavDescriptorSize = 0;
	INT  _currentBackBuffer       = 0;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC _defaultPSODesc;

	// ViewPort
	D3D12_VIEWPORT _screenViewport = {0,0,0,0,0,0};
	D3D12_RECT     _scissorRect    = {0,0,0,0};

	// Synchronization Object
	UINT64      _currentFence = 0; // fence
	FenceComPtr _fence;
	HANDLE      _fenceEvent   = nullptr;

	// MSAA: One of the Anti-Alias
	bool _4xMsaaState   = false;
	UINT _4xMsaaQuality = 0;

	// Customize starting value
	DXGI_FORMAT _backBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

#endif