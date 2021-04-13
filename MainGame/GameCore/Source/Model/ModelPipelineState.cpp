//////////////////////////////////////////////////////////////////////////////////
///             @file   CubeMapPipeLineState.cpp
///             @brief  CubeMap PipeLine State
///             @author Toide Yutaro
///             @date   2021_03_21
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/ModelPipelineState.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12Debug.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include "GameCore/Include/Model/MMD/PMDConfig.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define

namespace model
{

	bool BuildMMDPipeline(ModelPipeline& model);
	bool BuildFBXPipeline(ModelPipeline& model);
	bool BuildObjPipeline(ModelPipeline& model);

	bool BuildMMDRootSignature(ModelPipeline& model);
	bool BuildFBXRootSignature(ModelPipeline& model);
	bool BuildObjRootSignature(ModelPipeline& model);

	void BuildMMDRasterizer  (D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	void BuildMMDRenderTarget(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	void BuildMMDBlendState  (D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	void BuildMMDDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	
}

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
ModelPSOManager::ModelPSOManager()
{
	_model.resize((int)ModelType::MODEL_TYPE_COUNT);
	SetShaders();
	BuildModelRootSignatures();
	BuildModelPipelines();
}
#pragma region Public Function
/****************************************************************************
*                            BuildCubemapPipelines
*************************************************************************//**
*  @fn        bool CubemapPSOManager::BuildCubemapPipelines()
*  @brief     build cubemap pipeline 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool ModelPSOManager::BuildModelPipelines()
{
	using namespace model;

	/*-------------------------------------------------------------------
	-			Build MMD pipeline
	---------------------------------------------------------------------*/
	if (!BuildMMDPipeline(_model[(int)ModelType::MMD])) 
	{ 
		MessageBox(NULL, L"Could not create a mmd pipeline.", L"Warning", MB_ICONWARNING); 
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build FBX pipeline
	---------------------------------------------------------------------*/
	if (!BuildFBXPipeline((_model[(int)ModelType::FBX])))
	{ 
		MessageBox(NULL, L"Could not create a fbx pipeline.", L"Warning", MB_ICONWARNING); 
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build OBJ pipeline
	---------------------------------------------------------------------*/
	if (!BuildObjPipeline(_model[(int)ModelType::OBJ])) 
	{ 
		MessageBox(NULL, L"Could not create a obj pipeline.", L"Warning", MB_ICONWARNING); 
		return false;
	}
	
	return true;
}

/****************************************************************************
*                            BuildCubemapRootSignatures
*************************************************************************//**
*  @fn        bool CubemapPSOManager::BuildCubemapRootSignatures()
*  @brief     build cubemap rootSignature
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool ModelPSOManager::BuildModelRootSignatures()
{
	using namespace model;

	/*-------------------------------------------------------------------
	-			Build MMD rootSignature
	---------------------------------------------------------------------*/
	if (!BuildMMDRootSignature(_model[(int)ModelType::MMD]))
	{
		MessageBox(NULL, L"Could not create a mmd rootSignature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build FBX rootSignature
	---------------------------------------------------------------------*/
	if (!BuildFBXRootSignature((_model[(int)ModelType::FBX])))
	{
		MessageBox(NULL, L"Could not create a fbx rootSignature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build OBJ rootSignature
	---------------------------------------------------------------------*/
	if (!BuildObjRootSignature(_model[(int)ModelType::OBJ]))
	{
		MessageBox(NULL, L"Could not create a obj rootSignature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	return true;
}



/****************************************************************************
*                            SetShaders
*************************************************************************//**
*  @fn        bool CubemapPSOManager::SetShaders(SpriteType spriteType)
*  @brief     Prepare VS and PS for each type.
*  @param[in] SpriteType spriteType
*  @return 　　bool
*****************************************************************************/
bool ModelPSOManager::SetShaders()
{
	/*-------------------------------------------------------------------
	-			Build MMD Shader
	---------------------------------------------------------------------*/
	_model[(int)ModelType::MMD].VertexShader   = CompileShader(L"Shader\\EnvironmentMap\\ShaderStaticCubemap.hlsl", nullptr, "VSMain", "vs_5_1");
	_model[(int)ModelType::MMD].PixelShader    = CompileShader(L"Shader\\EnvironmentMap\\ShaderStaticCubemap.hlsl", nullptr, "PSMain", "ps_5_1");

	/*-------------------------------------------------------------------
	-			Build FBX Shader
	---------------------------------------------------------------------*/
	//_model[(int)ModelType::FBX].VertexShader   = CompileShader(L"Shader\\EnvironmentMap\\ShaderDynamicCubemap.hlsl", nullptr, "VSMain", "vs_5_1");
	//_model[(int)ModelType::FBX].PixelShader    = CompileShader(L"Shader\\EnvironmentMap\\ShaderDynamicCubemap.hlsl", nullptr, "PSMain", "ps_5_1");

	/*-------------------------------------------------------------------
	-			Build Skybox Shader
	---------------------------------------------------------------------*/
	//_model[(int)ModelType::OBJ].VertexShader   = CompileShader(L"Shader\\EnvironmentMap\\ShaderSkybox.hlsl", nullptr, "VSMain", "vs_5_1");
	//_model[(int)ModelType::OBJ].PixelShader    = CompileShader(L"Shader\\EnvironmentMap\\ShaderSkybox.hlsl", nullptr, "PSMain", "ps_5_1");

	return true;
}

#pragma endregion Public Function
#pragma region Private Function

#pragma endregion Private Functiopn

#pragma region model
namespace model
{
#pragma region FBX
	bool BuildFBXPipeline(ModelPipeline& pipeline)
	{
		return true;
	}

	bool BuildFBXRootSignature(ModelPipeline& model)
	{
		return true;
	}
#pragma endregion FBX

#pragma region OBJ
	bool BuildObjPipeline(ModelPipeline& pipeline)
	{
		return true;
	}

	bool BuildObjRootSignature(ModelPipeline& model)
	{
		return true;
	}
#pragma endregion OBJ

#pragma region MMD
	/****************************************************************************
	*                       BuildMMDPipeline
	*************************************************************************//**
	*  @fn        bool BuildMMDPipeline(ModelPipeline& model)
	*  @brief     Build MMD Pipeline
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildMMDPipeline(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Check Model Type
		---------------------------------------------------------------------*/
		if (model.Type == ModelType::MMD) { return false; }

		/*-------------------------------------------------------------------
		-			 Build RootSignature
		---------------------------------------------------------------------*/
		if (!BuildMMDRootSignature(model)) { return false; }

		/*-------------------------------------------------------------------
		-			 Build PSO
		---------------------------------------------------------------------*/
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
		BuildMMDRasterizer  (pipeLineState);
		BuildMMDRenderTarget(pipeLineState);
		BuildMMDBlendState  (pipeLineState);
		BuildMMDDepthStencil(pipeLineState);
		pipeLineState.InputLayout    = PMDVertex::InputLayout;
		pipeLineState.pRootSignature = model.RootSignature.Get();
		if (model.VertexShader == nullptr || model.PixelShader == nullptr) { MessageBox(NULL, L"don't set shaders.", L"Warning", MB_ICONWARNING); return false; }
		pipeLineState.VS =
		{
			reinterpret_cast<BYTE*>(model.VertexShader->GetBufferPointer()),
			model.VertexShader->GetBufferSize()
		};
		pipeLineState.PS =
		{
			reinterpret_cast<BYTE*>(model.PixelShader->GetBufferPointer()),
			model.PixelShader->GetBufferSize()
		};
		ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&model.PipeLineState)));
		return true;
	}

	/****************************************************************************
	*                            BuildMMDRootSignature
	*************************************************************************//**
	*  @fn        bool BuildMMDRootSignature(CubemapPipeline& cubemap)
	*  @brief     Prepare Skybox RootSignature
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildMMDRootSignature(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Check Model Type
		---------------------------------------------------------------------*/
		if (model.Type == ModelType::MMD) { return false; }

		/*-------------------------------------------------------------------
		-			Build texture table
		---------------------------------------------------------------------*/
		DESCRIPTOR_RANGE descriptorTable[1];
		//descriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, CUBEMAP, 0); // t6

		/*-------------------------------------------------------------------
		-			Build root parameter
		---------------------------------------------------------------------*/
		ROOT_PARAMETER rootParameter[4];
		rootParameter[0].InitAsConstantBufferView(0); // objectConstants
		rootParameter[1].InitAsConstantBufferView(1); // sceneConsntants
		rootParameter[2].InitAsConstantBufferView(2); // materialConstants
		rootParameter[3].InitAsDescriptorTable(1, &descriptorTable[0], D3D12_SHADER_VISIBILITY_ALL);

		/*-------------------------------------------------------------------
		-			Build sampler desc
		---------------------------------------------------------------------*/
		auto samplerDesc = GetStaticSamplers(); // linearsampler

		/*-------------------------------------------------------------------
		-			 Build root parameter
		---------------------------------------------------------------------*/
		// A root signature is a collection of descriptor tables 
		// (which feeds data other than vertices to the shader).
		ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, samplerDesc.size(), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &model.RootSignature);
		return true;
	}

	void BuildMMDRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.RasterizerState                      = RASTERIZER_DESC(D3D12_DEFAULT);
		pipeline.RasterizerState.CullMode             = D3D12_CULL_MODE_NONE;
		pipeline.RasterizerState.DepthBias            = 0.1f;
		pipeline.RasterizerState.SlopeScaledDepthBias = 0.01f;
	}
	void BuildMMDRenderTarget(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.NumRenderTargets = 3;
		pipeline.RTVFormats[0]    = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipeline.RTVFormats[1]    = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipeline.RTVFormats[2]    = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	void BuildMMDBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.BlendState = BLEND_DESC(D3D12_DEFAULT);
		pipeline.BlendState.RenderTarget[0].BlendEnable    = true;//今のところfalse
		pipeline.BlendState.RenderTarget[0].BlendOp        = D3D12_BLEND_OP_ADD;
		pipeline.BlendState.RenderTarget[0].BlendOpAlpha   = D3D12_BLEND_OP_ADD;
		pipeline.BlendState.RenderTarget[0].SrcBlend       = D3D12_BLEND_SRC_ALPHA;
		pipeline.BlendState.RenderTarget[0].DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
		pipeline.BlendState.RenderTarget[0].SrcBlendAlpha  = D3D12_BLEND_ONE;
		pipeline.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	}

	void BuildMMDDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.DepthStencilState                = DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipeline.DepthStencilState.DepthEnable    = true;
		pipeline.DepthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC_LESS;
		pipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipeline.DepthStencilState.StencilEnable  = false;
		pipeline.DSVFormat                        = DXGI_FORMAT_D32_FLOAT;
	}
#pragma endregion MMD

}
#pragma endregion model
