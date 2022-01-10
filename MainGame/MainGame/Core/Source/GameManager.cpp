//////////////////////////////////////////////////////////////////////////////////
//              Title:  GameManager.cpp
//            Content:  Game Scene Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/GameManager.hpp"
#include "GameCore/Include/Core/GameCorePipelineDeleter.hpp"
#include <cassert>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void GameManager::GameStart(GameTimer& gameTimer)
{
	_directX12.Initialize(_mainWindow);
	_directX12.ResetCommandList();
	_renderingEngine.Initialize(Screen::GetScreenWidth(), Screen::GetScreenHeight());
	_sceneManager.Initialize(gameTimer);
}

void GameManager::GameMain()
{
	Update();
	Draw();
}

void GameManager::GameEnd()
{
	//_directX12.ResetCommandList();
	_sceneManager.CallSceneTerminate();
	_sceneManager.Finalize();
	_renderingEngine.ClearAllRenderingPass();
	_renderingEngine.Finalize();
	GameCorePipelineDeleter::DeleteAllPipelineState();
	_directX12.Finalize();
}

#pragma region Property
DirectX12* GameManager::GetDirectX12()
{
	return &_directX12;
}
void GameManager::SetHWND(HWND hwnd)
{
	assert(hwnd != nullptr);
	_mainWindow = hwnd;
}
#pragma endregion Property
#pragma region Private Function
void GameManager::Draw()
{
	_sceneManager.CallSceneDraw();
}

void GameManager::Update()
{
	_sceneManager.CallSceneUpdate();
}

void GameManager::LoadGameConfig()
{

}

#pragma endregion