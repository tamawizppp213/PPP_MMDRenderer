//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12Base.hpp
///             @brief  DirectX12 Initialize Å` BackGround
///             @author Toide Yutaro
///             @date   2021_01_30 (for texture)
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_BASE_HPP
#define DIRECTX12_BASE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Config.hpp"
#include "DirectX12BufferAllocator.hpp"
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
	void Finalize();
	void ClearScreen();
	void CompleteInitialize();
	void CopyTextureToBackBuffer(Resource* resource, D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON);
	void CompleteRendering();
	void FlushCommandQueue();
	void FlushCommandQueuesOnResize();
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
	Resource* GetDepthStencil() const;
	Resource* GetCurrentRenderTarget()       const;
	Resource* GetRenderTargetResource(RenderTargetType type)       const;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetDefaultPSOConfig() const;
	D3D12_COMPUTE_PIPELINE_STATE_DESC  GetDefaultComputePSOConfig();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvSrvUavHeapStart() const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvSrvUavHeapStart() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUResourceView(HeapType heapType, int offsetIndex) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUResourceView(HeapType heapType, int offsetIndex) const;
	ID3D12DescriptorHeap* GetCbvSrvUavHeap() const;
	ID3D12DescriptorHeap* GetCPURtvHeapStart() const;
	DXGI_FORMAT GetBackBufferRenderFormat() const { return _backBufferFormat; }

	INT  GetCbvSrvUavDescriptorHeapSize() const;
	UINT IssueViewID(HeapType heapType);
	D3D12_VIEWPORT GetViewport()     const;
	D3D12_RECT     GetScissorRect()  const;
	INT  GetCurrentFrameIndex()      const;
	INT  GetCurrentBackBufferIndex() const;

	bool Get4xMsaaState() const;
	void Set4xMsaaState(bool value);
	void SetHWND(HWND hwnd);
	HWND GetHWND() const;

	void ReportLiveObjects();
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
	~DirectX12();
	void LoadPipeLine();
	void LoadAssets();
	

#pragma region View
	void BuildRenderTargetView();
	void BuildDepthStencilView();
	void BuildResourceAllocator();

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

#pragma region HDR
	void EnsureSwapChainColorSpace();
	bool CheckHDRDisplaySupport();
	void SetHDRMetaData();
#pragma endregion HDR
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
	DeviceComPtr                 _device;             /// Device
	FactoryComPtr                _dxgiFactory;        /// DXGI
	AdapterComPtr                _useAdapter;
	SwapchainComPtr              _swapchain;          /// SwapChain
	CommandQueueComPtr           _commandQueue;       /// Command Queue (Command Execution Unit)
	CommandListComPtr            _commandList;        /// Graphics Command List
	CommandAllocatorComPtr       _commandAllocator[FRAME_BUFFER_COUNT];   /// Command Memory Allocator
	DescriptorHeapComPtr         _rtvHeap;            /// Heap For Render Target View 
	DescriptorHeapComPtr         _dsvHeap;            /// Heap For Depth Stencil View
	DescriptorHeapComPtr         _cbvSrvUavHeap;      /// Heap For Constant Buffer View
	PipelineStateComPtr          _pipelineState;      /// Graphic Pipeline State
	ResourceComPtr               _depthStencilBuffer; /// DepthStencl Buffer   
	ResourceComPtr               _renderTargetList[FRAME_BUFFER_COUNT];
	RenderTargetViewAllocator    _rtvAllocator;
	DepthStencilViewAllocator    _dsvAllocator;
	ConstantBufferViewAllocator  _cbvAllocator;
	ShaderResourceViewAllocator  _srvAllocator;
	UnorderedAccessViewAllocator _uavAllocator;
	UINT _rtvDescriptorSize       = 0;
	UINT _dsvDescriptorSize       = 0;
	UINT _cbvSrvUavDescriptorSize = 0;
	INT  _currentFrameIndex       = 0;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC _defaultPSODesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC();

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
	DXGI_FORMAT _backBufferFormat   = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DXGI_FORMAT _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	bool _isWarpAdapter = false;

	bool _isHDRSupport  = true;
};

#endif