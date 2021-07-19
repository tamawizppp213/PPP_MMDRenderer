//////////////////////////////////////////////////////////////////////////////////
///             @file   PostEfect.cpp
///             @brief  PostEffect 
///             @author Toide Yutaro
///             @date   2021_07_08
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Effect/Blur.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define WEIGHT_TABLE_SIZE (8)
using namespace blur;
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
bool Blur::Initialize()
{
	_colorBuffer.resize(4);
	if (!PrepareRootSignature())     { return false;}
	if (!PreparePipelineState())     { return false;}
	if (!PrepareVertexBuffer())      { return false; }
	if (!PrepareIndexBuffer())       { return false; }
	if (!PrepareConstantBuffer())    { return false; }
	if (!PrepareBlurParameters())    { return false; }
	if (!PrepareTextureSizeBuffer()) { return false; }
	if (!PrepareSprite())            { return false; }
	if (!PrepareResources())         { return false; }
	if (!PrepareDescriptors())       { return false; }
	return true;
}

bool Blur::Draw()
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].get()->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[currentFrameIndex].get()->IndexBufferView();
	
	ExecuteFirstBarrier(directX12, commandList);
	commandList->CopyResource(_colorBuffer[0].Resource.Get(), directX12.GetCurrentRenderTarget());
	ExecuteSecondBarrier(directX12, commandList);

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetComputeRootSignature(_rootSignature.Get());
	commandList->SetComputeRootConstantBufferView(0, _weightBuffer.get()->Resource()->GetGPUVirtualAddress()); 
	commandList->SetComputeRootConstantBufferView(1, _textureSizeBuffer.get()->Resource()->GetGPUVirtualAddress());

	/*-------------------------------------------------------------------
	-               Execute XBlur Command
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_xBlurPipeline.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[0].GPUHandler);// srv,
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[1].GPUHandler);//uav
	commandList->Dispatch(
		_textureSizeParameter.XBlurTexture[0] / 4, 
		_textureSizeParameter.XBlurTexture[1] / 4, 1);
	ExecuteThirdBarrier(directX12, commandList);

	/*-------------------------------------------------------------------
	-               Execute YBlur Command
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_yBlurPipeline.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[1].GPUHandler);// srv,
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[2].GPUHandler);//uav
	commandList->Dispatch(
		_textureSizeParameter.YBlurTexture[0] / 4,
		_textureSizeParameter.YBlurTexture[1] / 4, 1);
	ExecuteFourthBarrier(directX12, commandList);

	/*-------------------------------------------------------------------
	-               Execute FinalBlur Command
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_pipelineState.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[2].GPUHandler);// srv,
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[3].GPUHandler);//uav
	commandList->Dispatch(
		_textureSizeParameter.OriginalTexture[0] / 4, 
		_textureSizeParameter.OriginalTexture[1] / 4, 1);

	ExecuteFifthBarrier(directX12, commandList);

	/*-------------------------------------------------------------------
	-               Copy to back buffer
	---------------------------------------------------------------------*/
	commandList->CopyResource(directX12.GetCurrentRenderTarget(), _colorBuffer[3].Resource.Get());
	ExecuteFinalBarrier(directX12, commandList);
	return true;
}

bool Blur::OnResize()
{
	Screen screen;
	if (_colorBuffer[0].ImageSize.x != screen.GetScreenWidth()
		|| _colorBuffer[0].ImageSize.y != screen.GetScreenHeight())
	{
		PrepareDescriptors();
		PrepareResources();
		PrepareTextureSizeBuffer();
	}
	return true;
}

