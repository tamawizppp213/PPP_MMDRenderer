//////////////////////////////////////////////////////////////////////////////////
//              Title:  SceneManager.hpp
//            Content:  Scene Transition Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Include/SceneManager.hpp"
#include "MainGame/Include/Scene.hpp"
#include "MainGame/Include/Title.hpp"
#include "MainGame/Include/MotionTest.hpp"
#include "MainGame/Include/Test.hpp"
#include <iostream>
#include <cassert>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void SceneManager::Initialize()
{
#ifdef _DEBUG
	PushScene(new Test());
	CallSceneInitialize();
#else
	PushScene(new Title());
	CallSceneInitialize();
#endif
}

void SceneManager::TransitScene(const ScenePtr scene)
{
	_currentScene.pop();
	_currentScene.emplace(scene);
	CallSceneInitialize();
}

void SceneManager::CallSceneInitialize() const
{
	_currentScene.top()->Initialize();
}
void SceneManager::CallSceneUpdate() const
{
	_currentScene.top()->Update();
}

void SceneManager::CallSceneDraw() const
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