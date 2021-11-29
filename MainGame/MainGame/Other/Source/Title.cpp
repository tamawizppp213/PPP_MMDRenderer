//////////////////////////////////////////////////////////////////////////////////
//              Title:  Title.hpp
//            Content:  Title Scene 
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Input/GameInput.hpp"
#include "GameCore/Include/Sprite/Fade.hpp"
#include "MainGame/Other/Include/Title.hpp"
#include "GameCore/Include/Screen.hpp"
#include <iostream>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
gm::Matrix4 g_ProjViewMatrix = gm::MatrixIdentity();

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
Title::Title()
{
	
}

Title::~Title()
{

}

void Title::Initialize(GameTimer& gameTimer) 
{
	InitializeDirectX12(gameTimer);
	InitializeCoordinates();
	InitializeFader();
	_directX12.CompleteInitialize();
	_directX12.FlushCommandQueue();
}

void Title::Update()
{
	
}

void Title::Draw()
{
	_directX12.ClearScreen();




	_fader->Draw(*_gameTimer, g_ProjViewMatrix);
	_directX12.CompleteRendering();
}


void Title::Terminate()
{
	//_fader->Finalize();
	//delete _fader; _fader = nullptr;
}

#pragma region Protected Function

#pragma endregion Protected Function

#pragma region Private Function
/****************************************************************************
*                       InitializeDirectX12
*************************************************************************//**
*  @fn        bool Title::InitializeDirectX12(GameTimer& gameTimer)
*  @brief     Initialize DirectX12
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Title::InitializeDirectX12(GameTimer& gameTimer)
{
	_gameTimer   = &gameTimer;
	_device      = _directX12.GetDevice();
	_commandList = _directX12.GetCommandList();
	_directX12.ResetCommandList();
	return true;
}

/****************************************************************************
*                       InitializeCoordinates
*************************************************************************//**
*  @fn        bool Title::InitializeCoordinates()
*  @brief     Initialize coordinates (O(screen center Position), +x: screenWidth / 2, +y: screenHeight / 2)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Title::InitializeCoordinates()
{
	Screen screen;
	g_ProjViewMatrix = gm::OrthographicLH(
		(float)screen.GetScreenWidth(),
		(float)screen.GetScreenHeight(),
		0.0f,
		1000.0f);

	return true;
}

/****************************************************************************
*                       InitializeFader
*************************************************************************//**
*  @fn        bool Title::InitializeFader()
*  @brief     Initialize Fader
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool Title::InitializeFader()
{
	/*_fader = new Fader();
	_fader->Initialize();
	_fader->StartFadeIn(1.0f, 2.0f);

	Screen screen;
	_fader->SetSpriteSize((float)screen.GetScreenWidth(), (float)screen.GetScreenHeight());*/
	return true;
}

#pragma endregion Private Function