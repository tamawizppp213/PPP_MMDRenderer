//////////////////////////////////////////////////////////////////////////////////
///             @file   PostEfect.cpp
///             @brief  PostEffect 
///             @author Toide Yutaro
///             @date   2021_07_08
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include "GameCore/Include/Effect/PostEffect.hpp"
#include "GameMath/Include/GMColor.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
RootSignatureComPtr PostEffect::_defaultRootSignature = nullptr;
PipelineStateComPtr PostEffect::_defaultPipelineState = nullptr;
#define POSTEFFECT_NAME L"PostEffect"

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

PostEffect::~PostEffect()
{
	
}
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool PostEffect::Initialize()
*  @brief     Post Effect Initialize
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::Initialize(PostEffectBlendStateType type, const std::wstring& addName)
{
	if (_hasInitialized) { return true; }

	_colorBuffer.resize(1);
	/*-------------------------------------------------------------------
	-                 Set Addname
	---------------------------------------------------------------------*/
	if (addName != L"") { _addName = addName; _addName += L"::"; }
	/*-------------------------------------------------------------------
	-                 Prepare RootSignature
	---------------------------------------------------------------------*/
	if (!PrepareRootSignature(type))  { return false; } // empty rootSignature
	/*-------------------------------------------------------------------
	-                 Prepare PipelineState
	---------------------------------------------------------------------*/
	if (!PreparePipelineState(type))  { return false; } // empty pipelineState
	/*-------------------------------------------------------------------
	-                 Prepare VertexBuffer
	---------------------------------------------------------------------*/
	if (!PrepareVertexBuffer())   { return false; }
	/*-------------------------------------------------------------------
	-                 Prepare Index Buffer
	---------------------------------------------------------------------*/
	if (!PrepareIndexBuffer())    { return false; }
	/*-------------------------------------------------------------------
	-                 Prepare Constant Buffer
	---------------------------------------------------------------------*/
	if (!PrepareConstantBuffer()) { return false; }
	/*-------------------------------------------------------------------
	-                 Prepare Sprite
	---------------------------------------------------------------------*/
	if (!PrepareSprite())         { return false; }
	/*-------------------------------------------------------------------
	-                 Prepare Resource
	---------------------------------------------------------------------*/
	if (!PrepareResources())      { return false; }
	/*-------------------------------------------------------------------
	-                 Prepare Descriptor
	---------------------------------------------------------------------*/
	if (!PrepareDescriptors())    { return false; }

	_hasInitialized = true;
	return true;
}
/****************************************************************************
*                       Finalize
*************************************************************************//**
*  @fn        bool PostEffect::Finalize()
*  @brief     Post Effect Finalize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PostEffect::Finalize()
{
	/*-------------------------------------------------------------------
	-                      Clear ColorBuffer
	---------------------------------------------------------------------*/
	for (auto& colorBuffer : _colorBuffer) { colorBuffer.GetColorBuffer().Resource = nullptr; }
	_colorBuffer.clear(); _colorBuffer.shrink_to_fit();
	/*-------------------------------------------------------------------
	-                      Clear Vertex and Mesh Buffer
	---------------------------------------------------------------------*/
	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		_vertexBuffer[i].get()->Resource()->Release();
		_vertexBuffer[i].reset();
		_meshBuffer[i].get()->Dispose();
		_meshBuffer[i].reset();
	}
	/*-------------------------------------------------------------------
	-                      Clear Matrix4Buffer
	---------------------------------------------------------------------*/
	_constantBuffer.get()->Resource()->Release();
	_constantBuffer.reset();
	/*-------------------------------------------------------------------
	-                      Clear RootSignature
	---------------------------------------------------------------------*/
	_rootSignature = nullptr;
	/*-------------------------------------------------------------------
	-                      Clear PipelineState
	---------------------------------------------------------------------*/
	_pipelineState = nullptr;

}
void PostEffect::ClearDefaultPipelineState()
{
	_defaultPipelineState = nullptr;
	_defaultRootSignature = nullptr;
}
/****************************************************************************
*                       OnResize
*************************************************************************//**
*  @fn        bool PostEffect::OnResize(int newWidth, int newHeight)
*  @brief     Resize post effect color buffer
*  @param[in] int width
*  @param[in] int height
*  @return 　　bool
*****************************************************************************/
bool PostEffect::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth 
		|| _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		_colorBuffer[0].OnResize(newWidth, newHeight);
		_colorBuffer[1].OnResize(newWidth, newHeight);
	}
	
	return true;
}
/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        bool PostEffect::Draw()
*  @brief     Post Effect Drawing
*  @param[in,out] Resource* renderTarget
*  @param[in] D3D12_RESOURCE_STATE renderTargetState
*  @return 　　bool
*****************************************************************************/
bool PostEffect::Draw(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState)
{
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12                      = DirectX12::Instance();
	CommandList* commandList                  = directX12.GetCommandList();
	int currentFrameIndex                     = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _meshBuffer[currentFrameIndex].get()->VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = _meshBuffer[currentFrameIndex].get()->IndexBufferView();

	/*-------------------------------------------------------------------
	-               Barrier (Shader Resource->RenderTarget)
	---------------------------------------------------------------------*/
	_colorBuffer[0].CopyToColorBuffer(renderTarget, renderTargetState);

	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	ID3D12DescriptorHeap* heapList[] = { directX12.GetCbvSrvUavHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetGraphicsRootSignature(_rootSignature.Get());
	commandList->SetPipelineState        (_pipelineState.Get());
	commandList->SetGraphicsRootDescriptorTable(0, _colorBuffer[0].GetColorBuffer().GPUHandler);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers    (0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer      (&indexBufferView);
	/*-------------------------------------------------------------------
	-               Draw vertex data
	---------------------------------------------------------------------*/
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	/*-------------------------------------------------------------------
	-               Set Prev Frame Screen Size
	---------------------------------------------------------------------*/
	Screen screen;
	_colorBuffer[0].GetColorBuffer().ImageSize.x = static_cast<float> (screen.GetScreenWidth());
	_colorBuffer[0].GetColorBuffer().ImageSize.y = static_cast<float>(screen.GetScreenHeight());
	_vertexBuffer[currentFrameIndex].get()->CopyStart();
	for (int i = 0; i < 4; ++i)
	{
		_vertexBuffer[currentFrameIndex].get()->CopyData(i, _sprite.Vertices[i]);
	}
	_vertexBuffer[currentFrameIndex].get()->CopyEnd();
	return true;
}
/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        bool PostEffect::Draw()
*  @brief     Post Effect Drawing
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::Draw()
{
	return Draw(DirectX12::Instance().GetCurrentRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET);
}
#pragma region Protected Function
/****************************************************************************
*                       PrepareRootSignature
*************************************************************************//**
*  @fn        bool PostEffect::PrepareRootSignature()
*  @brief     Prepare empty root signature
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::PrepareRootSignature(PostEffectBlendStateType type)
{
	/*-------------------------------------------------------------------
	-			Build texture table
	---------------------------------------------------------------------*/
	DESCRIPTOR_RANGE textureTable[1];
	textureTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0); // s0

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[1];
	rootParameter[0].InitAsDescriptorTable(1, &textureTable[0], D3D12_SHADER_VISIBILITY_PIXEL);

	auto samplerDesc = GetStaticSamplers(); // linearsampler

	ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init((UINT)_countof(rootParameter), rootParameter, (UINT)samplerDesc.size(), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_defaultRootSignature);
	rootSignatureDesc.Create(DirectX12::Instance().GetDevice(), &_rootSignature);

	/*-------------------------------------------------------------------
	-			Set Name
	---------------------------------------------------------------------*/
	std::wstring name = _addName + POSTEFFECT_NAME + L"::RootSignature";
	_rootSignature->SetName(name.c_str());
	return true;
}

