//////////////////////////////////////////////////////////////////////////////////
///             @file   FrameResource.hpp
///             @brief  Per Frame Resource Manager
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef FRAME_RESOURCE_HPP
#define FRAME_RESOURCE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12Core.hpp"
#include "DirectX12/Include/DirectX12Buffer.hpp"
#include "DirectX12/Include/DirectX12MathHelper.hpp"
#include <DirectXMath.h>
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			ObjectConstants
*************************************************************************//**
*  @struct    ObjectConstants
*  @brief     Object Infomation per frame
*****************************************************************************/
struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = DirectXMathHelper::Identity4X4();
};

/****************************************************************************
*				  			PassConstants
*************************************************************************//**
*  @struct    PassConstants
*  @brief     Pass Infomation per frame
*****************************************************************************/
struct PassConstants
{
	DirectX::XMFLOAT4X4 View              = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT4X4 InvView           = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT4X4 Proj              = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT4X4 InvProj           = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT4X4 ViewProj          = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT4X4 InvViewProj       = DirectXMathHelper::Identity4X4();
	DirectX::XMFLOAT3   EyePosW           = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT2 RenderTargetSize    = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	float NearZ           = 0.0f;
	float FarZ            = 0.0f;
	float TotalTime       = 0.0f;
	float DeltaTime       = 0.0f;
};


/****************************************************************************
*				  			FrameResource
*************************************************************************//**
*  @struct     FrameResource
*  @brief      per frame resources
*****************************************************************************/
struct FrameResource
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	// We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    CommandAllocatorComPtr CmdListAlloc;

	// We cannot update a cbuffer until the GPU is done processing the commands
	// that reference it.  So each frame needs their own cbuffers.
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// Fence value to mark commands up to this fence point.  This lets us
	// check if these frame resources are still in use by the GPU.
	UINT64 Fence = 0;
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	FrameResource(Device* device, UINT passCount, UINT objectCount) {};
	FrameResource(const FrameResource& rhs)            = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	FrameResource(FrameResource&&)                     = delete;
	FrameResource& operator=(FrameResource&&)          = delete;
	~FrameResource() {};

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
};


#endif