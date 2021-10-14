//////////////////////////////////////////////////////////////////////////////////
//              Title:  CascadeShadowMap.cpp
//            Content:  Cascade Shadow Map
//             Author:  Toide Yutaro
//             Create:  2021_07_20s
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Rendering/CascadeShadowMap.hpp"
#include "GameCore/Include/Camera.hpp"
#include "DirectX12/Include/Core/DirectX12StaticSampler.hpp"
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameMath/Include/GMColor.hpp"
#include <d3dcompiler.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
static const int g_Resolution[(int)ShadowViewType::CountOfShadowViewType] =
{
	1024, // near   plane
	512,  // medium plane
	256,  // far    plane
};

#define CAMERA_WIDTH  (1000.0f)
#define CAMERA_HEIGHT (1000.0f)
#define CAMERA_FAR    (1000.0f)
#define CAMERA_FOV    (0.25f * GM_PI)
#define CAMERA_NEAR   (1.0f)


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void CascadeShadowMapMatrix::CalculateLightViewProjectionCropMatrix(const Camera& camera, const Vector3& lightDirection, const Vector4& plane)
{
	/*-------------------------------------------------------------------
	-      Calculate shadow map
	---------------------------------------------------------------------*/
	Matrix4 lvpMatrix = Shadow(-lightDirection, plane); // direction light only

	/*-------------------------------------------------------------------
	-      Calculate light view projection
	---------------------------------------------------------------------*/
	float cascadeAreaTable[(int)ShadowViewType::CountOfShadowViewType] =
	{
		camera.GetFarZ() * 0.05f,
		camera.GetFarZ() * 0.3f,
		camera.GetFarZ()
	};

	// get camera vector
	const auto& look  = camera.GetLook();
	const auto& right = camera.GetRight();
	const auto& up    = camera.GetUp();

	float nearDepth = camera.GetNearZ();
	for (int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		float nearY = tanf(camera.GetFovVertical() * 0.5f) * nearDepth;
		float nearX = nearY * camera.GetAspect();
		float farY  = tanf(camera.GetFovVertical() * 0.5f) * cascadeAreaTable[i];
		float farX  = farY * camera.GetAspect();

		/*-------------------------------------------------------------------
		-      Get plane center position
		---------------------------------------------------------------------*/
		Vector3 nearPosition = camera.GetPosition() + look * nearDepth;
		Vector3 farPosition  = camera.GetPosition() + look * cascadeAreaTable[i];

		/*-------------------------------------------------------------------
		-      Get vertex
		---------------------------------------------------------------------*/
		Vector3 vertex[8];
		vertex[0] += nearPosition + up * (+nearY) + right * (+nearX);
		vertex[1] += nearPosition + up * (+nearY) + right * (-nearX);
		vertex[2] += nearPosition + up * (-nearY) + right * (+nearX);
		vertex[3] += nearPosition + up * (-nearY) + right * (-nearX);
		vertex[4] += nearPosition + up * (+farY ) + right * (+farX );
		vertex[5] += nearPosition + up * (+farY ) + right * (-farX );
		vertex[6] += nearPosition + up * (-farY ) + right * (+farX );
		vertex[7] += nearPosition + up * (-farY ) + right * (-farX );

		Vector3 vMax, vMin;
		vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
		for (auto& v : vertex)
		{
			Vector4 temp = lvpMatrix * Vector4(v, 0);
			v = Vector3(temp);
			vMax = Max(vMax, v);
			vMin = Min(vMin, v);
		}
		
		/*-------------------------------------------------------------------
		-      Calculate crop matrix
		---------------------------------------------------------------------*/
		float xScale = 2.0f / (vMax.GetX() - vMin.GetX());
		float yScale = 2.0f / (vMax.GetY() - vMin.GetY());
		float xOffset = (vMax.GetX() + vMin.GetX()) * (-0.5f) * xScale;
		float yOffset = (vMax.GetY() + vMin.GetY()) * (-0.5f) * yScale;
		Matrix4 clopMatrix;
		clopMatrix.SetX(Vector4(xScale, 0, 0, 0));
		clopMatrix.SetY(Vector4(0, yScale, 0, 0));
		clopMatrix.SetW(Vector4(xOffset, yOffset, 0, 1));

		/*-------------------------------------------------------------------
		-      Multiply light view projection and crop matrix
		---------------------------------------------------------------------*/
		_lightViewMatrix[i] = lvpMatrix * clopMatrix;
		nearDepth = cascadeAreaTable[i];
	}

	
}

