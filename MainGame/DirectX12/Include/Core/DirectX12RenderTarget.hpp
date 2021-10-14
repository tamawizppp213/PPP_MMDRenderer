//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12RenderTarget.hpp
///             @brief  Create Render Target
///             @author Toide Yutaro
///             @date   2021_07_25
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_RENDER_TARGET
#define DIRECTX12_RENDER_TARGET

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12Texture.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			RenderTarget
*************************************************************************//**
*  @class     RenderTarget
*  @brief     Create RenderTarget class
*****************************************************************************/
class RenderTarget
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Create(int width, int height, 
		DXGI_FORMAT colorFormat        = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
		float clearColor[4]            = nullptr);
	bool OnResize(int newWidth, int newHeight);
	bool ClearBuffer();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPURtvHeapStart() const { return _rtvHeap.Get()->GetCPUDescriptorHandleForHeapStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDsvHeapStart() const { return _dsvHeap.Get()->GetCPUDescriptorHandleForHeapStart(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPURtvHeapStart() const { return _rtvHeap.Get()->GetGPUDescriptorHandleForHeapStart(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDsvHeapStart() const { return _dsvHeap.Get()->GetGPUDescriptorHandleForHeapStart(); }
	Texture& GetRenderTargetTexture() { return _renderTargetTexture; }
	Texture& GetDepthStencilTexture() { return _depthStencilTexture; }
	int GetWidth()  const { return _renderTargetTexture.ImageSize.x; }
	int GetHeight() const { return _renderTargetTexture.ImageSize.y; }
	DXGI_FORMAT  GetColorBufferFormat() const { return _renderTargetTexture.Format; }
	const float* GetRTVClearColor()     const { return _rtvClearColor; }
	float        GetDSVClearValue()     const { return _dsvClearValue; }
	bool IsExistDepthStencilBuffer()    const { return _depthStencilTexture.Resource.Get(); }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	RenderTarget() = default;
	~RenderTarget();
	RenderTarget(const RenderTarget&)            = default;
	RenderTarget& operator=(const RenderTarget&) = default;
	RenderTarget(RenderTarget&&)                 = default;
	RenderTarget& operator=(RenderTarget&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool CreateDescriptorHeap();
	bool CreateRenderTargetTexture(int width, int height, DXGI_FORMAT format, float clearColor[4]);
	bool CreateDepthStencilTexture(int width, int height, DXGI_FORMAT format);
	bool CreateDescriptor();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Texture _renderTargetTexture;
	Texture _depthStencilTexture;
	ResourceComPtr  _renderTarget = nullptr;
	DescriptorHeapComPtr _rtvHeap = nullptr;
	DescriptorHeapComPtr _dsvHeap = nullptr;
	UINT _rtvDescriptorSize = 0;
	UINT _dsvDescriptorSize = 0;
	float _rtvClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float _dsvClearValue    = 1.0f;
};
#endif