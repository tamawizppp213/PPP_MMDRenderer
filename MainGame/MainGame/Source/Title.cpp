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
#include "MainGame/Include/Title.hpp"
#include <iostream>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

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
	_gameTimer = &gameTimer;
}

void Title::Update()
{
	
}

void Title::Draw()
{
	_directX12.ClearScreen();

	_directX12.CompleteRendering();
}


void Title::Terminate()
{

}

#pragma region Protected Function
void Title::LoadShaders()
{

}

void Title::LoadGeometry()
{

}

void Title::LoadTextures()
{

}

void Title::BuildPSOs()
{

}
void Title::BuildRootSignature()
{

}

void Title::BuildRenderItem()
{

}

void Title::BuildFrameResources()
{

}

void Title::BuildDescriptorHeap()
{

}

void Title::BuildConstantBufferView()
{

}
#pragma endregion Protected Function