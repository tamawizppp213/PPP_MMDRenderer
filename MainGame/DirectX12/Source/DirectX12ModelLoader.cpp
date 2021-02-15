//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12ModelLoader.hpp
///             @brief  Load Model
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/DirectX12ModelLoader.hpp"
#include "DirectX12/Include/Core/DirectX12Debug.hpp"
#include "DirectX12/Include/DirectX12PMDFile.hpp"
#include "DirectX12/Include/DirectX12OBJFile.hpp"
#include "DirectX12/Include/DirectX12FBXFile.hpp"
#include "DirectX12/Include/DirectX12ModelFile.hpp"
#include <iostream>
#include <stdexcept>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*                       Load3DModel
*************************************************************************//**
*  @fn        void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<PMDData>* pmdData)
*  @brief     LoadPMDData
*  @param[in] const std::wstring& filePath
*  @param[out]std::shared_ptr<PMDData>* pmdData
*  @return 　　void
*****************************************************************************/
void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<PMDData>* pmdData)
{
	/*-------------------------------------------------------------------
	-               If the file is loaded once, read from it
	---------------------------------------------------------------------*/
	if (_modelTableManager.Instance().ModelTablePMD.find(filePath) != _modelTableManager.Instance().ModelTablePMD.end())
	{
		*pmdData = _modelTableManager.Instance().ModelTablePMD[filePath];
		return;
	}

	/*-------------------------------------------------------------------
	-       Choose extension and load 3Dmodel data
	---------------------------------------------------------------------*/
	std::wstring extension = GetExtension(filePath);
	if (extension == L"pmd")
	{
		// Load 3D model data (.pmd)
		std::shared_ptr<PMDData> pmdDataPtr = std::make_shared<PMDData>();
		pmdDataPtr->Load3DModel(filePath);

		// Add model table
		_modelTableManager.Instance().ModelTablePMD[filePath] = pmdDataPtr;
		*pmdData = pmdDataPtr;
	}
	else
	{
		auto error = std::invalid_argument("Invalid file path was specified.");
		std::cout << error.what() << std::endl;
	}
	
}

/****************************************************************************
*                       Load3DModel
*************************************************************************//**
*  @fn        void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<FBXData>* pmdData)
*  @brief     LoadFBXData
*  @param[in] const std::wstring& filePath
*  @param[out]std::shared_ptr<FBXData>* pmdData
*  @return 　　void
*****************************************************************************/
void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<FBXData>* fbxData)
{
	/*-------------------------------------------------------------------
	-               If the file is loaded once, read from it
	---------------------------------------------------------------------*/
	if (_modelTableManager.Instance().ModelTableFbx.find(filePath) != _modelTableManager.Instance().ModelTableFbx.end())
	{
		*fbxData = _modelTableManager.Instance().ModelTableFbx[filePath];
		return;
	}

	/*-------------------------------------------------------------------
	-       Choose extension and load 3Dmodel data 
	---------------------------------------------------------------------*/
	std::wstring extension = GetExtension(filePath);
	if (extension == L"fbx")
	{
		// Load 3D model data (.fbx)
		std::shared_ptr<FBXData> fbxDataPtr = std::make_shared<FBXData>();
		fbxDataPtr->Load3DModel(filePath);

		// Add model table
		_modelTableManager.Instance().ModelTableFbx[filePath] = fbxDataPtr;
	}
	else
	{
		auto error = std::invalid_argument("Invalid file path was specified.");
		std::cout << error.what() << std::endl;
	}
}

/****************************************************************************
*                       Load3DModel
*************************************************************************//**
*  @fn        void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<ObjData>* pmdData)
*  @brief     LoadObjData
*  @param[in] const std::wstring& filePath
*  @param[out]std::shared_ptr<ObjData>* objData
*  @return 　　void
*****************************************************************************/
void ModelLoader::Load3DModel(const std::wstring& filePath, std::shared_ptr<OBJData>* objData)
{
	/*-------------------------------------------------------------------
	-               If the file is loaded once, read from it
	---------------------------------------------------------------------*/
	if (_modelTableManager.Instance().ModelTableObj.find(filePath) != _modelTableManager.Instance().ModelTableObj.end())
	{
		*objData = _modelTableManager.Instance().ModelTableObj[filePath];
		return;
	}

	/*-------------------------------------------------------------------
	-       Choose extension and load 3Dmodel data 
	---------------------------------------------------------------------*/
	std::wstring extension = GetExtension(filePath);
	if (extension == L"obj")
	{
		// Load 3D model data (.obj)
		std::shared_ptr<OBJData> objDataPtr = std::make_shared<OBJData>();
		objDataPtr->Load3DModel(filePath);

		// Add model table
		_modelTableManager.Instance().ModelTableObj[filePath] = objDataPtr;
	}
	else
	{
		auto error = std::invalid_argument("Invalid file path was specified.");
		std::cout << error.what() << std::endl;
	}
}