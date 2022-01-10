//////////////////////////////////////////////////////////////////////////////////
//              Title:  MainBattle.cpp
//            Content:  Battle Scene
//             Author:  Toide Yutaro
//             Create:  2021_11_30
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/MMDRender/Include/MainRenderScene.hpp"
#include "MainGame/MMDRender/Include/MMDRendererTitle.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "GameCore/Include/FrameResources.hpp"
#include "MainGame/MMDRender/Include/Miku.hpp"
#include "GameCore/Include/Model/Primitive/PrimitiveModel.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "GameCore/Include/Effect/Bloom.hpp"
#include "GameMath/Include/GMDistribution.hpp"
#include "GameCore/Include/Sprite/Text.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Model/ModelLoader.hpp"
#include "GameCore/Include/Model/MotionLoader.hpp"
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
#define MAX_CHAR_COUNT (20)

namespace render
{
	static SceneConstants sceneParameter;
	static gm::Matrix4 projView = gm::MatrixIdentity();
	static AudioSource audio;
	static bool isBloom = true;

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
	_bloom  = std::make_unique<Bloom>();
	_postEffects.resize(_postEffectTypes);
	for (auto& postEffect : _postEffects)
	{
		postEffect = std::make_unique<PostEffect>();
	}
	
}

MainRenderScene::~MainRenderScene()
{

};
/****************************************************************************
*                       Initialize
*************************************************************************//**
*  @fn        void MainRenderScene::Initialize(GameTimer& gameTimer)
*  @brief     Initialize
*  @param[in] GameTimer& gameTimer
*  @return 　　void
*****************************************************************************/
void MainRenderScene::Initialize(GameTimer& gameTimer)
{
	Scene::Initialize(gameTimer);
	/*-------------------------------------------------------------------
	-                    FrameResources
	---------------------------------------------------------------------*/
	InitializeFrameResources();
	/*-------------------------------------------------------------------
	-                    SceneParameter
	---------------------------------------------------------------------*/
	InitializeSceneParameters();
	/*-------------------------------------------------------------------
	-                    RenderingEngine
	---------------------------------------------------------------------*/
	InitializeRenderingEngine();
	/*-------------------------------------------------------------------
	-                    Bloom
	---------------------------------------------------------------------*/
	_bloom.get()->Initialize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
	_bloom.get()->UpdateBloomPower(3);
	/*-------------------------------------------------------------------
	-                    PostEffect
	---------------------------------------------------------------------*/
	for (int i = 0; i < _postEffects.size(); ++i) { _postEffects[i]->Initialize((PostEffectBlendStateType)i); }

	_directX12.CompleteInitialize();
	
}

