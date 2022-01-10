//////////////////////////////////////////////////////////////////////////////////
//              Title:  SceneManager.hpp
//            Content:  Scene Transition Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/Core/Include/Scene.hpp"
#include "MainGame/Core/Include/RendererTitle.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <iostream>
#include <cassert>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        void SceneManger::Initialize(GameTimer& gameTimer)
*  @brief     Initialize
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void SceneManager::Initialize(GameTimer& gameTimer)
{
	PushScene(new CoreRendererTitle());
	CallSceneInitialize(gameTimer);
}
/****************************************************************************
*                       Finalize
*************************************************************************//**
*  @fn        void SceneManger::Finalize()
*  @brief     Finalize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void SceneManager::Finalize()
{
	while (!_currentScene.empty())
	{
		_currentScene.top().reset();
		_currentScene.pop();
	}
}
/****************************************************************************
*                       TransitScene
*************************************************************************//**
*  @fn        void SceneManager::TransitScene(const ScenePtr scene, GameTimer& gameTimer)
*  @brief     Transit scene
*  @param[in] ScenePtr scene
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void SceneManager::TransitScene(const ScenePtr scene, GameTimer& gameTimer)
{
	CallSceneTerminate();
	_currentScene.pop();
	_currentScene.emplace(scene);
	CallSceneInitialize(gameTimer);
}
/****************************************************************************
*                       CallSceneInitialize
*************************************************************************//**
*  @fn        void SceneManager::CallSceneInitialize(GameTimer& gameTimer) const 
*  @brief     Call current scene Initialize function
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void SceneManager::CallSceneInitialize(GameTimer& gameTimer) const
{
	_currentScene.top()->Initialize(gameTimer);
}
/****************************************************************************
*                       CallSceneUpdate
*************************************************************************//**
*  @fn        void SceneManager::CallSceneUpdate()
*  @brief     Call current scene Update function
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void SceneManager::CallSceneUpdate()
{
	_currentScene.top()->Update();
}
/****************************************************************************
*                       CallSceneDraw
*************************************************************************//**
*  @fn        void SceneManager::CallSceneDraw()
*  @brief     Call current scene Draw function
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void SceneManager::CallSceneDraw()
{
	_currentScene.top()->Draw();
}

/****************************************************************************
*                       CallSceneTerminate
*************************************************************************//**
*  @fn        void SceneManager::CallSceneTerminate() const 
*  @brief     Call scene terminate function
*  @param[in] test
*  @return 　　void
*****************************************************************************/
void SceneManager::CallSceneTerminate() const
{
	_currentScene.top()->Terminate();
}
/****************************************************************************
*                       PushScene
*************************************************************************//**
*  @fn        void SceneManager::PushScene(ScenePtr scene)
*  @brief     Push scene
*  @param[in] ScenePtr scene
*  @return 　　void
*****************************************************************************/
void SceneManager::PushScene(ScenePtr scene)
{
	_currentScene.emplace(scene);
}
/****************************************************************************
*                       PopScene
*************************************************************************//**
*  @fn        void SceneManager::PopScene()
*  @brief     Pop current scene
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void SceneManager::PopScene()
{
	_currentScene.pop();
	assert(!_currentScene.empty());
}