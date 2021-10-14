//////////////////////////////////////////////////////////////////////////////////
///             @file   CubeMapPipeLineState.cpp
///             @brief  CubeMap PipeLine State
///             @author Toide Yutaro
///             @date   2021_03_21
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/EnvironmentMap/CubeMapPipeLineState.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12Debug.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define CUBEMAP 6
namespace cubemap
{
	bool BuildStaticCubemapPipeline (CubemapPipeline& cubemap);
	bool BuildDynamicCubemapPipeline(CubemapPipeline& cubemap);
	bool BuildSkyboxPipeline        (CubemapPipeline& cubemap);

	bool BuildStaticCubemapRootSignature (CubemapPipeline& cubemap);
	bool BuildDynamicCubemapRootSignature(CubemapPipeline& cubemap);
	bool BuildSkyboxRootSignature        (CubemapPipeline& cubemap);

	D3D12_BLEND_DESC BuildStaticCubemapBlendDescriptor ();
	D3D12_BLEND_DESC BuildDynamicCubemapBlendDescriptor();
	D3D12_BLEND_DESC BuildSkyboxCubemapBlendDescriptor();

	RASTERIZER_DESC  BuildSkyboxRasterizerDescriptor();
}

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
CubemapPSOManager::CubemapPSOManager()
{
	_cubemap.resize((int)CubemapType::CUBEMAP_COUNT);
	SetShaders();
	BuildCubemapRootSignatures();
	BuildCubemapPipelines();
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
bool CubemapPSOManager::BuildCubemapPipelines()
{
	using namespace cubemap;

	/*-------------------------------------------------------------------
	-			Build Static Cubemap pipeline
	---------------------------------------------------------------------*/
	if (!BuildStaticCubemapPipeline(_cubemap[(int)CubemapType::STATIC])) 
	{ 
		MessageBox(NULL, L"Could not create a static cubemap pipeline.", L"Warning", MB_ICONWARNING); 
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build Dynamic Cubemap pipeline
	---------------------------------------------------------------------*/
	if (!BuildDynamicCubemapPipeline((_cubemap[(int)CubemapType::DYNAMIC])))
	{ 
		MessageBox(NULL, L"Could not create a dynamic cubemap pipeline.", L"Warning", MB_ICONWARNING); 
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build Skybox Cubemap pipeline
	---------------------------------------------------------------------*/
	if (!BuildSkyboxPipeline(_cubemap[(int)CubemapType::SKYBOX])) 
	{ 
		MessageBox(NULL, L"Could not create a skybox cubemap pipeline.", L"Warning", MB_ICONWARNING); 
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
bool CubemapPSOManager::BuildCubemapRootSignatures()
{
	using namespace cubemap;

	/*-------------------------------------------------------------------
	-			Build Static Cubemap rootSignature
	---------------------------------------------------------------------*/
	if (!BuildStaticCubemapRootSignature(_cubemap[(int)CubemapType::STATIC]))
	{
		MessageBox(NULL, L"Could not create a static cubemap rootSignature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build Dynamic Cubemap rootSignature
	---------------------------------------------------------------------*/
	if (!BuildDynamicCubemapRootSignature((_cubemap[(int)CubemapType::DYNAMIC])))
	{
		MessageBox(NULL, L"Could not create a dynamic cubemap rootSignature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-			Build Skybox Cubemap rootSignature
	---------------------------------------------------------------------*/
	if (!BuildSkyboxRootSignature(_cubemap[(int)CubemapType::SKYBOX]))
	{
		MessageBox(NULL, L"Could not create a skybox cubemap rootSignature.", L"Warning", MB_ICONWARNING);
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
bool CubemapPSOManager::SetShaders()
{
	/*-------------------------------------------------------------------
	-			Build Static Cubemap Shader
	---------------------------------------------------------------------*/
	_cubemap[(int)CubemapType::STATIC].VertexShader   = CompileShader(L"Shader\\EnvironmentMap\\ShaderStaticCubemap.hlsl", nullptr, "VSMain", "vs_5_1");
	_cubemap[(int)CubemapType::STATIC].PixelShader    = CompileShader(L"Shader\\EnvironmentMap\\ShaderStaticCubemap.hlsl", nullptr, "PSMain", "ps_5_1");
	_cubemap[(int)CubemapType::STATIC].GeometryShader = nullptr;

	/*-------------------------------------------------------------------
	-			Build Dynamic Cubemap Shader
	---------------------------------------------------------------------*/
	_cubemap[(int)CubemapType::DYNAMIC].VertexShader   = CompileShader(L"Shader\\EnvironmentMap\\ShaderDynamicCubemap.hlsl", nullptr, "VSMain", "vs_5_1");
	_cubemap[(int)CubemapType::DYNAMIC].PixelShader    = CompileShader(L"Shader\\EnvironmentMap\\ShaderDynamicCubemap.hlsl", nullptr, "PSMain", "ps_5_1");
	_cubemap[(int)CubemapType::DYNAMIC].GeometryShader = nullptr;

	/*-------------------------------------------------------------------
	-			Build Skybox Shader
	---------------------------------------------------------------------*/
	_cubemap[(int)CubemapType::SKYBOX].VertexShader   = CompileShader(L"Shader\\EnvironmentMap\\ShaderSkybox.hlsl", nullptr, "VSMain", "vs_5_1");;
	_cubemap[(int)CubemapType::SKYBOX].PixelShader    = CompileShader(L"Shader\\EnvironmentMap\\ShaderSkybox.hlsl", nullptr, "PSMain", "ps_5_1");
	_cubemap[(int)CubemapType::SKYBOX].GeometryShader = nullptr;

	return true;
}

#pragma endregion Public Function
#pragma region Private Function

#pragma endregion Private Functiopn

#pragma region cubemap
namespace cubemap
{
#pragma region Static Cubemap
	bool BuildStaticCubemapPipeline(CubemapPipeline& cubemap)
	{
		return true;
	}
#pragma endregion Static Cubemap

#pragma region Dynamic Cubemap

#pragma endregion Dynamic Cubemap

#pragma region Skybox
	/****************************************************************************
	*                       BuildSkyboxPipeline
	*************************************************************************//**
	*  @fn        bool BuildSkyboxPipeline(CubemapPipeline& cubemap)
	*  @brief     Blend Skybox pipeline
	*  @param[in] void
	*  @return 　　D3D12_BLEND_DESC
	*****************************************************************************/
	bool BuildSkyboxPipeline(CubemapPipeline& cubemap)
	{
		/*-------------------------------------------------------------------
		-			 Check Cubemap Type
		---------------------------------------------------------------------*/
		if (cubemap.Type == CubemapType::SKYBOX) { return false; }

		/*-------------------------------------------------------------------
		-			 Build RootSignature
		---------------------------------------------------------------------*/
		if (!BuildSkyboxRootSignature(cubemap)) { return false; }

		/*-------------------------------------------------------------------
		-			 Build PSO
		---------------------------------------------------------------------*/
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
		
		pipeLineState.RasterizerState             = BuildSkyboxRasterizerDescriptor();
		pipeLineState.InputLayout                 = VertexPositionNormalTexture::InputLayout;
		pipeLineState.pRootSignature              = cubemap.RootSignature.Get();
		pipeLineState.DepthStencilState           = DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipeLineState.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		if (cubemap.VertexShader == nullptr || cubemap.PixelShader == nullptr) { MessageBox(NULL, L"don't set shaders.", L"Warning", MB_ICONWARNING); return false; }
		pipeLineState.VS =
		{
			reinterpret_cast<BYTE*>(cubemap.VertexShader->GetBufferPointer()),
			cubemap.VertexShader->GetBufferSize()
		};
		pipeLineState.PS =
		{
			reinterpret_cast<BYTE*>(cubemap.PixelShader->GetBufferPointer()),
			cubemap.PixelShader->GetBufferSize()
		};
		pipeLineState.BlendState = BLEND_DESC(D3D12_DEFAULT);
		ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&cubemap.PipeLineState)));
		return true;
	}

	/****************************************************************************
	*                            BuildSkyboxRootSignature
	*************************************************************************//**
	*  @fn        bool BuildSkyboxRootSignature(CubemapPipeline& cubemap)
	*  @brief     Prepare Skybox RootSignature
	*  @param[in] void
	*  @return 　　bool
	*****************************************************************************/
	bool BuildSkyboxRootSignature(CubemapPipeline& cubemap)
	{
		/*-------------------------------------------------------------------
		-			 Check Cubemap Type
		---------------------------------------------------------------------*/
		if (cubemap.Type == CubemapType::SKYBOX) { return false; }

		/*-------------------------------------------------------------------
		-			Build texture table
		---------------------------------------------------------------------*/
		DESCRIPTOR_RANGE descriptorTable[1];
		descriptorTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, CUBEMAP, 0); // t6

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
		rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, static_cast<UINT>(samplerDesc.size()), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &cubemap.RootSignature);
		return true;
	}

	/****************************************************************************
	*                       BuildSkyboxBlendDescriptor
	*************************************************************************//**
	*  @fn        D3D12_BLEND_DESC BuildSkyboxCubemapBlendDescriptor()
	*  @brief     Blend Descriptor for Cubemap
	*  @param[in] void
	*  @return 　　D3D12_BLEND_DESC
	*****************************************************************************/
	D3D12_BLEND_DESC BuildSkyboxCubemapBlendDescriptor()
	{
		D3D12_BLEND_DESC cubemapBlendDesc                      = {};
		cubemapBlendDesc.AlphaToCoverageEnable                 = FALSE;
		cubemapBlendDesc.IndependentBlendEnable                = FALSE;
		cubemapBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
		cubemapBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		cubemapBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		cubemapBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		return cubemapBlendDesc;
	}

	RASTERIZER_DESC BuildSkyboxRasterizerDescriptor()
	{
		RASTERIZER_DESC rasterizerState = RASTERIZER_DESC(D3D12_DEFAULT);
		rasterizerState.CullMode        = D3D12_CULL_MODE_NONE;
		return rasterizerState;
	}
#pragma endregion Skybox
	bool BuildDynamicCubemapPipeline(CubemapPipeline& cubemap)
	{
		return true;
	}

	bool BuildStaticCubemapRootSignature(CubemapPipeline& cubemap)
	{
		return true;
	}
	bool BuildDynamicCubemapRootSignature(CubemapPipeline& cubemap)
	{
		return true;
	}



	D3D12_BLEND_DESC BuildStaticCubemapBlendDescriptor()
	{
		D3D12_BLEND_DESC cubemapBlendDesc = {};
		cubemapBlendDesc.AlphaToCoverageEnable = FALSE;
		cubemapBlendDesc.IndependentBlendEnable = FALSE;
		cubemapBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		cubemapBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		cubemapBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		cubemapBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		return cubemapBlendDesc;
	}

	D3D12_BLEND_DESC BuildDynamicCubemapBlendDescriptor()
	{
		D3D12_BLEND_DESC cubemapBlendDesc                      = {};
		cubemapBlendDesc.AlphaToCoverageEnable                 = FALSE;
		cubemapBlendDesc.IndependentBlendEnable                = FALSE;
		cubemapBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
		cubemapBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		cubemapBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
		cubemapBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		cubemapBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		return cubemapBlendDesc;
	}
}
#pragma endregion cubemap
