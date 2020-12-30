//////////////////////////////////////////////////////////////////////////////////
///             @file   GamePad.cpp
///             @brief  GamePad
///             @author Toide Yutaro
///             @date   2020_12_29
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Input/GamePad.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define SHORT_PLUS_MAX  32767.0f
#define SHORT_MINUS_MAX 32768.0f
#define USHORT_MAX 65535.0f
#pragma warning(disable : 4995)

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
GamePad::GamePad()
{
	_currentState.dwPacketNumber        = 0;
	_currentState.Gamepad.sThumbLX      = 0;
	_currentState.Gamepad.sThumbLY      = 0;
	_currentState.Gamepad.sThumbRX      = 0;
	_currentState.Gamepad.sThumbRY      = 0;
	_currentState.Gamepad.bLeftTrigger  = 0;
	_currentState.Gamepad.bRightTrigger = 0;
	_currentState.Gamepad.wButtons      = 0;

	_previousState.dwPacketNumber        = 0;
	_previousState.Gamepad.sThumbLX      = 0;
	_previousState.Gamepad.sThumbLY      = 0;
	_previousState.Gamepad.sThumbRX      = 0;
	_previousState.Gamepad.sThumbRY      = 0;
	_previousState.Gamepad.bLeftTrigger  = 0;
	_previousState.Gamepad.bRightTrigger = 0;
	_previousState.Gamepad.wButtons      = 0;

	_vibrationState.wLeftMotorSpeed  = 0;
	_vibrationState.wRightMotorSpeed = 0;

	ZeroMemory(&_currentState,   sizeof(XINPUT_STATE));
	ZeroMemory(&_previousState,  sizeof(XINPUT_STATE));
	ZeroMemory(&_vibrationState, sizeof(XINPUT_VIBRATION));
}
/****************************************************************************
*							Initialize
*************************************************************************//**
*  @fn        bool GamePad::Initialize()
*  @brief     GamePad Initialize
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::Initialize()
{
	DWORD dwResult = XInputGetState(0, &_currentState);

	if (dwResult != ERROR_SUCCESS)
	{
		MessageBox(NULL, L"XInput can't create", L"Warning", MB_ICONWARNING);
		return false;
	}

	XInputGetState(0, &_previousState);
	return true;
}

/****************************************************************************
*							Update
*************************************************************************//**
*  @fn        bool GamePad::Update()
*  @brief     GamePad Check Input
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void GamePad::Update()
{
	_previousState = _currentState;
	XInputGetState(0, &_currentState);
}

/****************************************************************************
*							Finalize
*************************************************************************//**
*  @fn        bool GamePad::Finalize()
*  @brief     GamePad Finalize
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void GamePad::Finalize()
{

}

/****************************************************************************
*							IsEnabledXInput
*************************************************************************//**
*  @fn        void GamePad::IsEnabledXInput(bool isEnabled)
*  @brief     Enable XInput (for vibration)
*  @param[in] bool isEnabled
*  @return 　　void
*****************************************************************************/
void GamePad::EnableXInput(bool isEnabled)
{
	XInputEnable(isEnabled);
}

