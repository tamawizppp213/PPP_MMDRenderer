//////////////////////////////////////////////////////////////////////////////////
///             @file   SpritePipeLineState.hpp
///             @brief  Sprite PipeLine State
///             @author Toide Yutaro
///             @date   2021_01_20
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SPLITE_PIPELINE_STATE_HPP
#define SPLITE_PIPELINE_STATE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Core.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12BaseStruct.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class SpriteType
{
	TEXTURE,
	COLOR,
	SPRITE_TYPE_COUNT
};

/****************************************************************************
*					SpritePipelineStateDescriptor
*************************************************************************//**
*  @struct    SpritePipelineStateDescriptor
*  @brief     SpritePipelineState Descriptor
*****************************************************************************/
struct SpritePipelineStateDescriptor
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool BuildSpritePipeline();
	bool BuildRootSignature();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	bool SetShaders(SpriteType spriteType);
	PipelineStateComPtr& GetPipelineState() { return this->_pipeLineState; }
	RootSignatureComPtr& GetRootSignature() { return this->_rootSignature; }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	SpritePipelineStateDescriptor()  = default;
	~SpritePipelineStateDescriptor() = default;
	

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	D3D12_BLEND_DESC BuildBlendDescriptor();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	PipelineStateComPtr _pipeLineState = nullptr;
	RootSignatureComPtr _rootSignature = nullptr;
	BlobComPtr _vertexShader           = nullptr;
	BlobComPtr _pixelShader            = nullptr;
	
};

/****************************************************************************
*				  			SpritePSOManager
*************************************************************************//**
*  @class     SpritePSOManager
*  @brief     Create Sprite PSO Manager
*****************************************************************************/
class SpritePSOManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	const PipelineStateComPtr& GetPipelineState(SpriteType type);
	const RootSignatureComPtr& GetRootSignature(SpriteType type);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static SpritePSOManager& Instance()
	{
		static SpritePSOManager spritePSOManager;
		return spritePSOManager;
	}
	SpritePSOManager(const SpritePSOManager&)            = delete;
	SpritePSOManager& operator=(const SpritePSOManager&) = delete;
	SpritePSOManager(SpritePSOManager&&)                 = delete;
	SpritePSOManager& operator=(SpritePSOManager&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	SpritePSOManager();
	~SpritePSOManager();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::vector<SpritePipelineStateDescriptor> _psoList;
};
#endif

