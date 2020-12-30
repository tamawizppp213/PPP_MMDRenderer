//////////////////////////////////////////////////////////////////////////////////
///             @file   GamePad.hppÅ@(current 1Player only)
///             @brief  GamePad
///             @author Toide Yutaro
///             @date   2020_12_29
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef GAMEPAD_HPP
#define GAMEPAD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <Xinput.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#pragma region GamePad
/****************************************************************************
*				  			Gamepad
*************************************************************************//**
*  @class     GamePad
*  @brief     GamePad
*****************************************************************************/
class GamePad
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize();
	void Update();
	void Finalize();

#pragma region XINPUT
	void EnableXInput(bool isEnabled);
#pragma endregion XINPUT

#pragma region BUTTON
	bool IsPressButton  (int button);
	bool IsTriggerButton(int button);
	bool IsReleaseButton(int button);
	bool IsPressLeftButton   ();
	bool IsTriggerLeftButton ();
	bool IsReleaseLeftButton ();
	bool IsPressRightButton  (); 
	bool IsTriggerRightButton();
	bool IsReleaseRightButton();
	int LeftButtonValue(); // 0 Å` 100
	int RightButtonValue();// 0 Å` 100
#pragma endregion BUTTON

#pragma region STICK
	bool IsPressLStick();
	bool IsPressRStick();
	bool IsPressLStick(float threshold);
	bool IsPressRStick(float threshold);
	float LStick_X(); // 0 Å` 100
	float LStick_Y(); // 0 Å` 100
	float RStick_X(); // 0 Å` 100 
	float RStick_Y(); // 0 Å` 100
#pragma endregion STICK

#pragma region VIBRATION
	void SetVibration(float leftMotor, float rightMotor);
#pragma endregion VIBRATION

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static GamePad& Instance()
	{
		static GamePad gamePad;
		return gamePad;
	}
	GamePad(const GamePad&)            = delete;
	GamePad& operator=(const GamePad&) = delete;
	GamePad(GamePad&&)                 = delete;
	GamePad& operator=(GamePad&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	GamePad();
	~GamePad() = default;


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	XINPUT_STATE _currentState;
	XINPUT_STATE _previousState;
	XINPUT_VIBRATION _vibrationState;
	
};

#endif
