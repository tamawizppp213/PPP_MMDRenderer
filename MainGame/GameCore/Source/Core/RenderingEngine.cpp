//////////////////////////////////////////////////////////////////////////////////
//              Title:  ZPrepass.cpp
//            Content:  ZPrepass
//             Author:  Toide Yutaro
//             Create:  2021_07_20
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/RenderingEngine.hpp"
#include "GameCore/Include/Core/RenderingEngineStruct.hpp"
#include "GameCore/Include/Rendering/CascadeShadowMap.hpp"
#include "GameCore/Include/Rendering/ZPrepass.hpp"
#include "GameCore/Include/Rendering/LightCulling.hpp"
#include "GameCore/Include/Core/GameActor.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameCore/Include/Model/ModelPipelineState.hpp"
#include "GameCore/Include/Rendering/GBuffer.hpp"
#include "GameCore/Include/Rendering/SSAO.hpp"
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "GameCore/Include/Sprite/Sprite.hpp"
#include "GameCore/Include/Sprite/Font.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
SceneLightConstants::~SceneLightConstants(){}
RenderingEngine::RenderingEngine()
{
	/*-------------------------------------------------------------------
	-                   New scece render objects
	---------------------------------------------------------------------*/
	ZPrepassPtr     zPrepass         = std::make_unique<ZPrepass>();
	SceneLightsPtr  sceneLights      = std::make_unique<SceneLightConstants>();
	LightCullingPtr lightCulling     = std::make_unique<LightCulling>();
	GBufferPtr      gBuffer          = std::make_unique<GBuffer>();
	SSAOPtr         ssao             = std::make_unique<SSAO>();
	TextRendererPtr textRenderer     = std::make_unique<TextRenderer>();
	SpriteRendererPtr spriteRenderer = std::make_unique<SpriteRenderer>();

	sceneLights.get()->PointLightNum = NUM_POINT_LIGHTS;
	sceneLights.get()->SpotLightNum  = NUM_SPOT_LIGHTS;

	/*-------------------------------------------------------------------
	-                   move scene render objects
	---------------------------------------------------------------------*/
	_zPrepass       = std::move(zPrepass);
	_sceneLights    = std::move(sceneLights);
	_lightCulling   = std::move(lightCulling);
	_gBuffer        = std::move(gBuffer);
	_ssao           = std::move(ssao);
	_textRenderer   = std::move(textRenderer);
	_spriteRenderer = std::move(spriteRenderer);

	for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i)
	{
		ShadowMapPtr shadowMap = std::make_unique<CascadeShadowMap>();
		_cascadeShadowMaps[i]  = std::move(shadowMap);
	}
}

RenderingEngine::~RenderingEngine()
{
	
}

#pragma region Public Function
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        bool RenderingEngine::Initialize(int width, int height)
*  @brief     Initialize 
*  @param[in] int screen width
*  @param[in] int screen height
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::Initialize(int width, int height)
{
	/*-------------------------------------------------------------------
	-               Prepare scene light buffer
	---------------------------------------------------------------------*/
	PrepareSceneLightsBuffer();
	for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; ++i)
	{
		_cascadeShadowMaps[i].get()->Initialize();
	}

	/*-------------------------------------------------------------------
	-                   Initialize scene render objects
	---------------------------------------------------------------------*/
	_zPrepass      .get()->Initialize(width, height);
	_lightCulling  .get()->Initialize(_zPrepass.get()->GetFinalColorBuffer());
	_lightCulling  .get()->SetLightGPUAddress(_sceneLightsBuffer.get()->Resource()->GetGPUVirtualAddress());
	_gBuffer       .get()->Initialize(width, height);
	_ssao          .get()->Initialize(width, height, _gBuffer.get()->GetGBuffer(GBufferType::Normal), _zPrepass.get()->GetFinalColorBuffer());
	_textRenderer  .get()->Initialize();
	_spriteRenderer.get()->Initialize();
	return true;
}

void RenderingEngine::OnAfterSceneTransition()
{
	_textRenderer.get()->ReloadFont();
}

