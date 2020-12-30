//////////////////////////////////////////////////////////////////////////////////
///             @file   BackGround2D.hpp
///             @brief  BackGround 2D
///             @author Toide Yutaro
///             @date   2020_12_24
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef BACKGROUND2D_HPP
#define BACKGROUND2D_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12PrimitiveGeometry.hpp"
#include "DirectX12/Include/DirectX12Geometry.hpp"
#include "DirectX12/Include/DirectX12Core.hpp"
#include "DirectX12/Include/DirectX12Base.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     BackGround2D 
*  @brief     Show BackGround 
*****************************************************************************/
class BackGround2D
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Initialize(const std::vector<std::wstring>& imagePath, DirectX12& directX12);
	void Update();
	void Draw();
	void Terminate();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	BackGround2D()  = default;
	~BackGround2D() = default;

protected:
	/****************************************************************************
	**                Protected Function
	*****************************************************************************/
	
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	DirectX12                   _directX12;
	GeometryGenerator           _geometryGenerator;
	MeshData                    _rect;
	BlobComPtr                  _vertexShader;
	BlobComPtr                  _pixelShader;
	std::vector<SubMeshGeometry>_subMeshGeometry;
	std::vector<MeshGeometry>   _meshGeometry;
	std::vector<ResourceComPtr> _textureBuffers;
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void LoadTextures(const std::vector<std::wstring>& imagePath);
	void LoadShaders();
	void ResizeVectors(const std::vector<std::wstring>& imagePath);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

#endif