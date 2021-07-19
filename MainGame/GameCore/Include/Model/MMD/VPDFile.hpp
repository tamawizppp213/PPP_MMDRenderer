//////////////////////////////////////////////////////////////////////////////////
///             @file   VPDFile.hpp
///             @brief  Load VPDFile
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef VPD_FILE_HPP
#define VPD_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "VPDConfig.hpp"
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                               Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			     VPDFile
*************************************************************************//**
*  @class     VPDFile
*  @brief     LoadVPDFile
*****************************************************************************/
class VPDFile
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool LoadVPDFile(const std::wstring& filePath);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::string _directory;
};

#endif