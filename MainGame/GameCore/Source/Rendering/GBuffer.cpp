//////////////////////////////////////////////////////////////////////////////////
//              Title:  GBuffer.cpp
//            Content:  GBuffer
//             Author:  Toide Yutaro
//             Create:  2021_07_20
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/GBuffer.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameCore/Include/Model/ModelPipelineState.hpp"
#include "GameMath/Include/GMColor.hpp"
#include <d3dcompiler.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
GBuffer::~GBuffer()
{
	_rootSignatures.clear();
	_shader.clear();
	_pipelines.clear();
	_actors.clear();

	_rootSignatures.shrink_to_fit();
	_shader   .shrink_to_fit();
	_pipelines.shrink_to_fit();
	_actors   .shrink_to_fit();
}

bool GBuffer::Initialize(int width, int height)
{
	if (!PrepareShader())                 { return false; }
	if (!PrepareRootSignature())          { return false; }
	if (!PreparePipelineState())          { return false; }
	if (!PrepareResources(width, height)) { return false; }
	return true;
}

/****************************************************************************
*                       OnResize
*************************************************************************//**
*  @fn        bool GBuffer::OnResize()
*  @brief     This function resize output window.
*  @param[in] int newWidth
*  @param[in] int newHeight
*  @return 　　bool
*****************************************************************************/
bool GBuffer::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth
		|| _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		for (int i = 0; i < _colorBuffer.size(); ++i)
		{
			_colorBuffer[i].OnResize(newWidth, newHeight);
		}
	}
	return true;
}

bool GBuffer::Draw(D3D12_GPU_VIRTUAL_ADDRESS scene)
{
	DirectX12& directX12     = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();
	auto dsv = directX12.GetCPUResourceView(HeapType::DSV, 0);

	/*-------------------------------------------------------------------
	-               Change Render Target
	---------------------------------------------------------------------*/
	for (int i = 0; i < _colorBuffer.size(); ++i)
	{
		_colorBuffer[i].TransitionResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList->ClearRenderTargetView(_colorBuffer[i].GetCPURTV(), _colorBuffer[i].GetClearColor(), 0, nullptr);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[(int)GBufferType::CountOfGBufferType];
	for (int i = 0; i < _countof(renderTargets); ++i)
	{
		auto colorRTV    = _colorBuffer[i].GetCPURTV();
		renderTargets[i] = colorRTV;
	}
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(_colorBuffer.size(), renderTargets, true, &dsv);
	commandList->SetGraphicsRootConstantBufferView(1, scene);

	/*-------------------------------------------------------------------
	-               Execute Command List
	---------------------------------------------------------------------*/
	for (int i = 0; i < _actors.size(); ++i)
	{
		if (!_actors[i]->IsActive()) { continue; }
		switch ((ActorType)_actors[i]->GetActorType())
		{
		case ActorType::PMX: { DrawPMXActor(i); break; }
		case ActorType::Primitive: { DrawPrimitiveActor(i); break; }
		default: { break; }
		}
	}

	/*-------------------------------------------------------------------
	-               Return Render Target
	---------------------------------------------------------------------*/
	auto rtv = directX12.GetCPUResourceView(HeapType::RTV, currentFrameIndex);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
	for (int i = 0; i < _colorBuffer.size(); ++i)
	{
		_colorBuffer[i].TransitionResourceState(D3D12_RESOURCE_STATE_COMMON);
	}
	return true;
}

/****************************************************************************
*                       ClearActors
*************************************************************************//**
*  @fn        bool GBuffer::ClearActors()
*  @brief     This function clears actors.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GBuffer::ClearActors()
{
	_actors.clear();
	return true;
}

/****************************************************************************
*                       ClearActor
*************************************************************************//**
*  @fn        bool GBuffer::ClearActor()
*  @brief     This function clears the actor.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GBuffer::ClearActor(GameActor& actor)
{
	for (int i = 0; i < _actors.size(); ++i)
	{
		if (_actors[i] == &actor)
		{
			std::erase(_actors, _actors[i]);
			return true;
		}
	}
	return false;
}

/****************************************************************************
*                       AddActors
*************************************************************************//**
*  @fn        bool GBuffer::AddActor()
*  @brief     This function clears actor.
*  @param[in] GameActor& actor
*  @return 　　bool
*****************************************************************************/
bool GBuffer::AddActor(GameActor& actor)
{
	_actors.push_back(&actor);
	return true;
}
#pragma endregion Public Function
#pragma region Private Function
bool GBuffer::PrepareShader()
{
	_shader.resize((int)ActorType::CountOfActorType);
	_shader[0].VertexShader = CompileShader(L"Shader\\Lighting\\ShaderGBuffer.hlsl", nullptr, "PMX_VSMain", "vs_5_1");
	_shader[0].PixelShader = CompileShader(L"Shader\\Lighting\\ShaderGBuffer.hlsl", nullptr, "PMX_PSMain", "ps_5_1");
	_shader[1].VertexShader = CompileShader(L"Shader\\Lighting\\ShaderGBuffer.hlsl", nullptr, "Primitive_VSMain", "vs_5_1");
	_shader[1].PixelShader = CompileShader(L"Shader\\Lighting\\ShaderGBuffer.hlsl", nullptr, "Primitive_PSMain", "ps_5_1");
	return true;
}

bool GBuffer::PrepareRootSignature()
{
	_rootSignatures.resize((int)ActorType::CountOfActorType);
	_rootSignatures[0] = ModelPSOManager::Instance().GetRootSignature(ModelType::PMX);
	_rootSignatures[1] = ModelPSOManager::Instance().GetRootSignature(ModelType::PRIMITIVE);
	return true;
}

/****************************************************************************
*                       PreparePipelineState
*************************************************************************//**
*  @fn        bool GBuffer::PreparePipelineState()
*  @brief     This function prepares pipelineState about each of gameActor types.
*  @param[in] int width
*  @param[in] int height
*  @return 　　bool
*****************************************************************************/
bool GBuffer::PreparePipelineState()
{
	_pipelines.resize((int)ActorType::CountOfActorType);

	// pmx
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
	pipeLineState.NumRenderTargets = (int)GBufferType::CountOfGBufferType;
	pipeLineState.RTVFormats[1] = DirectX12::Instance().GetBackBufferRenderFormat();
	pipeLineState.RTVFormats[2] = DirectX12::Instance().GetBackBufferRenderFormat();

	pipeLineState.InputLayout = PMXVertex::InputLayout;
	pipeLineState.pRootSignature = _rootSignatures[0].Get();
	pipeLineState.VS =
	{
		reinterpret_cast<BYTE*>(_shader[0].VertexShader->GetBufferPointer()),
		_shader[0].VertexShader->GetBufferSize()
	};
	pipeLineState.PS =
	{
		reinterpret_cast<BYTE*>(_shader[0].PixelShader->GetBufferPointer()),
		_shader[0].PixelShader->GetBufferSize()
	};
	pipeLineState.BlendState = GetBlendDesc(BlendStateType::Normal);

	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_pipelines[0])));

	// Primitve
	pipeLineState.InputLayout = VertexPositionNormalTexture::InputLayout;
	pipeLineState.pRootSignature = _rootSignatures[1].Get();
	pipeLineState.VS =
	{
		reinterpret_cast<BYTE*>(_shader[1].VertexShader->GetBufferPointer()),
		_shader[1].VertexShader->GetBufferSize()
	};
	pipeLineState.PS =
	{
		reinterpret_cast<BYTE*>(_shader[1].PixelShader->GetBufferPointer()),
		_shader[1].PixelShader->GetBufferSize()
	};
	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_pipelines[1])));
	return true;
}

