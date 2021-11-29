//////////////////////////////////////////////////////////////////////////////////
//              Title:  MainBattle.cpp
//            Content:  Battle Scene
//             Author:  Toide Yutaro
//             Create:  2021_03_13
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/MMDRender/Include/MainRenderScene.hpp"
#include "GameCore/Include/FrameResources.hpp"
#include "MainGame/MMDRender/Include/Miku.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "GameCore/Include/Effect/Bloom.hpp"
#include "GameMath/Include/GMDistribution.hpp"
#include "GameCore/Include/Sprite/Text.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include <iostream>
using namespace gm;

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define OBJECT_COUNT 1 
#define SCENE_COUNT  1
#define MATERIAL_COUNT 1
#define LIGHT_COUNT  1
#define ACTION_SPEED 15.0f

namespace render
{
	static SceneConstants sceneParameter;
	static gm::Matrix4 projView = gm::MatrixIdentity();
	static Bloom bloom;
	static std::vector<PostEffect> postEffects(5);
	static AudioSource audio;
	static bool _isStart = false;
}

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
using namespace render;

MainRenderScene::MainRenderScene()
{
	_stage  = std::make_unique<PMXModel>();
	_skybox = std::make_unique<Skybox>();
	_miku   = std::make_unique<Miku>();
	
}

MainRenderScene::~MainRenderScene()
{

};
void MainRenderScene::Initialize(GameTimer& gameTimer)
{
	Scene::Initialize(gameTimer);
	InitializeFrameResources();
	InitializeSceneParameters();
	InitializeRenderingEngine();
	bloom.Initialize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
	for (int i = 0; i < postEffects.size(); ++i) { postEffects[i].Initialize((PostEffectBlendStateType)i); }

	_directX12.CompleteInitialize();
	
}

void MainRenderScene::Update()
{   
	OnGameInput();

	_frameResource->UpdateSceneConstants(&sceneParameter, &_fpsCamera);
	_miku.get()->Update();
	_stage.get()->Update();

	
}

void MainRenderScene::Draw()
{
	_directX12.ClearScreen();
    
	/*-------------------------------------------------------------------
	-           Draw skybox
	---------------------------------------------------------------------*/
	_skybox.get()->Draw(_frameResource->SceneConstantsBuffer->Resource()->GetGPUVirtualAddress());
	/*-------------------------------------------------------------------
	-           Execute Rendering Engine Pipeline
	---------------------------------------------------------------------*/
	_renderingEngine.Draw();
	/*-------------------------------------------------------------------
	-           PostEffect
	---------------------------------------------------------------------*/
	bloom.OnResize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
	bloom.Draw();

	//_fader.get()->Draw(*_gameTimer, projView);
	/*-------------------------------------------------------------------
	-           Explain Text
	---------------------------------------------------------------------*/
	DrawExplainText();

	if (_postEffectIndex != (int)PostEffectBlendStateType::None)
	{
		postEffects[_postEffectIndex].OnResize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
		postEffects[_postEffectIndex].Draw();
	}


	_directX12.CompleteRendering();
}

void MainRenderScene::Terminate()
{
	delete _frameResource;
	_renderingEngine.ClearAllRenderingPass();
}

/****************************************************************************
*                          OnGameInput
*************************************************************************//**
*  @fn        void MainBattle::OnGameInput()
*  @brief     GameInput (Keyboard, Mouse, GamePad)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainRenderScene::OnGameInput()
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
*                       InitializeFrameResources
*************************************************************************//**
*  @fn        bool MainBattle::InitializeFrameResources()
*  @brief     Initialize frame resources
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainRenderScene::InitializeFrameResources()
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
bool MainRenderScene::InitializeSceneParameters()
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
*                       LoadMaterials
*************************************************************************//**
*  @fn        bool MainRenderScene::LoadMaterials(GameTimer& gameTimer)
*  @brief     Load All Scene Material 
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainRenderScene::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           UI Texture
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/Default/White.png", _whiteTexture);

	/*-------------------------------------------------------------------
	-           Skybox
	---------------------------------------------------------------------*/
	if (!_skybox.get()->Initialize(L"Resources/Texture/grasscube1024.dds")) { return false; };

	/*-------------------------------------------------------------------
	-           Stage
	---------------------------------------------------------------------*/
	if (!_stage.get()->Initialize(L"Resources/Model/Cyber/Cyber.pmx")) { return false; };
	_stage.get()->SetPosition(0, 0, 0);

	/*-------------------------------------------------------------------
	-           Miku
	---------------------------------------------------------------------*/
	_miku.get()->Initialize(L"Resources/Model/YYB Hatsune Miku/YYB Hatsune Miku_10th_v1.02_toonchange.pmx");
	_miku.get()->AddMotion(L"Resources/Model/oddsends/oddsends.vmd", L"pose");
	_miku.get()->SetWorldTimer(gameTimer);;
	_miku.get()->EnableAmbientOcculusionMap(true);
	_miku.get()->SetPosition(0, 0, 10);
	return true;
}

bool MainRenderScene::InitializeRenderingEngine()
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