CascadeShadowMap::CascadeShadowMap()
{
	CameraPtr camera = std::make_unique<Camera>();

	_lightCamera = std::move(camera);
}

CascadeShadowMap::~CascadeShadowMap()
{

}

bool CascadeShadowMap::Initialize()
{
	if (_isInitialized) { return true; }
	_colorBuffers.resize((int)ShadowViewType::CountOfShadowViewType);
	if (!PrepareShader())        { return false; }
	if (!PrepareRootSignature()) { return false; }
	if (!PreparePipelineState())  { return false; }
	if (!PrepareResources())     { return false; }
	if (!PrepareCamera())        { return false; }
	if (!PrepareConstantBuffer()) { return false; }
	_isInitialized = true;
	return true;
}

void CascadeShadowMap::AddActor(GameActor* nearActor, GameActor* mediumActor, GameActor* farActor)
{
	if (nearActor   != nullptr) { _actors[(int)ShadowViewType::Near]  .push_back(nearActor); }
	if (mediumActor != nullptr) { _actors[(int)ShadowViewType::Medium].push_back(mediumActor); }
	if (farActor    != nullptr) { _actors[(int)ShadowViewType::Far]   .push_back(farActor); }
}


bool CascadeShadowMap::Execute(const gm::Vector3& lightDirection)
{
	if (gm::NormSquared(lightDirection) < 0.001f) { return true ; }
	UpdateLightCamera(lightDirection);

	/*-------------------------------------------------------------------
	-               Set Valiables
	---------------------------------------------------------------------*/
	DirectX12& directX12     = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex    = directX12.GetCurrentFrameIndex();

	/*-------------------------------------------------------------------
	-               Change Render Target
	---------------------------------------------------------------------*/
	for (int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		_colorBuffers[i].TransitionResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	/*-------------------------------------------------------------------
	-               Execute Command List
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(_rootSignature.Get());

	for (int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		auto colorRTV = _colorBuffers[i].GetCPURTV();
		commandList->ClearRenderTargetView(colorRTV, _colorBuffers[i].GetClearColor(), 0, nullptr);
		commandList->OMSetRenderTargets(1, &colorRTV, true, nullptr);
		commandList->SetGraphicsRootConstantBufferView(1, _shadowBuffers[i].get()->Resource()->GetGPUVirtualAddress());
		for (int j = 0; j < _actors[i].size(); ++j)
		{
			if (!_actors[i][j]->IsActive()) { continue; }
			switch ((ActorType)_actors[i][j]->GetActorType())
			{
				case ActorType::PMX: { DrawPMXActor(i, j); break; }
				case ActorType::Primitive: { DrawPrimitiveActor(i, j); break; }
				default: { break; }
			}
		}
	}


	/*-------------------------------------------------------------------
	-               Return Render Target
	---------------------------------------------------------------------*/
	for (int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		_colorBuffers[i].TransitionResourceState(D3D12_RESOURCE_STATE_COMMON);
	}
	return true;
}


#pragma region Private Function
bool CascadeShadowMap::PrepareShader()
{
	_shader.resize((int)ActorType::CountOfActorType);
	_shader[0].VertexShader = CompileShader(L"Shader\\Lighting\\ShaderCascadeShadow.hlsl", nullptr, "PMX_VSMain", "vs_5_1");
	_shader[0].PixelShader = CompileShader(L"Shader\\Lighting\\ShaderCascadeShadow.hlsl", nullptr, "PMX_PSMain", "ps_5_1");
	_shader[1].VertexShader = CompileShader(L"Shader\\Lighting\\ShaderCascadeShadow.hlsl", nullptr, "Primitive_VSMain", "vs_5_1");
	_shader[1].PixelShader = CompileShader(L"Shader\\Lighting\\ShaderCascadeShadow.hlsl", nullptr, "Primitive_PSMain", "ps_5_1");
	return true;
}

bool CascadeShadowMap::PrepareRootSignature()
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
	return true;
}