/****************************************************************************
*							IsPressButton
*************************************************************************//**
*  @fn        void GamePad::IsPressButton()
*  @brief     Detect pressing button
*  @param[in] int button
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressButton(int button)
{
	return (_currentState.Gamepad.wButtons & button) ? true : false;
}

/****************************************************************************
*							IsTriggerButton
*************************************************************************//**
*  @fn        void GamePad::IsTriggerButton()
*  @brief     Detect trigger button
*  @param[in] int button
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsTriggerButton(int button)
{
	if (!(_previousState.Gamepad.wButtons & button) &&
		(_currentState.Gamepad.wButtons & button))
	{
		return true;
	}
	return false;
}

/****************************************************************************
*							IsReleaseButton
*************************************************************************//**
*  @fn        void GamePad::IsReleaseButton()
*  @brief     Detect release button
*  @param[in] int button
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsReleaseButton(int button)
{
	if ((_previousState.Gamepad.wButtons & button) &&
		!(_currentState.Gamepad.wButtons & button))
	{
		return true;
	}
	return false;
}

/****************************************************************************
*							IsPressLeftButton
*************************************************************************//**
*  @fn        bool GamePad::IsPressLeftButton()
*  @brief     Detect press Left button
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressLeftButton()
{
	BYTE axis = _currentState.Gamepad.bLeftTrigger;
	return (axis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;
}

/****************************************************************************
*							IsTriggerLeftButton
*************************************************************************//**
*  @fn        bool GamePad::IsTriggerLeftButton()
*  @brief     Detect trigger Left button
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsTriggerLeftButton()
{
	BYTE previousAxis    = _previousState.Gamepad.bLeftTrigger;
	BYTE currentAxis     = _currentState .Gamepad.bLeftTrigger;
	bool previousTrigger = (previousAxis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;
	bool currentTrigger  = (currentAxis  > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;

	if ((!previousTrigger) && (currentTrigger))
	{
		return true;
	}
	return false;
}

/****************************************************************************
*							IsReleaseLeftButton
*************************************************************************//**
*  @fn        bool GamePad::IsReleaseLeftButton()
*  @brief     Detect release Left button
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsReleaseLeftButton()
{
	BYTE previousAxis    = _previousState.Gamepad.bLeftTrigger;
	BYTE currentAxis     = _currentState.Gamepad.bLeftTrigger;
	bool previousTrigger = (previousAxis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;
	bool currentTrigger  = (currentAxis  > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;

	if ((previousTrigger) && (!currentTrigger))
	{
		return true;
	}
	return false;
}

/****************************************************************************
*							IsPressRightButton
*************************************************************************//**
*  @fn        bool GamePad::IsPressRightButton()
*  @brief     Detect press Right button
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressRightButton()
{
	BYTE axis = _currentState.Gamepad.bRightTrigger;
	return (axis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;
}

/****************************************************************************
*							IsTriggerRightButton
*************************************************************************//**
*  @fn        bool GamePad::IsTriggerRightButton()
*  @brief     Detect trigger Right button
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsTriggerRightButton()
{
	BYTE previousAxis    = _previousState.Gamepad.bRightTrigger;
	BYTE currentAxis     = _currentState.Gamepad.bRightTrigger;
	bool previousTrigger = (previousAxis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;
	bool currentTrigger  = (currentAxis  > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;

	if ((!previousTrigger) && (currentTrigger))
	{
		return true;
	}
	return false;
}

/****************************************************************************
*							IsReleaseRightButton
*************************************************************************//**
*  @fn        bool GamePad::IsReleaseRightButton()
*  @brief     Detect release Right button
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsReleaseRightButton()
{
	BYTE previousAxis    = _previousState.Gamepad.bRightTrigger;
	BYTE currentAxis     = _currentState .Gamepad.bRightTrigger;
	bool previousTrigger = (previousAxis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;
	bool currentTrigger  = (currentAxis  > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true : false;

	if ((previousTrigger) && (!currentTrigger))
	{
		return true;
	}
	return false;
}

/****************************************************************************
*							LeftButtonValue
*************************************************************************//**
*  @fn        bool GamePad::LeftButtonValue()
*  @brief     Return Left Button Value(Threshold ～ 100) : not pressing 0
*  @param[in] void
*  @return 　　float
*****************************************************************************/
int GamePad::LeftButtonValue()
{
	BYTE axis = _currentState.Gamepad.bLeftTrigger;
	if (axis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		float result = axis * 100.0f / 255.0f;
		return static_cast<int>(result);
	}
	return 0;
}

/****************************************************************************
*							RightButtonValue
*************************************************************************//**
*  @fn        bool GamePad::RightButtonValue()
*  @brief     Return Right Button Value(Threshold ～ 100) : not pressing 0
*  @param[in] void
*  @return 　　float
*****************************************************************************/
int GamePad::RightButtonValue()
{
	BYTE axis = _currentState.Gamepad.bRightTrigger;
	if (axis > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		float result = axis * 100.0f / 255.0f;
		return static_cast<int>(result);
	}
	return 0;
}

