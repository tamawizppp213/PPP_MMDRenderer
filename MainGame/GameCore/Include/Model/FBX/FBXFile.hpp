//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12FBXFile.hpp
///             @brief  FBXFile 
///             @author Toide Yutaro
///             @date   2020_12_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef FBX_FILE_HPP
#define FBX_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "GameCore/Include/Model/ModelFile.hpp"
#include <fbxsdk/fbxsdk.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <vector>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ObjFile
*************************************************************************//**
*  @class     ObjFile
*  @brief     Obj File Loader
*****************************************************************************/
class FBXData final : public ModelData
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Load3DModel(const std::wstring& filePath) override;

#pragma region Property

#pragma endregion Property
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	~FBXData();
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	bool CreateFBXManager();
	bool CreateFBXScene();
	bool CreateFBXImporter();

	bool OpenFBXFile(const std::wstring& filePath);
	bool LoadFBXVertex();


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	fbxsdk::FbxManager*  _fbxManager  = nullptr;
	fbxsdk::FbxImporter* _fbxImporter = nullptr;
	fbxsdk::FbxScene*    _fbxScene    = nullptr;
	fbxsdk::FbxNode*     _fbxMeshNode = nullptr;
	fbxsdk::FbxMesh*     _fbxMesh     = nullptr;

	std::vector<VertexPositionNormalColorTexture> _vertex;
};
#endif