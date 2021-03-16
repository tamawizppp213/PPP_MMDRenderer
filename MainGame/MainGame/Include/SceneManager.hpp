//////////////////////////////////////////////////////////////////////////////////
//              Title:  SceneManager.hpp
//            Content:  Scene Transition Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore.hpp"
#include <stack>
#include "MainGame/Include/Scene.hpp"

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
	void Initialize         (GameTimer& gameTimer);
	void TransitScene       (const ScenePtr scene, GameTimer& gameTimer);
	void CallSceneInitialize(GameTimer& gameTimer) const;
	void CallSceneUpdate()     const;
	void CallSceneDraw()       const;
	void CallSceneTerminate()  const;
	void PushScene(const ScenePtr scene);
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
