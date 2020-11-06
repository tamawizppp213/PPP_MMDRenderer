//////////////////////////////////////////////////////////////////////////////////
//              Title:  SceneManager.hpp
//            Content:  Scene Transition Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_06
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore.hpp"
#include <stack>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         SceneManager Class (Singleton)
//////////////////////////////////////////////////////////////////////////////////
class SceneManager final
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize();
	void TransitScene(ScenePtr scene);
	void CallSceneUpdate();
	void PushScene(ScenePtr scene);
	void PopScene();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	/*---------------------------------------------------------------------------
						Singleton
	-----------------------------------------------------------------------------*/
	static SceneManager& Instance()
	{
		static SceneManager sceneManager;
		return sceneManager;
	}

	// Prohibit move and copy
	SceneManager(const SceneManager&)            = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&)                 = delete;
	SceneManager& operator=(SceneManager&&)      = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	SceneManager()  = default;
	~SceneManager() = default;


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::stack<SceneSharedPtr> _currentScene;
};


#endif SCENEMANAGER_HPP