/****************************************************************************
*                       PreparePipelineState
*************************************************************************//**
*  @fn        bool PostEffect::PreparePipelineState()
*  @brief     Prepare empty pipeline State
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::PreparePipelineState(PostEffectBlendStateType type)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
	pipeLineState.InputLayout    = VertexPositionNormalColorTexture::InputLayout;

	if (_defaultPipelineState == nullptr)
	{
		pipeLineState.pRootSignature = _defaultRootSignature.Get();
		pipeLineState.VS =
		{
			reinterpret_cast<BYTE*>(GetShaderBlendData(PostEffectBlendStateType::None).VertexShader->GetBufferPointer()),
			GetShaderBlendData(PostEffectBlendStateType::None).VertexShader->GetBufferSize()
		};
		pipeLineState.PS =
		{
			reinterpret_cast<BYTE*>(GetShaderBlendData(PostEffectBlendStateType::None).PixelShader->GetBufferPointer()),
			GetShaderBlendData(PostEffectBlendStateType::None).PixelShader->GetBufferSize()
		};
		pipeLineState.BlendState = GetBlendDesc(PostEffectBlendStateType::None);
		ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_defaultPipelineState)));

		std::wstring name = _addName + POSTEFFECT_NAME + L"DefaultPipelineState";
		_defaultPipelineState->SetName(name.c_str());
	}
	
	/*-------------------------------------------------------------------
	-                   Set pipelineState
	---------------------------------------------------------------------*/
	pipeLineState.pRootSignature = _rootSignature.Get();
	pipeLineState.VS =
	{
		reinterpret_cast<BYTE*>(GetShaderBlendData(type).VertexShader->GetBufferPointer()),
		GetShaderBlendData(type).VertexShader->GetBufferSize()
	};
	pipeLineState.PS =
	{
		reinterpret_cast<BYTE*>(GetShaderBlendData(type).PixelShader->GetBufferPointer()),
		GetShaderBlendData(type).PixelShader->GetBufferSize()
	};
	pipeLineState.BlendState = GetBlendDesc(type);
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_pipelineState)));
	std::wstring name = _addName + POSTEFFECT_NAME + L"PipelineState";
	_pipelineState->SetName(name.c_str());
	return true;
}

