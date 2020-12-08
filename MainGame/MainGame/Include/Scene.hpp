//////////////////////////////////////////////////////////////////////////////////
//              Title:  Scene.hpp
//            Content:  Scene Parent State (Pure Virtual Function)
//             Author:  Toide Yutaro
//             Create:  2020_11_07
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef SCENE_HPP
#define SCENE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12Base.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              SceneState
//////////////////////////////////////////////////////////////////////////////////
enum SceneState
{
	TITLE,
	MAINGAME,
	GAMEOVER,
	GAMECLEAR,
	PAUSE,
	MOTIONTEST,
	NONE
};

//////////////////////////////////////////////////////////////////////////////////
//                     Scene Class (Pure Virtual Class)
//////////////////////////////////////////////////////////////////////////////////
class Scene 
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	Scene();
	~Scene();
	virtual void Initialize(const DirectX12& directX12) = 0;
	virtual void Draw()       = 0;
	virtual void Update()     = 0;
	virtual void Terminate()  = 0;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

protected:
	virtual void LoadShaders()             = 0;
	virtual void LoadGeometry()            = 0;
	virtual void BuildRootSignature()      = 0;
	virtual void BuildFrameResources()     = 0;
	virtual void BuildPSOs()               = 0;
	virtual void BuildRenderItem()         = 0;
	virtual void BuildDescriptorHeap()     = 0;
	virtual void BuildConstantBufferView() = 0;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

#endif
