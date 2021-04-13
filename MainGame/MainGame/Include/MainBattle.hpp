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
#include "GameCore/Include/Camera.hpp"
#include "GameCore/Include/EnvironmentMap/CubeMap.hpp"
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
struct FrameResource;

//////////////////////////////////////////////////////////////////////////////////
//                           Title Class
//////////////////////////////////////////////////////////////////////////////////
class MainBattle final : public Scene
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	MainBattle() {};
	~MainBattle() {};
	void Initialize(GameTimer& gameTimer) override;
	void Draw()       override;
	void Update()     override;
	void Terminate()  override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool InitializeDirectX12(GameTimer& gameTimer);
	bool InitializeFrameResources();
	bool InitializeSceneParameters();
	bool InitializeFader();
	bool LoadTextures();
	void OnGameInput();
	void OnKeyboardInput();
	void OnMouseInput();
	void OnGamePadInput();


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Camera _fpsCamera;
	Skybox _skybox;
	FrameResource* _frameResource     = nullptr;
	DirectX::XMFLOAT3 _playerPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};
#endif