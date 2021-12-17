//////////////////////////////////////////////////////////////////////////////////
///             @file   CubeMapPipeLineState.hpp
///             @brief  Cube Map PipeLine State
///             @author Toide Yutaro
///             @date   2021_03_25
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MODEL_PIPELINE_STATE_HPP
#define MODEL_PIPELINE_STATE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include <vector>

#pragma warning(disable: 26495)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
enum class ModelType
{
	PMD,
	PMX,
	FBX,
	OBJ,
	PRIMITIVE,
	MODEL_TYPE_COUNT
};

struct ModelPipeline
{
	PipelineStateComPtr PipeLineState  = nullptr;
	RootSignatureComPtr RootSignature  = nullptr;
	BlobComPtr          VertexShader   = nullptr;
	BlobComPtr          PixelShader    = nullptr;
	ModelType           Type;
};
/****************************************************************************
*					 ModelPSOManager
*************************************************************************//**
*  @class     ModelPSOManager
*  @brief     ModelPSOManager
*****************************************************************************/
class ModelPSOManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	PipelineState* GetPipelineState(ModelType type) { return this->_model[(int)type].PipeLineState.Get(); }
	RootSignature* GetRootSignature(ModelType type) { return this->_model[(int)type].RootSignature.Get(); }

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static ModelPSOManager& Instance()
	{
		static ModelPSOManager modelPSOManager;
		return modelPSOManager;
	}
	ModelPSOManager(const ModelPSOManager&)            = delete;
    ModelPSOManager& operator=(const ModelPSOManager&) = delete;
	ModelPSOManager(ModelPSOManager&&)                 = delete;
	ModelPSOManager& operator=(ModelPSOManager&&)      = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool BuildModelPipelines();
	bool BuildModelRootSignatures();
	bool SetShaders();
	
	ModelPSOManager();
	~ModelPSOManager() { _model.clear(); _model.shrink_to_fit(); };
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::vector<ModelPipeline> _model;
};


#endif

