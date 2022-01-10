//////////////////////////////////////////////////////////////////////////////////
///             @file   PostEfect.cpp
///             @brief  PostEffect 
///             @author Toide Yutaro
///             @date   2021_07_08
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Effect/DepthOfField.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace dof;
#define DOF_COLORBUFFER_NAME L"DOF::ColorBuffer"

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
bool Dof::Initialize(int width, int height)
{
	if (_hasInitialized) { return true; }

	_colorBuffer.resize(6);
	/*-------------------------------------------------------------------
	-                  Prepare RootSignature
	---------------------------------------------------------------------*/
	if (!PrepareRootSignature())  { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare PipelineState
	---------------------------------------------------------------------*/
	if(!PreparePipelineState())   { return false;}
	/*-------------------------------------------------------------------
	-                  Prepare VertexBuffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer())   { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare IndexBuffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer())    { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare ConstantBuffer
	---------------------------------------------------------------------*/
	if (!PrepareConstantBuffer()) { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare Sprite
	---------------------------------------------------------------------*/
	if (!PrepareSprite())         { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare Resource
	---------------------------------------------------------------------*/
	if (!PrepareResources(width, height)) { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare Descriptor
	---------------------------------------------------------------------*/
	if (!PrepareDescriptors()) { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare Blur Paramater
	---------------------------------------------------------------------*/
	if (!PrepareBlurConstantBuffer(width, height, 4.0f)) { return false; }
	/*-------------------------------------------------------------------
	-                  Prepare DofConfig
	---------------------------------------------------------------------*/
	if (!PrepareDofConfigBuffer(0.5f, 1.0f)) { return false; }
	/*-------------------------------------------------------------------
	-                 Set Resource Name
	---------------------------------------------------------------------*/

	_hasInitialized = true;
	return true;

}

void Dof::Finalize()
{
	PostEffect::Finalize();
	_verticalBlurPipeline = nullptr;
	_rhomboidBlurPipeline = nullptr;
	_blurBuffer.get()->Resource()->Release();
	_blurBuffer.reset();
	_dofBuffer.get()->Resource()->Release();
	_dofBuffer.reset();
}

bool Dof::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth
		|| _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		for (int i = 0; i < _colorBuffer.size(); ++i)
		{
			if (!_colorBuffer[i].OnResize(newWidth, newHeight)) { return false; };
		}
	}
	return true;
}

bool Dof::Draw(Resource* renderTarget, Resource* zPrepass, D3D12_RESOURCE_STATES renderTargetState)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].get()->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView = _meshBuffer[currentFrameIndex].get()->IndexBufferView();

	_colorBuffer[0].CopyToColorBuffer(renderTarget, renderTargetState);
	_colorBuffer[1].CopyToColorBuffer(zPrepass, D3D12_RESOURCE_STATE_COMMON);
	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetComputeRootSignature(_rootSignature.Get());


	/*-------------------------------------------------------------------
	-               Execute vertical blur
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_verticalBlurPipeline.Get());
	commandList->SetComputeRootConstantBufferView(0, _blurBuffer.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetComputeRootConstantBufferView(1, _dofBuffer.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[0].GetGPUSRV()); // colorTexture
	commandList->SetComputeRootDescriptorTable(4, _colorBuffer[2].GetGPUUAV()); // vertical blur texture
	commandList->SetComputeRootDescriptorTable(5, _colorBuffer[3].GetGPUUAV()); // diagonal blur texture
	commandList->Dispatch(
		static_cast<UINT>(_colorBuffer[0].GetColorBuffer().ImageSize.x / 8),
		static_cast<UINT>(_colorBuffer[0].GetColorBuffer().ImageSize.y / 8), 1);

	/*-------------------------------------------------------------------
	-               Execute rhomboid blur
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_rhomboidBlurPipeline.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[2].GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[3].GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(4, _colorBuffer[4].GetGPUUAV());
	commandList->Dispatch(
		static_cast<UINT>(_colorBuffer[0].GetColorBuffer().ImageSize.x / 8),
		static_cast<UINT>(_colorBuffer[0].GetColorBuffer().ImageSize.y / 8), 1);

	/*-------------------------------------------------------------------
	-               Execute rhomboid blur
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_pipelineState.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[2].GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[1].GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(6, _colorBuffer[0].GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(4, _colorBuffer[5].GetGPUUAV());
	commandList->Dispatch(
		static_cast<UINT>(_colorBuffer[0].GetColorBuffer().ImageSize.x / 8),
		static_cast<UINT>(_colorBuffer[0].GetColorBuffer().ImageSize.y / 8), 1);

	FinalCopyToResource(renderTarget, renderTargetState);
	return true;
}

bool Dof::Draw()
{
	return true;
}

/****************************************************************************
*                       UpdateBlurEffectParameter
*************************************************************************//**
*  @fn        bool Dof::UpdateBlurEffectParameter(int width, int height, float radius)
*  @brief     This function updates blur effect parameter (screenWidth, screenHeight, blurRadius = 8.0f)
*  @param[in] int screen width
*  @param[in] int screen height
*  @return 　　void
*****************************************************************************/
void Dof::UpdateBlurEffectParameter(int width, int height, float radius)
{
	BlurParameter blurParameter;
	blurParameter.TextureSize[0] = (float)width;
	blurParameter.TextureSize[1] = (float)height;
	blurParameter.Radius         = radius;

	_blurBuffer.get()->CopyStart();
	_blurBuffer.get()->CopyTotalData(&blurParameter, 1);
	_blurBuffer.get()->CopyEnd();

}

/****************************************************************************
*                       UpdateDofClipSize
*************************************************************************//**
*  @fn        bool Dof::UpdateDofClipSize(float nearClip, float farClip)
*  @brief     This function updates dof clip size 
*             default value (nearClip: 200.0f, farClip: 500.0f) maxDepth: 1000.0f
*  @param[in] float nearClip = 200.0f
*  @param[in] float farClip = 500.0f
*  @return 　　void
*****************************************************************************/
void Dof::UpdateDofClipSize(float nearClip, float farClip)
{
	DofClipSize dofClipSize;
	dofClipSize.NearClip = nearClip;
	dofClipSize.FarClip  = farClip;

	_dofBuffer.get()->CopyStart();
	_dofBuffer.get()->CopyTotalData(&dofClipSize, 1);
	_dofBuffer.get()->CopyEnd();

}
#pragma endregion Public Function
#pragma region Private Function
/****************************************************************************
*                       PrepareResources
*************************************************************************//**
*  @fn        bool Blur::PrepareResources(int width, int height)
*  @brief     This function prepares colorBuffer resource
*  @param[in] int screen width
*  @param[in] int screen height 
*  @return 　　bool
*****************************************************************************/
bool Dof::PrepareResources(int width, int height)
{
	for (int i = 0; i < _colorBuffer.size(); ++i)
	{
		if(!_colorBuffer[i].Create(width, height, DirectX12::Instance().GetBackBufferRenderFormat(), nullptr, L"Dof")) { return false; }
	}
	return true;
}

bool Dof::PrepareDescriptors()
{
	return true;
}

/****************************************************************************
*                       PrepareRootSignature
*************************************************************************//**
*  @fn        bool Dof::PrepareRootSignature()
*  @brief     This function prepares rootSignature
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Dof::PrepareRootSignature()
{
	DESCRIPTOR_RANGE textureTable[7];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0); // b0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0); // b1
	textureTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0
	textureTable[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0); // s1
	textureTable[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0); // u0
	textureTable[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, 0); // u1
	textureTable[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0); // s2
	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[7];
	rootParameter[0].InitAsConstantBufferView(0);
	rootParameter[1].InitAsConstantBufferView(1);
	rootParameter[2].InitAsDescriptorTable(1, &textureTable[2], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[3].InitAsDescriptorTable(1, &textureTable[3], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[4].InitAsDescriptorTable(1, &textureTable[4], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[5].InitAsDescriptorTable(1, &textureTable[5], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[6].InitAsDescriptorTable(1, &textureTable[6], D3D12_SHADER_VISIBILITY_ALL);


	/*-------------------------------------------------------------------
	-			Build sampler desc
	---------------------------------------------------------------------*/
	auto samplerDesc = GetStaticSamplers();

	/*-------------------------------------------------------------------
	-			 Build root parameter
	---------------------------------------------------------------------*/
	// A root signature is a collection of descriptor tables 
	// (which feeds data other than vertices to the shader).
	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)samplerDesc.size(), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_rootSignature);
	_rootSignature->SetName(L"Dof::RootSignature");
	return true;
}

