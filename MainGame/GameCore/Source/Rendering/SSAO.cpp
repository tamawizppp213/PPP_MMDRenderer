//////////////////////////////////////////////////////////////////////////////////
//              Title:  ZPrepass.cpp
//            Content:  ZPrepass
//             Author:  Toide Yutaro
//             Create:  2021_07_20
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/SSAO.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "DirectX12/Include/Core/DirectX12Shader.hpp"
#include "GameCore/Include/Effect/Blur.hpp"
#include "GameMath/Include/GMColor.hpp"
#include "GameMath/Include/GMDistribution.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
using namespace ssao;
#define DEFAULT_RADIUS (0.5f)
#define DEFAULT_FADE_START (0.2f)
#define DEFAULT_FADE_END (1.0f)
#define DEFAULT_SURFACE_EPSILON (0.05f)
#define DEFAULT_BLUR_SIZE (5)
#define DEFAULT_SHARPNESS (4.0f)

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
SSAO::SSAO()
{
	auto blur    = std::make_unique<Blur>();
	auto random  = std::make_unique<ColorBuffer>();
	_blur        = std::move(blur);
	_randomMap   = std::move(random);
	_initialized = false;
}

SSAO::~SSAO()
{

}

#pragma region Public Function
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        bool SSAO::Initialize(int width, int height, ColorBuffer& normalMap, ColorBuffer& depthMap)
*  @brief     Initialize
*  @param[in] int screenWidth
*  @param[in] int screenHeight
*  @param[in] ColorBuffer& normalMap
*  @param[in] ColorBuffer& depthMap
*  @return 　　bool
*****************************************************************************/
bool SSAO::Initialize(int width, int height, ColorBuffer& normalMap, ColorBuffer& depthMap)
{
	if (_initialized) { return true; }
	SetNormalMap(normalMap);
	SetDepthMap(depthMap);
	_blur.get()->Initialize(width, height, DirectX12::Instance().GetBackBufferRenderFormat());
	_blur.get()->UpdateWeightsTable(DEFAULT_BLUR_SIZE);
	if (!PrepareRootSignature())                { return false; }
	if (!PreparePipelineState())                { return false; }
	if (!PrepareResources(width, height))       { return false; }
	if (!PrepareParameterBuffer(width, height)) { return false; }
	if (!PrepareRandomTexture())                { return false; }

	_initialized = true;
	return true; 
}

/****************************************************************************
*							ExecuteSSAO
*************************************************************************//**
*  @fn        bool SSAO::ExecuteSSAO(SceneGPUAddress scene)
*  @brief     Prepare execute ssao (This function uses in Initialize)
*  @param[in] SceneGPUAddress (=D3D12_GPU_Address)scene
*  @return 　　bool
*****************************************************************************/
bool SSAO::ExecuteSSAO(SceneGPUAddress scene)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();

	int currentFrameIndex = directX12.GetCurrentFrameIndex();
	auto dsv = directX12.GetCPUResourceView(HeapType::DSV, 0);
	auto rtv = directX12.GetCPUResourceView(HeapType::RTV, currentFrameIndex);
	OnResize((int)_normalMap->GetTextureSize().x, (int)_normalMap->GetTextureSize().y);
	CopyToGPUSSAOParameter();

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	_colorBuffer.TransitionResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ClearRenderTargetView(_colorBuffer.GetCPURTV(), _colorBuffer.GetClearColor(), 0, nullptr);
	auto colorRTV = _colorBuffer.GetCPURTV();
	commandList->OMSetRenderTargets(1, &colorRTV, 0, nullptr);
	commandList->SetGraphicsRootConstantBufferView(1, scene);

	commandList->SetGraphicsRootSignature(_rootSignature.Get());
	commandList->SetPipelineState(_pipeline.Get());
	commandList->SetGraphicsRootConstantBufferView(0, _parameterBuffer.get()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, scene);
	commandList->SetGraphicsRootDescriptorTable(2, _normalMap->GetGPUSRV());// srv,
	commandList->SetGraphicsRootDescriptorTable(3, _depthMap->GetGPUSRV());
	commandList->SetGraphicsRootDescriptorTable(4, _randomMap.get()->GetGPUSRV());//uav
	commandList->IASetVertexBuffers(0, 0, nullptr);
	commandList->IASetIndexBuffer(nullptr);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawInstanced(6, 1, 0, 0);

	_colorBuffer.TransitionResourceState(D3D12_RESOURCE_STATE_COMMON);
	commandList->OMSetRenderTargets(1, &rtv, true, &dsv);

	_blur.get()->Draw(_colorBuffer.GetColorBuffer().Resource.Get());
	_blur.get()->Draw(_colorBuffer.GetColorBuffer().Resource.Get());

	_ambientMap.CopyToColorBuffer(_colorBuffer.GetColorBuffer().Resource.Get());
	return true;
}

