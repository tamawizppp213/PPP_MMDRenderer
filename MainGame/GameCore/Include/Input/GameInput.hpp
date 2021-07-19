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
#include "GameCore/Include/Input/Keyboard.hpp"
#include "GameCore/Include/Input/GamePad.hpp"
#include "GameCore/Include/Input/Mouse.hpp"

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
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	void Update();
	void Finalize();
	Keyboard& GetKeyboard() { return this->_keyboard; }
	Mouse&    GetMouse()    { return this->_mouse; }
	GamePad&  GetGamePad()  { return this->_gamePad; }
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
	bool DInputInitialize(HINSTANCE hInstance, HWND hwnd);
	void DInputFinalize();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	Keyboard& _keyboard    = Keyboard::Instance();
	Mouse&    _mouse       = Mouse::Instance();
	GamePad&  _gamePad     = GamePad::Instance();
	LPDIRECTINPUT8 _dInput = nullptr;
	

};

#endif