/****************************************************************************
*                       PreparePipelineState
*************************************************************************//**
*  @fn        bool Blur::PreparePipelineState()
*  @brief     This function prepares pipelineState
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Dof::PreparePipelineState()
{
	std::wstring defaultPath = L"Shader\\Effect\\ShaderDof.hlsl";
	Device* device = DirectX12::Instance().GetDevice();

	BlobComPtr verticalCS, rhomboidBlurCS, finalBlurCS;
	verticalCS     = CompileShader(defaultPath, nullptr, "VerticalBlur", "cs_5_1");
	rhomboidBlurCS = CompileShader(defaultPath, nullptr, "RhomboidBlur", "cs_5_1");
	finalBlurCS    = CompileShader(defaultPath, nullptr, "FinalBlur", "cs_5_1");

	/*-------------------------------------------------------------------
	-			Luminance pipelineState
	---------------------------------------------------------------------*/
	D3D12_COMPUTE_PIPELINE_STATE_DESC verticalPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	verticalPipeline.pRootSignature = _rootSignature.Get();
	verticalPipeline.CS =
	{
		reinterpret_cast<BYTE*>(verticalCS->GetBufferPointer()),
		verticalCS->GetBufferSize()
	};

	ThrowIfFailed(device->CreateComputePipelineState(&verticalPipeline, IID_PPV_ARGS(&_verticalBlurPipeline)));
	_verticalBlurPipeline->SetName(L"Dof::VerticalBlurPipeLine");

	/*-------------------------------------------------------------------
	-			Luminance pipelineState
	---------------------------------------------------------------------*/
	D3D12_COMPUTE_PIPELINE_STATE_DESC rhomboidPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	rhomboidPipeline.pRootSignature = _rootSignature.Get();
	rhomboidPipeline.CS =
	{
		reinterpret_cast<BYTE*>(rhomboidBlurCS->GetBufferPointer()),
		rhomboidBlurCS->GetBufferSize()
	};

	ThrowIfFailed(device->CreateComputePipelineState(&rhomboidPipeline, IID_PPV_ARGS(&_rhomboidBlurPipeline)));
	_rhomboidBlurPipeline->SetName(L"Dof::RhomboidBlurPipelineState");

	/*-------------------------------------------------------------------
	-			FinalDof pipelineState
	---------------------------------------------------------------------*/
	auto finalDofPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	finalDofPipeline.pRootSignature = _rootSignature.Get();
	finalDofPipeline.CS =
	{
		reinterpret_cast<BYTE*>(finalBlurCS->GetBufferPointer()),
		finalBlurCS->GetBufferSize()
	};
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateComputePipelineState(&finalDofPipeline, IID_PPV_ARGS(&_pipelineState)));
	_pipelineState->SetName(L"Dof::FinalDofPipelineState");
	return true;
}

