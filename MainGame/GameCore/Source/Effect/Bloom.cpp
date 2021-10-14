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
bool Bloom::Initialize(int width, int height)
{
	/*0: renderTarget, 1`4: luminanceBufferSRV, 5*UAV final buffer*/
	_colorBuffer.resize(3); 
	if (!PrepareRootSignature())  { return false; }
	if(!PreparePipelineState())   { return false;}
	if (!PrepareVertexBuffer())   { return false; }
	if (!PrepareIndexBuffer())    { return false; }
	if (!PrepareConstantBuffer()) { return false; }
	if (!PrepareSprite())         { return false; }
	if (!PrepareResources(width, height)) { return false; }
	if (!PrepareDescriptors())    { return false; }
	UpdateBloomPower(_explosion);
	return true;
}

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
	commandList->SetPipelineState(_pipelineState.Get());
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

bool Bloom::Draw()
{
	return Draw(DirectX12::Instance().GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}
bool Bloom::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth
		|| _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		_blur[0].OnResize(newWidth, newHeight);
		for (int i = 0; i < _countof(_blur); ++i)
		{
			_blur[i].OnResize(newWidth / (int)pow(2, i), newHeight / pow(2, i));
		}
		for (int i = 0; i < _colorBuffer.size(); ++i)
		{
			_colorBuffer[i].OnResize(newWidth, newHeight);
		}
	}
	return true;
}

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
		if (!_colorBuffer[i].Create(width, height, DirectX12::Instance().GetBackBufferRenderFormat())) { return false; }
	}
	if (!_blur[0].Initialize(width, height, DirectX12::Instance().GetBackBufferRenderFormat())) { return false; }
	for (int i = 1; i < _countof(_blur); ++i)
	{
		if(!_blur[i].Initialize(width / pow(2, i), height / pow(2, i), DirectX12::Instance().GetBackBufferRenderFormat())){return false;}
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
	return true;
}

#pragma endregion Private Function