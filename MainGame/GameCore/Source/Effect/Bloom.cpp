//////////////////////////////////////////////////////////////////////////////////
///             @file   PostEfect.cpp
///             @brief  PostEffect 
///             @author Toide Yutaro
///             @date   2021_07_08
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Effect/Bloom.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define BLOOM_COLOR_BUFFER_NAME L"Bloom::ColorBuffer"
/****************************************************************************
*                          Initialize
*************************************************************************//**
*  @fn        bool Bloom::Initialize(int width, int height)
*  @brief     Initialize Bloom Class
*  @param[in] int screenPixelWidth
*  @param[in] int screenPixelHeight
*  @return 　　bool
*****************************************************************************/
bool Bloom::Initialize(int width, int height)
{
	/*0: renderTarget, 1〜4: luminanceBufferSRV, 5*UAV final buffer*/
	_colorBuffer.resize(3); 
	/*-------------------------------------------------------------------
	-               Prepare RootSignature
	---------------------------------------------------------------------*/
	if (!PrepareRootSignature())  { return false; }
	/*-------------------------------------------------------------------
	-               Prepare PipelineState
	---------------------------------------------------------------------*/
	if(!PreparePipelineState())   { return false;}
	/*-------------------------------------------------------------------
	-               Prepare VertexBuffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer())   { return false; }
	/*-------------------------------------------------------------------
	-               Prepare IndexBuffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer())    { return false; }
	/*-------------------------------------------------------------------
	-               Prepare ConstantBuffer
	---------------------------------------------------------------------*/
	if (!PrepareConstantBuffer()) { return false; }
	/*-------------------------------------------------------------------
	-               Prepare SpriteInfo
	---------------------------------------------------------------------*/
	if (!PrepareSprite())         { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Resources
	---------------------------------------------------------------------*/
	if (!PrepareResources(width, height)) { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Descriptors
	---------------------------------------------------------------------*/
	if (!PrepareDescriptors())    { return false; }
	/*-------------------------------------------------------------------
	-               Set Bloom Power
	---------------------------------------------------------------------*/
	UpdateBloomPower(_explosion);
	return true;
}
/****************************************************************************
*                       Finalize
*************************************************************************//**
*  @fn        void Bloom::Finalize
*  @brief     Finalize 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Bloom::Finalize()
{
	PostEffect::Finalize();

	_luminancePipeline = nullptr;
	for (auto& blur : _blur)
	{
		blur.Finalize();
	}
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        bool Bloom::Draw(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState)
*  @brief     Draw Bloom post effect
*  @param[in,out] Resource* renderTarget
*  @param[in] D3D12_RESOURCE_STATES renderTargetState
*  @return 　　bool
*****************************************************************************/
bool Bloom::Draw(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].get()->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[currentFrameIndex].get()->IndexBufferView();

	_colorBuffer[0].CopyToColorBuffer(renderTarget, renderTargetState);

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetComputeRootSignature(_rootSignature.Get());
	
	/*-------------------------------------------------------------------
	-               Execute Luminance command
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_luminancePipeline.Get());
	commandList->SetComputeRootDescriptorTable(0, _colorBuffer[0].GetGPUSRV());
	commandList->SetComputeRootDescriptorTable(5, _colorBuffer[1].GetGPUUAV());
	Screen screen;
	commandList->Dispatch(screen.GetScreenWidth() / 8, screen.GetScreenHeight() / 8, 1);

	/*-------------------------------------------------------------------
	-               Execute Blur 
	---------------------------------------------------------------------*/
	_blur[0].Draw(_colorBuffer[1].GetColorBuffer().Resource.Get(), _colorBuffer[1].GetUsageState());
	for (int i = 1; i < _countof(_blur); ++i)
	{
		_blur[i].Draw(
			_blur[i - 1].GetHalfDownSamplingColorBuffer().GetColorBuffer().Resource.Get(),
			_blur[i - 1].GetHalfDownSamplingColorBuffer().GetUsageState());
	}

	/*-------------------------------------------------------------------
	-               Execute Addition synthesis
	---------------------------------------------------------------------*/
	commandList->SetComputeRootSignature(_rootSignature.Get());
	commandList->SetPipelineState       (_pipelineState.Get());
	commandList->SetComputeRootDescriptorTable(0, _colorBuffer[0].GetGPUSRV());
	for (int i = 0; i < _countof(_blur); ++i)
	{
		commandList->SetComputeRootDescriptorTable(i + 1 , _blur[i].GetFinalColorBuffer().GetGPUSRV());
	}
	commandList->SetComputeRootDescriptorTable(5, _colorBuffer[_colorBuffer.size() - 1].GetGPUUAV());
	commandList->Dispatch(screen.GetScreenWidth() / 8, screen.GetScreenHeight() / 8, 1);

	FinalCopyToResource(renderTarget, renderTargetState);
	return true;
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        bool Bloom::Draw()
*  @brief     Draw Bloom post effect (Draw CurrentRenderTarget)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Bloom::Draw()
{
	return Draw(DirectX12::Instance().GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}
/****************************************************************************
*                          OnResize
*************************************************************************//**
*  @fn        bool Bloom::OnResize(int newWidth, int newHeight)
*  @brief     Resize screen
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Bloom::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth || _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		_blur[0].OnResize(newWidth, newHeight);
		for (int i = 0; i < _countof(_blur); ++i)
		{
			_blur[i].OnResize((int)(newWidth / pow(2, i)), (int)(newHeight / pow(2, i)));
		}
		for (int i = 0; i < _colorBuffer.size(); ++i)
		{
			_colorBuffer[i].OnResize(newWidth, newHeight);
		}
	}
	return true;
}
/****************************************************************************
*                          UpdateBloomPower
*************************************************************************//**
*  @fn        void Bloom::UpdateBloomPower(float power)
*  @brief     Update bloom power (Default value is 10)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
void Bloom::UpdateBloomPower(float power)
{
	for (int i = 0; i < _countof(_blur); ++i)
	{
		_blur[i].UpdateWeightsTable(power);
	}
}
#pragma region Private Function
bool Bloom::PrepareResources(int width, int height)
{
	for (int i = 0; i < _colorBuffer.size(); ++i)
	{
		if (!_colorBuffer[i].Create(width, height, DirectX12::Instance().GetBackBufferRenderFormat(), nullptr, BLOOM_COLOR_BUFFER_NAME)) { return false; }
	}
	if (!_blur[0].Initialize(width, height, DirectX12::Instance().GetBackBufferRenderFormat())) { return false; }
	for (int i = 1; i < _countof(_blur); ++i)
	{
		if(!_blur[i].Initialize((int)(width / pow(2, i)), (int)(height / pow(2, i)), DirectX12::Instance().GetBackBufferRenderFormat())){return false;}
	}
	return true;
}

bool Bloom::PrepareDescriptors()
{
	return true;
}

bool Bloom::PrepareRootSignature()
{
	DESCRIPTOR_RANGE textureTable[6];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // t0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0); // t1
	textureTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0); // t2
	textureTable[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0); // t3
	textureTable[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4, 0); // t4
	textureTable[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0); // u0

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[6];
	rootParameter[0].InitAsDescriptorTable(1, &textureTable[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[1].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[2].InitAsDescriptorTable(1, &textureTable[2], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[3].InitAsDescriptorTable(1, &textureTable[3], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[4].InitAsDescriptorTable(1, &textureTable[4], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[5].InitAsDescriptorTable(1, &textureTable[5], D3D12_SHADER_VISIBILITY_ALL);

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
	_rootSignature->SetName(L"Bloom::RootSignature");
	return true;
}

bool Bloom::PreparePipelineState()
{
	std::wstring defaultPath = L"Shader\\Effect\\ShaderBloom.hlsl";
	Device* device = DirectX12::Instance().GetDevice();
	
	BlobComPtr luminanceCS, finalBloomCS;
	luminanceCS  = CompileShader(defaultPath, nullptr, "SamplingLuminance", "cs_5_1");
	finalBloomCS = CompileShader(defaultPath, nullptr, "FinalBloom", "cs_5_1");

	/*-------------------------------------------------------------------
	-			Luminance pipelineState
	---------------------------------------------------------------------*/
	D3D12_COMPUTE_PIPELINE_STATE_DESC luminancePipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	luminancePipeline.pRootSignature = _rootSignature.Get();
	luminancePipeline.CS =
	{
		reinterpret_cast<BYTE*>(luminanceCS->GetBufferPointer()),
		luminanceCS->GetBufferSize()
	};
	
	ThrowIfFailed(device->CreateComputePipelineState(&luminancePipeline, IID_PPV_ARGS(&_luminancePipeline)));
	_luminancePipeline->SetName(L"Bloom::LuminancePipeLineState");

	/*-------------------------------------------------------------------
	-			FinalBloom pipelineState
	---------------------------------------------------------------------*/
	auto finalBloomPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	finalBloomPipeline.pRootSignature = _rootSignature.Get();
	finalBloomPipeline.CS =
	{
		reinterpret_cast<BYTE*>(finalBloomCS->GetBufferPointer()),
		finalBloomCS->GetBufferSize()
	};
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateComputePipelineState(&finalBloomPipeline, IID_PPV_ARGS(&_pipelineState)));
	_pipelineState->SetName(L"Bloom::FinalBloomPipelineState");
	return true;
}

#pragma endregion Private Function