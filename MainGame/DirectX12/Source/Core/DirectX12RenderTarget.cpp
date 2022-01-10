//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12RenderTarget.hpp
///             @brief  Render Target
///             @author Toide Yutaro
///             @date   2021_07_25
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12RenderTarget.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                             Define
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
RenderTarget::~RenderTarget()
{
	_renderTargetTexture.Resource = nullptr;
	_depthStencilTexture.Resource = nullptr;
	_renderTarget                 = nullptr;
	_dsvHeap = nullptr;
	_rtvHeap = nullptr;
}

#pragma region Public Function
bool RenderTarget::Create(int width, int height, DXGI_FORMAT colorFormat, DXGI_FORMAT depthStencilFormat, float clearColor[4])
{

	/*-------------------------------------------------------------------
	-             Setting image width and height
	---------------------------------------------------------------------*/
	_renderTargetTexture.ImageSize.x = static_cast<float>(width);
	_renderTargetTexture.ImageSize.y = static_cast<float>(height);
	_depthStencilTexture.ImageSize.x = static_cast<float>(width);
	_depthStencilTexture.ImageSize.y = static_cast<float>(height);

	/*-------------------------------------------------------------------
	-             Create render target texture
 	---------------------------------------------------------------------*/
	if (!CreateRenderTargetTexture(width, height, colorFormat, clearColor))
	{
		 MessageBox(NULL, L" Failed to create texture for the render target.", L"Warning", MB_ICONWARNING);
		 return false;
	}
	/*-------------------------------------------------------------------
	-             Create depth stencil texture
	---------------------------------------------------------------------*/
	if (!depthStencilFormat != DXGI_FORMAT_UNKNOWN)
	{
		if (CreateDepthStencilTexture(width, height, depthStencilFormat))
		{
			MessageBox(NULL, L"Failed to create texture for the depth stencil.", L"Warning", MB_ICONWARNING);
			return false;
		}
	}
	/*-------------------------------------------------------------------
	-             Create descriptor heap
	---------------------------------------------------------------------*/
	if (!CreateDescriptorHeap())
	{
		MessageBox(NULL, L"Failed to create descriptor heap.", L"Warning", MB_ICONWARNING);
		return false;
	}
	/*-------------------------------------------------------------------
	-             Create descriptor.
	---------------------------------------------------------------------*/
	CreateDescriptor();
	if (clearColor)
	{
		memcpy(_rtvClearColor, clearColor, sizeof(_rtvClearColor));
	}

	return true;
}

bool RenderTarget::OnResize(int newWidth, int newHeight)
{
	return true;
}

bool RenderTarget::ClearBuffer()
{
	return true;
}


#pragma endregion Public Function

#pragma region Private Function

/****************************************************************************
*							CreateDescriptorHeap
*************************************************************************//**
*  @fn        bool RenderTarget::CreateDescriptorHeap()
*  @brief     Create descriptor heap
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderTarget::CreateDescriptorHeap()
{
	Device* device = DirectX12::Instance().GetDevice();
	
	/*-------------------------------------------------------------------
	-             Create render target texture heap
	---------------------------------------------------------------------*/
	_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT;
	rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask       = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.GetAddressOf())));

	/*-------------------------------------------------------------------
	-             Create depth stencil texture heap
	---------------------------------------------------------------------*/
	if (_depthStencilTexture.Resource)
	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = DSV_DESC_COUNT;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(device->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.GetAddressOf())));
		_dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	return true;
}

/****************************************************************************
*							CreateRenderTargetTexture
*************************************************************************//**
*  @fn        bool RenderTarget::CreateRenderTargetTexture(int width, int height, DXGI_FORMAT format, float clearColor[4])
*  @brief     Create render target texture
*  @param[in] int width
*  @param[in] int height
*  @param[in] DXGI_FORMAT format
*  @param[in] float clearColor[4]
*  @return 　　bool
*****************************************************************************/
bool RenderTarget::CreateRenderTargetTexture(int width, int height, DXGI_FORMAT format, float clearColor[4])
{
	D3D12_HEAP_PROPERTIES heapProperty = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC   resourceDesc = RESOURCE_DESC::Texture2D(format, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	if (clearColor != nullptr)
	{
		for (int i = 0; i < 4; ++i) { clearValue.Color[i] = clearColor[0]; }
	}
	else
	{
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;
	}

	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(&_renderTarget)));

	_renderTargetTexture.Resource = _renderTarget;
	return true;
}

/****************************************************************************
*							CreateDepthStencliTexture
*************************************************************************//**
*  @fn        bool RenderTarget::CreateDepthStencilTexture(int width, int height, DXGI_FORMAT format)
*  @brief     Create depth stencilt texture 
*  @param[in] int width
*  @param[in] int height 
*  @param[in] DXGI_FORMAT format
*  @return 　　bool
*****************************************************************************/
bool RenderTarget::CreateDepthStencilTexture(int width, int height, DXGI_FORMAT format)
{
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format               = format;
	clearValue.DepthStencil.Depth   = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	D3D12_RESOURCE_DESC resourceDesc = RESOURCE_DESC::Texture2D(format, width, height, 
		1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_TEXTURE_LAYOUT_UNKNOWN);

	auto heapProp = HEAP_PROPERTY(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(_depthStencilTexture.Resource.GetAddressOf())));
	return true;
}

/****************************************************************************
*							CreateDescriptor
*************************************************************************//**
*  @fn        bool RenderTarget::CreateDecriptor()
*  @brief     Create render target view descriptor and depth stencil view descriptor
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderTarget::CreateDescriptor()
{
	Device* device = DirectX12::Instance().GetDevice();

	/*-------------------------------------------------------------------
	-             Create render target view 
	---------------------------------------------------------------------*/
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format        = DXGI_FORMAT_R8G8B8A8_UNORM;

	auto rtvHandler = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateRenderTargetView(_renderTargetTexture.Resource.Get(), &rtvDesc, rtvHandler);
	
	/*-------------------------------------------------------------------
	-             Create depth stencil view
	---------------------------------------------------------------------*/
	if (_depthStencilTexture.Resource)
	{
		auto dsvHandler = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags              = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;
		device->CreateDepthStencilView(_depthStencilTexture.Resource.Get(), &dsvDesc, dsvHandler);
	}
	return true;
}
#pragma endregion Private Function
