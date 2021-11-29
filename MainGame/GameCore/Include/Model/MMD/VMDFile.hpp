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
using MotionMap   = std::unordered_map<std::string, std::vector<VMDKeyFrame>>;
using MorphingMap = std::unordered_map<std::string, std::vector<VMDKeyFrameMorph>>;

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
	UINT32 GetAnimationDuration()                    { return _animationDuration;}
	const MotionMap&          GetMotionMap()   const { return _motionMap; }
	const MorphingMap&        GetMorphingMap() const { return _morphingMap; }
	std::vector<VMDKeyFrame>* GetMotionData(const std::string& boneName) { return &_motionMap.at(boneName); }
	const std::vector<VMDIK>& GetEnableIKData() { return _iks; }
	
	bool IsExistedMorphingMap() { return !_morphingMap.empty(); }
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
	MotionMap      _motionMap;
	MorphingMap    _morphingMap;
	std::vector<vmd::VMDCamera> _cameras;
	std::vector<vmd::VMDLight>  _lights;
	std::vector<vmd::VMDShadow> _shadows;
	std::vector<VMDIK>          _iks;

	UINT32 _animationDuration = 0;

};
#endif