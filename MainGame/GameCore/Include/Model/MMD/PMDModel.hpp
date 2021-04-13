//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXConfig.hpp
///             @brief  PMXFile Config
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMD_MODEL_HPP
#define PMD_MODEL_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/Model.hpp"
#include "GameCore/Include/Model/MMD/PMDFile.hpp"
#include <DirectXMath.h>
#include <Windows.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using VertexBuffer   = std::unique_ptr<UploadBuffer<PMDVertex>>;
using IndexBuffer    = std::unique_ptr<UploadBuffer<UINT16>>;
using MaterialBuffer = std::unique_ptr<UploadBuffer<PMDMaterial>>;
class PMDModel : public DefaultModel
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Initialize(const std::wstring& filePath);

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
	bool PrepareVertexBuffer();
	bool PrepareIndexBuffer();
	bool PrepareMaterialBuffer();
	

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
private:
	/****************************************************************************
	**                PrivateFunction
	*****************************************************************************/
	std::shared_ptr<PMDData>              _pmdData = nullptr;
	VertexBuffer                          _vertexBuffer[FRAME_BUFFER_COUNT];
	MaterialBuffer                        _materialBuffer;
	std::unordered_map<std::string, UINT> _materialView;
	
	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/

};

#endif