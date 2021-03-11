//////////////////////////////////////////////////////////////////////////////////
///             @file   DirectX12PMDFile.cpp
///             @brief  PMDFile 
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/PMDFile.hpp"
#include "GameCore/Include/File/FileUtility.hpp"
#include <iostream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <filesystem>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Public Function
/****************************************************************************
*							Load3DModel
*************************************************************************//**
*  @fn        bool PMDData::Load3DModel(const std::wstring& filePath)
*  @brief     Load PMD Model Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::Load3DModel(const std::wstring& filePath)
{
	using namespace pmd;

	/*-------------------------------------------------------------------
	-             Open File
	---------------------------------------------------------------------*/
	FILE* filePtr = file::OpenFile(filePath);
	std::filesystem::path directory = filePath;
	_directory = directory.parent_path().string();

	/*-------------------------------------------------------------------
	-             Load Header Data
	---------------------------------------------------------------------*/
	if (!LoadPMDHeader(filePtr))         { MessageBox(NULL, L" PMDHeader cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Vertex Data
	---------------------------------------------------------------------*/
	if (!LoadPMDVertex(filePtr))         { MessageBox(NULL, L" PMDVertex cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Index Data
	---------------------------------------------------------------------*/
	if (!LoadPMDIndex(filePtr))          { MessageBox(NULL, L" PMDIndex cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Material Data
	---------------------------------------------------------------------*/
	if (!LoadPMDMaterial(filePtr))       { MessageBox(NULL, L" PMDMaterial cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Bone Data
	---------------------------------------------------------------------*/
	if (!LoadPMDBone(filePtr))           { MessageBox(NULL, L" PMDBone cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             LoadBoneIKData (!note: It's taking a long time to load.)
	---------------------------------------------------------------------*/
	if (!LoadPMDBoneIK(filePtr))         { MessageBox(NULL, L" PMDBoneIK cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load FaceExpression Data
	---------------------------------------------------------------------*/
	if (!LoadPMDFaceExpression(filePtr)) { MessageBox(NULL, L" PMDFaceExpression cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load PMDFaceLabels Data
	---------------------------------------------------------------------*/
	if (!LoadPMDFaceLabels(filePtr))     { MessageBox(NULL, L" PMDFaceLabels cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load PMD Bone Label List Data
	---------------------------------------------------------------------*/
	if (!LoadPMDBoneLabels(filePtr))     { MessageBox(NULL, L" PMDBoneLabels cannot be read.", L"Warning", MB_ICONWARNING); };

	/*-------------------------------------------------------------------
	-             Load Localize Data (the following data is extented data)
	---------------------------------------------------------------------*/
	if (!LoadPMDLocalizeData(filePtr)) { };

	/*-------------------------------------------------------------------
	-             Load PMD ToonTextureList
	---------------------------------------------------------------------*/
	if (!LoadPMDToonTextureList(filePtr)) { };

	/*-------------------------------------------------------------------
	-             Load RigidBody
	---------------------------------------------------------------------*/
	if (!LoadPMDRigidBody(filePtr)) { };

	/*-------------------------------------------------------------------
	-             Load Localize Data
	---------------------------------------------------------------------*/
	if (!LoadPMDJointList(filePtr)) { };

	/*-------------------------------------------------------------------
	-             Close File
	---------------------------------------------------------------------*/
	fclose(filePtr);
	return true;
}


#pragma region Property
#pragma endregion Property
#pragma endregion Public Function

#pragma region Protected Function

#pragma endregion Protected Function

#pragma region Private Function
/****************************************************************************
*							LoadPMDHeader
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDHeader(FILE* filePtr)
*  @brief     Load PMD Header Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDHeader(FILE* filePtr)
{

	fread_s(&_header, sizeof(_header), sizeof(pmd::PMDHeader), 1, filePtr);
	return true;
}

/****************************************************************************
*							LoadPMDVertex
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDVertex(FILE* filePtr)
*  @brief     Load PMD Vertex Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDVertex(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Vertex Count
	---------------------------------------------------------------------*/
	UINT32 vertexCount = 0;
	fread_s(&vertexCount, sizeof(vertexCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load VertexData
	---------------------------------------------------------------------*/
	_vertices.resize(vertexCount);
	fread_s(_vertices.data(), sizeof(pmd::PMDVertex) * _vertices.size(), sizeof(pmd::PMDVertex), _vertices.size(), filePtr);
	
	return true;
}

/****************************************************************************
*							LoadPMDIndex
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDIndex(FILE* filePtr)
*  @brief     Load PMD Index Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDIndex(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Index Count
	---------------------------------------------------------------------*/
	UINT32 indexCount = 0;
	fread_s(&indexCount, sizeof(indexCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Indices Data
	---------------------------------------------------------------------*/
	_indices.resize(indexCount);
	fread_s(_indices.data(), sizeof(UINT16) * _indices.size(), sizeof(UINT16), _indices.size(), filePtr);

	return true;
}

/****************************************************************************
*							LoadPMDMaterial
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDMaterial(FILE* filePtr)
*  @brief     Load PMD Material Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDMaterial(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Material Count
	---------------------------------------------------------------------*/
	UINT32 materialCount = 0;
	fread_s(&materialCount, sizeof(materialCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Material Data
	---------------------------------------------------------------------*/
	_materials.resize(materialCount);
	fread_s(_materials.data(), sizeof(pmd::PMDMaterial) * _materials.size(), sizeof(pmd::PMDMaterial), _materials.size(), filePtr);
	
	return true;
}

/****************************************************************************
*							LoadPMDBone
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDBone(FILE* filePtr)
*  @brief     Load PMD Bone Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDBone(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Bone Count
	---------------------------------------------------------------------*/
	UINT16 boneCount = 0;
	fread_s(&boneCount, sizeof(boneCount), sizeof(UINT16), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Bone Data
	---------------------------------------------------------------------*/
	_bones.resize(boneCount);
	fread_s(_bones.data(), sizeof(pmd::PMDBone) * _bones.size(), sizeof(pmd::PMDBone), _bones.size(), filePtr);

	return true;
}

/****************************************************************************
*							LoadPMDBoneIK
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDBoneIK(FILE* filePtr)
*  @brief     Load PMD Header Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDBoneIK(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Bone IK Count
	---------------------------------------------------------------------*/
	UINT16 boneIKCount = 0;
	fread_s(&boneIKCount, sizeof(boneIKCount), sizeof(UINT16), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Bone IK
	---------------------------------------------------------------------*/
	_boneIKs.resize(boneIKCount);
	for (auto& ik : _boneIKs)
	{
		fread_s(&ik.IKBoneID      , sizeof(ik.IKBoneID)      , sizeof(UINT16), 1, filePtr);
		fread_s(&ik.IKTargetBoneID, sizeof(ik.IKTargetBoneID), sizeof(UINT16), 1, filePtr);
		fread_s(&ik.IKChainLength , sizeof(ik.IKChainLength) , sizeof(UINT8) , 1, filePtr);
		fread_s(&ik.IterationCount, sizeof(ik.IterationCount), sizeof(UINT16), 1, filePtr);
		fread_s(&ik.AngleLimit    , sizeof(ik.AngleLimit)    , sizeof(float) , 1, filePtr);

		ik.Chains.resize(ik.IKChainLength);
		for (auto& chain : ik.Chains)
		{
			fread_s(&chain, sizeof(chain), sizeof(UINT16), 1, filePtr);
		}
	}
	return true;
}

/****************************************************************************
*							LoadPMDFaceExpression
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDFaceExpression(FILE* filePtr)
*  @brief     Load PMD Face Morph Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDFaceExpression(FILE* filePtr)
{
	using namespace DirectX;

	/*-------------------------------------------------------------------
	-             Load Face Count
	---------------------------------------------------------------------*/
	UINT16 faceCount = 0;
	fread_s(&faceCount, sizeof(faceCount), sizeof(UINT16), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Face Morph Data
	---------------------------------------------------------------------*/
	_faces.resize(faceCount);
	for (auto& face : _faces)
	{
		fread_s(&face.FaceExpressionName, sizeof(face.FaceExpressionName), sizeof(char)         , 20, filePtr);
		fread_s(&face.VertexNum         , sizeof(face.VertexNum)         , sizeof(UINT32)       , 1 , filePtr);
		fread_s(&face.FaceExpressionType, sizeof(face.FaceExpressionType), sizeof(pmd::FacePart), 1 , filePtr);
		face.Vertices.resize(face.VertexNum);
		face.Indices .resize(face.VertexNum);
		for (int i = 0; i < face.VertexNum; ++i)
		{
			switch (face.FaceExpressionType)
			{
			case pmd::FacePart::Base: // index, vertex Position
				fread_s(&face.Vertices[i], sizeof(face.Vertices[i]), sizeof(XMFLOAT3), 1, filePtr);
				fread_s(&face.Indices [i], sizeof(face.Indices[i]) , sizeof(UINT32)  , 1, filePtr);
				break;
			default: // index => index about FacePart::Base, vertex => offset Position
				fread_s(&face.Vertices[i], sizeof(face.Vertices[i]), sizeof(XMFLOAT3), 1, filePtr);
				fread_s(&face.Indices [i], sizeof(face.Indices[i] ), sizeof(UINT32)  , 1, filePtr);
				break;
			}
		}
	}

	return true;
}

/****************************************************************************
*							LoadPMDFaceLabels
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDFaceLabels(FILE* filePtr)
*  @brief     Load PMD Face Labels Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDFaceLabels(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Face List Count
	---------------------------------------------------------------------*/
	UINT8 faceListCount = 0;
	fread_s(&faceListCount, sizeof(faceListCount), sizeof(UINT8), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Face Label Indices
	---------------------------------------------------------------------*/
	_faceLabelIndices.resize(faceListCount);
	fread_s(_faceLabelIndices.data(), sizeof(pmd::PMDFaceLabelIndex) * _faceLabelIndices.size(), sizeof(pmd::PMDFaceLabelIndex), _faceLabelIndices.size(), filePtr);

	return true;
}

/****************************************************************************
*							LoadPMDBoneLabels
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDBoneLabels(FILE* filePtr)
*  @brief     Load PMD Bone Labels Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDBoneLabels(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Bone List Count
	---------------------------------------------------------------------*/
	UINT8 boneListCount = 0;
	fread_s(&boneListCount, sizeof(boneListCount), sizeof(UINT8), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Bone Label Indices
	---------------------------------------------------------------------*/
	_boneLabelIndices.resize((UINT64)boneListCount + 1);
	bool first = true;
	for (auto& boneLabel : _boneLabelIndices)
	{
		if (first) { first = false; }
		else
		{
			fread_s(&boneLabel.BoneName, sizeof(boneLabel.BoneName), sizeof(char), 50, filePtr);
		}
	}

	/*-------------------------------------------------------------------
	-             Load Bone Display Count
	---------------------------------------------------------------------*/
	UINT32 displayCount = 0;
	fread_s(&displayCount, sizeof(displayCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Bone Labels
	---------------------------------------------------------------------*/
	for (UINT32 displayIndex = 0; displayIndex < displayCount; ++displayIndex)
	{
		UINT16 boneIndex = 0;
		fread_s(&boneIndex, sizeof(boneIndex), sizeof(UINT16), 1, filePtr);

		UINT8 frameIndex = 0;
		fread_s(&frameIndex, sizeof(frameIndex), sizeof(UINT8), 1, filePtr);
		_boneLabelIndices[frameIndex].BoneLabelIndices.push_back(boneIndex);
	}

	return true;
}

/****************************************************************************
*							LoadPMDLocalizeData
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDLocalizeData(FILE* filePtr)
*  @brief     Load PMD Localize Data (extended data)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDLocalizeData(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Flag
	---------------------------------------------------------------------*/
	fread_s(&_localizeInfo.LocalizeFlag, sizeof(_localizeInfo.LocalizeFlag), sizeof(UINT8), 1, filePtr);

	if (_localizeInfo.LocalizeFlag != 0)
	{
		/*-------------------------------------------------------------------
		-             Load Header
		---------------------------------------------------------------------*/
		fread_s(&_localizeInfo.ModelName, sizeof(_localizeInfo.ModelName), sizeof(char), sizeof(_localizeInfo.ModelName), filePtr);
		fread_s(&_localizeInfo.Comment  , sizeof(_localizeInfo.Comment)  , sizeof(char), sizeof(_localizeInfo.Comment)  , filePtr);

		/*-------------------------------------------------------------------
		-             Load Bone Name
		---------------------------------------------------------------------*/
		for (auto& bone : _bones)
		{
			fread_s(&bone.EnglishBoneName, sizeof(bone.EnglishBoneName), sizeof(char), sizeof(bone.EnglishBoneName), filePtr);
		}

		/*-------------------------------------------------------------------
		-             Load Face Labels
		---------------------------------------------------------------------*/
		size_t faceCount = _faces.size();
		for (size_t faceIndex = 1; faceIndex < faceCount; ++faceIndex)
		{
			auto& face = _faces[faceIndex];
			fread_s(&face.EnglishFaceExpressionName, sizeof(face.EnglishFaceExpressionName), sizeof(char), sizeof(face.EnglishFaceExpressionName), filePtr);
		}

		/*-------------------------------------------------------------------
		-             Load Bone Labels
		---------------------------------------------------------------------*/
		size_t boneLabelCount = _boneLabelIndices.size();
		for (size_t boneLabelIndex = 1; boneLabelIndex < boneLabelCount; ++boneLabelIndex)
		{
			auto& label = _boneLabelIndices[boneLabelIndex];
			fread_s(&label, sizeof(label.BoneEnglishName), sizeof(char), sizeof(label.BoneEnglishName), filePtr);
		}
	}
	return true;
}

/****************************************************************************
*							LoadPMDToonTextureList
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDToonTextureList(FILE* filePtr)
*  @brief     Load PMD ToonTexture List Data (extended data)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDToonTextureList(FILE* filePtr)
{
	size_t toonTextureIndex = 1;

	/*-------------------------------------------------------------------
	-             Load Toon Texture Names (default Name)
	---------------------------------------------------------------------*/
	for (auto& toonTextureName : _toonTextureNames)
	{
		std::stringstream stringStream;
		stringStream << "toon" << std::setfill('0') << std::setw(2) << toonTextureIndex << ".bmp";

		std::string fileName = _directory + "/" + stringStream.str();
		for (int i = 0; i < fileName.size(); ++i)
		{
			toonTextureName[i] = fileName[i];
		}
		toonTextureIndex++;
	}


	if (feof(filePtr) != 0) { return true; }

	/*-------------------------------------------------------------------
	-             Load Toon Texture Names (extended)
	---------------------------------------------------------------------*/
	for (auto& toonTextureName : _toonTextureNames)
	{
		fread_s(&toonTextureName, sizeof(toonTextureName), sizeof(char), sizeof(toonTextureName), filePtr);
	}

	return true;
}

/****************************************************************************
*							LoadPMDRigidBody
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDRigidBody(FILE* filePtr)
*  @brief     Load PMD RigidBody Data (extended data)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDRigidBody(FILE* filePtr)
{
	using namespace DirectX;
	if (feof(filePtr) != 0) { return false; }

	/*-------------------------------------------------------------------
	-             Load RigidBody Count
	---------------------------------------------------------------------*/
	UINT32 rigidBodyCount = 0;
	fread_s(&rigidBodyCount, sizeof(rigidBodyCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load RigidBody Data
	---------------------------------------------------------------------*/
	_rigidBodies.resize(rigidBodyCount);
	for (auto& rigidBody : _rigidBodies)
	{
		fread_s(&rigidBody, sizeof(rigidBody), sizeof(pmd::PMDRigidBody), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadPMDJointList
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDJointList(FILE* filePtr)
*  @brief     Load PMD Joint List Data (extended data)
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMDData::LoadPMDJointList(FILE* filePtr)
{
	if (feof(filePtr) != 0) { return false; }

	/*-------------------------------------------------------------------
	-             Load Joint Count
	---------------------------------------------------------------------*/
	UINT32 jointCount = 0;
	fread_s(&jointCount, sizeof(jointCount), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Loint List
	---------------------------------------------------------------------*/
	_jointList.resize(jointCount);
	for (auto& joint : _jointList)
	{
		fread_s(&joint, sizeof(joint), sizeof(pmd::PMDJoint), 1, filePtr);
	}
	return true;
}

#pragma endregion Private Functoon