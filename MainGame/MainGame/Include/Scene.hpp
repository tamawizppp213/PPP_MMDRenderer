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
#include "GameCore/Include/Sprite/Fade.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              SceneState
//////////////////////////////////////////////////////////////////////////////////
enum SceneState
{
	TITLE,
	MAINGAME,
	GAMEOVER,
	GAMECLEAR,
	PAUSE,
	MOTIONTEST,
	NONE
};

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
	virtual void Initialize(GameTimer& gameTimer) = 0;
	virtual void Draw()       = 0;
	virtual void Update()     = 0;
	virtual void Terminate()  = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	DirectX12&        _directX12   = DirectX12::Instance();
	DeviceComPtr      _device      = nullptr;
	CommandListComPtr _commandList = nullptr;
	GameInput&        _gameInput   = GameInput::Instance();
	GameTimer*        _gameTimer   = nullptr;
	Fader*            _fader       = nullptr;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

#endif
