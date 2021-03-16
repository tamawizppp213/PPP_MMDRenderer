//////////////////////////////////////////////////////////////////////////////////
///             @file   VMDFile.hpp
///             @brief  Load VPDFile
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef VMD_FILE_HPP
#define VMD_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "VMDConfig.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                               Class
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			     VPDFile
*************************************************************************//**
*  @class     VMDFile
*  @brief     LoadVPDFile
*****************************************************************************/
class VMDFile
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool LoadVMDFile(const std::wstring& filePath);

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
	bool LoadVMDHeader     (FILE* filePtr);
	bool LoadVMDMotion     (FILE* filePtr);
	bool LoadVMDBlendShape (FILE* filePtr);
	bool LoadVMDCamera     (FILE* filePtr);
	bool LoadVMDLight      (FILE* filePtr);
	bool LoadVMDShadow     (FILE* filePtr);
	bool LoadVMDIK         (FILE* filePtr);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::string _directory;
	vmd::VMDHeader _header;
	std::vector<vmd::VMDMotion> _motions;
	std::vector<vmd::VMDMorph>  _morphs;
	std::vector<vmd::VMDCamera> _cameras;
	std::vector<vmd::VMDLight>  _lights;
	std::vector<vmd::VMDShadow> _shadows;
	std::vector<vmd::VMDIK>     _iks;

};
#endif