bool CascadeShadowMap::PreparePipelineState()
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
	pipeLineState.DepthStencilState.DepthEnable   = false;
	pipeLineState.DepthStencilState.StencilEnable = false;

	ThrowIfFailed(DirectX12::Instance().GetDevice()->CreateGraphicsPipelineState(&pipeLineState, IID_PPV_ARGS(&_pipelines[0])));

	// primitive
	pipeLineState.InputLayout = VertexPositionNormalTexture::InputLayout;
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

bool CascadeShadowMap::PrepareResources()
{
	float clearColor[4];
	clearColor[0] = gm::color::SteelBlue.f[0];
	clearColor[1] = gm::color::SteelBlue.f[1];
	clearColor[2] = gm::color::SteelBlue.f[2];
	clearColor[3] = gm::color::SteelBlue.f[3];
	for (int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		_colorBuffers[i].Create(g_Resolution[i], g_Resolution[i], DXGI_FORMAT_R32_FLOAT, clearColor);
	}
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
bool CascadeShadowMap::DrawPMXActor(int shadowType, int modelIndex)
{
	PMXModel* actor = (PMXModel*)_actors[shadowType][modelIndex];

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
	commandList->SetGraphicsRootConstantBufferView(0, actor->GetModelWorldInfo()->Resource()->GetGPUVirtualAddress());
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
bool CascadeShadowMap::DrawPrimitiveActor(int shadowType, int modelIndex)
{
	PrimitiveModel* actor = (PrimitiveModel*)_actors[shadowType][modelIndex];

	/*-------------------------------------------------------------------
	-               Prepare variable
	---------------------------------------------------------------------*/
	DirectX12& directX12 = DirectX12::Instance();
	CommandList* commandList = directX12.GetCommandList();
	int currentFrameIndex = directX12.GetCurrentFrameIndex();
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = actor->GetCurrentMeshBuffer().VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW  indexBufferView = actor->GetCurrentMeshBuffer().IndexBufferView();


	/*-------------------------------------------------------------------
	-               Execute commandlist
	---------------------------------------------------------------------*/
	commandList->SetPipelineState(_pipelines[1].Get());
	commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->SetGraphicsRootConstantBufferView(0, actor->GetModelWorldInfo()->Resource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced((UINT)actor->GetIndexCount(), 1, 0, 0, 0);


	return true;
}

bool CascadeShadowMap::PrepareCamera()
{
	auto camera = _lightCamera.get();
	camera->SetOrthoLens(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_NEAR, CAMERA_FAR);
	return true;
}

bool CascadeShadowMap::PrepareConstantBuffer()
{
	// sceneConstant Buffer data
	for (int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		shadow::ShadowCameraInfo shadowInfo;
		shadowInfo.ViewProjection = _cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(i);
		_shadowBuffers[i] = std::make_unique<UploadBuffer<shadow::ShadowCameraInfo>>(DirectX12::Instance().GetDevice(), 1, true);
		_shadowBuffers[i].get()->CopyStart();
		_shadowBuffers[i].get()->CopyData(0, shadowInfo);
		_shadowBuffers[i].get()->CopyEnd();
	}
	return true;
}

bool CascadeShadowMap::UpdateLightCamera(const Vector3& lightDirection)
{
	_lightCamera.get()->SetPosition((lightDirection * (-_lightCamera.get()->GetFarZ())).ToFloat3());
	_lightCamera.get()->LookAt(_lightCamera.get()->GetPosition3f(), gm::Float3(0, 0, 0), _lightCamera.get()->GetUp3f());
	_lightCamera.get()->UpdateViewMatrix();
	_cascadeShadowMapMatrix.CalculateLightViewProjectionCropMatrix(*_lightCamera.get(), lightDirection);
	for(int i = 0; i < (int)ShadowViewType::CountOfShadowViewType; ++i)
	{
		shadow::ShadowCameraInfo shadowInfo;
		shadowInfo.ViewProjection = _cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(i);
		_shadowBuffers[i].get()->CopyStart();
		_shadowBuffers[i].get()->CopyData(0, shadowInfo);
		_shadowBuffers[i].get()->CopyEnd();
	}
	return true;
}


#pragma endregion Private Function