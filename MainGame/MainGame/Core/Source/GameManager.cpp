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