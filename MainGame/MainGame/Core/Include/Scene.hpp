//////////////////////////////////////////////////////////////////////////////////
//              Title:  Scene.hpp
//            Content:  Scene Parent State (Pure Virtual Function)
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SCENE_HPP
#define SCENE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Base.hpp"
#include "GameCore/Include/Input/GameInput.hpp"
#include "GameCore/Include/GameTimer.hpp"
#include <memory>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class SpriteRenderer;

using SpriteRendererPtr = std::unique_ptr<SpriteRenderer>;
//////////////////////////////////////////////////////////////////////////////////
//                              SceneState
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                     Scene Class (Pure Virtual Class)
//////////////////////////////////////////////////////////////////////////////////
class Scene 
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	Scene();
	~Scene();
	virtual void Initialize(GameTimer& gameTimer);
	virtual void Update();
	virtual void Draw  ()     = 0;
	virtual void Terminate()  = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	bool InitializeDirectX12(GameTimer& gameTimer);
	virtual bool LoadMaterials(GameTimer& gameTimer) = 0;

	// input function
	virtual void OnKeyboardInput();
	virtual void OnMouseInput   ();
	virtual void OnGamePadInput ();
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	DirectX12&        _directX12      = DirectX12::Instance();
	DeviceComPtr      _device         = nullptr;
	CommandListComPtr _commandList    = nullptr;
	GameInput&        _gameInput      = GameInput::Instance();
	GameTimer*        _gameTimer      = nullptr;
	SpriteRendererPtr _spriteRenderer = nullptr;
	Screen            _screen;
	bool              _hasExecutedSceneTransition = false;
	bool              _hasExecutedBackScene = false;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void OnGameInput();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

#endif
