//////////////////////////////////////////////////////////////////////////////////
///             @file   TemplateText.hpp
///             @brief  TemplateText
///             @author Toide Yutaro
///             @date   2020_12_28
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MOUSE_HPP
#define MOUSE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <queue>
#include <dinput.h>
#include <Windows.h>
#include <DirectXMath.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                           Mouse, MouseEvent Class
//////////////////////////////////////////////////////////////////////////////////
struct MousePosition : POINT
{

};

enum MouseButton
{
	LEFT  = 0,
	RIGHT = 1,
	WHEEL = 2,
	TOTAL_COUNT = 3
};
#pragma region Mouse
/****************************************************************************
*				  			MouseEvent Class
*************************************************************************//**
*  @class     Mouse
*  @brief     Mouse
*****************************************************************************/
class Mouse
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(LPDIRECTINPUT8 dInput, HINSTANCE hInstance, HWND hwnd);
	void Update();
	void Finalize();
	bool IsPress  (int mouseButton);
	bool IsTrigger(int mouseButton);
	bool IsRelease(int mouseButton);
	MousePosition& GetMousePosition();
	DirectX::XMFLOAT2 GetMouseVelocity();
	int GetMousePosition_X();
	int GetMousePosition_Y();
	

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static Mouse& Instance()
	{
		static Mouse mouse;
		return mouse;
	}
	Mouse(const Mouse&)            = delete;
	Mouse& operator=(const Mouse&) = delete;
	Mouse(Mouse&&)                 = delete;
	Mouse& operator=(Mouse&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	Mouse();
	~Mouse() = default;

	bool CreateHWND();
	bool CreateHInstance();
	bool CreateMouseDevice();
	bool CreateDataFormat();
	bool CreateCooperativeLevel();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	LPDIRECTINPUT8       _dInput    = nullptr;
	LPDIRECTINPUTDEVICE8 _mouse     = nullptr;
	HINSTANCE            _hInstance = nullptr;
	HWND                 _hwnd      = nullptr;
	DIMOUSESTATE2        _currentMouseState;
	DIMOUSESTATE2        _previousMouseState;
	MousePosition        _mousePosition;
};

#pragma endregion
#endif