/****************************************************************************
*                       PrepareBlurConstantBuffer
*************************************************************************//**
*  @fn        bool Dof::PrepareBlurConstantBuffer(int width, int height, float radius)
*  @brief     This function prepares blur constant buffer(int screenWidth, int screenHeight, float blurRadis)
*  @param[in] int width
*  @param[in] int height
*  @param[in] float radius
*  @return 　　bool
*****************************************************************************/
bool Dof::PrepareBlurConstantBuffer(int width, int height, float radius)
{
	Device* device = DirectX12::Instance().GetDevice();

	BlurParameter blurParameter;
	blurParameter.TextureSize[0] = (float)width;
	blurParameter.TextureSize[1] = (float)height;
	blurParameter.Radius         = radius;

	auto blurBuffer = std::make_unique < UploadBuffer<BlurParameter>>(device, 1, true, L"Dof::BlurParameter");
	blurBuffer.get()->CopyStart();
	blurBuffer.get()->CopyTotalData(&blurParameter, 1);
	blurBuffer.get()->CopyEnd();

	_blurBuffer = std::move(blurBuffer);
	return true;
}

/****************************************************************************
*                       PrepareDofConfigBuffer
*************************************************************************//**
*  @fn        bool Dof::PrepareDofConfigBuffer(float nearClip, float farClip)
*  @brief     This function prepares dof config (near clip and far clip).
*             default value (near clip = 200.0f, farClip = 500.0f), maxDepth = 1000.0f 
*  @param[in] float nearClip
*  @param[in] float farClip
*  @return 　　bool
*****************************************************************************/
bool Dof::PrepareDofConfigBuffer(float nearClip, float farClip)
{
	Device* device = DirectX12::Instance().GetDevice();

	DofClipSize dofClipSize;
	dofClipSize.NearClip = nearClip;
	dofClipSize.FarClip  = farClip;

	auto dofBuffer = std::make_unique<UploadBuffer<DofClipSize>>(device, 1, true, L"Dof::DofConfig");
	dofBuffer.get()->CopyStart();
	dofBuffer.get()->CopyTotalData(&dofClipSize, 1);
	dofBuffer.get()->CopyEnd();

	_dofBuffer = std::move(dofBuffer);
	return true;
}
#pragma endregion Private Function