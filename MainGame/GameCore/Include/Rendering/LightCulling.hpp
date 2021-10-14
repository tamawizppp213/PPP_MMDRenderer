//////////////////////////////////////////////////////////////////////////////////
///             @file   LightCulling.hpp
///             @brief  LightCulling
///             @author Toide Yutaro
///             @date   2021_08_05
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef LIGHT_CULLING_HPP
#define LIGHT_CULLING_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/FrameResources.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			LightCulling
*************************************************************************//**
*  @class     LightCulling
*  @brief     LightCulling
*****************************************************************************/
class LightCulling final
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(ColorBuffer& zPrepass);
	bool ExecuteCulling(); // scene, light buffer

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void SetSceneGPUAddress(D3D12_GPU_VIRTUAL_ADDRESS scene) { _scene = scene; }
	void SetLightGPUAddress(D3D12_GPU_VIRTUAL_ADDRESS light) { _light = light; }
	RWStructuredBuffer& GetPointLightList() { return _pointLightList; }
	RWStructuredBuffer& GetSpotLightList() { return _spotLightList; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	LightCulling()                                = default;
	~LightCulling()                               = default;
	LightCulling(const LightCulling&)             = default;
	LightCulling& operator=(const LightCulling&)  = default;
	LightCulling(LightCulling&&)                  = default;
	LightCulling& operator=(LightCulling&&)       = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareResources();
	bool PrepareRootSignature();
	bool PreparePipelineState();

	bool OnResize();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	D3D12_GPU_VIRTUAL_ADDRESS _scene;
	D3D12_GPU_VIRTUAL_ADDRESS _light;

	RootSignatureComPtr _rootSignature = nullptr;
	PipelineStateComPtr _pipeline      = nullptr;

	ColorBuffer* _zPrepass = nullptr;
	RWStructuredBuffer  _pointLightList;
	RWStructuredBuffer  _spotLightList;
};

#endif