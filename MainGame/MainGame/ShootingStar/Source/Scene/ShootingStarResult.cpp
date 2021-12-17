//////////////////////////////////////////////////////////////////////////////////
//              @file   ShootingStarResult.cpp
///             @brief  ShootingStarResult.cpp
///             @author Toide Yutaro
///             @date   ------------------
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/ShootingStar/Include/Scene/ShootingStarResult.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"
#include "MainGame/Core/Include/SceneManager.hpp"
#include "MainGame/ShootingStar/Include/Scene/ShootingStarTitle.hpp"
#include "GameCore/Include/Audio/AudioSource.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;


//////////////////////////////////////////////////////////////////////////////////
//                          Implement
//////////////////////////////////////////////////////////////////////////////////
ShootingStarResult::ShootingStarResult()
{
	_spriteRenderer = std::make_unique<SpriteRenderer>();
}

ShootingStarResult::~ShootingStarResult()
{

}

void ShootingStarResult::Initialize(GameTimer& gameTimer)
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
void ShootingStarResult::Update()
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
		SceneManager::Instance().TransitScene(new ShootingStarTitle(), *_gameTimer);
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
void ShootingStarResult::Draw()
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
void ShootingStarResult::Terminate()
{
	_directX12.ResetCommandList();
	TextureTableManager::Instance().ClearTextureTable();
}

/****************************************************************************
*                          LoadMaterials
*************************************************************************//**
*  @fn        bool MMDRendererTitle::LoadMaterials(GameTimer& gameTimer)
*  @brief     Load materials
*  @param[in] GameTimer& gameTimer
*  @return 　　bool
*****************************************************************************/
bool ShootingStarResult::LoadMaterials(GameTimer& gameTimer)
{
	/*-------------------------------------------------------------------
	-           Prepare renderer
	---------------------------------------------------------------------*/
	_spriteRenderer.get()->Initialize();

	/*-------------------------------------------------------------------
	-           Prepare BackGround
	---------------------------------------------------------------------*/
	TextureLoader textureLoader;
	textureLoader.LoadTexture(L"Resources/Texture/ShootingStar/result.png", *_backGround.TexturePtr.get());
	_backGround.SpritePtr.get()->CreateSpriteForTexture(Float3(0, 0, 0), Float2(2, 2), Float2(0, 1), Float2(0, 1));

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
void ShootingStarResult::OnKeyboardInput()
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
void ShootingStarResult::DrawSprites()
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