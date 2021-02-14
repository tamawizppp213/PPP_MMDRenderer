//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Base.hpp
///             @brief  DirectX12 Initialize  BackGround
///             @author Toide Yutaro
///             @date   2021_01_30 (for texture)
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BASE_HPP
#define DIRECTX12_BASE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Core.hpp"
#include "DirectX12Config.hpp"
#include "DirectX12Debug.hpp"
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
		ThrowIfFailed(_commandAllocator[_currentFrameIndex]->Reset());
		ThrowIfFailed(_commandList->Reset(_commandAllocator[_currentFrameIndex].Get(), nullptr));
	}
	
#pragma region Property
	Device*       GetDevice()                const;
	CommandList*  GetCommandList()           const;
	CommandQueue* GetCommandQueue()          const;
	CommandAllocator* GetCommandAllocator()  const;
	Resource* GetCurrentRenderTarget()       const;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetDefaultPSOConfig() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()   const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvSrvUavHeapStart() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvSrvUavHeapStart() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvSrvUavHeapPtr(int offsetIndex) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvSrvUavHeapPtr(int offsetIndex) const;
	ID3D12DescriptorHeap* GetCbvSrvUavHeap() const;
	INT GetCbvSrvUavDescriptorHeapSize() const;
	D3D12_VIEWPORT GetViewport()     const;
	D3D12_RECT     GetScissorRect()  const;
	INT  GetCurrentFrameIndex()      const;
	INT  GetCurrentBackBufferIndex() const;
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

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static DirectX12& Instance()
	{
		static DirectX12 directX12;
		return directX12;
	}
	DirectX12(const DirectX12&)            = delete;
	DirectX12& operator=(const DirectX12&) = delete;
	DirectX12(DirectX12&&)                 = delete;
	DirectX12& operator=(DirectX12&&)      = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	DirectX12()  = default;
	~DirectX12() = default;
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
	CommandAllocatorComPtr _commandAllocator[FRAME_BUFFER_COUNT];   /// Command Memory Allocator
	DescriptorHeapComPtr   _rtvHeap;            /// Heap For Render Target View 
	DescriptorHeapComPtr   _dsvHeap;            /// Heap For Depth Stencil View
	DescriptorHeapComPtr   _cbvSrvUavHeap;      /// Heap For Constant Buffer View
	PipelineStateComPtr    _pipelineState;      /// Graphic Pipeline State
	ResourceComPtr         _depthStencilBuffer; /// DepthStencl Buffer   
	ResourceComPtr         _renderTargetList[FRAME_BUFFER_COUNT];
	UINT _rtvDescriptorSize       = 0;
	UINT _dsvDescriptorSize       = 0;
	UINT _cbvSrvUavDescriptorSize = 0;
	INT  _currentFrameIndex       = 0;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC _defaultPSODesc;

	// ViewPort
	D3D12_VIEWPORT _screenViewport = {0,0,0,0,0,0};
	D3D12_RECT     _scissorRect    = {0,0,0,0};

	// Synchronization Object
	UINT64      _currentFenceValue[FRAME_BUFFER_COUNT] = {0,0};
	FenceComPtr _fence;
	HANDLE      _fenceEvent   = nullptr;

	// MSAA: One of the Anti-Alias
	bool _4xMsaaState   = false;
	UINT _4xMsaaQuality = 0;

	// Customize starting value
	DXGI_FORMAT _backBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	bool _isWarpAdapter = false;

};

#endif
