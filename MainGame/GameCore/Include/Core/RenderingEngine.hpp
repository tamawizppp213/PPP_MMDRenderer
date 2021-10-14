//////////////////////////////////////////////////////////////////////////////////
///             @file   RenderingEngine.hpp
///             @brief  Rendering Engine
///             @author Toide Yutaro
///             @date   2021_08_05
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef RENDERING_ENGINE_HPP
#define RENDERING_ENGINE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Core/RenderingEngineConfig.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "GameCore/Include/Sprite/Font.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class CascadeShadowMap;
class ZPrepass;
class LightCulling;
class GBuffer;
class GameActor;
class SSAO;
class TextRenderer;
class SpriteRenderer;
struct TextString;
struct TextNumber;

//////////////////////////////////////////////////////////////////////////////////
//								Class 
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			RenderingEngine
*************************************************************************//**
*  @class    Rendering Engine
*  @brief    Rendering Engine
*****************************************************************************/
class RenderingEngine
{
	using ShadowMapPtr      = std::unique_ptr<CascadeShadowMap>;
	using ZPrepassPtr       = std::unique_ptr<ZPrepass>;
	using SceneLightsPtr    = std::unique_ptr<SceneLightConstants>;
	using SceneLightsBuffer = std::unique_ptr<UploadBuffer<SceneLightConstants>>;
	using LightCullingPtr   = std::unique_ptr<LightCulling>;
	using GBufferPtr        = std::unique_ptr<GBuffer>;
	using SSAOPtr           = std::unique_ptr<SSAO>;
	using TextRendererPtr   = std::unique_ptr<TextRenderer>;
	using SpriteRendererPtr = std::unique_ptr<SpriteRenderer>;
	using SceneGPUAddress   = D3D12_GPU_VIRTUAL_ADDRESS;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(int width, int height);
	bool OnResize(int newWidth, int newHeight);
	bool Draw();

	/*-------------------------------------------------------------------
	-               Text Renderer
	---------------------------------------------------------------------*/
	bool DrawTextStart(); // call before draw String and Number 2D.
	bool DrawString2D(FontType fontType, const TextString& text, const gm::Float4& color);
	bool DrawNumber2D(FontType fontType, const TextNumber& number, const gm::Float4& color);
	bool DrawTextEnd();   // call after draw string and number 2D.

	/*-------------------------------------------------------------------
	-               Sprite Renderer
	---------------------------------------------------------------------*/
	bool DrawSpriteStart(); // blend state type = normal
	bool DrawSprites(const std::vector<Sprite>& spriteList, const Texture& texture);
	bool DrawSpriteEnd();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	/*-------------------------------------------------------------------
	-               Renderer config 
	---------------------------------------------------------------------*/
#pragma region renderConfig
	void EnableSSAO(bool enabled);
#pragma endregion renderConfig
#pragma region Property
	/*-------------------------------------------------------------------
	-               Set light
	---------------------------------------------------------------------*/
	bool SetDirectionalLight(int lightID, const DirectionalLight& directionalLight);
	bool SetPointLight      (int lightID, const PointLight&       pointLight);
	bool SetSpotLight       (int lightID, const SpotLight&        spotLight);
	void SetSceneGPUAddress(D3D12_GPU_VIRTUAL_ADDRESS sceneAddress);
#pragma endregion Property

	/*-------------------------------------------------------------------
	-               AddObject  Renderer Function
	---------------------------------------------------------------------*/
#pragma region AddObject Fucnction
	void AddObjectToShadowMap       (GameActor& gameActor, int directionalLightNo, bool useNear = true, bool useMedium = true, bool useFar = true);
	bool AddObjectToZPrepass        (GameActor& gameActor);
	bool AddObjectToGBuffer         (GameActor& gameActor);
	void AddObjectToDifferedRenderer(GameActor& gameActor);
	void AddObjectToForwardRenderer (GameActor& gameActor);

#pragma endregion AddObject Function

	/*-------------------------------------------------------------------
	-               Set clear function
	---------------------------------------------------------------------*/
#pragma region Clear Function
	bool ClearAllRenderingPass();
	bool ClearZPrepassActors();
	bool ClearZPrepassActor(GameActor& gameActor);
	bool ClearGBufferActors();
	bool ClearGBufferActor(GameActor& gameActor);
	bool ClearForwardRenderingActors();
	bool ClearForwardRenderingActor(GameActor& gameActor);
	bool ClearDifferedRenderingActors();
	bool ClearDifferedRenderingActor(GameActor& gameActor);
#pragma endregion Clear Function

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static RenderingEngine& Instance()
	{
		static RenderingEngine renderingEngine;
		return renderingEngine;
	}
	RenderingEngine(const RenderingEngine&)            = delete;
	RenderingEngine& operator=(const RenderingEngine&) = delete;
	RenderingEngine(RenderingEngine&&)                 = delete;
	RenderingEngine& operator=(RenderingEngine&&)      = delete;
private:
	RenderingEngine();
	~RenderingEngine();

	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void PrepareSceneLightsBuffer();
	void CopyToGPUSceneLightsBuffer();

	bool DrawShadowMap();
	bool DrawForwardRenderingAllModel();
	bool DrawForwardRenderingPMXModel      (GameActor* gameActor);
	bool DrawForwardRenderingPrimitiveModel(GameActor* gameActor);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ShadowMapPtr      _cascadeShadowMaps[NUM_DIRECTIONAL_LIGHTS];
	ZPrepassPtr       _zPrepass          = nullptr;
	LightCullingPtr   _lightCulling      = nullptr;
	GBufferPtr        _gBuffer           = nullptr;
	SceneLightsPtr    _sceneLights       = nullptr;
	SceneLightsBuffer _sceneLightsBuffer = nullptr;
	TextRendererPtr   _textRenderer      = nullptr;
	SpriteRendererPtr _spriteRenderer    = nullptr;
	SSAOPtr           _ssao              = nullptr;
	SceneGPUAddress   _scene             = 0;
	std::vector<GameActor*> _forwardRenderingActors;
	std::vector<GameActor*> _differedRenderingActors;
	bool _isMappedLight = false;
	int _renderEffectFlag = 0;

};

#endif