/****************************************************************************
*                       PrepareVertexBuffer
*************************************************************************//**
*  @fn        bool Fader::PrepareVertexBuffer()
*  @brief     Prepare vertex buffer. (vertexNum: 4)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::PrepareVertexBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	for (int i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		VertexPositionNormalColorTexture* vertices = new VertexPositionNormalColorTexture[4];
		auto vertexBuffer = std::make_unique<UploadBuffer<VertexPositionNormalColorTexture>>(directX12.GetDevice(), 4, false, _addName + L"PostEffect::VertexBuffer");
		auto meshBuffer   = std::make_unique<MeshBuffer>();
		/*-------------------------------------------------------------------
		-			Vertex data initialize
		---------------------------------------------------------------------*/
		vertexBuffer->CopyStart();
		for (int j = 0; j < 4; ++j)
		{
			vertices[j].Position = gm::Float3(0.0f, 0.0f, 0.0f);
			vertices[j].Normal   = gm::Float3(0.0f, 0.0f, 0.0f);
			vertices[j].Color    = gm::Float4(0.0f, 0.0f, 0.0f, 1.0f);
			vertices[j].UV       = gm::Float2(0.0f, 0.0f);
			vertexBuffer->CopyData(j, vertices[j]);
		}
		vertexBuffer->CopyEnd();

		const UINT vbByteSize = (UINT)4 * sizeof(VertexPositionNormalColorTexture);

		/*-------------------------------------------------------------------
		-			Build CPU / GPU Vertex Buffer
		---------------------------------------------------------------------*/
		meshBuffer->BaseVertexLocation   = 0;
		meshBuffer->VertexBufferGPU      = vertexBuffer->Resource();
		meshBuffer->VertexByteStride     = sizeof(VertexPositionNormalColorTexture);
		meshBuffer->VertexBufferByteSize = vbByteSize;
		if (FAILED(D3DCreateBlob(vbByteSize, &meshBuffer->VertexBufferCPU)))
		{
			::OutputDebugString(L"Can't create blob data (vertex)");
			return false;
		}

		_vertexBuffer[i] = std::move(vertexBuffer);
		_meshBuffer[i]   = std::move(meshBuffer);
		delete[] vertices;
	}

	return true;
}

