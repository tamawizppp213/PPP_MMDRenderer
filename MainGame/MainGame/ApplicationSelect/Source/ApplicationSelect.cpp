//////////////////////////////////////////////////////////////////////////////////
//              @file   RendererTitle.cpp
///             @brief  CoreRendererTitle.cpp
///             @author Toide Yutaro
///             @date   2021_12_11
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ApplicationSelect/Include/ApplicationSelect.hpp"
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/MMDRender/Include/MMDRendererTitle.hpp"
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
ApplicationSelect::ApplicationSelect()
{
	_spriteRenderer = std::make_unique<SpriteRenderer>();
	_textRenderer   = std::make_unique<TextRenderer>();
	_decisionSound  = std::make_unique<AudioSource>();
	for (int i = 0; i < _countof(_moveSound); ++i)
	{
		_moveSound[i] = std::make_unique<AudioSource>();
	}
}

ApplicationSelect::~ApplicationSelect()
{

}

void ApplicationSelect::Initialize(GameTimer& gameTimer)
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
void ApplicationSelect::Update()
{
	/*-------------------------------------------------------------------
	-           Update local timer
	---------------------------------------------------------------------*/
	_localTimer += _gameTimer->DeltaTime();
	
	/*-------------------------------------------------------------------
	-           Check Input
	---------------------------------------------------------------------*/
	Scene::Update();
	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	SceneTransition();
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        void MMDRendererTitle::Draw()
*  @brief     Draw objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void ApplicationSelect::Draw()
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
void ApplicationSelect::Terminate()
{
	_directX12.ResetCommandList();
	TextureTableManager::Instance().ClearTextureTable();
	AudioTableManager  ::Instance().ClearAudioTable();
	_localTimer = 0.0f;

	_titleScreen.clear();
	_titleScreen.shrink_to_fit();
}

/****************************************************************************
*                          LoadMaterials
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     Load materials
*  @param[in] GameTimer& gameTimer
*  @return 　　bool
*****************************************************************************/
bool ApplicationSelect::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();
	_textRenderer.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare Title scene
	---------------------------------------------------------------------*/
	_titleScreen.resize((int)ApplicationTitle::CountOfTitle);
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/Title.png", *_titleScreen[(int)ApplicationTitle::ShootingStar].TexturePtr.get());
	textureLoader.LoadTexture(L"Resources/Texture/MMDTitle/TitleScreen.png", *_titleScreen[(int)ApplicationTitle::MMDRenderer].TexturePtr.get());
	
	
	for (auto& title : _titleScreen)
	{
		title.SpritePtr.get()->CreateSprite(Float3(0, 0, 0), Float2(2, 2), Float4(1, 1, 1, 1.0f), Float2(0, 1), Float2(0, 1));
	}

	/*-------------------------------------------------------------------
	-           Prepare Arrow UI
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(L"Resources/Texture/Default/Arrow.png", *_rightArrow.TexturePtr.get());
	textureLoader.LoadTexture(L"Resources/Texture/Default/Arrow.png", *_leftArrow.TexturePtr.get());
	_leftArrow .SpritePtr.get()->CreateSprite(Float3( 0.9625f, 0, 0), Float2(0.075f, 0.1f), Float4(1, 1, 0, 1.0f), Float2(0, 1), Float2(0, 1));
	_rightArrow.SpritePtr.get()->CreateSprite(Float3(-0.9625f, 0, 0), Float2(0.075f, 0.1f), Float4(1, 1, 0, 1.0f), Float2(0, 1), Float2(0, 1), GM_PI);
	
	/*-------------------------------------------------------------------
	-           Prepare sound
	---------------------------------------------------------------------*/
	for (int i = 0; i < _countof(_moveSound); ++i)
	{
		_moveSound[i].get()->LoadSound(L"Resources/Audio/Default/カーソル移動.wav", SoundType::SE);
	}

	_decisionSound.get()->LoadSound(L"Resources/Audio/Default/決定.wav", SoundType::SE);
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
void ApplicationSelect::OnKeyboardInput()
{
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RIGHTARROW))
	{
		_stageSelectIndex = (_stageSelectIndex + 1) % (int)ApplicationTitle::CountOfTitle;
		_moveSoundIndex = (_moveSoundIndex + 1) % _countof(_moveSound);
		_moveSound[_moveSoundIndex].get()->Play();
	}
	if (_gameInput.GetKeyboard().IsTrigger(DIK_LEFTARROW))
	{
		int stage = _stageSelectIndex - 1;
		_stageSelectIndex = stage >= 0 ? stage % (int)ApplicationTitle::CountOfTitle : (int)ApplicationTitle::CountOfTitle - 1;
		_moveSoundIndex = (_moveSoundIndex + 1) % _countof(_moveSound);
		_moveSound[_moveSoundIndex].get()->Play();
	}
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RETURN))
	{
		_hasExecutedSceneTransition = true;
		_decisionSound.get()->Play();
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
void ApplicationSelect::DrawSprites()
{
	std::vector<Sprite> sprites, leftArrow, rightArrow;
	/*-------------------------------------------------------------------
	-           Arrow
	---------------------------------------------------------------------*/
	leftArrow.push_back(*_leftArrow.SpritePtr.get());
	rightArrow.push_back(*_rightArrow.SpritePtr.get());
	_spriteRenderer.get()->Draw(leftArrow , *_leftArrow.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->Draw(rightArrow, *_rightArrow.TexturePtr.get(), MatrixIdentity());

	/*-------------------------------------------------------------------
	-           BackGroundTexture
	---------------------------------------------------------------------*/
	sprites.push_back(*_titleScreen[_stageSelectIndex].SpritePtr.get());
	_spriteRenderer.get()->DrawStart();
	_spriteRenderer.get()->Draw(sprites, *_titleScreen[_stageSelectIndex].TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->DrawEnd();

}

void ApplicationSelect::SceneTransition()
{
	if (!_hasExecutedSceneTransition) { return; }

	switch ((ApplicationTitle)_stageSelectIndex)
	{
		case ApplicationTitle::ShootingStar:
		{
			SceneManager::Instance().TransitScene(new ShootingStarTitle(), *_gameTimer);
			break;
		}
		case ApplicationTitle::MMDRenderer:
		{
			SceneManager::Instance().TransitScene(new MMDRendererTitle(), *_gameTimer);
			break;
		}
	}
}