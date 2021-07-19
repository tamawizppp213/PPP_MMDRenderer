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
#include <iostream>
#include <map>
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
	bool SetFBXConfig();
	bool LoadFBXVertex();
	
	void CollectMeshNode(fbxsdk::FbxNode* node, std::map<std::string, fbxsdk::FbxNode*>& nodeList);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	fbxsdk::FbxManager*  _fbxManager  = nullptr;
	fbxsdk::FbxImporter* _fbxImporter = nullptr;
	fbxsdk::FbxScene*    _fbxScene    = nullptr;
	fbxsdk::FbxNode*     _fbxMeshNode = nullptr;
	fbxsdk::FbxMesh*     _fbxMesh     = nullptr;
	fbxsdk::FbxNode*     _fbxRootNode = nullptr;

	std::vector<VertexPositionNormalTexture> _vertex;
};
#endif