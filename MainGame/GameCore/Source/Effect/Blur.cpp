//////////////////////////////////////////////////////////////////////////////////
///             @file   Blur.cpp
///             @brief  Blur 
///             @author Toide Yutaro
///             @date   2021_07_15
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

#define BLUR_COLORBUFFER_NAME L"Blur::ColorBuffer"
//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
bool Blur::Initialize(int width, int height, DXGI_FORMAT format)
{
	_colorBuffer.resize(4);
	/*-------------------------------------------------------------------
	-               Prepare RootSignature
	---------------------------------------------------------------------*/
	if (!PrepareRootSignature())     { return false;}
	/*-------------------------------------------------------------------
	-               Prepare PipelineState
	---------------------------------------------------------------------*/
	if (!PreparePipelineState())     { return false;}
	/*-------------------------------------------------------------------
	-               Prepare Vertex Buffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer())      { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Index Buffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer())       { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Constant Buffer
	---------------------------------------------------------------------*/
	if (!PrepareConstantBuffer())    { return false; }
	/*-------------------------------------------------------------------
	-               Set Blur Parameter
	---------------------------------------------------------------------*/
	if (!PrepareBlurParameters())    { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Texture Size Buffer
	---------------------------------------------------------------------*/
	if (!PrepareTextureSizeBuffer(width, height)) { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Sprite
	---------------------------------------------------------------------*/
	if (!PrepareSprite())            { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Resources
	---------------------------------------------------------------------*/
	if (!PrepareResources(width, height, format)) { return false; }
	/*-------------------------------------------------------------------
	-               Prepare Descriptor
	---------------------------------------------------------------------*/
	if (!PrepareDescriptors())       { return false; }
	return true;
}

void Blur::Finalize()
{
	PostEffect::Finalize();
	_weightBuffer     .get()->Resource()->Release();
	_textureSizeBuffer.get()->Resource()->Release();
	_xBlurPipeline = nullptr;
	_yBlurPipeline = nullptr;
}
bool Blur::Draw(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].get()->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView = _meshBuffer[currentFrameIndex].get()->IndexBufferView();

	_colorBuffer[0].CopyToColorBuffer(renderTarget, renderTargetState); // copy to back buffer

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
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[0].GetGPUSRV());// srv,
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[1].GetGPUUAV());//uav
	commandList->Dispatch(
		_textureSizeParameter.XBlurTexture[0] / 4,
		_textureSizeParameter.XBlurTexture[1] / 4, 1);

	/*-------------------------------------------------------------------
	-               Execute YBlur Command
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_yBlurPipeline.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[1].GetGPUSRV());// srv,
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[2].GetGPUUAV());//uav
	commandList->Dispatch(
		_textureSizeParameter.YBlurTexture[0] / 4,
		_textureSizeParameter.YBlurTexture[1] / 4, 1);

	/*-------------------------------------------------------------------
	-               Execute FinalBlur Command
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_pipelineState.Get());
	commandList->SetComputeRootDescriptorTable(2, _colorBuffer[2].GetGPUSRV());// srv,
	commandList->SetComputeRootDescriptorTable(3, _colorBuffer[3].GetGPUUAV());//uav
	commandList->Dispatch(
		_textureSizeParameter.OriginalTexture[0] / 4,
		_textureSizeParameter.OriginalTexture[1] / 4, 1);


	/*-------------------------------------------------------------------
	-               Copy to back buffer
	---------------------------------------------------------------------*/
	FinalCopyToResource(renderTarget, renderTargetState);
	return true;
}
bool Blur::Draw()
{
	return Draw(DirectX12::Instance().GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}

bool Blur::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth
		|| _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		_colorBuffer[0].OnResize(newWidth, newHeight);
		_colorBuffer[1].OnResize(newWidth / 2, newHeight);
		_colorBuffer[2].OnResize(newWidth / 2, newHeight / 2);
		_colorBuffer[3].OnResize(newWidth , newHeight);
		PrepareTextureSizeBuffer(newWidth, newHeight);
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
bool Blur::PrepareResources(int width, int height, DXGI_FORMAT format)
{
	_colorBuffer[0].Create(width    , height    , format, nullptr, BLUR_COLORBUFFER_NAME);
	_colorBuffer[1].Create(width / 2, height    , format, nullptr, BLUR_COLORBUFFER_NAME);
	_colorBuffer[2].Create(width / 2, height / 2, format, nullptr, BLUR_COLORBUFFER_NAME);
	_colorBuffer[3].Create(width    , height    , format, nullptr, BLUR_COLORBUFFER_NAME);

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

	auto weights = std::make_unique<UploadBuffer<BlurParameter>>(directX12.GetDevice(), 1, true, L"Blur::BlurParameterUploadBuffer");
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
bool Blur::PrepareTextureSizeBuffer(int width, int height)
{
	DirectX12& directX12 = DirectX12::Instance();
	Screen screen;

	TextureSizeParameter parameter;
	parameter.OriginalTexture[0] = width;
	parameter.OriginalTexture[1] = height;
	parameter.XBlurTexture[0]    = width / 2;
	parameter.XBlurTexture[1]    = height;
	parameter.YBlurTexture[0]    = width / 2;
	parameter.YBlurTexture[1]    = height / 2;
	_textureSizeParameter = parameter;

	auto textureSizeBuffer = std::make_unique<UploadBuffer<TextureSizeParameter>>(directX12.GetDevice(), 1, true, L"Blur::TextureSizeParameter");

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
	_rootSignature->SetName(L"Blur::RootSignature");
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
	_xBlurPipeline->SetName(L"Blur::XBlurPipelineState");

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
	_yBlurPipeline->SetName(L"Blur::YBlurPipelineState");

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
	_pipelineState->SetName(L"Blur::FinalBlurPipelineState");
	return true;
}

#pragma endregion Protected Function