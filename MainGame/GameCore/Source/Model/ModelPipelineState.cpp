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
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include "GameCore/Include/Model/MMD/PMDConfig.hpp"
#include "GameCore/Include/Model/MMD/PMXConfig.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define

namespace model
{

	bool BuildPMDPipeline(ModelPipeline& model);
	bool BuildPMXPipeline(ModelPipeline& model);
	bool BuildFBXPipeline(ModelPipeline& model);
	bool BuildObjPipeline(ModelPipeline& model);
	bool BuildPrimitivePipeline(ModelPipeline& model);

	bool BuildPMDRootSignature(ModelPipeline& model);
	bool BuildPMXRootSignature(ModelPipeline& model);
	bool BuildFBXRootSignature(ModelPipeline& model);
	bool BuildObjRootSignature(ModelPipeline& model);
	bool BuildPrimitiveRootSignature(ModelPipeline& model);

	void BuildRasterizer  (D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	void BuildRenderTarget(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	void BuildBlendState  (D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
	void BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline);
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
void ModelPSOManager::Clear()
{
	for (auto& model : _model)
	{
		model.Dispose();
	}
	_model.clear();
	_model.shrink_to_fit();

}
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
	-			Build PMD pipeline
	---------------------------------------------------------------------*/
	if (!BuildPMDPipeline(_model[(int)ModelType::PMD])) 
	{ 
		MessageBox(NULL, L"Could not create a mmd pipeline.", L"Warning", MB_ICONWARNING); 
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build PMD pipeline
	---------------------------------------------------------------------*/
	if (!BuildPMXPipeline(_model[(int)ModelType::PMX]))
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
	
	/*-------------------------------------------------------------------
	-			Build Primitive pipeline
	---------------------------------------------------------------------*/
	if (!BuildPrimitivePipeline(_model[(int)ModelType::PRIMITIVE]))
	{
		MessageBox(NULL, L"Could not create a obj pipeline.", L"Warning", MB_ICONWARNING);
		return false;
	}
	return true;
}

/****************************************************************************
*                            BuildModelRootSignatures
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
	if (!BuildPMDRootSignature(_model[(int)ModelType::PMD]))
	{
		MessageBox(NULL, L"Could not create a mmd rootSignature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build MMD rootSignature
	---------------------------------------------------------------------*/
	if (!BuildPMXRootSignature(_model[(int)ModelType::PMX]))
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

	/*-------------------------------------------------------------------
	-			Build OBJ rootSignature
	---------------------------------------------------------------------*/
	if (!BuildPrimitiveRootSignature(_model[(int)ModelType::PRIMITIVE]))
	{
		MessageBox(NULL, L"Could not create a primitive rootSignature.", L"Warning", MB_ICONWARNING);
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
	_model[(int)ModelType::PMD].VertexShader   = CompileShader(L"Shader\\Model\\ShaderModelRenderer.hlsl", nullptr, "PMD_VSMain", "vs_5_1");
	_model[(int)ModelType::PMD].PixelShader    = CompileShader(L"Shader\\Model\\ShaderModelRenderer.hlsl", nullptr, "PMD_PSMain", "ps_5_1");

	/*-------------------------------------------------------------------
	-			Build MMD Shader
	---------------------------------------------------------------------*/
	_model[(int)ModelType::PMX].VertexShader = CompileShader(L"Shader\\Model\\ShaderModelRenderer.hlsl", nullptr, "PMX_VSMain", "vs_5_1");
	_model[(int)ModelType::PMX].PixelShader = CompileShader(L"Shader\\Model\\ShaderModelRenderer.hlsl", nullptr, "PMX_PSMain", "ps_5_1");
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

	/*-------------------------------------------------------------------
	-			Build Primitive Shader
	---------------------------------------------------------------------*/
	_model[(int)ModelType::PRIMITIVE].VertexShader = CompileShader(L"Shader\\Model\\ShaderModelRenderer.hlsl", nullptr, "Primitive_VSMain", "vs_5_1");
	_model[(int)ModelType::PRIMITIVE].PixelShader = CompileShader(L"Shader\\Model\\ShaderModelRenderer.hlsl", nullptr, "Primitive_PSMain", "ps_5_1");
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
#pragma region Primitive
	/****************************************************************************
	*                       BuildPMDPipeline
	*************************************************************************//**
	*  @fn        bool BuildPMDPipeline(ModelPipeline& model)
	*  @brief     Build PMD Pipeline
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildPrimitivePipeline(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Substitute Model Type
		---------------------------------------------------------------------*/
		if (model.Type != ModelType::PRIMITIVE) { return false; }

		/*-------------------------------------------------------------------
		-			 Build PSO
		---------------------------------------------------------------------*/
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
		BuildRasterizer(pipeLineState);
		BuildRenderTarget(pipeLineState);
		BuildBlendState(pipeLineState);
		BuildDepthStencil(pipeLineState);
		pipeLineState.InputLayout = VertexPositionNormalTexture::InputLayout;
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
		model.PipeLineState->SetName(L"PrimitiveModel::PipelineState");
		return true;
	}

	/****************************************************************************
	*                            BuildPMDRootSignature
	*************************************************************************//**
	*  @fn        bool BuildPMDRootSignature(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD RootSignature
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildPrimitiveRootSignature(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Substitute Model Type
		---------------------------------------------------------------------*/
		model.Type = ModelType::PRIMITIVE;

		/*-------------------------------------------------------------------
		-			Build texture table
		---------------------------------------------------------------------*/
		DESCRIPTOR_RANGE descriptorTable[2];
		descriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // t0
		descriptorTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0); // t4

		DESCRIPTOR_RANGE lightListTable[2];
		lightListTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10, 0); // t10
		lightListTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11, 0); // t10

		/*-------------------------------------------------------------------
		-			Build root parameter
		---------------------------------------------------------------------*/
		ROOT_PARAMETER rootParameter[8];
		rootParameter[0].InitAsConstantBufferView(0); // objectConstants
		rootParameter[1].InitAsConstantBufferView(1); // sceneConsntants
		rootParameter[2].InitAsConstantBufferView(2); // materialConstants
		rootParameter[3].InitAsConstantBufferView(3); // lightConstants
		rootParameter[4].InitAsDescriptorTable(1, &descriptorTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[5].InitAsDescriptorTable(1, &descriptorTable[1], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[6].InitAsDescriptorTable(1, &lightListTable[0], D3D12_SHADER_VISIBILITY_ALL);
		rootParameter[7].InitAsDescriptorTable(1, &lightListTable[1], D3D12_SHADER_VISIBILITY_ALL);
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
		rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)(samplerDesc.size()), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &model.RootSignature);
		model.RootSignature->SetName(L"PrimitiveModel::RootSignature");
		return true;
	}