void RenderingEngine::Finalize()
{
	for (int i = 0; i < _countof(_cascadeShadowMaps); ++i)
	{
		_cascadeShadowMaps[i].get()->Finalize();
	}
	_zPrepass      .get()->Finalize();
	_lightCulling  .get()->Finalize();
	_gBuffer       .get()->Finalize();
	_ssao          .get()->Finalize();
	_textRenderer  .get()->Finalize();
	_spriteRenderer.get()->Finalize();
	_sceneLights.reset();

	_sceneLightsBuffer.reset();

	_forwardRenderingActors.clear();  _forwardRenderingActors.shrink_to_fit();
	_differedRenderingActors.clear(); _differedRenderingActors.shrink_to_fit();
}
/****************************************************************************
*                       OnResize
*************************************************************************//**
*  @fn        bool RenderingEngine::OnResize(int newWidth, int newHeight)
*  @brief     Resize screen 
*  @param[in] int screen width
*  @param[in] int screen height
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::OnResize(int newWidth, int newHeight)
{
	_zPrepass.get()->OnResize(newWidth, newHeight);
	_gBuffer.get()->OnResize(newWidth, newHeight);
	
	return true;
}
/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        bool RenderingEngine::Draw()
*  @brief     Execute rendering engine pass
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::Draw()
{
	CopyToGPUSceneLightsBuffer();
	OnResize(Screen::GetScreenWidth(), Screen::GetScreenHeight());
	/*-------------------------------------------------------------------
	-         Is there a scene object
	---------------------------------------------------------------------*/
	if (_scene == NULL)                              { return false; }
	//if (!DrawShadowMap())                            { return false; }
	/*-------------------------------------------------------------------
	-         ZPrepass
	---------------------------------------------------------------------*/
	if (!_zPrepass    .get()->Draw(_scene))          { return false; }
	/*-------------------------------------------------------------------
	-         LightCulling 
	---------------------------------------------------------------------*/
	if (!_lightCulling.get()->ExecuteCulling())      { return false; }
	/*-------------------------------------------------------------------
	-         G Buffer (Get normal map)
	---------------------------------------------------------------------*/
	if (!_gBuffer     .get()->Draw(_scene))          { return false; }
	/*-------------------------------------------------------------------
	-         SSAO (後でSSAO使わない場合, 白いテクスチャを出力するようにする.)
	---------------------------------------------------------------------*/
	if (!_ssao        .get()->ExecuteSSAO(_scene))   { return false; }
	/*-------------------------------------------------------------------
	-         Draw 3D model  (tile based forward rendering)
	---------------------------------------------------------------------*/
	if (!DrawForwardRenderingAllModel())             { return false; }
	return true;
}

