//////////////////////////////////////////////////////////////////////////////////
//              Title:  ZPrepass.cpp
//            Content:  ZPrepass
//             Author:  Toide Yutaro
//             Create:  2021_07_20
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/ZPrepass.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
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
ZPrepass::~ZPrepass()
{
	_rootSignature = nullptr;
	for (auto& shader : _shader) { shader.VertexShader = nullptr;  shader.PixelShader = nullptr; }
	for (auto& pipelineState : _pipelines) { pipelineState = nullptr; }

	_shader     .clear();
	_pipelines  .clear();
	_actors     .clear();
	_colorBuffer.clear();

	_shader     .shrink_to_fit();
	_pipelines  .shrink_to_fit();
	_actors     .shrink_to_fit();
	_colorBuffer.shrink_to_fit();
}
bool ZPrepass::AddActors(GameActor& actor)
{
	_actors.push_back(&actor);
	return true;
}

bool ZPrepass::Initialize(int width, int height)
{
	if (_initialized) { return true; }
	_colorBuffer.resize(1);
	if (!PrepareShader())        { return false; }
	if (!PrepareRootSignature()) { return false; }
	if (!PreparePipelineState()) { return false; }
	if (!PrepareResources(width, height)) { return false; }
	_initialized = true;
	return true;
}

void ZPrepass::Finalize()
{
	/*-------------------------------------------------------------------
	-                       ClearColorBuffer
	---------------------------------------------------------------------*/
	for (auto& colorBuffer : _colorBuffer) { colorBuffer.GetColorBuffer().Resource = nullptr;}
	_colorBuffer.clear(); _colorBuffer.shrink_to_fit();
	/*-------------------------------------------------------------------
	-                       ClearRootSignature
	---------------------------------------------------------------------*/
	_rootSignature = nullptr;
	/*-------------------------------------------------------------------
	-                       Clear PipelineState
	---------------------------------------------------------------------*/
	for (auto& pipeline : _pipelines) { pipeline = nullptr; }
	_pipelines.clear(); _pipelines.shrink_to_fit();
	/*-------------------------------------------------------------------
	-                       ClearShader
	---------------------------------------------------------------------*/
	for (auto& shader : _shader)
	{
		shader.VertexShader = nullptr;
		shader.PixelShader = nullptr;
	}
	_shader.clear(); _shader.shrink_to_fit();
	/*-------------------------------------------------------------------
	-                       ClearActors
	---------------------------------------------------------------------*/
	_actors.clear(); _actors.shrink_to_fit();
}

bool ZPrepass::OnResize(int newWidth, int newHeight)
{
	if (_colorBuffer[0].GetColorBuffer().ImageSize.x != newWidth
		|| _colorBuffer[0].GetColorBuffer().ImageSize.y != newHeight)
	{
		_colorBuffer[0].OnResize(newWidth, newHeight);
	}

	return true;
}
bool ZPrepass::Draw(D3D12_GPU_VIRTUAL_ADDRESS scene)
{
	DirectX12& directX12     = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();
	auto colorRTV            = _colorBuffer[0].GetCPURTV();
	auto dsv = directX12.GetCPUResourceView(HeapType::DSV, 0);

	/*-------------------------------------------------------------------
	-               Change Render Target
	---------------------------------------------------------------------*/
	_colorBuffer[0].TransitionResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);

	/*-------------------------------------------------------------------
	-               Execute Command List
	---------------------------------------------------------------------*/
	commandList->ClearRenderTargetView(colorRTV, _colorBuffer[0].GetClearColor(), 0, nullptr);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(1, &colorRTV, true, &dsv);
	commandList->SetGraphicsRootSignature(_rootSignature.Get());
	commandList->SetGraphicsRootConstantBufferView(1, scene);
	for (int i = 0; i < _actors.size(); ++i)
	{
		if (!_actors[i]->IsActive()) { continue; }
		switch ((ActorType)_actors[i]->GetActorType())
		{
			case ActorType::PMX:{ DrawPMXActor(i); break; }
			case ActorType::Primitive: { DrawPrimitiveActor(i); break; }
			default: { break;}

		}
	}

	/*-------------------------------------------------------------------
	-               Return Render Target
	---------------------------------------------------------------------*/
	auto rtv = directX12.GetCPUResourceView(HeapType::RTV, currentFrameIndex);
	commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
	_colorBuffer[0].TransitionResourceState(D3D12_RESOURCE_STATE_COMMON);
	return true;
}