#pragma endregion Public Function

#pragma region Private Function
#pragma region Prepare
/****************************************************************************
*							PrepareRootSignature
*************************************************************************//**
*  @fn        bool SSAO::PrepareRootSignature()
*  @brief     Prepare rootSignature  (This function uses in Initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SSAO::PrepareRootSignature()
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
	DESCRIPTOR_RANGE textureTable[4];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0
	textureTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0); // s1
	textureTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0); // s2

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[5];
	rootParameter[0].InitAsConstantBufferView(0); // ssao parameter
	rootParameter[1].InitAsConstantBufferView(1); //scene
	rootParameter[2].InitAsDescriptorTable(1, &textureTable[0], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[3].InitAsDescriptorTable(1, &textureTable[1], D3D12_SHADER_VISIBILITY_ALL);
	rootParameter[4].InitAsDescriptorTable(1, &textureTable[2], D3D12_SHADER_VISIBILITY_ALL);

	/*-------------------------------------------------------------------
	-			Build sampler desc
	---------------------------------------------------------------------*/
	auto samplerDesc = GetStaticSamplers();
	const auto samplerPointClamp = samplerDesc[1];
	const auto samplerLinearWrap = samplerDesc[2];
	const STATIC_SAMPLER_DESC samplerDepthMap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,
		0,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);
	std::array<const STATIC_SAMPLER_DESC, 3> sampler
	{
		samplerDepthMap,
		samplerPointClamp,
		samplerLinearWrap
	};

	/*-------------------------------------------------------------------
	-			 Build root parameter
	---------------------------------------------------------------------*/
	// A root signature is a collection of descriptor tables 
	// (which feeds data other than vertices to the shader).
	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)sampler.size(), sampler.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_rootSignature);
	return true;
}

/****************************************************************************
*							PreparePipelineState
*************************************************************************//**
*  @fn        bool SSAO::PreparePipelineState()
*  @brief     Prepare pipelineState for ssao compute shader (This function uses in Initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SSAO::PreparePipelineState()
{
	std::wstring defaultPath = L"Shader\\Lighting\\ShaderSSAO.hlsl";
	Device* device = DirectX12::Instance().GetDevice();
	_vertexShader = CompileShader(defaultPath, nullptr, "VSMain", "vs_5_1");
	_pixelShader  = CompileShader(defaultPath, nullptr, "SSAO", "ps_5_1");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineState;
	pipelineState = DirectX12::Instance().GetDefaultPSOConfig();
	pipelineState.InputLayout    = { nullptr, 0 };
	pipelineState.pRootSignature = _rootSignature.Get();
	pipelineState.VS =
	{
		reinterpret_cast<BYTE*>(_vertexShader->GetBufferPointer()),
		_vertexShader->GetBufferSize()
	};
	pipelineState.PS =
	{
		reinterpret_cast<BYTE*>(_pixelShader->GetBufferPointer()),
		_pixelShader->GetBufferSize()
	};
	pipelineState.DepthStencilState.DepthEnable = false;
	pipelineState.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	pipelineState.DSVFormat = DXGI_FORMAT_UNKNOWN;
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipelineState, IID_PPV_ARGS(&_pipeline)));
	
	return true;
}

/****************************************************************************
*							PrepareResources 
*************************************************************************//**
*  @fn        bool SSAO::PrepareResources()
*  @brief     Prepare random texture (This function uses in Initialize)
*  @param[in] int screenWidth
*  @param[in] int screenHeight
*  @return 　　bool
*****************************************************************************/
bool SSAO::PrepareResources(int width, int height)
{
	float clearColor[4];
	clearColor[0] = gm::color::SteelBlue.f[0];
	clearColor[1] = gm::color::SteelBlue.f[1];
	clearColor[2] = gm::color::SteelBlue.f[2];
	clearColor[3] = gm::color::SteelBlue.f[3];
	_colorBuffer.Create(static_cast<int>(_normalMap->GetTextureSize().x), static_cast<int>(_normalMap->GetTextureSize().y), DirectX12::Instance().GetBackBufferRenderFormat(), clearColor);
	_ambientMap .Create(static_cast<int>(_normalMap->GetTextureSize().x), static_cast<int>(_normalMap->GetTextureSize().y), DirectX12::Instance().GetBackBufferRenderFormat());
	return true;
}