/****************************************************************************
*                       PrepareResources
*************************************************************************//**
*  @fn        bool GBuffer::PrepareResources()
*  @brief     This function prepares resources.
*  @param[in] int width
*  @param[in] int height
*  @return 　　bool
*****************************************************************************/
bool GBuffer::PrepareResources(int width, int height)
{
	float clearColor[4];
	clearColor[0] = gm::color::SteelBlue.f[0];
	clearColor[1] = gm::color::SteelBlue.f[1];
	clearColor[2] = gm::color::SteelBlue.f[2];
	clearColor[3] = gm::color::SteelBlue.f[3];

	for (int i = 0; i < _colorBuffer.size(); ++i)
	{
		if (!_colorBuffer[i].Create(width, height, DirectX12::Instance().GetBackBufferRenderFormat(), clearColor))
		{
			return false;
		}
	}
	return true;
}

/****************************************************************************
*                       DrawPMXActor
*************************************************************************//**
*  @fn        bool GBuffer::DrawPMXActor()
*  @brief     This function draws pmx type models.
*  @param[in] int index
*  @return 　　bool
*****************************************************************************/
bool GBuffer::DrawPMXActor(int index)
{
	PMXModel* actor = (PMXModel*)_actors[index];
	
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12     = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = actor->GetCurrentMeshBuffer().VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = actor->GetCurrentMeshBuffer().IndexBufferView();

	commandList->SetGraphicsRootSignature(_rootSignatures[0].Get());
	commandList->SetPipelineState(_pipelines[0].Get());
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0, actor->GetModelWorldInfo()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(4, actor->GetBoneBuffer()->Resource()->GetGPUVirtualAddress());

	/*-------------------------------------------------------------------
	-               Drawing process for each material
	---------------------------------------------------------------------*/
	auto address = actor->GetMaterialBuffer()->Resource()->GetGPUVirtualAddress();
	UINT offset = 0;
	for (int i = 0; i < (int)actor->GetPMXData()->GetMaterialCount(); ++i)
	{
		commandList->SetGraphicsRootConstantBufferView(2, address);
		commandList->SetGraphicsRootDescriptorTable(5, actor->GetPMXData()->GetTextureList(i).Texture.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(6, actor->GetPMXData()->GetTextureList(i).SphereMultiply.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(7, actor->GetPMXData()->GetTextureList(i).SphereAddition.GPUHandler);
		commandList->SetGraphicsRootDescriptorTable(8, actor->GetPMXData()->GetTextureList(i).ToonTexture.GPUHandler);
		commandList->DrawIndexedInstanced(actor->GetPMXData()->GetIndexCountForMaterial(i), 1, offset, actor->GetCurrentMeshBuffer().BaseVertexLocation, 0);
		offset += actor->GetPMXData()->GetIndexCountForMaterial(i);
		address += CalcConstantBufferByteSize(sizeof(PMXMaterial));
	}

	return true;
}
/****************************************************************************
*                       DrawPMXActor
*************************************************************************//**
*  @fn        bool GBuffer::DrawPMXActor()
*  @brief     This function draws pmx type models.
*  @param[in] int index
*  @return 　　bool
*****************************************************************************/
bool GBuffer::DrawPrimitiveActor(int index)
{
	PrimitiveModel* actor = (PrimitiveModel*)_actors[index];

	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = actor->GetCurrentMeshBuffer().VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView = actor->GetCurrentMeshBuffer().IndexBufferView();

	commandList->SetPipelineState(_pipelines[0].Get());
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0, actor->GetModelWorldInfo()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, actor->GetMaterialBuffer()->Resource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(actor->GetIndexCount(), 1, 0, actor->GetCurrentMeshBuffer().BaseVertexLocation, 0);

	return true;
}

#pragma endregion Private Function