//////////////////////////////////////////////////////////////////////////////////
//              Title:  Application.hpp
//            Content:  Windows Application (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/GameTimer.hpp"
#include "GameCore/Include/Screen.hpp"
#include "GameCore/Include/Input/GameInput.hpp"
#include "GameCore/Include/Audio/AudioMaster.hpp"
#include "GameManager.hpp"
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Application Class
//////////////////////////////////////////////////////////////////////////////////
class Application final
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize();
	void Run();
	void Terminate();
	LRESULT WindowMessageProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	/*---------------------------------------------------------------------------
					 Singleton
	-----------------------------------------------------------------------------*/
	static Application& Instance()
	{
		static Application application;
		return application;
	}

	// Prohibit move and copy
	Application(const Application&)            = delete;
	Application& operator=(const Application&) = delete;
	Application(Application&&)                 = delete;
	Application& operator=(Application&&)      = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	Application()  = default;
	~Application() = default;
	bool InitializeMainWindow();
	bool InitializeGameInput();
	bool InitializeGameAudio();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	GameManager& _gameManager = GameManager::Instance();
	GameInput&   _gameInput   = GameInput::Instance();
	AudioMaster& _gameAudio   = AudioMaster::Instance();
	GameTimer    _gameTimer;

	/*----------------------------------------------------------
	                     Window Handler
	------------------------------------------------------------*/
	HINSTANCE  _appInstance;       // application instance handler
	HWND       _mainWindow;        // window handler
	WNDCLASSEX _windowClass = {};
	Screen     _screen;
	

	bool _isApplicationPaused;
	bool _isMinimized;
	bool _isMaximized;
	bool _isResizing;

};

#endif