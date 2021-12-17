//////////////////////////////////////////////////////////////////////////////////
//              @file   MMDRendererTitle.cpp
///             @brief  MMDRendererTitle.cpp
///             @author Toide Yutaro
///             @date   2021_11_30
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/MMDRender/Include/MMDRendererTitle.hpp"
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/MMDRender/Include/MainRenderScene.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"
#include "GameCore/Include/Audio/AudioTable.hpp"
#include "MainGame/ApplicationSelect/Include/ApplicationSelect.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define TEXT_ANIMATION_TIME (1.5f)

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
MMDRendererTitle::MMDRendererTitle()
{
	_spriteRenderer    = std::make_unique<SpriteRenderer>();
	_textRenderer      = std::make_unique<TextRenderer>();
	_pressButtonText   = std::make_unique<TextString>();
	_pressSound        = std::make_unique<AudioSource>();
	_titleText         = std::make_unique<TextString>();
}

MMDRendererTitle::~MMDRendererTitle()
{

}

void MMDRendererTitle::Initialize(GameTimer& gameTimer)
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
void MMDRendererTitle::Update()
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
	-           Update text animation
	---------------------------------------------------------------------*/
	UpdateTextAnimation();
	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	if(_hasExecutedSceneTransition){ SceneManager::Instance().TransitScene(new MainRenderScene(), *_gameTimer); }
	else if (_hasExecutedBackScene) { SceneManager::Instance().TransitScene(new ApplicationSelect(), *_gameTimer); }
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        void MMDRendererTitle::Draw()
*  @brief     Draw objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MMDRendererTitle::Draw()
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
void MMDRendererTitle::Terminate()
{
	_directX12.ResetCommandList();
	TextureTableManager::Instance().ClearTextureTable();
	AudioTableManager::Instance().ClearAudioTable();
}

/****************************************************************************
*                          LoadMaterials
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     Load materials
*  @param[in] GameTimer& gameTimer
*  @return 　　bool
*****************************************************************************/
bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();
	_textRenderer  .get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/MMDTitle/BackGround.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0, 0), Float2(2, 2), Float2(0, 1), Float2(0, 1));
	
	/*-------------------------------------------------------------------
	-           Prepare Title
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(L"Resources/Texture/MMDTitle/TitleCall.png", *_titleBack.TexturePtr.get());
	_titleBack.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0.1f, 0), Float2(2, 0.32f), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Prepare Click 
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(L"Resources/Texture/MMDTitle/click.png", *_click.TexturePtr.get());
	_click.SpritePtr.get()->CreateSpriteForTexture(Float3(-0.15f, -0.7f, 0), Float2(1.4f, 0.2f), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Prepare Back UI
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(L"Resources/Texture/ApplicationSelect/Back.png", *_backUI.TexturePtr.get());
	_backUI.SpritePtr.get()->CreateSpriteForTexture(Float3(-0.9f, 0.9f, 0), Float2(0.1f, 0.15f), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Prepare Text
	---------------------------------------------------------------------*/
	_pressButtonText.get()->String = L"Press Enter Key";
	float charWidth = 1.1f / _pressButtonText.get()->String.size();
	_pressButtonText.get()->StartPosition = Float3(-0.75f, -0.5f, 0);
	_pressButtonText.get()->SizePerChar   = Float2(charWidth, 1.5f * charWidth * _screen.AspectRatio());
	_pressButtonText.get()->Space         = 0.0f;

	_titleText.get()->String = L"MMD Renderer";
	charWidth = 1.2f / _titleText.get()->String.size();
	_titleText.get()->StartPosition = Float3(-0.6f, 0.36f, 0);
	_titleText.get()->SizePerChar   = Float2(charWidth, 1.5f * charWidth * _screen.AspectRatio());
	_titleText.get()->Space         = 0.0f;

	/*-------------------------------------------------------------------
	-           Prepare sound
	---------------------------------------------------------------------*/
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
void MMDRendererTitle::OnKeyboardInput()
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
*                          UpdateTextAnimation
*************************************************************************//**
*  @fn        void MMDRendererTitle::UpdateTextAnimation()
*  @brief     Update text animation (時間が経ったらanimation flagをon/offする)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MMDRendererTitle::UpdateTextAnimation()
{
	if (_localTimer >= TEXT_ANIMATION_TIME)
	{
		_animationOn = _animationOn ? false : true;
		_localTimer = 0;
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
void MMDRendererTitle::DrawSprites()
{
	/*-------------------------------------------------------------------
	-           TextRenderer
	---------------------------------------------------------------------*/
	_textRenderer.get()->DrawStart();
	_textRenderer.get()->DrawString(FontType::GenNoKaku_Gothic_English, *_titleText.get(), Float4(1, 1, 1, 1), MatrixIdentity());
	if (_animationOn)
	{
		_textRenderer.get()->DrawString(FontType::GenNoKaku_Gothic_English, *_pressButtonText.get(), Float4(0, 0.6f, 0.9f, 1), MatrixIdentity());
	}
	_textRenderer.get()->DrawEnd();
	/*-------------------------------------------------------------------
	-           BackGroundTexture
	---------------------------------------------------------------------*/
	std::vector<Sprite> sprites, title, click, back; 
	sprites.push_back(*_backGround.SpritePtr.get());
	title  .push_back(*_titleBack.SpritePtr.get());
	click.push_back(*_click.SpritePtr.get());
	back.push_back(*_backUI.SpritePtr.get());
	_spriteRenderer.get()->DrawStart();
	_spriteRenderer.get()->Draw(click, *_click.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->Draw(back, *_backUI.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->Draw(title, *_titleBack.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->Draw(sprites, *_backGround.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->DrawEnd();


}