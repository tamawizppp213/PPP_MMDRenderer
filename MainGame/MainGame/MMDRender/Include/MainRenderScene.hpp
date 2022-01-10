//////////////////////////////////////////////////////////////////////////////////
//              Title:  MainBattle.hpp
//            Content:  Battle Scene 
//             Author:  Toide Yutaro
//             Create:  2021_03_13
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MAIN_BATTLE_HPP
#define MAIN_BATTLE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/Scene.hpp"
#include "GameCore/Include/Core/RenderingEngine.hpp"
#include "GameCore/Include/Camera.hpp"
#include "GameCore/Include/EnvironmentMap/CubeMap.hpp"
#include "GameCore/Include/Sprite/Sprite.hpp"
#include <memory>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct FrameResource;
class SpriteRenderer;
class PMXModel;
class Miku;
class Camera;
class PostEffect;
class Bloom;

//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class MainRenderScene final : public Scene
{
	using SkyboxPtr      = std::unique_ptr<Skybox>;
	using StagePtr       = std::unique_ptr<PMXModel>;
	using MikuPtr        = std::unique_ptr<Miku>;
	using BloomPtr       = std::unique_ptr<Bloom>;
	using PostEffectPtr  = std::unique_ptr<PostEffect>;
	using PostEffectPtrs = std::vector<PostEffectPtr>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	MainRenderScene();
	~MainRenderScene();
	void Initialize(GameTimer& gameTimer) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	MainRenderScene(const MainRenderScene&) = default;
	MainRenderScene& operator=(const MainRenderScene&) = default;
	MainRenderScene(MainRenderScene&&)                 = default;
	MainRenderScene& operator=(MainRenderScene&&)      = default;
protected:
	bool LoadMaterials(GameTimer& gameTimer) override;
	void OnGamePadInput () override;
	void OnKeyboardInput() override;
	void OnMouseInput   () override;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool InitializeFrameResources();
	bool InitializeSceneParameters();
	bool InitializeLights();
	bool InitializeRenderingEngine();

	void UpdateCamera();

	void DrawExplainText();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Camera _fpsCamera;
	RenderingEngine& _renderingEngine = RenderingEngine::Instance();
	
	/*-------------------------------------------------------------------
	-          3DModel
	---------------------------------------------------------------------*/
	SkyboxPtr _skybox = nullptr;
	StagePtr  _stage  = nullptr;
	MikuPtr   _miku   = nullptr;
	
	FrameResource*                  _frameResource  = nullptr;
	gm::Float3 _playerPosition = gm::Float3(0.0f, 0.0f, 0.0f);

	/*-------------------------------------------------------------------
	-          ExplainText
	---------------------------------------------------------------------*/
	bool    _enabledExplainText = false;
	Texture _whiteTexture;
	Sprite  _backSprite;
	std::vector<std::wstring> _explainTexts;
	const int _explainRows = 7;

	/*-------------------------------------------------------------------
	-          Post Effect Infomation
	---------------------------------------------------------------------*/
	BloomPtr       _bloom;
	PostEffectPtrs _postEffects;
	int            _postEffectIndex = 0;
	const int      _postEffectTypes = 5;

	
};
#endif