#pragma region SpriteRenderer
/****************************************************************************
*                       DrawSpriteStart
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawSpriteStart()
*  @brief     This function is called at the start of drawing.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawSpriteStart()
{
	return _spriteRenderer.get()->DrawStart();
}

/****************************************************************************
*                       DrawSprites
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawSprites(const std::vector<Sprite>& spriteList, const Texture& texture)
*  @brief     Draw sprites (call by each texture)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawSprites(const std::vector<Sprite>& spriteList, const Texture& texture)
{
	return _spriteRenderer.get()->Draw(spriteList, texture, gm::MatrixIdentity());
}

/****************************************************************************
*                       DrawSpriteEnd
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawSpriteEnd()
*  @brief     This function is called at the end of drawing.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawSpriteEnd()
{
	return _spriteRenderer.get()->DrawEnd();
}

#pragma endregion SpriteRenderer
#pragma region TextRenderer
/****************************************************************************
*                       DrawTextStart
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawTextStart()
*  @brief     This function is called at the start of drawing.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawTextStart()
{
	return _textRenderer.get()->DrawStart();
}

/****************************************************************************
*                       DrawString2D
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawString2D(FontType fontType, const TextString& text, const gm::Float4& color)
*  @brief     Draw string
*  @param[in] FontType fontType
*  @param[in] TextString string, sizePerChar (0~1), startPosition(x,y,z(default z = 0)), Space(0~1)
*  @param[in] Float4 color
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawString2D(FontType fontType, const TextString& text, const gm::Float4& color)
{
	return _textRenderer.get()->DrawString(fontType, text, color, gm::MatrixIdentity());
}

/****************************************************************************
*                       DrawNumber2D
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawNumber2D(FontType fontType, const TextNumber& number, const gm::Float4& color)
*  @brief     Draw number
*  @param[in] FontType fontType
*  @param[in] TextNumber number, digit, sizeofDigit (0~1), startPosition(x,y,z(default z = 0)), Space(0~1)
*  @param[in] Float4 color
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawNumber2D(FontType fontType, const TextNumber& number, const gm::Float4& color)
{
	return _textRenderer.get()->DrawNumber(fontType, number, color, gm::MatrixIdentity());
}

/****************************************************************************
*                       DrawTextEnd
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawTextEnd()
*  @brief     This function is called at the end of drawing.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawTextEnd()
{
	return _textRenderer.get()->DrawEnd();
}
#pragma endregion   TextRenderer
#pragma region Clear Function
/****************************************************************************
*                       ClearAllRenderingPass
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearAllRenderingPass()
*  @brief     Clear rendering engine pass
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearAllRenderingPass()
{
	_zPrepass.get()->ClearActors();
	_gBuffer .get()->ClearActors();
	_forwardRenderingActors .clear();
	_differedRenderingActors.clear();
	return true;
}
/****************************************************************************
*                       ClearZPrepassActors
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearZPrepassActors()
*  @brief     Clear ZPrepass all actors
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearZPrepassActors()
{
	return _zPrepass.get()->ClearActors();
}
/****************************************************************************
*                       ClearZPrepassActor
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearZPrepassActor(GameActor& gameActpr)
*  @brief     Clear ZPrepass actor
*  @param[in,out] GameActor& gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearZPrepassActor(GameActor& gameActor)
{
	return _zPrepass.get()->ClearActor(gameActor);
}
/****************************************************************************
*                      ClearGBufferActors
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearGBufferActors()
*  @brief     Clear GBuffer actors
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearGBufferActors()
{
	_gBuffer.get()->ClearActors();
	return true;
}
/****************************************************************************
*                      ClearGBufferActor
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearGBufferActor(GameActor& gameActor)
*  @brief     Clear GBuffer actor
*  @param[in,out] GameActor& gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearGBufferActor(GameActor& gameActor)
{
	return _gBuffer.get()->ClearActor(gameActor);
}
/****************************************************************************
*                      ClearForwardRenderingActors
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearForwardRenderingActors()
*  @brief     Clear Forward Rendering actor
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearForwardRenderingActors()
{
	_forwardRenderingActors.clear();
	return true;
}
/****************************************************************************
*                      ClearForwardRenderingActor
*************************************************************************//**
*  @fn        bool RenderingEngine::ClearForwardRenderingActor(GameActor& gameActor)
*  @brief     Clear ForwardRendering Actor
*  @param[in,out] GameActor& gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::ClearForwardRenderingActor(GameActor& gameActor)
{
	for (int i = 0; i < _forwardRenderingActors.size(); ++i)
	{
		if (_forwardRenderingActors[i] == &gameActor)
		{
			std::erase(_forwardRenderingActors, _forwardRenderingActors[i]);
			return true;
		}
	}
	return false;
}

bool RenderingEngine::ClearDifferedRenderingActors()
{
	_differedRenderingActors.clear();
	return true;
}
bool RenderingEngine::ClearDifferedRenderingActor(GameActor& gameActor)
{
	for (int i = 0; i < _differedRenderingActors.size(); ++i)
	{
		if (_differedRenderingActors[i] == &gameActor)
		{
			std::erase(_differedRenderingActors, _differedRenderingActors[i]);
			return true;
		}
	}
	return false;
}
#pragma endregion ClearFunction
#pragma region Add Object
/****************************************************************************
*                       AddObjectToShadowMap
*************************************************************************//**
*  @fn        void RenderingEngine::AddObjectToShadowMap(GameActor& gameActor)
*  @brief     This function adds object to shadow map
*  @param[in] GameActor& gameActor
*  @return 　　void
*****************************************************************************/
void RenderingEngine::AddObjectToShadowMap(GameActor& gameActor, int directionalLightNo, bool useNear, bool useMedium, bool useFar)
{
	GameActor* nearActor   = useNear   ? &gameActor : nullptr;
	GameActor* mediumActor = useMedium ? &gameActor : nullptr;
	GameActor* farActor    = useFar    ? &gameActor : nullptr;
	
	_cascadeShadowMaps->get()->AddActor(nearActor, mediumActor, farActor);
}                                        

/****************************************************************************
*                       AddObjectToZPrepass
*************************************************************************//**
*  @fn        void RenderingEngine::AddObjectToZPrepass(GameActor& gameActor)
*  @brief     This function adds object to z prepass
*  @param[in] GameActor& gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::AddObjectToZPrepass(GameActor& gameActor)
{
	return _zPrepass.get()->AddActors(gameActor);
}

/****************************************************************************
*                       AddObjectToGBuffer
*************************************************************************//**
*  @fn        void RenderingEngine::AddObjectToGBuffer(GameActor& gameActor)
*  @brief     This function adds object to GBuffer
*  @param[in] GameActor& gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::AddObjectToGBuffer(GameActor& gameActor)
{
	return _gBuffer.get()->AddActor(gameActor);
}
/****************************************************************************
*                       AddObjectToForwardRenderer
*************************************************************************//**
*  @fn        void RenderingEngine::AddObjectToForwardRenderer(GameActor& gameActor)
*  @brief     This function adds object to forward renderer
*  @param[in] GameActor& gameActor
*  @return 　　void
*****************************************************************************/
void RenderingEngine::AddObjectToForwardRenderer(GameActor& gameActor)
{
	_forwardRenderingActors.push_back(&gameActor);
}