/****************************************************************************
*							PrepareParameterBuffer
*************************************************************************//**
*  @fn        bool SSAO::PrepareParameterBuffer()
*  @brief     Prepare ssao parameter buffer (This function uses in Initialize)
*  @param[in] int screenWidth
*  @param[in] int screenHeight
*  @return 　　bool
*****************************************************************************/
bool SSAO::PrepareParameterBuffer(int width, int height)
{
	SSAOParameter parameter;
	parameter.TextureSize[0]  = static_cast<float>(width);
	parameter.TextureSize[1]  = static_cast<float>(height);
	parameter.OcclusionRadius = DEFAULT_RADIUS;
	parameter.OcclusionSharpness = DEFAULT_SHARPNESS;
	parameter.OcclusionFadeStart = DEFAULT_FADE_START;
	parameter.OcculusionFadeEnd  = DEFAULT_FADE_END;
	parameter.SurfaceEpsilon     = DEFAULT_SURFACE_EPSILON;

	parameter.OffsetVectors[0]  = Float4(+1.0f, +1.0f, +1.0f, 0.0f);
	parameter.OffsetVectors[1]  = Float4(-1.0f, -1.0f, -1.0f, 0.0f);
	parameter.OffsetVectors[2]  = Float4(-1.0f, +1.0f, +1.0f, 0.0f);
	parameter.OffsetVectors[3]  = Float4(+1.0f, -1.0f, -1.0f, 0.0f);
	parameter.OffsetVectors[4]  = Float4(+1.0f, +1.0f, -1.0f, 0.0f);
	parameter.OffsetVectors[5]  = Float4(-1.0f, -1.0f, +1.0f, 0.0f);
	parameter.OffsetVectors[6]  = Float4(-1.0f, +1.0f, -1.0f, 0.0f);
	parameter.OffsetVectors[7]  = Float4(+1.0f, -1.0f, +1.0f, 0.0f);
	parameter.OffsetVectors[8]  = Float4(-1.0f, +0.0f, +0.0f, 0.0f);
	parameter.OffsetVectors[9]  = Float4(+1.0f, +0.0f, +0.0f, 0.0f);
	parameter.OffsetVectors[10] = Float4(+0.0f, -1.0f, +0.0f, 0.0f);
	parameter.OffsetVectors[11] = Float4(+0.0f, +1.0f, +0.0f, 0.0f);
	parameter.OffsetVectors[12] = Float4(+0.0f, +0.0f, -1.0f, 0.0f);
	parameter.OffsetVectors[13] = Float4(+0.0f, +0.0f, +1.0f, 0.0f);

	Random<float> random;
	random.SetRange(0.25f, 1.0f);
	for (int i = 0; i < OFFSET_VECTOR_SIZE; ++i)
	{
		float s = random.GetRandomValue();
		Vector4 offset = s * Normalize(parameter.OffsetVectors[i]);
		parameter.OffsetVectors[i] = offset.ToFloat4();
	}

	_parameter = parameter;
	auto parameterBuffer = std::make_unique<UploadBuffer<SSAOParameter>>(DirectX12::Instance().GetDevice(), 1, true);
	parameterBuffer.get()->CopyStart();
	parameterBuffer.get()->CopyData(0, parameter);
	parameterBuffer.get()->CopyEnd();
	_parameterBuffer = std::move(parameterBuffer);
	return true;
}

