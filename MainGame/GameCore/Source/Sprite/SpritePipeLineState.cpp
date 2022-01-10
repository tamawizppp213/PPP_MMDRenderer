//////////////////////////////////////////////////////////////////////////////////
///             @file   SpritePipeLineState.cpp
///             @brief  Sprite PipeLine State
///             @author Toide Yutaro
///             @date   2021_01_21
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Sprite/SpritePipeLineState.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Sprite PSO Manager
SpritePSOManager::SpritePSOManager()
{
	_psoList.resize((int)SpriteType::SPRITE_TYPE_COUNT);
	
	for (int i = 0; i < (int)SpriteType::SPRITE_TYPE_COUNT - 1; ++i)
	{
		SpriteType type = static_cast<SpriteType>(i);
		_psoList[i].SetShaders(type);
		_psoList[i].BuildRootSignature();
		_psoList[i].BuildSpritePipeline();
	}

	_psoList[0].GetRootSignature()->SetName(LPCWSTR(L"SpriteManager::Texture"));
	_psoList[0].GetPipelineState()->SetName(LPCWSTR(L"SpriteManager::Texture"));
}

SpritePSOManager::~SpritePSOManager()
{
}

void SpritePSOManager::Clear()
{
	for (auto& pso : _psoList)
	{
		pso.Dispose();
	}
	_psoList.clear(); 
	_psoList.shrink_to_fit();
}
#pragma region Propety
/****************************************************************************
*                         GetPipelineState
*************************************************************************//**
*  @fn        const PipelineStateComPtr& SpritePSOManager::GetPipelineState(SpriteType type)
*  @brief     Get a pipelineState for each type.
*  @param[in] SpriteType type
*  @return 　　const PipelineStaetComPtr&
*****************************************************************************/
const PipelineStateComPtr& SpritePSOManager::GetPipelineState(SpriteType type)
{
	return _psoList[(int)type].GetPipelineState();
}

/****************************************************************************
*                         GetRootSignature
*************************************************************************//**
*  @fn        const RootSignatureComPtr& SpritePSOManager::GetRootSignature(SpriteType type)
*  @brief     Get a rootsignature for each type.
*  @param[in] SpriteType type
*  @return 　　const RootSignatureComPtr&
*****************************************************************************/
const RootSignatureComPtr& SpritePSOManager::GetRootSignature(SpriteType type)
{
	return _psoList[(int)type].GetRootSignature();
}

#pragma endregion Property
#pragma endregion Sprite PSO Manager

#pragma region SpritePipelineStateDescriptor
#pragma region Public Function
void SpritePipelineStateDescriptor::Dispose()
{
	_pipeLineState = nullptr;
	_rootSignature = nullptr;
	_vertexShader = nullptr;
	_pixelShader = nullptr;
}
/****************************************************************************
*                            BuildSpritePipeline
*************************************************************************//**
*  @fn        bool SpritePipelineStateDescriptor::BuildSpritePipeline()
*  @brief     Prepare Pipeline (prepara only once)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SpritePipelineStateDescriptor::BuildSpritePipeline()
{
	/*-------------------------------------------------------------------
	-			 Build RootSignature
	---------------------------------------------------------------------*/
	if (!BuildRootSignature())
	{ 
		MessageBox(NULL, L"Couldn't create root signature.", L"Warning", MB_ICONWARNING);
		return false;
	}

	/*-------------------------------------------------------------------
	-			 Build PSO 
	---------------------------------------------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
	pipeLineState.InputLayout = VertexPositionNormalColorTexture::InputLayout;
	pipeLineState.pRootSignature = _rootSignature.Get();
	if (_vertexShader == nullptr || _pixelShader == nullptr) { MessageBox(NULL, L"don't set shaders.", L"Warning", MB_ICONWARNING); return false; }
	pipeLineState.VS =
	{
		reinterpret_cast<BYTE*>(_vertexShader->GetBufferPointer()),
		_vertexShader->GetBufferSize()
	};
	pipeLineState.PS =
	{
		reinterpret_cast<BYTE*>(_pixelShader->GetBufferPointer()),
		_pixelShader->GetBufferSize()
	};
	pipeLineState.BlendState = BuildBlendDescriptor();
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_pipeLineState)));
	_pipeLineState->SetName(L"Sprite::PipelineState");
	return true;
}

/****************************************************************************
*                            BuildRootSignature
*************************************************************************//**
*  @fn        bool SpritePipelineStateDescriptor::BuildRootSignature()
*  @brief     Prepare RootSignature
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SpritePipelineStateDescriptor::BuildRootSignature()
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
  	DESCRIPTOR_RANGE textureTable[2];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0); // b0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[2];
	rootParameter[0].InitAsConstantBufferView(0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[1].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_PIXEL);

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
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_rootSignature);
	_rootSignature->SetName(L"Sprite::RootSignature");
	return true;
}

/****************************************************************************
*                            SetShaders
*************************************************************************//**
*  @fn        bool SpritePipelineStateDescriptor::SetShaders(SpriteType spriteType)
*  @brief     Prepare VS and PS for each type.
*  @param[in] SpriteType spriteType
*  @return 　　bool
*****************************************************************************/
bool SpritePipelineStateDescriptor::SetShaders(SpriteType spriteType)
{
	switch (spriteType)
	{
	case SpriteType::TEXTURE:
		_vertexShader = CompileShader(L"Shader\\ShaderTestVertex.hlsl", nullptr, "VSMain", "vs_5_1");
		_pixelShader  = CompileShader(L"Shader\\ShaderTestPixel.hlsl" , nullptr, "PSMain", "ps_5_1");
		break;
	case SpriteType::COLOR:
		_vertexShader = CompileShader(L"Shader\\ShaderTestVertex.hlsl", nullptr, "VSMain", "vs_5_1");
		_pixelShader  = CompileShader(L"Shader\\ShaderTestPixel.hlsl ", nullptr, "PSColor", "ps_5_1");
		break;
	}
	return true;
}

#pragma endregion Public Function
#pragma region Private Function
/****************************************************************************
*                       BuildBlendDescriptor
*************************************************************************//**
*  @fn        D3D12_BLEND_DESC SpritePipelineStateDescriptor::BuildBlendDescriptor()
*  @brief     Blend Descriptor for Sprite Pipelne 
*  @param[in] void
*  @return 　　D3D12_BLEND_DESC
*****************************************************************************/
D3D12_BLEND_DESC SpritePipelineStateDescriptor::BuildBlendDescriptor()
{
	D3D12_BLEND_DESC spriteBlendDesc                      = {};
	spriteBlendDesc.AlphaToCoverageEnable                 = FALSE;
	spriteBlendDesc.IndependentBlendEnable                = FALSE;
	spriteBlendDesc.RenderTarget[0].BlendEnable           = TRUE;
	spriteBlendDesc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
	spriteBlendDesc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
	spriteBlendDesc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
	spriteBlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_SRC_ALPHA;
	spriteBlendDesc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
	spriteBlendDesc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
	spriteBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return spriteBlendDesc;
}
#pragma endregion Private Functiopn
#pragma endregion SpritePipelineStateDescriptor