/****************************************************************************
*							IsPressLStick
*************************************************************************//**
*  @fn        bool GamePad::IsPressLStick()
*  @brief     Detect press LStick
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressLStick()
{
	short x = _currentState.Gamepad.sThumbLX;
	short y = _currentState.Gamepad.sThumbLY;

	if ((x > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ||
		(x <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		return false;
	}

	if ((y > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ||
		(y <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		return false;
	}

	return true;
}

/****************************************************************************
*							IsPressRStick
*************************************************************************//**
*  @fn        bool GamePad::IsPressRStick()
*  @brief     Detect press RStick
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressRStick()
{
	short x = _currentState.Gamepad.sThumbRX;
	short y = _currentState.Gamepad.sThumbRY;

	if ((x > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ||
		(x <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		return false;
	}

	if ((y > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ||
		(y <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		return false;
	}

	return true;
}

/****************************************************************************
*							IsPressLStick
*************************************************************************//**
*  @fn        bool GamePad::IsPressLStick(float threshold)
*  @brief     Detect press LStick
*  @param[in] float threshold(0.0～100.0f)
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressLStick(float threshold)
{
	short x = _currentState.Gamepad.sThumbLX;
	short y = _currentState.Gamepad.sThumbLY;
	short deadZone = static_cast<short>(threshold / 100.0f * SHORT_PLUS_MAX);

	if ((-deadZone < x) || (x < deadZone))
	{
		return false;
	}

	if ((-deadZone < y) || (y < deadZone))
	{
		return false;
	}

	return true;
}

/****************************************************************************
*							IsPressRStick
*************************************************************************//**
*  @fn        bool GamePad::IsPressRStick(int threshold)
*  @brief     Detect press RStick
*  @param[in] float threshold(0.0～100.0f)
*  @return 　　bool
*****************************************************************************/
bool GamePad::IsPressRStick(float threshold)
{
	short x = _currentState.Gamepad.sThumbLX;
	short y = _currentState.Gamepad.sThumbLY;
	short deadZone = static_cast<short>(threshold / 100.0f * SHORT_PLUS_MAX);

	if ((-deadZone < x) || (x < deadZone))
	{
		return false;
	}

	if ((-deadZone < y) || (y < deadZone))
	{
		return false;
	}

	return true;
}

/****************************************************************************
*							LStick_X
*************************************************************************//**
*  @fn        float GamePad::LStick_X()
*  @brief     Return LStick_X Value (0.0f ～ 100.0f)
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float GamePad::LStick_X()
{
	short  y      = _currentState.Gamepad.sThumbLX;
	bool   isPlus = (y >= 0) ? true : false;
	float  result = isPlus ? (static_cast<float>(y) / SHORT_PLUS_MAX) * 100.0f : (static_cast<float>(y) / SHORT_MINUS_MAX) * 100.0f;
	return result;
}

/****************************************************************************
*							LStick_Y
*************************************************************************//**
*  @fn        float GamePad::LStick_Y()
*  @brief     Return LStick_Y Value (0.0f ～ 100.0f)
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float GamePad::LStick_Y()
{
	short  y      = _currentState.Gamepad.sThumbLY;
	bool   isPlus = (y >= 0) ? true : false;
	float  result = isPlus ? (static_cast<float>(y) / SHORT_PLUS_MAX) * 100.0f : (static_cast<float>(y) / SHORT_MINUS_MAX) * 100.0f;
	return result;
}

/****************************************************************************
*							RStick_X
*************************************************************************//**
*  @fn        float GamePad::RStick_X()
*  @brief     Return RStick_X Value (0.0f ～ 100.0f)
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float GamePad::RStick_X()
{
	short  y = _currentState.Gamepad.sThumbRX;
	bool   isPlus = (y >= 0) ? true : false;
	float  result = isPlus ? (static_cast<float>(y) / SHORT_PLUS_MAX) * 100.0f : (static_cast<float>(y) / SHORT_MINUS_MAX) * 100.0f;
	return result;
}

/****************************************************************************
*							RStick_Y
*************************************************************************//**
*  @fn        float GamePad::RStick_Y()
*  @brief     Return RStick_Y Value (0.0f ～ 100.0f)
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float GamePad::RStick_Y()
{
	short  y      = _currentState.Gamepad.sThumbRY;
	bool   isPlus = (y >= 0) ? true : false;
	float  result = isPlus ? (static_cast<float>(y) / SHORT_PLUS_MAX) * 100.0f : (static_cast<float>(y) / SHORT_MINUS_MAX) * 100.0f; 
	return result;
}

/****************************************************************************
*							SetVibration
*************************************************************************//**
*  @fn        void GamePad::SetVibration(float leftMotor, float rightMotor)
*  @brief     Set Motor Vibration(0.0f ～ 1.0f)
*  @param[in] void
*  @return 　　float
*****************************************************************************/
void GamePad::SetVibration(float leftMotor, float rightMotor)
{
	if (leftMotor  > 1.0f) { leftMotor  = 1.0f; }
	if (leftMotor  < 0.0f) { leftMotor  = 0.0f; }
	if (rightMotor > 1.0f) { rightMotor = 1.0f; }
	if (rightMotor < 0.0f) { rightMotor = 0.0f; }

	leftMotor  = leftMotor  * USHORT_MAX;
	rightMotor = rightMotor * USHORT_MAX;

	_vibrationState.wLeftMotorSpeed  = static_cast<unsigned short>(leftMotor);
	_vibrationState.wRightMotorSpeed = static_cast<unsigned short>(rightMotor);

	XInputSetState(0, &_vibrationState);
}
#pragma endregion Public Function

#pragma region Private Function
#pragma endregion Private Function