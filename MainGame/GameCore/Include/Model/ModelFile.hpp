//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12ModelFile.hpp
///             @brief  3D Model File Abstract Class (.obj, .fbx, .pmd...)) 
///             @author Toide Yutaro
///             @date   2020_12_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MODEL_FILE_HPP
#define MODEL_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			ModelData 
*************************************************************************//**
*  @class     ModelData
*  @brief     ModelData Abstract Class
*****************************************************************************/
class ModelData
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	virtual bool Load3DModel(const std::wstring& filePath) = 0;
	
#pragma region Property

#pragma endregion Property
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

	/****************************************************************************
	**                Protected Member Variables
	*****************************************************************************/
	std::string _modelName;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};
#endif