#pragma endregion Primitive
#pragma region MMD
	/****************************************************************************
	*                       BuildPMDPipeline
	*************************************************************************//**
	*  @fn        bool BuildPMDPipeline(ModelPipeline& model)
	*  @brief     Build PMD Pipeline
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildPMDPipeline(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Substitute Model Type
		---------------------------------------------------------------------*/
		if (model.Type != ModelType::PMD) { return false; }

		/*-------------------------------------------------------------------
		-			 Build PSO
		---------------------------------------------------------------------*/
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
		BuildRasterizer  (pipeLineState);
		BuildRenderTarget(pipeLineState);
		BuildBlendState  (pipeLineState);
		BuildDepthStencil(pipeLineState);
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
		model.PipeLineState->SetName(L"PMDModel::PipelineState");
		return true;
	}

	/****************************************************************************
	*                       BuildPMDPipeline
	*************************************************************************//**
	*  @fn        bool BuildPMDPipeline(ModelPipeline& model)
	*  @brief     Build PMD Pipeline
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildPMXPipeline(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Substitute Model Type
		---------------------------------------------------------------------*/
		if (model.Type != ModelType::PMX) { return false; }

		/*-------------------------------------------------------------------
		-			 Build PSO
		---------------------------------------------------------------------*/
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
		BuildRasterizer(pipeLineState);
		BuildRenderTarget(pipeLineState);
		BuildBlendState(pipeLineState);
		BuildDepthStencil(pipeLineState);
		pipeLineState.InputLayout = PMXVertex::InputLayout;
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
		model.PipeLineState->SetName(L"PMXModel::PipelineState");
		return true;
	}

	/****************************************************************************
	*                            BuildPMDRootSignature
	*************************************************************************//**
	*  @fn        bool BuildPMDRootSignature(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD RootSignature
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildPMDRootSignature(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Substitute Model Type
		---------------------------------------------------------------------*/
		model.Type = ModelType::PMD;

		/*-------------------------------------------------------------------
		-			Build texture table
		---------------------------------------------------------------------*/
		DESCRIPTOR_RANGE descriptorTable[1];
		descriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // t0

		DESCRIPTOR_RANGE sphereMapMultiplyTable[1];
		sphereMapMultiplyTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0); // t1

		DESCRIPTOR_RANGE sphereMapAdditionTable[1];
		sphereMapAdditionTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0); // t2

		DESCRIPTOR_RANGE toonTextureTable[1];
		toonTextureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0); // t3


		/*-------------------------------------------------------------------
		-			Build root parameter
		---------------------------------------------------------------------*/
		ROOT_PARAMETER rootParameter[9];
		rootParameter[0].InitAsConstantBufferView(0); // objectConstants
		rootParameter[1].InitAsConstantBufferView(1); // sceneConsntants
		rootParameter[2].InitAsConstantBufferView(2); // materialConstants
		rootParameter[3].InitAsConstantBufferView(3); // light
		rootParameter[4].InitAsConstantBufferView(4); // bone
		rootParameter[5].InitAsDescriptorTable(1, &descriptorTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[6].InitAsDescriptorTable(1, &sphereMapMultiplyTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[7].InitAsDescriptorTable(1, &sphereMapAdditionTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[8].InitAsDescriptorTable(1, &toonTextureTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		
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
		rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)samplerDesc.size(), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &model.RootSignature);
		model.RootSignature->SetName(L"PMDModel::RootSignature");
		return true;
	}

	/****************************************************************************
	*                            BuildPMDRootSignature
	*************************************************************************//**
	*  @fn        bool BuildPMDRootSignature(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD RootSignature
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildPMXRootSignature(ModelPipeline& model)
	{
		/*-------------------------------------------------------------------
		-			 Substitute Model Type
		---------------------------------------------------------------------*/
		model.Type = ModelType::PMX;

		/*-------------------------------------------------------------------
		-			Build texture table
		---------------------------------------------------------------------*/
		DESCRIPTOR_RANGE descriptorTable[2];
		descriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // t0
		descriptorTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0); // t4

		DESCRIPTOR_RANGE sphereMapMultiplyTable[1];
		sphereMapMultiplyTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0); // t1

		DESCRIPTOR_RANGE sphereMapAdditionTable[1];
		sphereMapAdditionTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0); // t2

		DESCRIPTOR_RANGE toonTextureTable[1];
		toonTextureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0); // t3

		DESCRIPTOR_RANGE lightListTable[2];
		lightListTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10, 0); // t10
		lightListTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11, 0); // t10

		/*-------------------------------------------------------------------
		-			Build root parameter
		---------------------------------------------------------------------*/
		ROOT_PARAMETER rootParameter[12];
		rootParameter[0].InitAsConstantBufferView(0); // objectConstants
		rootParameter[1].InitAsConstantBufferView(1); // sceneConsntants
		rootParameter[2].InitAsConstantBufferView(2); // materialConstants
		rootParameter[3].InitAsConstantBufferView(3); // lightConstants
		rootParameter[4].InitAsConstantBufferView(4); // bone
		rootParameter[5].InitAsDescriptorTable(1, &descriptorTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[6].InitAsDescriptorTable(1, &sphereMapMultiplyTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[7].InitAsDescriptorTable(1, &sphereMapAdditionTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[8].InitAsDescriptorTable(1, &toonTextureTable[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameter[9].InitAsDescriptorTable(1, &lightListTable[0], D3D12_SHADER_VISIBILITY_ALL);
		rootParameter[10].InitAsDescriptorTable(1, &lightListTable[1], D3D12_SHADER_VISIBILITY_ALL);
		rootParameter[11].InitAsDescriptorTable(1, &descriptorTable[1], D3D12_SHADER_VISIBILITY_PIXEL);
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
		rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)(samplerDesc.size()), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &model.RootSignature);
		model.RootSignature->SetName(L"PMXModel::RootSignature");
		return true;
	}

	/****************************************************************************
	*                            BuildPMDRasterizer
	*************************************************************************//**
	*  @fn        bool BuildPMDRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD Rasterizer
	*  @param[in] void
	*  @return 　　void
	*****************************************************************************/
	void BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.RasterizerState                       = RASTERIZER_DESC(D3D12_DEFAULT);
		pipeline.RasterizerState.CullMode              = D3D12_CULL_MODE_NONE;
		pipeline.RasterizerState.DepthBias             = D3D12_DEFAULT_DEPTH_BIAS;
		pipeline.RasterizerState.DepthBiasClamp        = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		pipeline.RasterizerState.SlopeScaledDepthBias  = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		pipeline.RasterizerState.AntialiasedLineEnable = false;
		pipeline.RasterizerState.ForcedSampleCount     = 0;
		pipeline.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	}

	/****************************************************************************
	*                            BuildPMDRenderTarget
	*************************************************************************//**
	*  @fn        bool BuildPMDRenderTarget(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD RenderTarget
	*  @param[in] void
	*  @return 　　void
	*****************************************************************************/
	void BuildRenderTarget(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
	
		pipeline.NumRenderTargets = 1;
		pipeline.RTVFormats[0]    = DirectX12::Instance().GetBackBufferRenderFormat();
		//pipeline.RTVFormats[1]    = DXGI_FORMAT_R8G8B8A8_UNORM;
		//pipeline.RTVFormats[2]    = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	/****************************************************************************
	*                            BuildPMDBlendState
	*************************************************************************//**
	*  @fn        bool BuildPMDBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD BlendState
	*  @param[in] void
	*  @return 　　void
	*****************************************************************************/
	void BuildBlendState(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.BlendState = BLEND_DESC(D3D12_DEFAULT);
		pipeline.BlendState.AlphaToCoverageEnable          = true;
		pipeline.BlendState.IndependentBlendEnable         = false;
		pipeline.BlendState.RenderTarget[0].BlendEnable    = true;
		pipeline.BlendState.RenderTarget[0].LogicOpEnable  = false;
		pipeline.BlendState.RenderTarget[0].BlendOp        = D3D12_BLEND_OP_ADD;
		pipeline.BlendState.RenderTarget[0].BlendOpAlpha   = D3D12_BLEND_OP_ADD;
		pipeline.BlendState.RenderTarget[0].SrcBlend       = D3D12_BLEND_SRC_ALPHA;
		pipeline.BlendState.RenderTarget[0].DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;	
		pipeline.BlendState.RenderTarget[0].SrcBlendAlpha  = D3D12_BLEND_SRC_ALPHA;
		pipeline.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		pipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}

	/****************************************************************************
	*                            BuildPMDDepthStencil
	*************************************************************************//**
	*  @fn        bool BuildPMDDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	*  @brief     Prepare PMD DepthStencil
	*  @param[in] void
	*  @return 　　void
	*****************************************************************************/
	void BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC& pipeline)
	{
		pipeline.DepthStencilState                = DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipeline.DepthStencilState.DepthEnable    = true;
		pipeline.DepthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC_LESS;
		pipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipeline.DepthStencilState.StencilEnable  = true;
		//pipeline.DSVFormat                        = DXGI_FORMAT_D32_FLOAT;
		pipeline.DSVFormat                        = DXGI_FORMAT_D24_UNORM_S8_UINT;

	}
#pragma endregion MMD

}
#pragma endregion model
