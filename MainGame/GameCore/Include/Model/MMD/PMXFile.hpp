//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXFile.hpp
///             @brief  PMXFile Loader
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMX_FILE_HPP
#define PMX_FILE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "PMXConfig.hpp"
#include "GameCore/Include/Model/ModelFile.hpp"
#include <d3d12.h>
#include <vector>
#include <iostream>
#include <array>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
class PMXData final : public ModelData
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Load3DModel(const std::wstring& filePath) override;

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
	bool LoadPMXHeader         (FILE* filePtr);
	bool LoadPMXInfo           (FILE* filePtr);
	bool LoadPMXVertex         (FILE* filePtr);
	bool LoadPMXFace           (FILE* filePtr);
	bool LoadPMXTexture        (FILE* filePtr);
	bool LoadPMXMaterial       (FILE* filePtr);
	bool LoadPMXBone           (FILE* filePtr);
	bool LoadPMXFaceExpression (FILE* filePtr);
	bool LoadPMXDisplayFrame   (FILE* filePtr);
	bool LoadPMXRigidBody      (FILE* filePtr);
	bool LoadPMXJoint          (FILE* filePtr);
	bool LoadPMXSoftBody       (FILE* filePtr);

	bool ReadPMXString(FILE* filePtr, std::string* string);
	bool ReadPMXIndex (FILE* filePtr, INT32* pmxIndex, UINT8 indexSize);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	pmx::PMXHeader                      _header;
	pmx::PMXInfo                        _info;
	std::vector<pmx::PMXVertex>         _vertices;
	std::vector<pmx::PMXFace>           _faces;
	std::vector<pmx::PMXTexture>        _textures;
	std::vector<pmx::PMXMaterial>       _materials;
	std::vector<pmx::PMXBone>           _bones;
	std::vector<pmx::PMXFaceExpression> _faceExpressions;
	std::vector<pmx::PMXDisplayFrame>   _displayFrames;
	std::vector<pmx::PMXRigidBody>      _rigidBodies;
	std::vector<pmx::PMXJoint>          _joints;
	std::vector<pmx::PMXSoftBody>       _softBodies;
	std::string _directory;
};
#endif