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
#include "MainGame/Other/Include/Title.hpp"
#include "MainGame/MMDRender/Include/MainRenderScene.hpp"
#include "MainGame/Other/Include/Test.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <iostream>
#include <cassert>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void SceneManager::Initialize(GameTimer& gameTimer)
{
#ifdef _DEBUG
	PushScene(new MainRenderScene());
	CallSceneInitialize(gameTimer);
#else
	PushScene(new Title());
	CallSceneInitialize(gameTimer);
#endif
}

void SceneManager::TransitScene(const ScenePtr scene, GameTimer& gameTimer)
{
	CallSceneTerminate();
	_currentScene.pop();
	_currentScene.emplace(scene);
	CallSceneInitialize(gameTimer);
}

void SceneManager::CallSceneInitialize(GameTimer& gameTimer) const
{
	_currentScene.top()->Initialize(gameTimer);
}
void SceneManager::CallSceneUpdate()
{
	_currentScene.top()->Update();
}

void SceneManager::CallSceneDraw()
{
	_currentScene.top()->Draw();
}


void SceneManager::CallSceneTerminate() const
{
	_currentScene.top()->Terminate();
}

void SceneManager::PushScene(ScenePtr scene)
{
	_currentScene.emplace(scene);
}

void SceneManager::PopScene()
{
	_currentScene.pop();
	assert(!_currentScene.empty());
}