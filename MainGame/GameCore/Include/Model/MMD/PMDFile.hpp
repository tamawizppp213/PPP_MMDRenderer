//////////////////////////////////////////////////////////////////////////////////
///             @file   PMDFile.hpp
///             @brief  PMDFile Loader
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMD_FILE_HPP
#define PMD_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "PMDConfig.hpp"
#include "GameCore/Include/Model/ModelFile.hpp"
#include <d3d12.h>
#include <vector>
#include <iostream>
#include <array>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*				  			PMDFile
*************************************************************************//**
*  @class     PMDData
*  @brief     PMD Data
*****************************************************************************/
class PMDData final : public ModelData
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Load3DModel(const std::wstring& filePath) override;
	
#pragma region Property
	const std::vector<PMDVertex>& GetVertex() const { return _vertices; }
	const std::vector<UINT16>   & GetIndex()  const { return _indices;  }
	const PMDVertex& GetPMDVertex(int index)  const { return _vertices[index]; }

#pragma endregion Property
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
	bool LoadPMDHeader         (FILE* filePtr);
	bool LoadPMDVertex         (FILE* filePtr);
	bool LoadPMDIndex          (FILE* filePtr);
	bool LoadPMDMaterial       (FILE* filePtr);
	bool LoadPMDBone           (FILE* filePtr);
	bool LoadPMDBoneIK         (FILE* filePtr);
	bool LoadPMDFaceExpression (FILE* filePtr);
	bool LoadPMDFaceLabels     (FILE* filePtr);
	bool LoadPMDBoneLabels     (FILE* filePtr);
	bool LoadPMDLocalizeData   (FILE* filePtr);
	bool LoadPMDToonTextureList(FILE* filePtr);
	bool LoadPMDRigidBody      (FILE* filePtr);
	bool LoadPMDJointList      (FILE* filePtr);

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	pmd::PMDHeader                      _header;
	std::vector<PMDVertex>              _vertices;
	std::vector<UINT16>                 _indices;
	std::vector<pmd::PMDMaterial>       _materials;
	std::vector<pmd::PMDBone>           _bones;
	std::vector<pmd::PMDBoneIK>         _boneIKs;
	std::vector<pmd::PMDFaceExpression> _faces;
	std::vector<pmd::PMDFaceLabelIndex> _faceLabelIndices;
	std::vector<pmd::PMDBoneLabelList>  _boneLabelIndices;
	std::vector<pmd::PMDRigidBody>      _rigidBodies;
	std::vector<pmd::PMDJoint>          _jointList;
	std::array<char[100], 10>           _toonTextureNames;
	pmd::PMDLocalizeInfo                _localizeInfo;
	std::string _directory;
};

#endif