bool MainRenderScene::InitializeLights()
{
	_renderingEngine.SetDirectionalLight(0, DirectionalLight(Float3(1.0f, -1.0f, 1), 1.15f, Float3(1.0f, 0.9f, 0.9f)));
	_renderingEngine.SetDirectionalLight(1, DirectionalLight(Float3(-1.0f, -1.0f, 1), 1.15f, Float3(1.0f, 0.9f, 0.9f)));
	_renderingEngine.SetDirectionalLight(2, DirectionalLight(Float3(-1.0f, -1.0f, -1), 1.15f, Float3(1.0f, 0.9f, 0.9f)));
	_renderingEngine.SetDirectionalLight(3, DirectionalLight(Float3(1.0f, -1.0f, -1), 1.15f, Float3(1.0f, 0.9f, 0.9f)));
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
void MainRenderScene::OnKeyboardInput()
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

	if (_gameInput.GetKeyboard().IsTrigger(DIK_T))
	{
		_enabledExplainText = _enabledExplainText ? false : true;
	}
	
	if (_gameInput.GetKeyboard().IsTrigger(DIK_P))
	{
		_postEffectIndex = (_postEffectIndex + 1) % postEffects.size();
	}

	if (_gameInput.GetKeyboard().IsTrigger(DIK_O))
	{
		audio.LoadSound(L"Resources/Audio/oddsends.wav", SoundType::BGM);
		audio.Play();
		_miku.get()->StartAnimation(L"pose");
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
void MainRenderScene::OnMouseInput()
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
void MainRenderScene::OnGamePadInput()
{
	const float deltaTime = _gameTimer->DeltaTime();
	/*-------------------------------------------------------------------
	-           GamePad Input LStick
	---------------------------------------------------------------------*/
	if (_gameInput.GetGamePad().IsPressLStick())
	{
		StickValue leftStick = _gameInput.GetGamePad().LStick();
		Float3 dPosition;
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

		Float2 dRotation;
		dRotation.x = 0.25f * deltaTime * rightStick.XAxis * 0.10f ;
		dRotation.y = -0.25f * deltaTime * rightStick.YAxis * 0.10f;
		_fpsCamera.RotateWorldY(dRotation.x);
		_fpsCamera.RotatePitch(dRotation.y);
	}

}


#pragma endregion Input

#pragma region DrawFunction
void MainRenderScene::DrawExplainText()
{
	

	/*-------------------------------------------------------------------
	-           DrawIntroductionText
	---------------------------------------------------------------------*/
	if (_enabledExplainText)
	{
	
		/*-------------------------------------------------------------------
		-           Draw Text
		---------------------------------------------------------------------*/
		float pixelWidth  = Screen::GetOnePixelWidth();
		float pixelHeight = Screen::GetOnePixelHeight();
		Float2 charSize   = Float2(30 * pixelWidth, 45 * pixelHeight);
		/*-------------------------------------------------------------------
		-           Draw Back Texture
		---------------------------------------------------------------------*/
		std::vector<Sprite> sprites;
		_backSprite.CreateSpriteForColor(Float3(0.8f, 0.5f, 0.1f), Float2(0.4f, 1.0f), Float4(0.0f, 0.0f, 0.0f, 1.0f));
		sprites.push_back(_backSprite);
		_renderingEngine.DrawSpriteStart();
		_renderingEngine.DrawSprites(sprites, _whiteTexture);
		_renderingEngine.DrawSpriteEnd();

		_renderingEngine.DrawTextStart();
		_renderingEngine.DrawString2D(FontType::GenNoKaku_Gothic_English, TextString(L"WASD: move", charSize, Float3(0.66f, 0.98f, 1.0f), 0.0f), Float4(1, 1, 1, 1));
		_renderingEngine.DrawString2D(FontType::GenNoKaku_Gothic_English, TextString(L"P: Change postEffect", charSize, Float3(0.66f, 0.90f, 1.0f), 0.0f), Float4(1, 1, 1, 1));
		_renderingEngine.DrawString2D(FontType::GenNoKaku_Gothic_English, TextString(L"T: Hide text", charSize, Float3(0.66f, 0.82f, 1.0f), 0.0f), Float4(1, 1, 1, 1));
		_renderingEngine.DrawTextEnd();

	}
	else
	{
		/*-------------------------------------------------------------------
		-           Draw Back Texture
		---------------------------------------------------------------------*/
		std::vector<Sprite> sprites;
		_backSprite.CreateSpriteForColor(Float3(0.8f, 1.0f, 0.1f), Float2(0.4f, 0.2f), Float4(0.0f, 0.0f, 0.0f, 1.0f));
		sprites.push_back(_backSprite);
		_renderingEngine.DrawSpriteStart();
		_renderingEngine.DrawSprites(sprites, _whiteTexture);
		_renderingEngine.DrawSpriteEnd();
		/*-------------------------------------------------------------------
		-           Draw Text
		---------------------------------------------------------------------*/
		float pixelWidth = Screen::GetOnePixelWidth();
		float pixelHeight = Screen::GetOnePixelHeight();
		Float2 charSize = Float2(30 * pixelWidth, 45 * pixelHeight);

		_renderingEngine.DrawTextStart();
		_renderingEngine.DrawString2D(FontType::GenNoKaku_Gothic_English, TextString(L"t: showText", charSize, Float3(0.66f, 1.0f, 1.0), 0.0f), Float4(1, 1, 1, 1));
		_renderingEngine.DrawTextEnd();
	}

}
#pragma endregion DrawFunction
#pragma endregion Private Function