/****************************************************************************
*                       AddObjectToDifferedRenderer
*************************************************************************//**
*  @fn        void RenderingEngine::AddObjectToDifferedRenderer(GameActor& gameActor)
*  @brief     This function adds object to differed renderer
*  @param[in] GameActor& gameActor
*  @return 　　void
*****************************************************************************/
void RenderingEngine::AddObjectToDifferedRenderer(GameActor& gameActor)
{
	_differedRenderingActors.push_back(&gameActor);
}
#pragma endregion     AddObject

#pragma region Property
void RenderingEngine::EnableSSAO(bool enabled)
{
	if (enabled)
	{
		_renderEffectFlag = _renderEffectFlag | (UINT32)RenderEffectFlag::AllowSSAO;
	}
	else
	{
		_renderEffectFlag = _renderEffectFlag & ~(UINT32)(RenderEffectFlag::AllowSSAO);
	}
}
/****************************************************************************
*                       SetDirectionalLight
*************************************************************************//**
*  @fn        bool RenderingEngine::SetDirectionalLight(int lightID, const DirectionalLight& directionalLight)
*  @brief     This function sets directrional light data.
*  @param[in] int lightID
*  @param[in] DirectionalLight& directionalLight
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::SetDirectionalLight(int lightID, const DirectionalLight& directionalLight)
{
	if (lightID < 0 || NUM_DIRECTIONAL_LIGHTS <= lightID) {return false; }
	_isMappedLight = true;
	_sceneLights.get()->DirectionalLights[lightID] = directionalLight;
	return true;
}

/****************************************************************************
*                       SetPointLight
*************************************************************************//**
*  @fn        bool RenderingEngine::SetPointLight(int lightID, const PointLight& pointLight)
*  @brief     This function sets point light data.
*  @param[in] int lightID
*  @param[in] PointLight& pointLight
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::SetPointLight(int lightID, const PointLight& pointLight)
{
	if (lightID < 0 || NUM_POINT_LIGHTS <= lightID) { return false; }
	_isMappedLight = true;
	_sceneLights.get()->PointLights[lightID] = pointLight;
	return true;
}

/****************************************************************************
*                       SetSpotLight
*************************************************************************//**
*  @fn        bool RenderingEngine::SetSpotLight(int lightID, const SpotLight& spotLight)
*  @brief     This function sets spot light data.
*  @param[in] int lightID
*  @param[in] SpotLight& spotLight
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::SetSpotLight(int lightID, const SpotLight& spotLight)
{
	if (lightID < 0 || NUM_SPOT_LIGHTS <= lightID) { return false; }
	_isMappedLight = true;
	_sceneLights.get()->SpotLights[lightID] = spotLight;
	return true;
}

/****************************************************************************
*                       SetSceneGPUAddress
*************************************************************************//**
*  @fn        void RenderingEngine::SetSceneGPUAddress(D3D12_GPU_VIRTUAL_ADDRESS sceneAddress)
*  @brief     This function sets scene constants type gpu address.
*  @param[in] D3D12_GPU_VIRTUALADDRESS sceneConstantsAddress
*  @return 　　void
*****************************************************************************/
void RenderingEngine::SetSceneGPUAddress(D3D12_GPU_VIRTUAL_ADDRESS sceneAddress)
{
	_scene = sceneAddress;
	_lightCulling.get()->SetSceneGPUAddress(_scene);
}
#pragma endregion Property
#pragma endregion Public Function

#pragma region Private Function
void RenderingEngine::PrepareSceneLightsBuffer()
{
	SceneLightsBuffer sceneLightsBuffer = std::make_unique<UploadBuffer<SceneLightConstants>>
		(DirectX12::Instance().GetDevice(), 1, true, L"RenderingEngine::SceneLightsBuffer");
	sceneLightsBuffer.get()->CopyStart();
	sceneLightsBuffer.get()->CopyData(0, *_sceneLights.get());
	sceneLightsBuffer.get()->CopyEnd();
	_sceneLightsBuffer = std::move(sceneLightsBuffer);
}

void RenderingEngine::CopyToGPUSceneLightsBuffer()
{
	if (!_isMappedLight) { return; }
	_sceneLightsBuffer.get()->CopyStart();
	_sceneLightsBuffer.get()->CopyData(0, *_sceneLights.get());
	_sceneLightsBuffer.get()->CopyEnd();
	_isMappedLight = false;
}