/****************************************************************************
*							PrepareRandomTexture
*************************************************************************//**
*  @fn        bool SSAO::PrepareRandomTexture()
*  @brief     Prepare random texture (This function uses in Initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SSAO::PrepareRandomTexture()
{
	_randomMap.get()->Create(256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);

	/*-------------------------------------------------------------------
	-			 Prepare intermediate upload buffer
	---------------------------------------------------------------------*/
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(_randomMap.get()->GetColorBuffer().Resource.Get(), 0, 1);
	auto buffer = RESOURCE_DESC::Buffer(uploadBufferSize);
	D3D12_HEAP_PROPERTIES heapProp = HEAP_PROPERTY(D3D12_HEAP_TYPE_UPLOAD);
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_uploadBuffer.GetAddressOf())));

	struct RGBA
	{
		char r;
		char g;
		char b;
		char a;
	};

	/*-------------------------------------------------------------------
	-			 Prepare texture data (set random value)
	---------------------------------------------------------------------*/
	RGBA* color = new RGBA[256 * 256];
	RandomInt random;
	random.SetRange(0, 255);
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			color[i * 256 + j].r = random.GetRandomValue();
			color[i * 256 + j].g = random.GetRandomValue();
			color[i * 256 + j].b = random.GetRandomValue();
			color[i * 256 + j].a = 0.0f;
		}
	}
	
	/*-------------------------------------------------------------------
	-			 Prepare subresource data
	---------------------------------------------------------------------*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData      = color;
	subResourceData.RowPitch   = 256 * sizeof(RGBA);
	subResourceData.SlicePitch = subResourceData.RowPitch * 256;

	/*-------------------------------------------------------------------
	-			 Send texture resource to gpu. 
	---------------------------------------------------------------------*/
	CommandList* commandList = DirectX12::Instance().GetCommandList();
	_randomMap.get()->TransitionResourceState(D3D12_RESOURCE_STATE_COPY_DEST);
	UpdateSubresources(
		commandList,
		_randomMap.get()->GetColorBuffer().Resource.Get(),
		_uploadBuffer.Get(),
		0, 0, 1, &subResourceData);
	_randomMap.get()->TransitionResourceState(D3D12_RESOURCE_STATE_COMMON);

	delete[] color;
	return true;
}

/****************************************************************************
*							CopyToGPUSSAOParameter
*************************************************************************//**
*  @fn        bool SSAO::CopyToGPUSSAOParameter()
*  @brief     Send gpu constant buffer information for ssao parameter 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool SSAO::CopyToGPUSSAOParameter()
{
	_parameterBuffer->CopyStart();
	_parameterBuffer->CopyData(0, _parameter);
	_parameterBuffer->CopyEnd();
	return true;
}

/****************************************************************************
*							OnResize
*************************************************************************//**
*  @fn        bool SSAO::OnResize()
*  @brief     Resize buffer size. 
*  @param[in] int newWidth,
*  @param[in] int newHeight
*  @return 　　bool
*****************************************************************************/
bool SSAO::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer.GetColorBuffer().ImageSize.x != newWidth
		|| _colorBuffer.GetColorBuffer().ImageSize.y != newHeight)
	{
		_colorBuffer.OnResize(newWidth, newHeight);
		_ambientMap.OnResize(newWidth, newHeight);
		_blur.get()->OnResize(newWidth, newHeight);
		_parameter.TextureSize[0] = static_cast<float>(newWidth);
		_parameter.TextureSize[1] = static_cast<float> (newHeight);
	}

	return true;
}
#pragma endregion Prepare

#pragma endregion Private Function