/****************************************************************************
*                       Update
*************************************************************************//**
*  @fn        void MainRenderScene::Update()
*  @brief     Update scene objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainRenderScene::Update()
{   
	/*-------------------------------------------------------------------
	-           Update input 
	---------------------------------------------------------------------*/
	Scene::Update(); 
	/*-------------------------------------------------------------------
	-           Update camera
	---------------------------------------------------------------------*/
	UpdateCamera();
	/*-------------------------------------------------------------------
	-           Update scene objects
	---------------------------------------------------------------------*/
	_miku.get()->Update();
	_stage.get()->Update();
	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	if (_hasExecutedSceneTransition) { SceneManager::Instance().TransitScene(new MMDRendererTitle(), *_gameTimer); }
}
/****************************************************************************
*                       Draw
*************************************************************************//**
*  @fn        void MainRenderScene::Draw()
*  @brief     Draw
*  @param[in] void
*  @return 　　void
*****************************************************************************/
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
	if (isBloom)
	{
		_bloom.get()->OnResize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
		_bloom.get()->Draw();
	}

	//_fader.get()->Draw(*_gameTimer, projView);
	/*-------------------------------------------------------------------
	-           Explain Text
	---------------------------------------------------------------------*/
	DrawExplainText();

	if (_postEffectIndex != (int)PostEffectBlendStateType::None)
	{
		_postEffects[_postEffectIndex].get()->OnResize(_screen.GetScreenWidth(), _screen.GetScreenHeight());
		_postEffects[_postEffectIndex].get()->Draw();
	}


	_directX12.CompleteRendering();
}
/****************************************************************************
*                       Terminate
*************************************************************************//**
*  @fn        void MainRenderScene::Terminate()
*  @brief     Terminate scene 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MainRenderScene::Terminate()
{
	/*-------------------------------------------------------------------
	-           Clear 3DModel 
	---------------------------------------------------------------------*/
	_stage.get ()->Finalize(); _stage.reset();
	_miku .get ()->Finalize(); _miku .reset();
	_skybox.get()->Finalize(); _skybox.reset();
	ModelTableManager ::Instance().Clear();
	MotionTableManager::Instance().Clear();
	
	/*-------------------------------------------------------------------
	-           Clear PostEffects
	---------------------------------------------------------------------*/
	_bloom.get()->Finalize(); _bloom.reset();
	for (auto& postEffect : _postEffects)
	{
		postEffect.get()->Finalize();
		postEffect.reset();
	}
	_postEffects.clear(); _postEffects.shrink_to_fit();
	/*-------------------------------------------------------------------
	-           Clear text
	---------------------------------------------------------------------*/
	_explainTexts.clear(); _explainTexts.shrink_to_fit();
	/*-------------------------------------------------------------------
	-           Reset Clear AllRendering Pass
	---------------------------------------------------------------------*/
	_renderingEngine.ClearAllRenderingPass();
	/*-------------------------------------------------------------------
	-           Clear GameObjects
	---------------------------------------------------------------------*/
	GameObject::ClearAllGameObjects();
	//audio.Stop();
	/*-------------------------------------------------------------------
	-           Clear texture table
	---------------------------------------------------------------------*/
	TextureTableManager::Instance().ClearTextureTable();
	/*-------------------------------------------------------------------
	-           Set Resource Nullptr
	---------------------------------------------------------------------*/
	_whiteTexture.Resource = nullptr;
	_frameResource->Finalize();
	delete _frameResource; _frameResource = nullptr;
	/*-------------------------------------------------------------------
	-           Reset CommandList
	---------------------------------------------------------------------*/
	_directX12.ResetCommandList();

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
	_miku.get()->SetWorldTimer(gameTimer);
	_miku.get()->EnableAmbientOcculusionMap(true);
	_miku.get()->SetPosition(0, 0, 10);

	/*-------------------------------------------------------------------
	-           Explain Text
	---------------------------------------------------------------------*/
	_explainTexts.resize(_explainRows);
	_explainTexts[0] = L"WASD: Move";
	_explainTexts[1] = L"Mouse: Rotate";
	_explainTexts[2] = L"I: Bloom On and Off";
	_explainTexts[3] = L"O: Dance";
	_explainTexts[4] = L"P: Change PostEffect";
	_explainTexts[5] = L"Enter: Back Title";
	_explainTexts[6] = L"T: Hide text";
	return true;
}


