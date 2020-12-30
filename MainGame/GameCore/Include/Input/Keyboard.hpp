//////////////////////////////////////////////////////////////////////////////////
///             @file   Keyboard.hpp
///             @brief  Keyboard
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <queue>
#include <dinput.h>
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define MAX_KEY_BUFFER (256)

//////////////////////////////////////////////////////////////////////////////////
//                         Keyboard, class
//////////////////////////////////////////////////////////////////////////////////


#pragma region Keyboard
/****************************************************************************
*				  			Keyboard (Singleton)
*************************************************************************//**
*  @class     Keyboard
*  @brief     Keyboard Input 
*****************************************************************************/
class Keyboard
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(LPDIRECTINPUT8 dInput, HINSTANCE hInstance, HWND hwnd);
	void Update();
	void Finalize();
	bool IsPress  (int keyCode);
	bool IsTrigger(int keyCode);
	bool IsRelease(int keyCode);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static Keyboard& Instance()
	{
		static Keyboard keyboard;
		return keyboard;
	}
	Keyboard(const Keyboard&)            = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	Keyboard(Keyboard&&)                 = delete;
	Keyboard& operator=(Keyboard&&)      = delete;
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	Keyboard();
	~Keyboard() = default;

	bool CreateHWND();
	bool CreateHInstance();
	bool CreateKeyboardDevice();
	bool CreateDataFormat();
	bool CreateCooperativeLevel();


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	LPDIRECTINPUT8       _dInput    = nullptr;
	LPDIRECTINPUTDEVICE8 _keyboard  = nullptr;
	HINSTANCE            _hInstance = nullptr;
	HWND                 _hwnd      = nullptr;
	
	BYTE _keyState[MAX_KEY_BUFFER];
	BYTE _keyStateTrigger[MAX_KEY_BUFFER];
	BYTE _keyStateRelease[MAX_KEY_BUFFER];

};

#endif