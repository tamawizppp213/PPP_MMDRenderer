//////////////////////////////////////////////////////////////////////////////////
///             @file   TemplateText.hpp
///             @brief  TemplateText
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef DIRECTX12_CUBE_MAP_HPP
#define DIRECTX12_CUBE_MAP_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12Core.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Class
//////////////////////////////////////////////////////////////////////////////////
enum class CubemapFace : int
{
	Right  = 0,
	Left   = 1,
	Top    = 2,
	Bottom = 3,
	Front  = 4,
	Back   = 5
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class DynamicCubemap
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	Resource* Resource();
	GPU_DESC_HANDLER ShaderResourceView();
	GPU_DESC_HANDLER RenderTargetView(int faceIndex);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	DynamicCubemap();
	DynamicCubemap(Device* device);
	DynamicCubemap(const DynamicCubemap& cubemap)            = delete; // prohibit copy
	DynamicCubemap& operator=(const DynamicCubemap& cubemap) = delete; // prohibit copy
	~DynamicCubemap();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Device* _device = nullptr;

	D3D12_VIEWPORT   _viewPort;
	D3D12_RECT       _rect;

	CPU_DESC_HANDLER _cpuSrv;
	GPU_DESC_HANDLER _gpuSrv;
	CPU_DESC_HANDLER _cpuRtv[6];

	ResourceComPtr _cubemap = nullptr;
};

#endif