/****************************************************************************
*                       InitializeRenderingEngine
*************************************************************************//**
*  @fn        bool MainRenderScene::InitializeRenderingEngine()
*  @brief     Regist scene objects to rendering engine.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool MainRenderScene::InitializeRenderingEngine()
{
	_renderingEngine.OnAfterSceneTransition();
	_renderingEngine.SetSceneGPUAddress(_frameResource->SceneConstantsBuffer.get()->Resource()->GetGPUVirtualAddress());
	_renderingEngine.AddObjectToForwardRenderer(*_stage.get());
	_renderingEngine.AddObjectToForwardRenderer(*_miku.get());
	_renderingEngine.AddObjectToZPrepass       (*_stage.get());
	_renderingEngine.AddObjectToZPrepass       (*_miku.get());
	_renderingEngine.AddObjectToGBuffer        (*_stage.get());
	_renderingEngine.AddObjectToGBuffer        (*_miku.get());
	_renderingEngine.AddObjectToShadowMap      (*_miku.get(), 0);
	InitializeLights();
	return true;
}
/****************************************************************************
*                       InitializeLights
*************************************************************************//**
*  @fn        bool MainRenderScene::InitializeLights()
*  @brief     Set four directional lights.
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
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
	-           Keyboad Input W (Move Camera )
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsPress(DIK_W))
	{
		_fpsCamera.Walk(ACTION_SPEED * deltaTime);
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input S (Move Camera)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsPress(DIK_S))
	{
		_fpsCamera.Walk(-ACTION_SPEED * deltaTime);
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input D (Move Camera)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsPress(DIK_D))
	{
		_fpsCamera.Strafe(ACTION_SPEED * deltaTime);
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input A (Move Camera)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsPress(DIK_A))
	{
		_fpsCamera.Strafe(-ACTION_SPEED * deltaTime);
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input T (Shot Text)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsTrigger(DIK_T))
	{
		_enabledExplainText = _enabledExplainText ? false : true;
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input P (Change Post Effect)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsTrigger(DIK_P))
	{
		_postEffectIndex = (_postEffectIndex + 1) % _postEffects.size();
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input O (Pose)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsTrigger(DIK_O))
	{
		//audio.LoadSound(L"Resources/Audio/oddsends.wav", SoundType::BGM);
		//audio.Play();
		_miku.get()->StartAnimation(L"pose");
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input I (Bloom On or off)
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsTrigger(DIK_I))
	{
		isBloom = isBloom ? false : true;
	}
	/*-------------------------------------------------------------------
	-           Keyboad Input Return Key
	---------------------------------------------------------------------*/
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RETURN))
	{
		_hasExecutedSceneTransition = true;
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
	float charWidth     = 0.46f / MAX_CHAR_COUNT; // 最大文字列数
	float verticalSpace = 2.0f / MAX_CHAR_COUNT;
	Float2 charSize            = Float2(charWidth, 1.5f * charWidth * _screen.AspectRatio());
	Float3 charDefaultPosition = Float3(0.52f, 1.02f, 0.0f);
	/*-------------------------------------------------------------------
	-           DrawIntroductionText
	---------------------------------------------------------------------*/
	if (_enabledExplainText)
	{
		/*-------------------------------------------------------------------
		-           Draw Back Texture
		---------------------------------------------------------------------*/
		std::vector<Sprite> sprites;
		_backSprite.CreateSpriteForColor(Float3(0.75f, 0.5f, 0.1f), Float2(0.5f, charDefaultPosition.y), Float4(0.0f, 0.0f, 0.0f, 1.0f));
		sprites.push_back(_backSprite);
		_renderingEngine.DrawSpriteStart();
		_renderingEngine.DrawSprites(sprites, _whiteTexture);
		_renderingEngine.DrawSpriteEnd();

		/*-------------------------------------------------------------------
		-           Draw Text
		---------------------------------------------------------------------*/
		_renderingEngine.DrawTextStart();
		for (int i = 0; i < _explainTexts.size(); ++i)
		{
			_renderingEngine.DrawString2D(FontType::GenNoKaku_Gothic_English, TextString(_explainTexts[i], charSize, Float3(charDefaultPosition.x, charDefaultPosition.y - i * verticalSpace, charDefaultPosition.z), 0.0f), Float4(1, 1, 1, 1));
		}
		_renderingEngine.DrawTextEnd();

	}
	else
	{
		/*-------------------------------------------------------------------
		-           Draw Back Texture
		---------------------------------------------------------------------*/
		std::vector<Sprite> sprites;
		_backSprite.CreateSpriteForColor(Float3(0.75f, 1.0f, 0.1f), Float2(0.5f, 0.2f), Float4(0.0f, 0.0f, 0.0f, 1.0f));
		sprites.push_back(_backSprite);
		_renderingEngine.DrawSpriteStart();
		_renderingEngine.DrawSprites(sprites, _whiteTexture);
		_renderingEngine.DrawSpriteEnd();
		/*-------------------------------------------------------------------
		-           Draw Text
		---------------------------------------------------------------------*/
		_renderingEngine.DrawTextStart();
		_renderingEngine.DrawString2D(FontType::GenNoKaku_Gothic_English, TextString(L"t: showText", charSize, Float3(charDefaultPosition.x, charDefaultPosition.y, 1.0), 0.0f), Float4(1, 1, 1, 1));
		_renderingEngine.DrawTextEnd();
	}

}
#pragma endregion DrawFunction
#pragma region Update
void MainRenderScene::UpdateCamera()
{
	_fpsCamera.SetLens(0.25f * GM_PI, _screen.AspectRatio(), 1.0f, 1000.0f);
	_fpsCamera.UpdateViewMatrix();
	_frameResource->UpdateSceneConstants(&sceneParameter, &_fpsCamera);
}
#pragma endregion Update
#pragma endregion Private Function