/****************************************************************************
*                       UpdateWeightsTable
*************************************************************************//**
*  @fn        void Blur::UpdateWeightsTable(float blurPower)
*  @brief     Update weight table
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
void Blur::UpdateWeightsTable(float sigma)
{
	float weights[WEIGHT_TABLE_SIZE];
	float total = 0.0f;
	for (int i = 0; i < WEIGHT_TABLE_SIZE; ++i)
	{
		weights[i] = expf(-0.5f * (float)(i * i) / (sigma * sigma));
		if (i == 0) { total += weights[i]; }
		else { total += 2.0f * weights[i]; }
	}

	// normalize
	for (int i = 0; i < 8; ++i)
	{
		weights[i] /= total;
	}

	// Copy weights table
	BlurParameter parameter;
	parameter.Weights[0] = Float4(weights[0], weights[1], weights[2], weights[3]);
	parameter.Weights[1] = Float4(weights[4], weights[5], weights[6], weights[7]);
	
	_weightBuffer.get()->CopyStart();
	_weightBuffer.get()->CopyTotalData(&parameter, 1);
	_weightBuffer.get()->CopyEnd();
}

/****************************************************************************
*                       UpdateBlurSampling
*************************************************************************//**
*  @fn        void Blur::UpdateBlurSampling(int xBlurWidthDivision, int xBlurHeightDivision, int yBlurWidthDivision, int yBlurHeightDivision)
*  @brief     Update blur sampling
*  @param[in] int, int, int, int blur division count;
*  @return 　　void
*****************************************************************************/
void Blur::UpdateBlurSampling(int xBlurWidthDivision, int xBlurHeightDivision, int yBlurWidthDivision, int yBlurHeightDivision)
{
	DirectX12& directX12 = DirectX12::Instance();
	Screen screen;

	TextureSizeParameter parameter;
	parameter.OriginalTexture[0] = screen.GetScreenWidth();
	parameter.OriginalTexture[1] = screen.GetScreenHeight();
	parameter.XBlurTexture[0]    = screen.GetScreenWidth()  / xBlurWidthDivision;
	parameter.XBlurTexture[1]    = screen.GetScreenHeight() / xBlurHeightDivision;
	parameter.YBlurTexture[0]    = screen.GetScreenWidth()  / yBlurWidthDivision;
	parameter.YBlurTexture[1]    = screen.GetScreenHeight() / yBlurHeightDivision;;

	_textureSizeBuffer.get()->CopyStart();
	_textureSizeBuffer.get()->CopyTotalData(&parameter, 1);
	_textureSizeBuffer.get()->CopyEnd();
}
#pragma region Protected Function
/****************************************************************************
*                       PrepareResources
*************************************************************************//**
*  @fn        bool Blur::PrepareResourcess()
*  @brief     This function prepares OffScreen render target )
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Blur::PrepareResources()
{
	Screen screen;
	_colorBuffer[0] = DirectX12::Instance().ResizeOffScreenRenderTarget(0, screen.GetScreenWidth(), screen.GetScreenHeight());
	_colorBuffer[1] = DirectX12::Instance().ResizeOffScreenRenderTarget(1, screen.GetScreenWidth(), screen.GetScreenHeight());
	_colorBuffer[2] = DirectX12::Instance().ResizeOffScreenRenderTarget(2, screen.GetScreenWidth() / 2, screen.GetScreenHeight());
	_colorBuffer[3] = DirectX12::Instance().ResizeOffScreenRenderTarget(3, screen.GetScreenWidth() / 2, screen.GetScreenHeight() / 2);

	return true;
}

/****************************************************************************
*                       PrepareDescriptors
*************************************************************************//**
*  @fn        bool Blur::PrepareDescriptors()
*  @brief     This function prepares UAV )
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Blur::PrepareDescriptors()
{

	return true;
}

/****************************************************************************
*                       PrepareBlurParameters
*************************************************************************//**
*  @fn        bool Blur::PrepareBlurParameters()
*  @brief     This function prepares blur weights by zero.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Blur::PrepareBlurParameters()
{
	DirectX12& directX12 = DirectX12::Instance();
	BlurParameter parameter;
	parameter.Weights[0] = gm::Float4(0, 0, 0, 0);
	parameter.Weights[1] = gm::Float4(0, 0, 0, 0);

	auto weights = std::make_unique<UploadBuffer<BlurParameter>>(directX12.GetDevice(), 1, true);
	weights->CopyStart();
	weights->CopyTotalData(&parameter, 1);
	weights->CopyEnd();
	
	_weightBuffer = std::move(weights);
	return true;
}

/****************************************************************************
*                       PrepareTextureSizeBuffer
*************************************************************************//**
*  @fn        bool Blur::PrepareTextureSizeBuffer()
*  @brief     Prepare texture size buffer (original, xBlur texture, yblur texture)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Blur::PrepareTextureSizeBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();
	Screen screen;

	TextureSizeParameter parameter;
	parameter.OriginalTexture[0] = screen.GetScreenWidth();
	parameter.OriginalTexture[1] = screen.GetScreenHeight();
	parameter.XBlurTexture[0]    = screen.GetScreenWidth() / 2;
	parameter.XBlurTexture[1]    = screen.GetScreenHeight();
	parameter.YBlurTexture[0]    = screen.GetScreenWidth() / 2;
	parameter.YBlurTexture[1]    = screen.GetScreenHeight() / 2;
	_textureSizeParameter = parameter;

	auto textureSizeBuffer = std::make_unique<UploadBuffer<TextureSizeParameter>>(directX12.GetDevice(), 1, true);

	textureSizeBuffer.get()->CopyStart();
	textureSizeBuffer.get()->CopyTotalData(&parameter, 1);
	textureSizeBuffer.get()->CopyEnd();
	_textureSizeBuffer = std::move(textureSizeBuffer);
	return true;
}

bool Blur::PrepareRootSignature()
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
	DESCRIPTOR_RANGE textureTable[2];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0); // u0
	
	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[4];
	rootParameter[0].InitAsConstantBufferView(0);
	rootParameter[1].InitAsConstantBufferView(1);
	rootParameter[2].InitAsDescriptorTable(1, &textureTable[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[3].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_ALL);

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

bool Blur::PreparePipelineState()
{
	std::wstring defaultPath = L"Shader\\Effect\\ShaderBlur.hlsl";
	Device* device = DirectX12::Instance().GetDevice();
	
	BlobComPtr xBlurCS, yBlurCS, finalBlurCS;
	xBlurCS     = CompileShader(defaultPath, nullptr, "XBlur", "cs_5_1");
	yBlurCS     = CompileShader(defaultPath, nullptr, "YBlur", "cs_5_1");
	finalBlurCS = CompileShader(defaultPath, nullptr, "FinalBlur", "cs_5_1");

	/*-------------------------------------------------------------------
	-			XBlur pipelineState
	---------------------------------------------------------------------*/
	D3D12_COMPUTE_PIPELINE_STATE_DESC xBlurPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	xBlurPipeline.pRootSignature = _rootSignature.Get();
	xBlurPipeline.CS =
	{
		reinterpret_cast<BYTE*>(xBlurCS->GetBufferPointer()),
		xBlurCS->GetBufferSize()
	};
	
	ThrowIfFailed(device->CreateComputePipelineState(&xBlurPipeline, IID_PPV_ARGS(&_xBlurPipeline)));

	/*-------------------------------------------------------------------
	-			YBlur pipelineState
	---------------------------------------------------------------------*/
	auto yBlurPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	yBlurPipeline.pRootSignature = _rootSignature.Get();
	yBlurPipeline.CS =
	{
		reinterpret_cast<BYTE*>(yBlurCS->GetBufferPointer()),
		yBlurCS->GetBufferSize()
	};
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateComputePipelineState(&yBlurPipeline, IID_PPV_ARGS(&_yBlurPipeline)));

	/*-------------------------------------------------------------------
	-			FinalBlur pipelineState
	---------------------------------------------------------------------*/
	auto finalBlurPipeline = DirectX12::Instance().GetDefaultComputePSOConfig();
	finalBlurPipeline.pRootSignature = _rootSignature.Get();
	finalBlurPipeline.CS =
	{
		reinterpret_cast<BYTE*>(finalBlurCS->GetBufferPointer()),
		finalBlurCS->GetBufferSize()
	};
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateComputePipelineState(&finalBlurPipeline, IID_PPV_ARGS(&_pipelineState)));
	return true;
}

