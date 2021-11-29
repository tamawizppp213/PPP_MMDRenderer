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

//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class MainRenderScene final : public Scene
{
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

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool InitializeFrameResources();
	bool InitializeSceneParameters();
	bool InitializeModels(GameTimer& gameTimer);
	bool InitializeLights();
	bool InitializeRenderingEngine();

	bool LoadSkybox();
	bool LoadTextures();
	bool LoadStage();
	void OnGameInput();
	void OnKeyboardInput();
	void OnMouseInput();
	void OnGamePadInput();

	void DrawExplainText();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Camera _fpsCamera;
	RenderingEngine& _renderingEngine = RenderingEngine::Instance();
	std::unique_ptr<Skybox>         _skybox         = nullptr;
	std::unique_ptr<PMXModel>       _stage          = nullptr;
	std::unique_ptr<Miku>            _miku           = nullptr;
	std::unique_ptr<SpriteRenderer> _spriteRenderer = nullptr;
	std::unordered_map<std::string, std::unique_ptr<Texture>> _textures;
	FrameResource* _frameResource     = nullptr;
	DirectX::XMFLOAT3 _playerPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	// explain text
	bool _enabledExplainText = false;
	Texture _whiteTexture;
	Sprite  _backSprite;

	// postEffect Index
	int _postEffectIndex = 0;
};
#endif