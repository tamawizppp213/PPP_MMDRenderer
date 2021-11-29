//////////////////////////////////////////////////////////////////////////////////
//              Title:  GameManager.hpp
//            Content:  Game Scene Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "SceneManager.hpp"
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "GameCore/Include/Core/RenderingEngine.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         GameManager Class
//////////////////////////////////////////////////////////////////////////////////
class GameManager final
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void GameStart(GameTimer& gameTimer);
	void GameMain();
	void GameEnd();
	void SetHWND(HWND hwnd);
	DirectX12* GetDirectX12();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	/*---------------------------------------------------------------------------
	                 Singleton 
	-----------------------------------------------------------------------------*/
	static GameManager& Instance()
	{
		static GameManager gameManager;
		return gameManager;
	}

	// Prohibit move and copy.
	// Singleton answer (public or private): https://stackoverflow.com/questions/18931133/must-a-deleted-constructor-be-private
	GameManager(const GameManager&)            = delete;
	GameManager& operator=(const GameManager&) = delete;
	GameManager(GameManager&&)                 = delete;
	GameManager& operator=(GameManager&&)      = delete;


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	// Constructor and Destructor
	GameManager()  = default;
	~GameManager() = default;
	void Draw();
	void Update();
	void LoadGameConfig();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	SceneManager& _sceneManager       = SceneManager::Instance();
	DirectX12&    _directX12          = DirectX12::Instance();
	RenderingEngine& _renderingEngine = RenderingEngine::Instance();
	HWND _mainWindow = nullptr;
	

};
#endif
