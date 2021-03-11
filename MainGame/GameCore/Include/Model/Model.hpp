//////////////////////////////////////////////////////////////////////////////////
///             @file   Model.hpp
///             @brief  Model
///             @author Toide Yutaro
///             @date   2021_03_10
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MODEL_HPP
#define MODEL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Core.hpp"
#include "DirectX12/Include/Core/DirectX12Config.hpp"
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12Buffer.hpp"
#include "DirectX12/Include/Core/DirectX12MeshBuffer.hpp"
#include <memory>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
class DefaultModel
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/

protected:
	/****************************************************************************
	**                ProtectedFunction
	*****************************************************************************/
	bool PrepareVertexBuffer() { return true; };
	bool PrepareIndexBuffer() { return true; };

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::wstring   _filePath;
	MeshBuffer     _meshBuffer[FRAME_BUFFER_COUNT];
	ResourceComPtr _indexBuffer  = nullptr;
	BlobComPtr     _vertexShader = nullptr;
	BlobComPtr     _pixelShader  = nullptr;
};
#endif