void Blur::ExecuteFirstBarrier(DirectX12& directX12, CommandList* commandList)
{
	BARRIER barrier[] =
	{
		BARRIER::Transition(directX12.GetCurrentRenderTarget(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE),
		BARRIER::Transition(_colorBuffer[0].Resource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)
	};
	commandList->ResourceBarrier(_countof(barrier), barrier);
}

void Blur::ExecuteSecondBarrier(DirectX12& directX12, CommandList* commandList)
{
	BARRIER barrier[] =
	{
		BARRIER::Transition(directX12.GetCurrentRenderTarget(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
		BARRIER::Transition(_colorBuffer[0].Resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ),
		BARRIER::Transition(_colorBuffer[1].Resource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
	};
	commandList->ResourceBarrier(_countof(barrier), barrier);
}

void Blur::ExecuteThirdBarrier(DirectX12& directX12, CommandList* commandList)
{
	BARRIER barrier[] =
	{
		BARRIER::Transition(_colorBuffer[1].Resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ),
		BARRIER::Transition(_colorBuffer[2].Resource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
	};
	commandList->ResourceBarrier(_countof(barrier), barrier);
}

void Blur::ExecuteFourthBarrier(DirectX12& directX12, CommandList* commandList)
{
	BARRIER barrier[] =
	{
		BARRIER::Transition(_colorBuffer[2].Resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ),
		BARRIER::Transition(_colorBuffer[3].Resource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
	};
	commandList->ResourceBarrier(_countof(barrier), barrier);
}

void Blur::ExecuteFifthBarrier(DirectX12& directX12, CommandList* commandList)
{
	BARRIER barrier[] =
	{
		BARRIER::Transition(_colorBuffer[0].Resource.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON),
		BARRIER::Transition(_colorBuffer[1].Resource.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON),
		BARRIER::Transition(_colorBuffer[2].Resource.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON),
		BARRIER::Transition(_colorBuffer[3].Resource.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,  D3D12_RESOURCE_STATE_COPY_SOURCE),
	};
	commandList->ResourceBarrier(_countof(barrier), barrier);
}

void Blur::ExecuteFinalBarrier(DirectX12& directX12, CommandList* commandList)
{
	BARRIER barrier[] =
	{
		BARRIER::Transition(_colorBuffer[3].Resource.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON),
		BARRIER::Transition(directX12.GetCurrentRenderTarget(),
		D3D12_RESOURCE_STATE_COPY_DEST,  D3D12_RESOURCE_STATE_RENDER_TARGET)
	};
	commandList->ResourceBarrier(_countof(barrier), barrier);
}
#pragma endregion Protected Function