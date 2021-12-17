#include "MainGame/Core/Include/Scene.hpp"
#include "GameCore/Include/Sprite/SpriteRenderer.hpp"

Scene::Scene()
{
	SpriteRendererPtr spriteRenderer = std::make_unique<SpriteRenderer>();
	_spriteRenderer = std::move(spriteRenderer);
	
}

Scene::~Scene()
{

}

void Scene::Initialize(GameTimer& gameTimer)
{
	InitializeDirectX12(gameTimer);
	LoadMaterials(gameTimer);
}

void Scene::Update()
{
	OnGameInput();
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
*                          OnGameInput
*************************************************************************//**
*  @fn        void MainBattle::OnGameInput()
*  @brief     GameInput (Keyboard, Mouse, GamePad)
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void Scene::OnGameInput()
{
	OnKeyboardInput();
	OnMouseInput();
	OnGamePadInput();

}
void Scene::OnKeyboardInput()
{

}
void Scene::OnMouseInput()
{

}
void Scene::OnGamePadInput()
{
	
}