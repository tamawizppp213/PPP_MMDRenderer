//////////////////////////////////////////////////////////////////////////////////
//              @file   ShootingStarGameOver.cpp
///             @brief  GameOver.cpp
///             @author Toide Yutaro
///             @date   ------------
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Scene/ShootingStarGameOver.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarTitle.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Audio/AudioTable.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;


//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////

ShootingStarGameOver::ShootingStarGameOver()
{
	_spriteRenderer = std::make_unique<SpriteRenderer>();
	_audioSource    = std::make_unique<AudioSource>();
}

ShootingStarGameOver::~ShootingStarGameOver()
{

}

void ShootingStarGameOver::Initialize(GameTimer& gameTimer)
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
void ShootingStarGameOver::Update()
{
	/*-------------------------------------------------------------------
	-           Check Input
	---------------------------------------------------------------------*/
	Scene::Update();

	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	if (_hasExecutedSceneTransition) { SceneManager::Instance().TransitScene(new ShootingStarTitle(), *_gameTimer); }
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        void MMDRendererTitle::Draw()
*  @brief     Draw objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ShootingStarGameOver::Draw()
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
void ShootingStarGameOver::Terminate()
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
	_backGround.Clear();

	/*-------------------------------------------------------------------
	-           Clear Sprite Renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Finalize();
	_spriteRenderer.reset();

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
bool ShootingStarGameOver::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/GameOver.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0, 0), Float2(2, 2), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Audio Source
	---------------------------------------------------------------------*/
	_audioSource.get()->LoadSound(L"Resources/Audio/ShootingStar/digitalworld.wav", SoundType::BGM);
	_audioSource.get()->Play();
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
void ShootingStarGameOver::OnKeyboardInput()
{
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RETURN))
	{
		_hasExecutedSceneTransition = true;
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
void ShootingStarGameOver::DrawSprites()
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