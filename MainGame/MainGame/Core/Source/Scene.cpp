#include "MainGame/Core/Include/Scene.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"

Scene::Scene()
{
	FaderPtr fader = std::make_unique<Fader>();
	SpriteRendererPtr spriteRenderer = std::make_unique<SpriteRenderer>();

	_fader          = std::move(fader);
	_spriteRenderer = std::move(spriteRenderer);
	
}

Scene::~Scene()
{

}

void Scene::Initialize(GameTimer& gameTimer)
{
	InitializeDirectX12(gameTimer);
	InitializeFader();
	LoadMaterials(gameTimer);
}

/****************************************************************************
*                       InitializeDirectX12
*************************************************************************//**
*  @fn        bool MainBattle::InitializeDirectX12()
*  @brief     Acquire directX12 infomation
*  @param[in] GameTimer& gameTimer
*  @return 　　bool
*****************************************************************************/
bool Scene::InitializeDirectX12(GameTimer& gameTimer)
{
	_gameTimer   = &gameTimer;
	_device      = _directX12.GetDevice();
	_commandList = _directX12.GetCommandList();
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
bool Scene::InitializeFader()
{
	auto fader = _fader.get();
	fader->Initialize();
	fader->StartFadeIn(1.0f, 2.0f);
	fader->SetSpriteSize((float)_screen.GetScreenWidth(), (float)_screen.GetScreenHeight());
	return true;
}