/****************************************************************************
*                       DrawForwardRenderingAllModel
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawForwardRenderingAllModel(GameActor* gameActor)
*  @brief     draw all 3DModel
*  @param[in] GameActor* gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawForwardRenderingAllModel()
{
	for (auto model : _forwardRenderingActors)
	{
		/*-------------------------------------------------------------------
		-               Active check
		---------------------------------------------------------------------*/
		if (!model->IsActive()) { continue; }

		/*-------------------------------------------------------------------
		-               Draw forward rendering all model
		---------------------------------------------------------------------*/
		switch ((ActorType)model->GetActorType())
		{
			case ActorType::PMX: 
			{ 
				if (!DrawForwardRenderingPMXModel(model)) { return false; }  break; 
			}
			case ActorType::Primitive:
			{
				if (!DrawForwardRenderingPrimitiveModel(model)) { return false; } break;
			}
			default: { break; }
		}
	}
	return true;
}


/****************************************************************************
*                       DrawForwardRenderingPMXModel
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawForwardRenderingPrimitiveModel(GameActor* gameActor)
*  @brief     draw PMX 3DModel
*  @param[in] GameActor* gameActor
*  @return 　　bool
*****************************************************************************/
bool RenderingEngine::DrawForwardRenderingPMXModel(GameActor* gameActor)
{
	/*-------------------------------------------------------------------
	-               Get pmxModel
	---------------------------------------------------------------------*/
	PMXModel* actor               = (PMXModel*)gameActor;
	ModelPSOManager& modelManager = ModelPSOManager::Instance();
	CommandList*     commandList  = DirectX12::Instance().GetCommandList();

	/*-------------------------------------------------------------------
	-               Set command list
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(modelManager.GetRootSignature(ModelType::PMX));
	commandList->SetPipelineState        (modelManager.GetPipelineState(ModelType::PMX));
	commandList->SetGraphicsRootDescriptorTable(9,  _lightCulling.get()->GetPointLightList().GetGPUSRV()); // point light culling list 
	commandList->SetGraphicsRootDescriptorTable(10, _lightCulling.get()->GetSpotLightList ().GetGPUSRV()); // spot  light culling list 
	commandList->SetGraphicsRootDescriptorTable(11, _ssao.get()->GetFinalColorBuffer().GetGPUSRV());       // ssao
	return actor->Draw(_scene, _sceneLightsBuffer.get()->Resource()->GetGPUVirtualAddress());
}

/****************************************************************************
*                       DrawForwardRenderingPrimitiveModel
*************************************************************************//**
*  @fn        bool RenderingEngine::DrawForwardRenderingPrimitiveModel(GameActor* gameActor)
*  @brief     draw primitive 3DModel
*  @param[in] GameActor* gameActor
*  @return 　　bool 
*****************************************************************************/
bool RenderingEngine::DrawForwardRenderingPrimitiveModel(GameActor* gameActor)
{
	/*-------------------------------------------------------------------
	-               Get Primitive model
	---------------------------------------------------------------------*/
	PrimitiveModel* actor               = (PrimitiveModel*)gameActor;
	ModelPSOManager& modelManager = ModelPSOManager::Instance();
	CommandList*     commandList  = DirectX12::Instance().GetCommandList();

	/*-------------------------------------------------------------------
	-               Set command list 
	---------------------------------------------------------------------*/
	commandList->SetGraphicsRootSignature(modelManager.GetRootSignature(ModelType::PRIMITIVE));
	commandList->SetPipelineState        (modelManager.GetPipelineState(ModelType::PRIMITIVE));
	commandList->SetGraphicsRootDescriptorTable(6, _lightCulling.get()->GetPointLightList().GetGPUSRV());  // point light culling list  
	commandList->SetGraphicsRootDescriptorTable(7, _lightCulling.get()->GetSpotLightList ().GetGPUSRV());  // spot  light culling list
	commandList->SetGraphicsRootDescriptorTable(5, _ssao        .get()->GetFinalColorBuffer().GetGPUSRV());// ssao  
	return actor->Draw(_scene, _sceneLightsBuffer.get()->Resource()->GetGPUVirtualAddress());
}

bool RenderingEngine::DrawShadowMap()
{
	for (int i = 0; i < _countof(_cascadeShadowMaps); ++i)
	{
		_cascadeShadowMaps[i].get()->Execute(_sceneLights.get()->DirectionalLights[i].Direction);
	}
	return true;
}
#pragma endregion Private Function 