//////////////////////////////////////////////////////////////////////////////////
//              Title:  MainBattle.cpp
//            Content:  Battle Scene
//             Author:  Toide Yutaro
//             Create:  2021_03_13
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Include/MainBattle.hpp"
#include <DirectXMath.h>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define ACTION_SPEED 10.0f


//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
void MainBattle::Initialize(GameTimer& gameTimer)
{
	_gameTimer   = &gameTimer;
	_device      = _directX12.GetDevice();
	_commandList = _directX12.GetCommandList();

}

void MainBattle::Update()
{
	OnGameInput();
}

void MainBattle::Draw()
{
	_directX12.ClearScreen();

	_directX12.CompleteRendering();
}

void MainBattle::Terminate()
{
	
}

/****************************************************************************
*                          OnGameInput
*************************************************************************//**
*  @fn        void MainBattle::OnGameInput()
*  @brief     GameInput (Keyboard, Mouse, GamePad)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainBattle::OnGameInput()
{
	OnKeyboardInput();
	OnMouseInput();
	OnGamePadInput();
	_fpsCamera.UpdateViewMatrix();
}

#pragma region Private Function
#pragma region Input
/****************************************************************************
*                          OnKeyboardInput
*************************************************************************//**
*  @fn        void MainBattle::OnKeyboardInput()
*  @brief     Keyboard Input
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainBattle::OnKeyboardInput()
{
	const float deltaTime = _gameTimer->DeltaTime();
	/*-------------------------------------------------------------------
	-           Keyboad Input W
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsPress('w'))
	{
		_fpsCamera.Walk(ACTION_SPEED * deltaTime);
	}
	
}

/****************************************************************************
*                          OnMouseInput
*************************************************************************//**
*  @fn        void MainBattle::OnMouseInput()
*  @brief     Mouse Input
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainBattle::OnMouseInput()
{
	/*-------------------------------------------------------------------
	-           Mouse Input Left Button
	---------------------------------------------------------------------*/
	if (_gameInput.GetMouse().IsPress(MouseButton::LEFT))
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(_gameInput.GetMouse().GetMouseVelocity().x)); // 0.25f: 感度設定したい
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(_gameInput.GetMouse().GetMouseVelocity().y));

		_fpsCamera.RotatePitch(dy);
		_fpsCamera.RotateWorldY(dx);
	}
}

/****************************************************************************
*                          OnGamePadInput
*************************************************************************//**
*  @fn        void MainBattle::OnGamePadInput()
*  @brief     GamePad Input
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainBattle::OnGamePadInput()
{
	const float deltaTime = _gameTimer->DeltaTime();
	/*-------------------------------------------------------------------
	-           GamePad Input LStick
	---------------------------------------------------------------------*/
	if (_gameInput.GetGamePad().IsPressLStick())
	{
		StickValue leftStick = _gameInput.GetGamePad().LStick();

		DirectX::XMFLOAT3 dPosition;
		dPosition.x = ACTION_SPEED * deltaTime * leftStick.XAxis;
		dPosition.y = 0.0f;
		dPosition.z = ACTION_SPEED* deltaTime, leftStick.YAxis;
		
		_fpsCamera.SetPosition(dPosition);
	}

	/*-------------------------------------------------------------------
	-           GamePad Input RStick
	---------------------------------------------------------------------*/
	if (_gameInput.GetGamePad().IsPressRStick())
	{
		StickValue rightStick = _gameInput.GetGamePad().RStick();

		DirectX::XMFLOAT2 dRotation;
		dRotation.x = 0.25f * deltaTime * rightStick.XAxis; 
		dRotation.y = 0.25f * deltaTime * rightStick.YAxis;

		_fpsCamera.RotateWorldY(dRotation.x);
		_fpsCamera.RotatePitch(dRotation.y);
	}
}
#pragma endregion Input
#pragma region ConstantBuffer

#pragma endregion Constant Buffer
#pragma endregion Private Function