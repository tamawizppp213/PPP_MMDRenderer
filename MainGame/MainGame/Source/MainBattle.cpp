//////////////////////////////////////////////////////////////////////////////////
//              Title:  MainBattle.cpp
//            Content:  Battle Scene
//             Author:  Toide Yutaro
//             Create:  2021_03_13
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Include/MainBattle.hpp"-
#include "GameCore/Include/FrameResources.hpp"
#include <DirectXMath.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define OBJECT_COUNT 1 
#define SCENE_COUNT  1
#define MATERIAL_COUNT 1
#define LIGHT_COUNT  1
#define ACTION_SPEED 10.0f
namespace battle
{
	static SceneConstants sceneParameter;
	static DirectX::XMFLOAT4X4 projView = DirectXMathHelper::Identity4X4();
}

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
using namespace battle;
void MainBattle::Initialize(GameTimer& gameTimer)
{
	InitializeDirectX12(gameTimer);
	InitializeFrameResources();
	InitializeSceneParameters();
	InitializeFader();
	LoadTextures();
	_directX12.CompleteInitialize();
	_directX12.FlushCommandQueue();
}

void MainBattle::Update()
{           
	OnGameInput();
	_frameResource->UpdateSceneConstants(&sceneParameter, &_fpsCamera);

}

void MainBattle::Draw()
{
	_directX12.ClearScreen();
	_skybox.Draw(_frameResource->SceneConstantsBuffer->Resource()->GetGPUVirtualAddress());
	_fader->Draw(*_gameTimer, DirectX::XMLoadFloat4x4(&projView));
	_directX12.CompleteRendering();
}

void MainBattle::Terminate()
{
	delete _frameResource;
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
	Screen screen;
	_fpsCamera.SetLens(0.25f * DirectX::XM_PI, screen.AspectRatio(), 1.0f, 1000.0f);
	_fpsCamera.UpdateViewMatrix();
}

#pragma region Private Function
#pragma region Initialize
/****************************************************************************
*                       InitializeDirectX12
*************************************************************************//**
*  @fn        bool MainBattle::InitializeDirectX12(GameTimer& gameTimer)
*  @brief     Initialize DirectX12
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainBattle::InitializeDirectX12(GameTimer& gameTimer)
{
	_gameTimer   = &gameTimer;
	_device      = _directX12.GetDevice();
	_commandList = _directX12.GetCommandList();
	_directX12.ResetCommandList();
	return true;
}

/****************************************************************************
*                       InitializeFrameResources
*************************************************************************//**
*  @fn        bool MainBattle::InitializeFrameResources()
*  @brief     Initialize frame resources
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainBattle::InitializeFrameResources()
{
	/*-------------------------------------------------------------------
	-           Set Resource Counter
	---------------------------------------------------------------------*/
	FrameResourceCounter resourceCounter;
	resourceCounter.MaxObjectCount     = OBJECT_COUNT;
	resourceCounter.MaxSceneCount      = SCENE_COUNT;
	resourceCounter.MaxMaterialCount   = MATERIAL_COUNT;
	resourceCounter.MaxSceneLightCount = LIGHT_COUNT;

	/*-------------------------------------------------------------------
	-           Create FrameResource
	---------------------------------------------------------------------*/
	_frameResource = new FrameResource(resourceCounter, *_gameTimer);
	return true;
}
/****************************************************************************
*                       InitializeSceneParameters
*************************************************************************//**
*  @fn        bool MainBattle::InitializeSceneParameters()
*  @brief     Initialize sceneParameters (O(screen center Position), +x: screenWidth / 2, +y: screenHeight / 2)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainBattle::InitializeSceneParameters()
{
	/*-------------------------------------------------------------------
	-           Set Scene Parameter
	---------------------------------------------------------------------*/
	_fpsCamera.SetPosition(0.0f, 2.0f, -15.0f);
	SceneConstants sceneParameter;
	_frameResource->UpdateSceneConstants(&sceneParameter, &_fpsCamera);

	/*-------------------------------------------------------------------
	-           Copy to sceneConstantBuffer
	---------------------------------------------------------------------*/
	auto scene = _frameResource->SceneConstantsBuffer.get();
	scene->CopyStart();
	scene->CopyData(0, sceneParameter);
	scene->CopyEnd();
	return true;
}

/****************************************************************************
*                       InitializeFader
*************************************************************************//**
*  @fn        bool MainBattle::InitializeFader()
*  @brief     Initialize Fader
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainBattle::InitializeFader()
{
	_fader = new Fader();
	_fader->Initialize();
	_fader->StartFadeIn(1.0f, 2.0f);

	Screen screen;
	_fader->SetSpriteSize((float)screen.GetScreenWidth(), (float)screen.GetScreenHeight());
	return true;
}

/****************************************************************************
*                       LoadTextures
*************************************************************************//**
*  @fn        bool MainBattle::LoadTextures()
*  @brief     Load Texture
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainBattle::LoadTextures()
{
	_skybox.Initialize(L"Resources/Texture/grasscube1024.dds");
	return true;
}
#pragma endregion Initialize
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
		dPosition.x = ACTION_SPEED * deltaTime * leftStick.XAxis * 0.01f;
		dPosition.y = 0.0f;
		dPosition.z = ACTION_SPEED* deltaTime, leftStick.YAxis * 0.01f;
		_fpsCamera.Walk(dPosition.z);
		_fpsCamera.Strafe(dPosition.x);
	}

	/*-------------------------------------------------------------------
	-           GamePad Input RStick
	---------------------------------------------------------------------*/
	if (_gameInput.GetGamePad().IsPressRStick())
	{
		StickValue rightStick = _gameInput.GetGamePad().RStick();

		DirectX::XMFLOAT2 dRotation;
		dRotation.x = 0.25f * deltaTime * rightStick.XAxis * 0.01f;
		dRotation.y = 0.25f * deltaTime * rightStick.YAxis * 0.01f;
		_fpsCamera.RotateWorldY(dRotation.x);
		//_fpsCamera.RotatePitch(dRotation.y);
	}

}
#pragma endregion Input

#pragma region ConstantBuffer

#pragma endregion Constant Buffer
#pragma endregion Private Function