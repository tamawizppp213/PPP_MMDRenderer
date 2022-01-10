//////////////////////////////////////////////////////////////////////////////////
//              @file   ShootingStarTitle.cpp
///             @brief  ShootingStarTitle.cpp
///             @author Toide Yutaro
///             @date   ------------
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Scene/ShootingStarIntroduction.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarGame.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Audio/AudioTable.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;


//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////

ShootingStarIntroduction::ShootingStarIntroduction()
{
	_spriteRenderer  = std::make_unique<SpriteRenderer>();
	_textRenderer    = std::make_unique<TextRenderer>();
	_audioSource     = std::make_unique<AudioSource>();
}

ShootingStarIntroduction::~ShootingStarIntroduction()
{

}

void ShootingStarIntroduction::Initialize(GameTimer& gameTimer)
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
void ShootingStarIntroduction::Update()
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
		SceneManager::Instance().TransitScene(new ShootingStarGame(), *_gameTimer);
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
void ShootingStarIntroduction::Draw()
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
void ShootingStarIntroduction::Terminate()
{
	/*-------------------------------------------------------------------
	-           Clear Audio Resource
	---------------------------------------------------------------------*/
	_audioSource.get()->Stop(); _audioSource.reset();
	AudioTableManager::Instance().ClearAudioTable();
	/*-------------------------------------------------------------------
	-           Clear Texture Resource
	---------------------------------------------------------------------*/
	TextureTableManager::Instance().ClearTextureTable();
	_spriteRenderer.get()->Finalize(); _spriteRenderer.reset();
	_textRenderer  .get()->Finalize(); _textRenderer  .reset();
	_backGround.Clear();
	for (auto& how : _howToManipulate) { how.reset(); }
	_howToManipulate.clear(); _howToManipulate.shrink_to_fit();
	_title          .reset();
	_clickButtonText.reset();

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
bool ShootingStarIntroduction::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();
	_textRenderer.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/Introduction.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0, 0), Float2(2, 2), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Audio Source
	---------------------------------------------------------------------*/
	_audioSource.get()->LoadSound(L"Resources/Audio/Default/決定.wav", SoundType::SE);

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
void ShootingStarIntroduction::OnKeyboardInput()
{
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RETURN))
	{
		_hasExecutedSceneTransition = true;
		_audioSource.get()->Play();
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
void ShootingStarIntroduction::DrawSprites()
{
	/*-------------------------------------------------------------------
	-           BackGroundTexture
	---------------------------------------------------------------------*/
	std::vector<Sprite> sprites;
	sprites.push_back(*_backGround.SpritePtr.get());
	_spriteRenderer.get()->DrawStart();
	_spriteRenderer.get()->Draw(sprites, *_backGround.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->DrawEnd();


}