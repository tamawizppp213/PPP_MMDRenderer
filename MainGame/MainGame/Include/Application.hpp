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
#include "GameManager.hpp"

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
	void Initialize();
	void Run();
	void Terminate();
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

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	GameManager& _gameManager = GameManager::Instance();
};

#endif