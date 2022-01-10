//////////////////////////////////////////////////////////////////////////////////
//              @file   ShootingStarTitle.cpp
///             @brief  ShootingStarTitle.cpp
///             @author Toide Yutaro
///             @date   ------------
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Scene/ShootingStarTitle.hpp"
#include "MainGame/ApplicationSelect/Include/ApplicationSelect.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarIntroduction.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Audio/AudioTable.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;


//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////

ShootingStarTitle::ShootingStarTitle()
{
	_spriteRenderer = std::make_unique<SpriteRenderer>();
	_audioSource    = std::make_unique<AudioSource>();
	_pressSound     = std::make_unique<AudioSource>();
}

ShootingStarTitle::~ShootingStarTitle()
{

}

void ShootingStarTitle::Initialize(GameTimer& gameTimer)
{
	Scene::Initialize(gameTimer);
	_directX12.CompleteInitialize();
}
/****************************************************************************
*                          Update
*************************************************************************//**
*  @fn        void MMDRendererTitle::Update()
*  @brief     Update scene objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarTitle::Update()
{
	/*-------------------------------------------------------------------
	-           Check Input
	---------------------------------------------------------------------*/
	Scene::Update();

	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	if (_hasExecutedSceneTransition) 
	{ 
		SceneManager::Instance().TransitScene(new ShootingStarIntroduction(), *_gameTimer);
	}
	else if(_hasExecutedBackScene)
	{
		SceneManager::Instance().TransitScene(new ApplicationSelect(), *_gameTimer);
	}
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        void MMDRendererTitle::Draw()
*  @brief     Draw objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarTitle::Draw()
{
	/*-------------------------------------------------------------------
	-           Clear screen
	---------------------------------------------------------------------*/
	_directX12.ClearScreen();
	/*-------------------------------------------------------------------
	-           Draw All Sprite
	---------------------------------------------------------------------*/
	DrawSprites();
	/*-------------------------------------------------------------------
	-           Call rendering complete
	---------------------------------------------------------------------*/
	_directX12.CompleteRendering();
}
/****************************************************************************
*                          Terminate
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     This function is used in case scene transition
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarTitle::Terminate()
{
	/*-------------------------------------------------------------------
	-           Clear Audio Resource
	---------------------------------------------------------------------*/
	_audioSource.get()->Stop(); _audioSource.reset();
	_pressSound .get()->Stop(); _pressSound .reset();
	AudioTableManager::Instance().ClearAudioTable();
	/*-------------------------------------------------------------------
	-           Clear Texture Resource
	---------------------------------------------------------------------*/
	TextureTableManager::Instance().ClearTextureTable();
	_spriteRenderer.get()->Finalize();
	_spriteRenderer.reset();
	_backGround.Clear();
	_backUI    .Clear();

	_directX12.ResetCommandList();
}

/****************************************************************************
*                          LoadMaterials
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     Load materials
*  @param[in] GameTimer& gameTimer
*  @return 　　bool
*****************************************************************************/
bool ShootingStarTitle::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/Title.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0, 0), Float2(2, 2), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Prepare Back UI
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(L"Resources/Texture/ApplicationSelect/Back.png", *_backUI.TexturePtr.get());
	_backUI.SpritePtr.get()->CreateSpriteForTexture(Float3(-0.9f, 0.9f, 0), Float2(0.1f, 0.15f), Float2(0, 1), Float2(0, 1));
	/*-------------------------------------------------------------------
	-           Audio Source
	---------------------------------------------------------------------*/
	_audioSource.get()->LoadSound(L"Resources/Audio/ShootingStar/digitalworld.wav", SoundType::BGM);
	_audioSource.get()->Play();
	_pressSound.get()->LoadSound(L"Resources/Audio/Default/決定.wav", SoundType::SE);
	return true;
}

/****************************************************************************
*                          OnKeyboardInput
*************************************************************************//**
*  @fn        void MainBattle::OnKeyboardInput()
*  @brief     Keyboard Input
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarTitle::OnKeyboardInput()
{
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RETURN))
	{
		_hasExecutedSceneTransition = true;
		_pressSound.get()->Play();
	}
	else if (_gameInput.GetKeyboard().IsTrigger(DIK_BACKSPACE))
	{
		_hasExecutedBackScene = true;
		_pressSound.get()->Play();
	}
}

/****************************************************************************
*                          DrawSprites
*************************************************************************//**
*  @fn        void MMDRendererTitle::DrawSprites()
*  @brief     Draw sprite (backGround and text)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarTitle::DrawSprites()
{

	/*-------------------------------------------------------------------
	-           BackGroundTexture
	---------------------------------------------------------------------*/
	std::vector<Sprite> sprites, back;
	sprites.push_back(*_backGround.SpritePtr.get());
	back.push_back(*_backUI.SpritePtr.get());
	_spriteRenderer.get()->DrawStart();
	_spriteRenderer.get()->Draw(back, *_backUI.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->Draw(sprites, *_backGround.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->DrawEnd();


}