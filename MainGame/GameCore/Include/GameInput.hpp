//////////////////////////////////////////////////////////////////////////////////
///             @file   GameInput.hpp
///             @brief  GameInput (Singleton)
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GAMEINPUT_HPP
#define GAMEINPUT_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Keyboard.hpp"
#include "GameCore/Include/GamePad.hpp"
#include "GameCore/Include/Mouse.hpp"
#include "GameCore/Include/KeyCode.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                        Geme Input
//////////////////////////////////////////////////////////////////////////////////
#pragma region GameInput
/****************************************************************************
*				  			GameInput
*************************************************************************//**
*  @class     GameInput
*  @brief     GameInput
*****************************************************************************/
class GameInput
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize();
	void Update();
	void Finalize();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	// Singleton 
	static GameInput& Instance()
	{
		static GameInput gameInput;
		return gameInput;
	}
	GameInput(const GameInput&)            = delete;
	GameInput& operator=(const GameInput&) = delete;
	GameInput(GameInput&&)                 = delete;
	GameInput& operator=(GameInput&&)      = delete;
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	GameInput()  = default;
	~GameInput() = default;


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Keyboard _keyboard;
	GamePad  _gamePad;
	Mouse    _mouse;

};

#endif

