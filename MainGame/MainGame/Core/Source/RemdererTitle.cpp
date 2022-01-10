//////////////////////////////////////////////////////////////////////////////////
//              @file   RendererTitle.cpp
///             @brief  CoreRendererTitle.cpp
///             @author Toide Yutaro
///             @date   2021_12_11
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Core/Include/RendererTitle.hpp"
#include "GameCore/Include/Sprite/TextRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/ApplicationSelect/Include/ApplicationSelect.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define TEXT_ANIMATION_TIME (1.5f)

//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
CoreRendererTitle::CoreRendererTitle()
{
	_spriteRenderer    = std::make_unique<SpriteRenderer>();
	_textRenderer      = std::make_unique<TextRenderer>();
	_creatorText       = std::make_unique<TextString>();
	_titleText         = std::make_unique<TextString>();
	_faderPtr          = std::make_unique<Fader>();
}

CoreRendererTitle::~CoreRendererTitle()
{

}

void CoreRendererTitle::Initialize(GameTimer& gameTimer)
{
	Scene::Initialize(gameTimer);
	_directX12.CompleteInitialize();
	_localTimer = 0.0f;
}
/****************************************************************************
*                          Update
*************************************************************************//**
*  @fn        void MMDRendererTitle::Update()
*  @brief     Update scene objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void CoreRendererTitle::Update()
{
	if (_firstUpdate) { return; }
	/*-------------------------------------------------------------------
	-           Update local timer
	---------------------------------------------------------------------*/
	_localTimer += _gameTimer->DeltaTime();
	if (_localTimer >= 5.0f) { _hasExecutedSceneTransition = true; }
	/*-------------------------------------------------------------------
	-           Check Input
	---------------------------------------------------------------------*/
	Scene::Update();
	/*-------------------------------------------------------------------
	-           Check Scene transition
	---------------------------------------------------------------------*/
	if(_hasExecutedSceneTransition){ SceneManager::Instance().TransitScene(new ApplicationSelect(), *_gameTimer); }
}
/****************************************************************************
*                          Draw
*************************************************************************//**
*  @fn        void MMDRendererTitle::Draw()
*  @brief     Draw objects
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void CoreRendererTitle::Draw()
{
	if (_firstUpdate) { _firstUpdate = false;  return; }
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
void CoreRendererTitle::Terminate()
{
	_localTimer = 0.0f;
	/*-------------------------------------------------------------------
	-           Clear Texture
	---------------------------------------------------------------------*/
	_backGround.TexturePtr.get()->Resource->Release();
	_creatorText.reset();
	_titleText.reset();

	/*-------------------------------------------------------------------
	-           Call Finalize
	---------------------------------------------------------------------*/
	TextureTableManager::Instance().ClearTextureTable();
	_spriteRenderer->Finalize();
	_textRenderer  ->Finalize();


	/*-------------------------------------------------------------------
	-           Call Destructor
	---------------------------------------------------------------------*/
	_faderPtr.get()->~Fader();
	_spriteRenderer->~SpriteRenderer();
	_textRenderer  ->~TextRenderer();
	
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
bool CoreRendererTitle::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();
	_textRenderer  .get()->Initialize();
	_faderPtr.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/Default/Black.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSprite(Float3(0, 0, 0), Float2(2, 2), Float4(1,1,1,1.0f), Float2(0, 1), Float2(0, 1));

	/*-------------------------------------------------------------------
	-           Prepare Text
	---------------------------------------------------------------------*/
	_creatorText.get()->String = L"Created by P.P.P";
	float charWidth = 0.8f / _creatorText.get()->String.size();
	_creatorText.get()->StartPosition = Float3(0, -0.36f, 0);
	_creatorText.get()->SizePerChar   = Float2(charWidth, 1.5f * charWidth * _screen.AspectRatio());
	_creatorText.get()->Space         = 0.0f;

	_titleText.get()->String = L"Renderer Engine";
	charWidth = 1.4f / _titleText.get()->String.size();
	_titleText.get()->StartPosition = Float3(-0.75f, 0.3f, 0);
	_titleText.get()->SizePerChar   = Float2(charWidth, 1.5f * charWidth * _screen.AspectRatio());
	_titleText.get()->Space         = 0.0f;
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
void CoreRendererTitle::OnKeyboardInput()
{
	if (_gameInput.GetKeyboard().IsTrigger(DIK_RETURN))
	{
		_hasExecutedSceneTransition = true;
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
void CoreRendererTitle::UpdateTextAnimation()
{
	
}
/****************************************************************************
*                          DrawSprites
*************************************************************************//**
*  @fn        void MMDRendererTitle::DrawSprites()
*  @brief     Draw sprite (backGround and text)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void CoreRendererTitle::DrawSprites()
{
	/*-------------------------------------------------------------------
	-           TextRenderer
	---------------------------------------------------------------------*/
	_textRenderer.get()->DrawStart();
	_textRenderer.get()->DrawString(FontType::GenNoKaku_Gothic_English, *_titleText.get(), Float4(1, 1, 1, 1), MatrixIdentity());
	_textRenderer.get()->DrawString(FontType::GenNoKaku_Gothic_English, *_creatorText.get(), Float4(0, 0.6f, 0.9f, 1), MatrixIdentity());
	_textRenderer.get()->DrawEnd();
	/*-------------------------------------------------------------------
	-           Draw All Sprite
	---------------------------------------------------------------------*/
	//_faderPtr.get()->Draw(*_gameTimer, MatrixIdentity());
	/*-------------------------------------------------------------------
	-           BackGroundTexture
	---------------------------------------------------------------------*/
	std::vector<Sprite> sprites; 
	sprites.push_back(*_backGround.SpritePtr.get());
	_spriteRenderer.get()->DrawStart();
	_spriteRenderer.get()->Draw(sprites, *_backGround.TexturePtr.get(), MatrixIdentity());
	_spriteRenderer.get()->DrawEnd();

	
}