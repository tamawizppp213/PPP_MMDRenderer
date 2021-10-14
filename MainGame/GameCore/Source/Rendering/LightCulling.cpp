//////////////////////////////////////////////////////////////////////////////////
///             @file   LightCulling.cpp
///             @brief  LightCulling 
///             @author Toide Yutaro
///             @date   2021_08_06
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/LightCulling.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define NUM_POINT_LIGHT (100)
#define NUM_SPOT_LIGHT  (100)

#define TILE_WIDTH  (16)
#define TILE_HEIGHT (16)

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
bool LightCulling::Initialize(ColorBuffer& zPrepass)
{
	_zPrepass = &zPrepass;

	if (!PrepareRootSignature()) { return false; }
	if (!PreparePipelineState()) { return false; }
	if (!PrepareResources())     { return false; }
	return true;
}

bool LightCulling::ExecuteCulling()
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	if (_scene == NULL || _light == NULL) { return false; }
	DirectX12& directX12     = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();

	OnResize(); // in case zBuffer image size changed, this function is executed.

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetComputeRootSignature(_rootSignature.Get());
	commandList->SetPipelineState       (_pipeline.Get());
	commandList->SetComputeRootConstantBufferView(1, _scene);
	commandList->SetComputeRootConstantBufferView(2, _light);
	commandList->SetComputeRootDescriptorTable(3, _zPrepass->GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(4, _pointLightList.GetGPUUAV());
	commandList->SetComputeRootDescriptorTable(5, _spotLightList.GetGPUUAV());
	commandList->Dispatch((UINT)(_zPrepass->GetColorBuffer().ImageSize.x / 16), (UINT)(_zPrepass->GetColorBuffer().ImageSize.y / 16), 1);

	return true;
}


#pragma endregion Public Function

#pragma region Private Function
bool LightCulling::OnResize()
{
	gm::Float2 textureSize = _zPrepass->GetTextureSize();
	int tileCount          = (textureSize.x / TILE_WIDTH) * (textureSize.y / TILE_HEIGHT);
	_pointLightList.OnResize(NUM_POINT_LIGHT * tileCount);
	_spotLightList.OnResize(NUM_SPOT_LIGHT * tileCount);
	return true;
}
bool LightCulling::PrepareResources()
{
	gm::Float2 textureSize = _zPrepass->GetTextureSize();
	int tileCount          = (textureSize.x / TILE_WIDTH) * (textureSize.y / TILE_HEIGHT);
	_pointLightList.Create(sizeof(int), NUM_POINT_LIGHT * tileCount); // pointlightの数 * タイルの数
	_spotLightList.Create (sizeof(int), NUM_SPOT_LIGHT * tileCount);

	return true;
}

/****************************************************************************
*                       PrepareRootSignature
*************************************************************************//**
*  @fn        bool LightCulling::PrepareRootSignature()
*  @brief     This function prepares root signature
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool LightCulling::PrepareRootSignature()
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
	DESCRIPTOR_RANGE textureTable[3];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0); // u0
	textureTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, 0); // u1

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[6];
	rootParameter[0].InitAsConstantBufferView(0); // objectConstants
	rootParameter[1].InitAsConstantBufferView(1); // scene 
	rootParameter[2].InitAsConstantBufferView(3); // light
	rootParameter[3].InitAsDescriptorTable(1, &textureTable[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[4].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[5].InitAsDescriptorTable(1, &textureTable[2], D3D12_SHADER_VISIBILITY_ALL);

	auto samplerDesc = GetStaticSamplers(); // linearsampler

	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)samplerDesc.size(), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_rootSignature);
	return true;
}

/****************************************************************************
*                       PreparePipelineState
*************************************************************************//**
*  @fn        bool LightCulling::PreparePipelineState()
*  @brief     This function prepares compute pipeline state
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool LightCulling::PreparePipelineState()
{
	std::wstring defaultPath = L"Shader\\Lighting\\ShaderLightCulling.hlsl";
	Device* device           = DirectX12::Instance().GetDevice();

	/*-------------------------------------------------------------------
	-           Compile light culling shader
	---------------------------------------------------------------------*/
	BlobComPtr lightCullingCS = nullptr;
	lightCullingCS = CompileShader(defaultPath, nullptr, "LightCulling", "cs_5_1");

	/*-------------------------------------------------------------------
	-			Light culling pipelineState
	---------------------------------------------------------------------*/
	D3D12_COMPUTE_PIPELINE_STATE_DESC lightCullingPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	lightCullingPipeline.pRootSignature = _rootSignature.Get();
	lightCullingPipeline.CS =
	{
		reinterpret_cast<BYTE*>(lightCullingCS->GetBufferPointer()),
		lightCullingCS->GetBufferSize()
	};

	ThrowIfFailed(device->CreateComputePipelineState(&lightCullingPipeline, IID_PPV_ARGS(&_pipeline)));
	return true;
}


#pragma endregion Private Function