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
#include "GameCore/Include/FrameResources.hpp"
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "GameCore/Include/Effect/Bloom.hpp"
#include "GameMath/Include/GMDistribution.hpp"
#include "GameCore/Include/Sprite/Text.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"

using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define OBJECT_COUNT 1 
#define SCENE_COUNT  1
#define MATERIAL_COUNT 1
#define LIGHT_COUNT  1
#define ACTION_SPEED 15.0f

namespace battle
{
	static SceneConstants sceneParameter;
	static gm::Matrix4 projView = gm::MatrixIdentity();
	static Bloom effect;
	static PostEffect postEffect;
	static AudioSource audio;
	static bool _isStart = false;
}

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
using namespace battle;

MainBattle::MainBattle()
{
	_stage  = std::make_unique<PMXModel>();
	_skybox = std::make_unique<Skybox>();
	_miku   = std::make_unique<PMXModel>();
	
}

MainBattle::~MainBattle() 
{

};
void MainBattle::Initialize(GameTimer& gameTimer)
{
	InitializeDirectX12(gameTimer);
	InitializeFrameResources();
	InitializeSceneParameters();
	InitializeFader();
	InitializeModels(gameTimer);
	LoadSkybox();
	LoadStage();
	InitializeRenderingEngine();
	effect.Initialize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
	audio.LoadSound(L"Resources/Audio/oddsends.wav", SoundType::BGM);
	
	_directX12.CompleteInitialize();

}

void MainBattle::Update()
{   
	if (!_isStart) { _isStart = true; audio.Play(); }
	OnGameInput();

	_frameResource->UpdateSceneConstants(&sceneParameter, &_fpsCamera);
	_miku.get()->Update();
	_stage.get()->Update();

	
}

void MainBattle::Draw()
{
	_directX12.ClearScreen();
	
	_skybox.get()->Draw(_frameResource->SceneConstantsBuffer->Resource()->GetGPUVirtualAddress());
	_renderingEngine.Draw();

	effect.OnResize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
	effect.Draw();


	//_fader.get()->Draw(*_gameTimer, projView);
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
	
	_fpsCamera.SetLens(0.25f * DirectX::XM_PI, _screen.AspectRatio(), 1.0f, 1000.0f);
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
	//_directX12.ResetCommandList();
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
	
	_fpsCamera.SetPosition(0.0f, 10.0f, -20.0f);
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
	auto fader = _fader.get();
	fader->Initialize();
	fader->StartFadeIn(1.0f, 2.0f);
	fader->SetSpriteSize((float)_screen.GetScreenWidth(), (float)_screen.GetScreenHeight());
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
	_spriteRenderer.get()->Initialize();
	return true;
}

/****************************************************************************
*                       LoadSkybox
*************************************************************************//**
*  @fn        bool MainBattle::LoadSkybox()
*  @brief     Load skybox
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainBattle::LoadSkybox()
{
	_skybox.get()->Initialize(L"Resources/Texture/grasscube1024.dds");
	return true;
}

bool MainBattle::LoadStage()
{
	_stage.get()->Initialize(L"Resources/Model/Cyber/Cyber.pmx");
	_stage.get()->SetPosition(0, 0, 0);
	return true;
}

bool MainBattle::InitializeModels(GameTimer& gameTimer)
{
	_miku.get()->Initialize(L"Resources/Model/YYB Hatsune Miku/YYB Hatsune Miku_10th_v1.02_toonchange.pmx");
	_miku.get()->AddMotion(L"Resources/Model/oddsends/oddsends.vmd", L"pose");
	_miku.get()->SetWorldTimer(gameTimer);
	_miku.get()->StartAnimation(L"pose");
	_miku.get()->EnableAmbientOcculusionMap(true);
	_miku.get()->SetPosition(0, 0, 10);
	return true;
}

bool MainBattle::InitializeRenderingEngine()
{
	_renderingEngine.SetSceneGPUAddress(_frameResource->SceneConstantsBuffer.get()->Resource()->GetGPUVirtualAddress());
	_renderingEngine.AddObjectToForwardRenderer(*_stage.get());
	_renderingEngine.AddObjectToForwardRenderer(*_miku.get());
	_renderingEngine.AddObjectToZPrepass(*_stage.get());
	_renderingEngine.AddObjectToZPrepass(*_miku.get());
	_renderingEngine.AddObjectToGBuffer(*_stage.get());
	_renderingEngine.AddObjectToGBuffer(*_miku.get());
	_renderingEngine.AddObjectToShadowMap(*_miku.get(), 0);
	InitializeLights();
	return true;
}

bool MainBattle::InitializeLights()
{
	_renderingEngine.SetDirectionalLight(0, DirectionalLight(Float3(1.0f, -1.0f, 1), 1.2f, Float3(1.0f, 0.85f, 0.85f)));
	_renderingEngine.SetDirectionalLight(1, DirectionalLight(Float3(-1.0f, -1.0f, 1), 1.2f, Float3(1.0f, 0.85f, 0.85f)));
	
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
	if (_gameInput.GetKeyboard().IsPress(DIK_W))
	{
		_fpsCamera.Walk(ACTION_SPEED * deltaTime);
	}

	if (_gameInput.GetKeyboard().IsPress(DIK_S))
	{
		_fpsCamera.Walk(-ACTION_SPEED * deltaTime);
	}
	
	if (_gameInput.GetKeyboard().IsPress(DIK_D))
	{
		_fpsCamera.Strafe(ACTION_SPEED * deltaTime);
	}

	if (_gameInput.GetKeyboard().IsPress(DIK_A))
	{
		_fpsCamera.Strafe(-ACTION_SPEED * deltaTime);
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
		dRotation.x = 0.25f * deltaTime * rightStick.XAxis * 0.10f ;
		dRotation.y = -0.25f * deltaTime * rightStick.YAxis * 0.10f;
		_fpsCamera.RotateWorldY(dRotation.x);
		_fpsCamera.RotatePitch(dRotation.y);
	}

}
#pragma endregion Input

#pragma region ConstantBuffer

#pragma endregion Constant Buffer
#pragma endregion Private Function