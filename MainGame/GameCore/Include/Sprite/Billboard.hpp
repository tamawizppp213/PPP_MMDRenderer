//////////////////////////////////////////////////////////////////////////////////
///             @file   BillboardRenderer.hpp
///             @brief  Billboard Renderer
///             @author Toide Yutaro
///             @date   2021_09_20
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef BILLBOARD_HPP
#define BILLBOARD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12BlendState.hpp"

#include <memory>


//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////]
struct Billboard
{
	int test;
};


/****************************************************************************
*					BillboardRenderer
*************************************************************************//**
*  @struct    Billboard renderer
*  @brief     BillboardRenderer (screen center is origin point. and x, y, from -1 to 1)
*****************************************************************************/
class BillboardRenderer 
{
	// rect Ç∆ÇµÇƒ3DÉÇÉfÉãÇçÏê¨
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(FastBlendStateType type = FastBlendStateType::Normal);
	bool DrawStart();
	bool Draw();
	bool DrawEnd();
	bool Finalize();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BillboardRenderer();
	~BillboardRenderer();
	BillboardRenderer(const BillboardRenderer&)            = delete; // prohibit copy
	BillboardRenderer& operator=(const BillboardRenderer&) = delete;
	BillboardRenderer(BillboardRenderer&&)                 = default; // arrow move
	BillboardRenderer& operator=(BillboardRenderer&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool PrepareRootSignature();
	bool PreparePipelineState(FastBlendStateType type);
	bool PrepareObjectBuffer();
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	PipelineStateComPtr     _pipelineState = nullptr;
	RootSignatureComPtr     _rootSignature = nullptr;

	
};


#endif

