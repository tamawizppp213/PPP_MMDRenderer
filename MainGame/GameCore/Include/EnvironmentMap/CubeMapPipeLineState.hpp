//////////////////////////////////////////////////////////////////////////////////
///             @file   CubeMapPipeLineState.hpp
///             @brief  Cube Map PipeLine State
///             @author Toide Yutaro
///             @date   2021_03_25
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CUBE_MAP_PIPELINE_STATE_HPP
#define CUBE_MAP_PIPELINE_STATE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class CubemapType
{
	STATIC,
	DYNAMIC,
	SKYBOX,
	CUBEMAP_COUNT
};

struct CubemapPipeline
{
	PipelineStateComPtr PipeLineState  = nullptr;
	RootSignatureComPtr RootSignature  = nullptr;
	BlobComPtr          VertexShader   = nullptr;
	BlobComPtr          PixelShader    = nullptr;
	BlobComPtr          GeometryShader = nullptr;
	CubemapType         Type           = CubemapType::STATIC;

	void Dispose()
	{
		PipeLineState = nullptr;
		RootSignature = nullptr;
		VertexShader = nullptr;
		PixelShader = nullptr;
		GeometryShader = nullptr;

	}
};
/****************************************************************************
*					 CubemapPSOManager
*************************************************************************//**
*  @class     CubemapPSOManager
*  @brief     CubemapPSOManager
*****************************************************************************/
class CubemapPSOManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Clear();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	PipelineState* GetPipelineState(CubemapType type) { return this->_cubemap[(int)type].PipeLineState.Get(); }
	RootSignature* GetRootSignature(CubemapType type) { return this->_cubemap[(int)type].RootSignature.Get(); }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static CubemapPSOManager& Instance()
	{
		static CubemapPSOManager cubemapPSOManager;
		return cubemapPSOManager;
	}
	CubemapPSOManager(const CubemapPSOManager&)            = delete;
    CubemapPSOManager& operator=(const CubemapPSOManager&) = delete;
	CubemapPSOManager(CubemapPSOManager&&)                 = delete;
	CubemapPSOManager& operator=(CubemapPSOManager&&)      = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool BuildCubemapPipelines();
	bool BuildCubemapRootSignatures();
	bool SetShaders();
	
	CubemapPSOManager();
	~CubemapPSOManager() = default;
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::vector<CubemapPipeline> _cubemap;
};


#endif