/****************************************************************************
*                       PrepareIndexBuffer
*************************************************************************//**
*  @fn        bool PostEffect::PrepareIndexBuffer()
*  @brief     Prepare index buffer. (indexNum: 6)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::PrepareIndexBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Create rect indices
	---------------------------------------------------------------------*/
	std::vector<UINT16> indices(6);
	UINT16 spriteIndex[] = { 0,1,3,1,2,3 };
	for (int i = 0; i < 6; ++i)
	{
		indices[i] = spriteIndex[i];
	}

	/*-------------------------------------------------------------------
	-			Build CPU / GPU Index Buffer
	---------------------------------------------------------------------*/
	const UINT ibByteSize = (UINT)indices.size() * sizeof(UINT16);
	_meshBuffer[0].get()->IndexBufferGPU = DefaultBuffer(directX12.GetDevice(), directX12.GetCommandList(), indices.data(), ibByteSize, _meshBuffer[0].get()->IndexBufferUploader, _addName + L"PostEffect::IndexBuffer").Resource();
	_meshBuffer[0].get()->IndexFormat = DXGI_FORMAT_R16_UINT;
	_meshBuffer[0].get()->IndexBufferByteSize = ibByteSize;
	if (FAILED(D3DCreateBlob(ibByteSize, &_meshBuffer[0].get()->IndexBufferCPU)))
	{
		::OutputDebugString(L"Can't create blob data (index)");
		return false;
	}
	CopyMemory(_meshBuffer[0].get()->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	/*-------------------------------------------------------------------
	-		Copy the index buffer by the amount of the frame buffer.
	---------------------------------------------------------------------*/
	for (int i = 1; i < FRAME_BUFFER_COUNT; ++i)
	{
		_meshBuffer[i].get()->IndexBufferCPU      = _meshBuffer[0].get()->IndexBufferCPU;
		_meshBuffer[i].get()->IndexBufferGPU      = _meshBuffer[0].get()->IndexBufferGPU;
		_meshBuffer[i].get()->IndexBufferUploader = _meshBuffer[0].get()->IndexBufferUploader;
		_meshBuffer[i].get()->IndexCount          = _meshBuffer[0].get()->IndexCount;
		_meshBuffer[i].get()->IndexFormat         = _meshBuffer[0].get()->IndexFormat;
		_meshBuffer[i].get()->StartIndexLocation  = _meshBuffer[0].get()->StartIndexLocation;
		_meshBuffer[i].get()->IndexBufferByteSize = _meshBuffer[0].get()->IndexBufferByteSize;
	}
	return true;
}

/****************************************************************************
*                       PrepareConstantBuffer
*************************************************************************//**
*  @fn        bool Fader::PrepareConstantBuffer()
*  @brief     Prepare constant buffer.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::PrepareConstantBuffer()
{
	DirectX12& directX12 = DirectX12::Instance();

	/*-------------------------------------------------------------------
	-			Map Constant Buffer
	---------------------------------------------------------------------*/
	auto constantBuffer = std::make_unique<UploadBuffer<Matrix4>>(directX12.GetDevice(), 1, true, _addName + L"PostEffect::Matrix4::UploadBuffer");
	constantBuffer->CopyStart();
	constantBuffer->CopyData(0, MatrixIdentity());
	constantBuffer->CopyEnd();

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View descriptor
	---------------------------------------------------------------------*/
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constantBuffer->Resource()->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(Matrix4));

	/*-------------------------------------------------------------------
	-			Build Constant Buffer View
	---------------------------------------------------------------------*/
	directX12.GetDevice()->CreateConstantBufferView(&cbvDesc, directX12.GetCPUResourceView(HeapType::CBV, directX12.IssueViewID(HeapType::CBV)));
	
	_constantBuffer = std::move(constantBuffer);
	return true;
}
/****************************************************************************
*                       PrepareSprite
*************************************************************************//**
*  @fn        bool PostEffect::PrepareSprite()
*  @brief     Prepare sprite
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PostEffect::PrepareSprite()
{
	// all screen size
	_sprite.CreateSprite(
		Float3(0.0f, 0.0f, 0.0f),
		Float2(2.0f, 2.0f),
		Float4(0.0f, 0.0f, 0.0f, 1.0f),
		Float2(0.0f, 1.0f),
		Float2(0.0f, 1.0f));
	
	return true;
}

bool PostEffect::PrepareResources()
{
	Screen screen;
	_colorBuffer[0].Create(screen.GetScreenWidth(), screen.GetScreenHeight(), DirectX12::Instance().GetBackBufferRenderFormat(),nullptr, L"PostEffect");
	return true;
}

bool PostEffect::PrepareDescriptors()
{
	return true;
}

bool PostEffect::FinalCopyToResource(Resource* renderTarget, D3D12_RESOURCE_STATES renderTargetState)
{
	CommandList* commandList = DirectX12::Instance().GetCommandList();
	BARRIER before[] =
	{
		BARRIER::Transition(renderTarget,
		renderTargetState, D3D12_RESOURCE_STATE_COPY_DEST),
		BARRIER::Transition(_colorBuffer[_colorBuffer.size() - 1].GetColorBuffer().Resource.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE),
	};
	commandList->ResourceBarrier(_countof(before), before);
	commandList->CopyResource(renderTarget, _colorBuffer[_colorBuffer.size() - 1].GetColorBuffer().Resource.Get());
	BARRIER after[] =
	{
		BARRIER::Transition(renderTarget,
		D3D12_RESOURCE_STATE_COPY_DEST, renderTargetState),
		BARRIER::Transition(_colorBuffer[_colorBuffer.size() - 1].GetColorBuffer().Resource.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON)
	};
	commandList->ResourceBarrier(_countof(after), after);
	return true;
}
#pragma endregion Protected Function