//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12ModelLoader.hpp
///             @brief  Load Model
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12Core.hpp"
#include <string>
#include <unordered_map>
#include <memory>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  		ModelTable (Singleton)
*************************************************************************//**
*  @class     ModelTableManager
*  @brief     Model Table
*****************************************************************************/
class PMDData;
class PMXData;
class OBJData;
class FBXData;
class ModelTableManager
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void Clear();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::unordered_map<std::wstring, std::shared_ptr<PMDData>> ModelTablePMD;
	std::unordered_map<std::wstring, std::shared_ptr<PMXData>> ModelTablePMX;
	std::unordered_map<std::wstring, std::shared_ptr<OBJData>> ModelTableObj;
	std::unordered_map<std::wstring, std::shared_ptr<FBXData>> ModelTableFbx;
	/***************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	static ModelTableManager& Instance()
	{
		static ModelTableManager modelTable;
		return modelTable;
	}
	// Prohibit move and copy.
	ModelTableManager(const ModelTableManager&)            = delete;
	ModelTableManager& operator=(const ModelTableManager&) = delete;
	ModelTableManager(ModelTableManager&&)                 = delete;
	ModelTableManager& operator=(ModelTableManager&&)      = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	ModelTableManager()  = default;
	~ModelTableManager() = default;

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/

};

/****************************************************************************
*				  		Model Loader
*************************************************************************//**
*  @class     Model Loader
*  @brief     Model Loader
*****************************************************************************/
class ModelLoader
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	void Load3DModel(const std::wstring& filePath, std::shared_ptr<PMXData>* pmxData);
	void Load3DModel(const std::wstring& filePath, std::shared_ptr<PMDData>* pmdData);
	void Load3DModel(const std::wstring& filePath, std::shared_ptr<FBXData>* fbxData);
	void Load3DModel(const std::wstring& filePath, std::shared_ptr<OBJData>* objData);

	/***************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	ModelTableManager& _modelTableManager = ModelTableManager::Instance();

};
#endif