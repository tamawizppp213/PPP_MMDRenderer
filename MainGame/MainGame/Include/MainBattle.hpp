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
#include "Scene.hpp"
#include "GameCore/Include/Core/RenderingEngine.hpp"
#include "GameCore/Include/Camera.hpp"
#include "GameCore/Include/EnvironmentMap/CubeMap.hpp"
#include <memory>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct FrameResource;
class SpriteRenderer;
class PMXModel;
class Camera;

//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class MainBattle final : public Scene
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	MainBattle();
	~MainBattle();
	void Initialize(GameTimer& gameTimer) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	MainBattle(const MainBattle&) = default;
	MainBattle& operator=(const MainBattle&) = default;
	MainBattle(MainBattle&&)                 = default;
	MainBattle& operator=(MainBattle&&)      = default;
protected:

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool InitializeDirectX12(GameTimer& gameTimer);
	bool InitializeFrameResources();
	bool InitializeSceneParameters();
	bool InitializeFader();
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


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Screen _screen;
	Camera _fpsCamera;
	RenderingEngine& _renderingEngine = RenderingEngine::Instance();
	std::unique_ptr<Skybox>         _skybox         = nullptr;
	std::unique_ptr<PMXModel>       _stage          = nullptr;
	std::unique_ptr<PMXModel>       _miku           = nullptr;
	std::unique_ptr<SpriteRenderer> _spriteRenderer = nullptr;
	std::unordered_map<std::string, std::unique_ptr<Texture>> _textures;
	FrameResource* _frameResource     = nullptr;
	DirectX::XMFLOAT3 _playerPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};
#endif