/****************************************************************************
*							ClearActors
*************************************************************************//**
*  @fn        bool ZPrepass::ClearActors()
*  @brief     Clear the all actor objects
*  @param[in] GameActor actor
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::ClearActors()
{
	_actors.clear();
	return true;
}

/****************************************************************************
*							ClearActor
*************************************************************************//**
*  @fn        bool ZPrepass::ClearActor(GameActor& gameActor)
*  @brief     Clear the actor object which is substituted
*  @param[in] GameActor actor
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::ClearActor(GameActor& actor)
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
#pragma endregion Public Function

#pragma region Private Function
#pragma region Prepare
/****************************************************************************
*							PrepareShader
*************************************************************************//**
*  @fn        bool ZPrepass::PrepareShader()
*  @brief     Prepare compute shader (This function uses in initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::PrepareShader()
{
	_shader.resize((int)ActorType::CountOfActorType);
	_shader[0].VertexShader = CompileShader(L"Shader\\Lighting\\ShaderZPrepass.hlsl",nullptr, "PMX_VSMain", "vs_5_1");
	_shader[0].PixelShader  = CompileShader(L"Shader\\Lighting\\ShaderZPrepass.hlsl", nullptr, "PMX_PSMain", "ps_5_1");
	_shader[1].VertexShader = CompileShader(L"Shader\\Lighting\\ShaderZPrepass.hlsl", nullptr, "Primitive_VSMain", "vs_5_1");
	_shader[1].PixelShader = CompileShader(L"Shader\\Lighting\\ShaderZPrepass.hlsl", nullptr, "Primitive_PSMain", "ps_5_1");
	return true;
}

/****************************************************************************
*							PrepareRootSignature
*************************************************************************//**
*  @fn        bool ZPrepass::PrepareRootSignature()
*  @brief     Prepare rootSignature (This function uses in initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::PrepareRootSignature()
{

	/*-------------------------------------------------------------------
	-			Build root parameter
	---------------------------------------------------------------------*/
	ROOT_PARAMETER rootParameter[4];
	rootParameter[0].InitAsConstantBufferView(0); // objectConstants
	rootParameter[1].InitAsConstantBufferView(1); // sceneConsntants
	rootParameter[2].InitAsConstantBufferView(2); // sceneConsntants
	rootParameter[3].InitAsConstantBufferView(4); // bone
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
	_rootSignature->SetName(L"ZPrepass::RootSignature");
	return true;
}

/****************************************************************************
*							PreparePipelineState
*************************************************************************//**
*  @fn        bool ZPrepass::PreparePipelineState()
*  @brief     Prepare pipelineState (This function uses in initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::PreparePipelineState()
{
	_pipelines.resize((int)ActorType::CountOfActorType);

	// pmx
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeLineState = DirectX12::Instance().GetDefaultPSOConfig();
	pipeLineState.InputLayout    = PMXVertex::InputLayout;
	pipeLineState.RTVFormats[0]  = DXGI_FORMAT_R32_FLOAT;
	pipeLineState.pRootSignature = _rootSignature.Get();
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
	_pipelines[0]->SetName(L"ZPrepass::PMXModel::PipelineState");

	pipeLineState.InputLayout   = VertexPositionNormalTexture::InputLayout;
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
	_pipelines[1]->SetName(L"ZPrepass::PrimitiveModel::PipelineState");
	return true;
}

/****************************************************************************
*							PrepareResources
*************************************************************************//**
*  @fn        bool SSAO::PrepareResources(int width, int height)
*  @brief     Prepare resources (This function uses in Initialize)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::PrepareResources(int width, int height)
{
	float clearColor[4];
	clearColor[0] = gm::color::SteelBlue.f[0];
	clearColor[1] = gm::color::SteelBlue.f[1];
	clearColor[2] = gm::color::SteelBlue.f[2];
	clearColor[3] = gm::color::SteelBlue.f[3];
	return _colorBuffer[0].Create(width, height, DXGI_FORMAT_R32_FLOAT, clearColor, L"ZPrepass::ColorBuffer");
}
#pragma endregion Prepare
/****************************************************************************
*							DrawPMXActor
*************************************************************************//**
*  @fn        bool ZPrepass::DrawPMXActor(int index)
*  @brief     Draw pmx format actor (This function uses in Initialize)
*  @param[in] int actor index
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::DrawPMXActor(int index)
{
	PMXModel* actor = (PMXModel*)_actors[index];
	
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = actor->GetCurrentMeshBuffer().VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = actor->GetCurrentMeshBuffer().IndexBufferView();


	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_pipelines[0].Get());
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0,actor->GetModelWorldInfo()->Resource()->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(3, actor->GetBoneBuffer()->Resource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced((UINT)actor->GetPMXData()->GetIndexCount(), 1, 0, 0, 0);
	

	return true;
}

/****************************************************************************
*							DrawPMXActor
*************************************************************************//**
*  @fn        bool ZPrepass::DrawPMXActor(int index)
*  @brief     Draw pmx format actor (This function uses in Initialize)
*  @param[in] int actor index
*  @return 　　bool
*****************************************************************************/
bool ZPrepass::DrawPrimitiveActor(int index)
{
	PrimitiveModel* actor = (PrimitiveModel*)_actors[index];
	
	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = actor->GetCurrentMeshBuffer().VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView  = actor->GetCurrentMeshBuffer().IndexBufferView();


	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_pipelines[1].Get());
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0,actor->GetModelWorldInfo()->Resource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced((UINT)actor->GetIndexCount(), 1, 0, 0, 0);
	

	return true;
}
#pragma endregion Private Function