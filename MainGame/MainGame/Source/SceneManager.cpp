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
#include <iostream>
#include <cassert>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void SceneManager::Initialize(const DirectX12& directX12)
{
#ifdef _DEBUG
	PushScene(new MotionTest());
	CallSceneInitialize(directX12);
#else
	PushScene(new Title());
	CallSceneInitialize(directX12);
#endif
}

void SceneManager::TransitScene(const ScenePtr scene, const DirectX12& directX12)
{
	_currentScene.pop();
	_currentScene.emplace(scene);
	CallSceneInitialize(directX12);
}

void SceneManager::CallSceneInitialize(const DirectX12& directX12) const
{
	_currentScene.top()->Initialize(directX12);
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