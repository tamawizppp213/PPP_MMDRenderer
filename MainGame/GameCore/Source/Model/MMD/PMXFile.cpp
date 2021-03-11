//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXFile.cpp
///             @brief  PMXFile 
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/PMXFile.hpp"
#include "GameCore/Include/File/FileUtility.hpp"
#include "GameCore/Include/File/UnicodeUtility.hpp"
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
*  @fn        bool PMXData::Load3DModel(const std::wstring& filePath)
*  @brief     Load PMX 3D model
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::Load3DModel(const std::wstring& filePath)
{
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Open File
	---------------------------------------------------------------------*/
	FILE* filePtr = file::OpenFile(filePath);
	std::filesystem::path directory = filePath;
	_directory = directory.parent_path().string();

	/*-------------------------------------------------------------------
	-             Load Header Data
	---------------------------------------------------------------------*/
	if (!LoadPMXHeader(filePtr)) { MessageBox(NULL, L" PMXHeader cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load PMXInfo Data
	---------------------------------------------------------------------*/
	if (!LoadPMXInfo  (filePtr)) { MessageBox(NULL, L" PMXInfo cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load PMXVertex Data
	---------------------------------------------------------------------*/
	if (!LoadPMXVertex(filePtr)) { MessageBox(NULL, L" PMXVertex cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Face Data
	---------------------------------------------------------------------*/
	if (!LoadPMXFace(filePtr)) { MessageBox(NULL, L" PMXFace cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Texture Data
	---------------------------------------------------------------------*/
	if (!LoadPMXTexture(filePtr)) { MessageBox(NULL, L" PMXTexture cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Material Data
	---------------------------------------------------------------------*/
	if (!LoadPMXMaterial(filePtr)) { MessageBox(NULL, L" PMXMaterial cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Bone Data
	---------------------------------------------------------------------*/
	if (!LoadPMXBone(filePtr)) { MessageBox(NULL, L" PMXBone cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load FaceExpression Data
	---------------------------------------------------------------------*/
	if (!LoadPMXFaceExpression(filePtr)) { MessageBox(NULL, L" PMXFaceExpression cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load DisplayFrame Data
	---------------------------------------------------------------------*/
	if (!LoadPMXDisplayFrame(filePtr)) { MessageBox(NULL, L" PMXDisplayFrame cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load RigidBody Data
	---------------------------------------------------------------------*/
	if (!LoadPMXRigidBody(filePtr)) { MessageBox(NULL, L" PMXRigidBody cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Joint Data
	---------------------------------------------------------------------*/
	if (!LoadPMXJoint(filePtr)) { MessageBox(NULL, L" PMXJoint cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load SoftBody Data
	---------------------------------------------------------------------*/
	if (!LoadPMXSoftBody(filePtr)) { };
	return true;
}

#pragma endregion Public Function
#pragma region Private Function
/****************************************************************************
*							LoadPMXHeader
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXHeader(FILE* filePtr)
*  @brief     Load PMX Header
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXHeader(FILE* filePtr)
{
	fread_s(&_header, sizeof(_header), sizeof(pmx::PMXHeader), 1, filePtr);
	return true;
}

/****************************************************************************
*							LoadPMXInfo
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXInfo(FILE* filePtr)
*  @brief     Load PMX Infomation
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXInfo(FILE* filePtr)
{
	if (!ReadPMXString(filePtr, &_info.ModelName))        { return false; };
	if (!ReadPMXString(filePtr, &_info.EngliseModelName)) { return false; };
	if (!ReadPMXString(filePtr, &_info.Comment))          { return false; };
	if (!ReadPMXString(filePtr, &_info.EnglishComment))   { return false; };
	return true;
}

/****************************************************************************
*							LoadPMXVertex
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXVertex(FILE* filePtr)
*  @brief     Load PMX Vertex
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXVertex(FILE* filePtr)
{
	using namespace DirectX;
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Load Vertex Count
	---------------------------------------------------------------------*/
	INT32 vertexCount = 0;
	fread_s(&vertexCount, sizeof(vertexCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Vertex Data
	---------------------------------------------------------------------*/
	_vertices.resize(vertexCount);
	for (auto vertex : _vertices)
	{
		/*-------------------------------------------------------------------
		-             Load Position , Normal, UV, and Add UV
		---------------------------------------------------------------------*/
		fread_s(&vertex.Position, sizeof(vertex.Position), sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&vertex.Normal  , sizeof(vertex.Normal  ), sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&vertex.UV      , sizeof(vertex.UV      ), sizeof(XMFLOAT2), 1, filePtr);

		for (UINT8 i = 0; i < _header.AddUVCount; ++i)
		{
			fread_s(&vertex.AddUV[i], sizeof(vertex.AddUV[i]), sizeof(XMFLOAT4), 1, filePtr);
		}

		fread_s(&vertex.WeightType, sizeof(vertex.WeightType), sizeof(UINT8), 1, filePtr);
		switch (vertex.WeightType)
		{
			/*-------------------------------------------------------------------
			-             Weight: BDEF 1 (boneIndices[0])
			---------------------------------------------------------------------*/
			case PMXVertexWeight::BDEF1:
			{
				ReadPMXIndex(filePtr, &vertex.BoneIndices[0], _header.BoneIndexSize);
				break;
			}
			/*-------------------------------------------------------------------
			-             Weight: BDEF 2 (boneIndices[0-1] + boneWeights[0]  (PMD Format weight(0.0-1.0))
			---------------------------------------------------------------------*/
			case PMXVertexWeight::BDEF2:
			{
				ReadPMXIndex(filePtr, &vertex.BoneIndices[0], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[1], _header.BoneIndexSize);
				fread_s(&vertex.BoneWeights[0], sizeof(vertex.BoneWeights[0]), sizeof(float), 1, filePtr);
				break;
			}
			/*-------------------------------------------------------------------
			-             Weight: BDEF 4 (boneIndices[0-3] + boneWeights[0-3] (may not weight total = 1.0))
			---------------------------------------------------------------------*/
			case PMXVertexWeight::BDEF4:
			{
				ReadPMXIndex(filePtr, &vertex.BoneIndices[0], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[1], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[2], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[3], _header.BoneIndexSize);
				fread_s(&vertex.BoneWeights, sizeof(vertex.BoneWeights), sizeof(float), 4, filePtr);
				break;
			}
			/*-------------------------------------------------------------------
			-             Weight: SDEF (boneIndices[0-1] + boneWeights[0] + SDefC + SDefR0 + SDefR1)
			---------------------------------------------------------------------*/
			case PMXVertexWeight::SDEF:
			{
				ReadPMXIndex(filePtr, &vertex.BoneIndices[0], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[1], _header.BoneIndexSize);
				fread_s(&vertex.BoneWeights[0], sizeof(vertex.BoneWeights[0]), sizeof(float)   , 1, filePtr);
				fread_s(&vertex.SDefC         , sizeof(vertex.SDefC)         , sizeof(XMFLOAT3), 1, filePtr);
				fread_s(&vertex.SDefR0        , sizeof(vertex.SDefR0)        , sizeof(XMFLOAT3), 1, filePtr);
				fread_s(&vertex.SDefR1        , sizeof(vertex.SDefR1)        , sizeof(XMFLOAT3), 1, filePtr);
				break;
			}
			/*-------------------------------------------------------------------
			-             Weight: QDEF (boneIndices[0-3] + boneWeights[0-3])
			---------------------------------------------------------------------*/
			case PMXVertexWeight::QDEF:
			{
				ReadPMXIndex(filePtr, &vertex.BoneIndices[0], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[1], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[2], _header.BoneIndexSize);
				ReadPMXIndex(filePtr, &vertex.BoneIndices[3], _header.BoneIndexSize);
				fread_s(vertex.BoneWeights, sizeof(vertex.BoneWeights), sizeof(float), 4, filePtr);
				break;
			}
			default:
			{
				return false;
			}
		}
		fread_s(&vertex.EdgeMagnitude, sizeof(vertex.EdgeMagnitude), sizeof(float), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadPMXFace
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXFace(FILE* filePtr)
*  @brief     Load PMX Face
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXFace(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Face Count
	---------------------------------------------------------------------*/
	INT32 faceCount = 0;
	fread_s(&faceCount, sizeof(faceCount), sizeof(INT32), 1, filePtr);
	faceCount /= 3;

	/*-------------------------------------------------------------------
	-             Load Face Data
	---------------------------------------------------------------------*/
	_faces.resize(faceCount);
	switch (_header.VertexIndexSize)
	{
		case 1:
		{
			std::vector<UINT8> vertices((UINT64)faceCount * 3);
			fread_s(vertices.data(), sizeof(UINT8) * vertices.size(), sizeof(UINT8), vertices.size(), filePtr);
			for (INT32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
			{
				_faces[faceIndex].Indices[0] = vertices[(UINT64)faceIndex * 3 + 0];
				_faces[faceIndex].Indices[1] = vertices[(UINT64)faceIndex * 3 + 1];
				_faces[faceIndex].Indices[2] = vertices[(UINT64)faceIndex * 3 + 2];
			}
			break;
		}
		case 2:
		{
			std::vector<UINT16> vertices((UINT64)faceCount * 3);
			fread_s(vertices.data(), sizeof(UINT16) * vertices.size(), sizeof(UINT16), vertices.size(), filePtr);
			for (INT32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
			{
				_faces[faceIndex].Indices[0] = vertices[(UINT64)faceIndex * 3 + 0];
				_faces[faceIndex].Indices[1] = vertices[(UINT64)faceIndex * 3 + 1];
				_faces[faceIndex].Indices[2] = vertices[(UINT64)faceIndex * 3 + 2];
			}
			break;
		}
		case 4:
		{
			std::vector<UINT32> vertices((UINT64)faceCount * 3);
			fread_s(vertices.data(), sizeof(UINT32) * vertices.size(), sizeof(UINT32), vertices.size(), filePtr);
			for (INT32 faceIndex = 0; faceIndex < faceCount; ++faceIndex)
			{
				_faces[faceIndex].Indices[0] = vertices[(UINT64)faceIndex * 3 + 0];
				_faces[faceIndex].Indices[1] = vertices[(UINT64)faceIndex * 3 + 1];
				_faces[faceIndex].Indices[2] = vertices[(UINT64)faceIndex * 3 + 2];
			}
			break;
		}
		default:
		{
			return false;
		}
	}
	return true;
}

/****************************************************************************
*							LoadPMXTexture
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXTexture(FILE* filePtr)
*  @brief     Load PMX Texture
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXTexture(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Texture Count
	---------------------------------------------------------------------*/
	INT32 textureCount = 0;
	fread_s(&textureCount, sizeof(textureCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Texture Data
	---------------------------------------------------------------------*/
	_textures.resize(textureCount);
	for (auto& texture : _textures)
	{
		ReadPMXString(filePtr, &texture.TextureName);
	}
	return true;
}

/****************************************************************************
*							LoadPMXMaterial
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXMaterial(FILE* filePtr)
*  @brief     Load PMX Material
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXMaterial(FILE* filePtr)
{
	using namespace DirectX;
	/*-------------------------------------------------------------------
	-             Load Material Count
	---------------------------------------------------------------------*/
	INT32 materialCount = 0;
	fread_s(&materialCount, sizeof(materialCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Material Data
	---------------------------------------------------------------------*/
	_materials.resize(materialCount);
	for (auto& material : _materials)
	{
		/*-------------------------------------------------------------------
		-             Material Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &material.MaterialName);
		ReadPMXString(filePtr, &material.EnglishName);

		/*-------------------------------------------------------------------
		-             Material Config
		---------------------------------------------------------------------*/
		fread_s(&material.Diffuse      , sizeof(material.Diffuse)      , sizeof(XMFLOAT4), 1, filePtr);
		fread_s(&material.Specular     , sizeof(material.Specular)     , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&material.SpecularPower, sizeof(material.SpecularPower), sizeof(float)   , 1, filePtr);
		fread_s(&material.Ambient      , sizeof(material.Ambient)      , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&material.DrawMode     , sizeof(material.DrawMode)     , sizeof(UINT8)   , 1, filePtr);
		fread_s(&material.EdgeColor    , sizeof(material.EdgeColor)    , sizeof(XMFLOAT4), 1, filePtr);
		fread_s(&material.EdgeSize     , sizeof(material.EdgeSize)     , sizeof(float)   , 1, filePtr);

		ReadPMXIndex(filePtr, &material.TextureIndex, _header.TextureIndexSize);
		ReadPMXIndex(filePtr, &material.SphereMapTextureIndex, _header.TextureIndexSize);

		fread_s(&material.SphereMapMode, sizeof(material.SphereMapMode), sizeof(UINT8), 1, filePtr);
		fread_s(&material.ToonTextureMode, sizeof(material.ToonTextureMode), sizeof(UINT8), 1, filePtr);
		switch (material.ToonTextureMode)
		{
			case pmx::PMXToonTextureMode::Separate:
			{
				ReadPMXIndex(filePtr, &material.ToonTextureIndex, _header.TextureIndexSize);
				break;
			}
			case pmx::PMXToonTextureMode::Common:
			{
				UINT8 toonIndex;
				fread_s(&toonIndex, sizeof(toonIndex), sizeof(UINT8), 1, filePtr);
				material.ToonTextureIndex = static_cast<INT32>(toonIndex);
				break;
			}
			default:
			{
				return false;
			}
		}

		/*-------------------------------------------------------------------
		-             Material Comment
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &material.Memo);

		/*-------------------------------------------------------------------
		-             Face Index Count
		---------------------------------------------------------------------*/
		fread_s(&material.FaceIndicesCount, sizeof(material.FaceIndicesCount), sizeof(INT32), 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadPMXBone
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXBone(FILE* filePtr)
*  @brief     Load PMX Bone
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXBone(FILE* filePtr)
{
	using namespace DirectX;
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Load Bone Count
	---------------------------------------------------------------------*/
	INT32 boneCount = 0;
	fread_s(&boneCount, sizeof(boneCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Bone Data
	---------------------------------------------------------------------*/
	_bones.resize(boneCount);
	for (auto& bone : _bones)
	{
		/*-------------------------------------------------------------------
		-             Bone Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &bone.BoneName);
		ReadPMXString(filePtr, &bone.EnglishName);

		/*-------------------------------------------------------------------
		-             Bone Config
		---------------------------------------------------------------------*/
		fread_s(&bone.Position   , sizeof(bone.Position)   , sizeof(XMFLOAT3)   , 1, filePtr);
		ReadPMXIndex(filePtr     , &bone.ParentBoneIndex   , _header.BoneIndexSize);
		fread_s(&bone.DeformDepth, sizeof(bone.DeformDepth), sizeof(INT32)      , 1, filePtr);
		fread_s(&bone.BoneFlag   , sizeof(bone.BoneFlag)   , sizeof(PMXBoneFlag), 1, filePtr);
		/*-------------------------------------------------------------------
		-             Bone Flag: TargetShowMode
		---------------------------------------------------------------------*/
		if (((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::TargetShowMode) == 0)
		{
			fread_s(&bone.PositionOffset, sizeof(bone.PositionOffset), sizeof(XMFLOAT3), 1, filePtr);
		}
		else
		{
			ReadPMXIndex(filePtr, &bone.LinkBoneIndex, _header.BoneIndexSize);
		}

		/*-------------------------------------------------------------------
		-             Bone Flag: AppendRotate , Translate
		---------------------------------------------------------------------*/
		if (((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::AppendRotate) ||
			((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::AppendTranslate))
		{
			ReadPMXIndex(filePtr, &bone.AppendBoneIndex, _header.BoneIndexSize);
			fread_s(&bone.AppendWeight, sizeof(bone.AppendWeight), sizeof(float), 1, filePtr);
		}

		/*-------------------------------------------------------------------
		-             Bone Flag: FixedAxis
		---------------------------------------------------------------------*/
		if ((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::FixedAxis)
		{
			fread_s(&bone.FixedAxis, sizeof(bone.FixedAxis), sizeof(XMFLOAT3), 1, filePtr);
		}

		/*-------------------------------------------------------------------
		-             Bone Flag: LocalAxis
		---------------------------------------------------------------------*/
		if ((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::LocalAxis)
		{
			fread_s(&bone.LocalAxis_X, sizeof(bone.LocalAxis_X), sizeof(XMFLOAT3), 1, filePtr);
			fread_s(&bone.LocalAxis_Z, sizeof(bone.LocalAxis_Z), sizeof(XMFLOAT3), 1, filePtr);
		}

		/*-------------------------------------------------------------------
		-             Bone Flag: DeformOuterParent
		---------------------------------------------------------------------*/
		if ((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::DeformOuterParent)
		{
			fread_s(&bone.KeyValue, sizeof(bone.KeyValue), sizeof(INT32), 1, filePtr);
		}

		/*-------------------------------------------------------------------
		-             Bone Flag: IKBone
		---------------------------------------------------------------------*/
		if ((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::IKBone)
		{
			ReadPMXIndex(filePtr, &bone.IKTargetBoneIndex, _header.BoneIndexSize);
			fread_s(&bone.IKIterationCount, sizeof(bone.IKIterationCount), sizeof(INT32), 1, filePtr);
			fread_s(&bone.IKAngleLimit    , sizeof(bone.IKAngleLimit)    , sizeof(float), 1, filePtr);

			/*-------------------------------------------------------------------
			-             Load IKLink Count
			---------------------------------------------------------------------*/
			INT32 linkCount = 0;
			fread_s(&linkCount, sizeof(linkCount), sizeof(INT32), 1, filePtr);

			/*-------------------------------------------------------------------
			-             Load IKLink
			---------------------------------------------------------------------*/
			bone.IKLinks.resize(linkCount);
			for (auto& ikLink : bone.IKLinks)
			{
				ReadPMXIndex(filePtr, &ikLink.IKBoneIndex, _header.BoneIndexSize);
				fread_s(&ikLink.EnableLimit, sizeof(ikLink.EnableLimit), sizeof(unsigned char), 1, filePtr);

				if (ikLink.EnableLimit != 0)
				{
					fread_s(&ikLink.AngleMin, sizeof(ikLink.AngleMin), sizeof(XMFLOAT3), 1, filePtr);
					fread_s(&ikLink.AngleMax, sizeof(ikLink.AngleMax), sizeof(XMFLOAT3), 1, filePtr);
				}
			}
		}

	}
	return true;
}

/****************************************************************************
*							LoadPMXFaceExpression
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXFaceExpression(FILE* filePtr)
*  @brief     Load PMX Face Expression
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXFaceExpression(FILE* filePtr)
{
	using namespace pmx;
	using namespace DirectX;

	/*-------------------------------------------------------------------
	-             Load Morph Count
	---------------------------------------------------------------------*/
	INT32 morphCount = 0;
	fread_s(&morphCount, sizeof(morphCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Face Morph Data
	---------------------------------------------------------------------*/
	_faceExpressions.resize(morphCount);
	for (auto& faceExpression : _faceExpressions)
	{
		/*-------------------------------------------------------------------
		-             Load Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &faceExpression.Name);
		ReadPMXString(filePtr, &faceExpression.EnglishName);

		fread_s(&faceExpression.FacePart, sizeof(faceExpression.FacePart), sizeof(pmx::PMXFacePart), 1, filePtr);
		fread_s(&faceExpression.MorphType, sizeof(faceExpression.MorphType), sizeof(pmx::PMXMorphType), 1, filePtr);
		
		/*-------------------------------------------------------------------
		-             Load morph data count
		---------------------------------------------------------------------*/
		INT32 dataCount = 0;
		fread_s(&dataCount, sizeof(dataCount), sizeof(INT32), 1, filePtr);

		switch (faceExpression.MorphType)
		{
			/*-------------------------------------------------------------------
			-             MorphType: Position
			---------------------------------------------------------------------*/
			case PMXMorphType::Position:
			{
				faceExpression.PositionMorphs.resize(dataCount);
				for (auto& positionMorph : faceExpression.PositionMorphs)
				{
					ReadPMXIndex(filePtr, &positionMorph.VertexIndex, _header.VertexIndexSize);
					fread_s(&positionMorph.Position, sizeof(positionMorph.Position), sizeof(XMFLOAT3), 1, filePtr);
				}
				break;
			}
			/*-------------------------------------------------------------------
			-             MorphType: UV
			---------------------------------------------------------------------*/
			case PMXMorphType::UV:
			case PMXMorphType::AddUV1:
			case PMXMorphType::AddUV2:
			case PMXMorphType::AddUV3:
			case PMXMorphType::AddUV4:
			{
				faceExpression.UVMorphs.resize(dataCount);
				for (auto& uvMorph : faceExpression.UVMorphs)
				{
					ReadPMXIndex(filePtr, &uvMorph.VertexIndex, _header.VertexIndexSize);
				}
				break;
			}
			/*-------------------------------------------------------------------
			-             MorphType: Bone
			---------------------------------------------------------------------*/
			case PMXMorphType::Bone:
			{
				faceExpression.BoneMorphs.resize(dataCount);
				for (auto& boneMorph : faceExpression.BoneMorphs)
				{
					ReadPMXIndex(filePtr, &boneMorph.BoneIndex, _header.BoneIndexSize);
					fread_s(&boneMorph.Position  , sizeof(boneMorph.Position)  , sizeof(XMFLOAT3), 1, filePtr);
					fread_s(&boneMorph.Quaternion, sizeof(boneMorph.Quaternion), sizeof(XMFLOAT4), 1, filePtr);
				}
				break;
			}
			/*-------------------------------------------------------------------
			-             MorphType: Material
			---------------------------------------------------------------------*/
			case PMXMorphType::Material:
			{
				faceExpression.MaterialMorphs.resize(dataCount);
				for (auto& materialMorph : faceExpression.MaterialMorphs)
				{
					ReadPMXIndex(filePtr, &materialMorph.MaterialIndex, _header.MaterialIndexSize);
					fread_s(&materialMorph.OpType           , sizeof(materialMorph.OpType)           , sizeof(UINT8)   , 1, filePtr);
					fread_s(&materialMorph.Diffuse          , sizeof(materialMorph.Diffuse)          , sizeof(XMFLOAT4), 1, filePtr);
					fread_s(&materialMorph.Specular         , sizeof(materialMorph.Specular)         , sizeof(XMFLOAT3), 1, filePtr);
					fread_s(&materialMorph.SpecularPower    , sizeof(materialMorph.SpecularPower)    , sizeof(float)   , 1, filePtr);
					fread_s(&materialMorph.Ambient          , sizeof(materialMorph.Ambient)          , sizeof(XMFLOAT3), 1, filePtr);
					fread_s(&materialMorph.EdgeColor        , sizeof(materialMorph.EdgeColor)        , sizeof(XMFLOAT4), 1, filePtr);
					fread_s(&materialMorph.EdgeSize         , sizeof(materialMorph.EdgeSize)         , sizeof(float)   , 1, filePtr);
					fread_s(&materialMorph.TextureFactor    , sizeof(materialMorph.TextureFactor)    , sizeof(XMFLOAT4), 1, filePtr);
					fread_s(&materialMorph.SphereMapFactor  , sizeof(materialMorph.SphereMapFactor)  , sizeof(XMFLOAT4), 1, filePtr);
					fread_s(&materialMorph.ToonTextureFactor, sizeof(materialMorph.ToonTextureFactor), sizeof(XMFLOAT4), 1, filePtr);
				}
				break;
			}
			/*-------------------------------------------------------------------
			-             MorphType: Group
			---------------------------------------------------------------------*/
			case PMXMorphType::Group:
			{
				faceExpression.GroupMorphs.resize(dataCount);
				for (auto& groupMorph : faceExpression.GroupMorphs)
				{
					ReadPMXIndex(filePtr, &groupMorph.MorphIndex, _header.FaceIndexSize);
					fread_s(&groupMorph.Weight, sizeof(groupMorph.Weight), sizeof(float), 1, filePtr);
				}
				break;
			}
			/*-------------------------------------------------------------------
			-             MorphType: Flip
			---------------------------------------------------------------------*/
			case PMXMorphType::Flip:
			{
				faceExpression.FlipMorphs.resize(dataCount);
				for (auto& flipMorph : faceExpression.FlipMorphs)
				{
					ReadPMXIndex(filePtr, &flipMorph.MorphIndex, _header.FaceIndexSize);
					fread_s(&flipMorph.Weight, sizeof(flipMorph.Weight), sizeof(float), 1, filePtr);
				}
				break;
			}
			/*-------------------------------------------------------------------
			-             MorphType: Impulse
			---------------------------------------------------------------------*/
			case PMXMorphType::Impulse:
			{
				faceExpression.ImpulseMorphs.resize(dataCount);
				for (auto& impulseMorph : faceExpression.ImpulseMorphs)
				{
					ReadPMXIndex(filePtr, &impulseMorph.RigidBodyIndex, _header.RigidBodyIndexSize);
					fread_s(&impulseMorph.LocalFlag        , sizeof(impulseMorph.LocalFlag)        , sizeof(UINT8)   , 1, filePtr);
					fread_s(&impulseMorph.TranslateVelocity, sizeof(impulseMorph.TranslateVelocity), sizeof(XMFLOAT3), 1, filePtr);
					fread_s(&impulseMorph.RotateTorque     , sizeof(impulseMorph.RotateTorque)     , sizeof(XMFLOAT3), 1, filePtr);
				}
				break;
			}
			default:
			{
				return false;
			}
		}

	}
	return true;
}

/****************************************************************************
*							LoadPMXDisplayFrame
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXDisplayFrame(FILE* filePtr)
*  @brief     Load PMX DisplayFrame
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXDisplayFrame(FILE* filePtr)
{
	using namespace pmx;
	/*-------------------------------------------------------------------
	-             Load display frame count
	---------------------------------------------------------------------*/
	INT32 displayFrameCount = 0;
	fread_s(&displayFrameCount, sizeof(displayFrameCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load display frame data
	---------------------------------------------------------------------*/
	_displayFrames.resize(displayFrameCount);
	for (auto& displayFrame : _displayFrames)
	{
		/*-------------------------------------------------------------------
		-             Load display frame name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &displayFrame.Name);
		ReadPMXString(filePtr, &displayFrame.EnglishName);

		/*-------------------------------------------------------------------
		-             Load display frame flag
		---------------------------------------------------------------------*/
		fread_s(&displayFrame.Flag, sizeof(displayFrame.Flag), sizeof(UINT8), 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load target count
		---------------------------------------------------------------------*/
		INT32 targetCount = 0;
		fread_s(&targetCount, sizeof(targetCount), sizeof(INT32), 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load display frame targets data
		---------------------------------------------------------------------*/
		displayFrame.Targets.resize(targetCount);
		for (auto& target : displayFrame.Targets)
		{
			fread_s(&target.Type, sizeof(target.Type), sizeof(UINT8), 1, filePtr);
			switch (target.Type)
			{
				case PMXDisplayFrame::TargetType::BoneIndex:
				{
					ReadPMXIndex(filePtr, &target.Index, _header.BoneIndexSize);
					break;
				}
				case PMXDisplayFrame::TargetType::MorphIndex:
				{
					ReadPMXIndex(filePtr, &target.Index, _header.FaceIndexSize);
					break;
				}
				default:
				{
					return false;
				}
			}
		}
	}

	return true;
}

/****************************************************************************
*							LoadPMXRigidBody
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXRigidBody(FILE* filePtr)
*  @brief     Load PMX RigidBody
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXRigidBody(FILE* filePtr)
{
	using namespace DirectX;
	using namespace pmx;
	
	/*-------------------------------------------------------------------
	-             Load RigidBody Count
	---------------------------------------------------------------------*/
	INT32 rigidBodyCount = 0;
	fread_s(&rigidBodyCount, sizeof(rigidBodyCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load rigid body Config
	---------------------------------------------------------------------*/
	_rigidBodies.resize(rigidBodyCount);
	for (auto& rigidBody : _rigidBodies)
	{
		ReadPMXString(filePtr, &rigidBody.Name);
		ReadPMXString(filePtr, &rigidBody.EnglishName);

		ReadPMXIndex(filePtr, &rigidBody.BoneIndex, _header.BoneIndexSize);
		fread_s(&rigidBody.Group             , sizeof(rigidBody.Group)             , sizeof(UINT8)   , 1, filePtr);
		fread_s(&rigidBody.CollisionGroup    , sizeof(rigidBody.CollisionGroup)    , sizeof(UINT16)  , 1, filePtr);
		fread_s(&rigidBody.Shape             , sizeof(rigidBody.Shape)             , sizeof(UINT8)   , 1, filePtr);
		fread_s(&rigidBody.ShapeSize         , sizeof(rigidBody.ShapeSize)         , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&rigidBody.Translation       , sizeof(rigidBody.Translation)       , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&rigidBody.Rotation          , sizeof(rigidBody.Rotation)          , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&rigidBody.Mass              , sizeof(rigidBody.Mass)              , sizeof(float)   , 1, filePtr);
		fread_s(&rigidBody.DampingTranslation, sizeof(rigidBody.DampingTranslation), sizeof(float)   , 1, filePtr);
		fread_s(&rigidBody.DampingRotation   , sizeof(rigidBody.DampingRotation)   , sizeof(float)   , 1, filePtr);
		fread_s(&rigidBody.Elasticity        , sizeof(rigidBody.Elasticity)        , sizeof(float)   , 1, filePtr);
		fread_s(&rigidBody.Friction          , sizeof(rigidBody.Friction)          , sizeof(float)   , 1, filePtr);
		fread_s(&rigidBody.RigidBodyOperation, sizeof(rigidBody.RigidBodyOperation), sizeof(UINT8)   , 1, filePtr);
	}
	return true;
}

/****************************************************************************
*							LoadPMXJoint
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXJoint(FILE* filePtr)
*  @brief     Load PMX Joint
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXJoint(FILE* filePtr)
{
	using namespace DirectX;
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Load joint count
	---------------------------------------------------------------------*/
	INT32 jointCount = 0;
	fread_s(&jointCount, sizeof(jointCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load joint data
	---------------------------------------------------------------------*/
	_joints.resize(jointCount);
	for (auto& joint : _joints)
	{
		ReadPMXString(filePtr, &joint.Name);
		ReadPMXString(filePtr, &joint.EnglishName);
		fread_s(&joint.JointType              , sizeof(joint.JointType)              , sizeof(UINT8)   , 1, filePtr);
		ReadPMXIndex(filePtr, &joint.RigidBodyIndex_A, _header.RigidBodyIndexSize);
		ReadPMXIndex(filePtr, &joint.RigidBodyIndex_B, _header.RigidBodyIndexSize);
		fread_s(&joint.Translation            , sizeof(joint.Translation)            , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.Rotation               , sizeof(joint.Rotation)               , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.TranslationMin         , sizeof(joint.TranslationMin)         , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.TranslationMax         , sizeof(joint.TranslationMax)         , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.RotationMin            , sizeof(joint.RotationMin)            , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.RotationMax            , sizeof(joint.RotationMax)            , sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.SpringTranslationFactor, sizeof(joint.SpringTranslationFactor), sizeof(XMFLOAT3), 1, filePtr);
		fread_s(&joint.SpringRotationFactor   , sizeof(joint.SpringRotationFactor)   , sizeof(XMFLOAT3), 1, filePtr);

	}
	return true;
}

/****************************************************************************
*							LoadPMXSoftBody
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXSoftBody(FILE* filePtr)
*  @brief     Load PMX SoftBody
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXSoftBody(FILE* filePtr)
{
	using namespace DirectX;
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Load soft body count
	---------------------------------------------------------------------*/
	INT32 softBodyCount = 0;
	fread_s(&softBodyCount, sizeof(softBodyCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Soft Body Data
	---------------------------------------------------------------------*/
	_softBodies.resize(softBodyCount);
	for (auto& softBody : _softBodies)
	{
		/*-------------------------------------------------------------------
		-             Load Soft Body Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &softBody.Name);
		ReadPMXString(filePtr, &softBody.EnglishName);

		/*-------------------------------------------------------------------
		-             Load SoftBodyType
		---------------------------------------------------------------------*/
		fread_s(&softBody.SoftBodyType, sizeof(softBody.SoftBodyType), sizeof(UINT8), 1, filePtr);
		
		/*-------------------------------------------------------------------
		-             Load Material Index
		---------------------------------------------------------------------*/
		ReadPMXIndex(filePtr, &softBody.MaterialIndex, _header.MaterialIndexSize);

		/*-------------------------------------------------------------------
		-             Load Material Config
		---------------------------------------------------------------------*/
		fread_s(&softBody.Group          , sizeof(softBody.Group)          , sizeof(UINT8)               , 1, filePtr);
		fread_s(&softBody.CollisionGroup , sizeof(softBody.CollisionGroup) , sizeof(UINT16)              , 1, filePtr);
		fread_s(&softBody.MaskFlag       , sizeof(softBody.MaskFlag)       , sizeof(UINT8)               , 1, filePtr);
		fread_s(&softBody.BoneLinkLength , sizeof(softBody.BoneLinkLength) , sizeof(INT32)               , 1, filePtr);
		fread_s(&softBody.ClustersCount  , sizeof(softBody.ClustersCount)  , sizeof(INT32)               , 1, filePtr);
		fread_s(&softBody.TotalMass      , sizeof(softBody.TotalMass)      , sizeof(float)               , 1, filePtr);
		fread_s(&softBody.CollisionMargin, sizeof(softBody.CollisionMargin), sizeof(float)               , 1, filePtr);
		fread_s(&softBody.AeroModel      , sizeof(softBody.AeroModel)      , sizeof(PMXAeroModel)        , 1, filePtr);
		fread_s(&softBody.Config         , sizeof(softBody.Config)         , sizeof(PMXSoftBodyConfig)   , 1, filePtr);
		fread_s(&softBody.Cluster        , sizeof(softBody.Cluster)        , sizeof(PMXSoftBodyCluster)  , 1, filePtr);
		fread_s(&softBody.Iteration      , sizeof(softBody.Iteration)      , sizeof(PMXSoftBodyIteration), 1, filePtr);
		fread_s(&softBody.Material       , sizeof(softBody.Material)       , sizeof(PMXSoftBodyMaterial) , 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load Anchor Count
		---------------------------------------------------------------------*/
		INT32 anchorCount = 0;
		fread_s(&anchorCount, sizeof(anchorCount), sizeof(INT32), 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load SoftBody Count
		---------------------------------------------------------------------*/
		softBody.Anchor.resize(anchorCount);

		/*-------------------------------------------------------------------
		-             Load Anchor
		---------------------------------------------------------------------*/
		for (auto& anchor : softBody.Anchor)
		{
			ReadPMXIndex(filePtr, &anchor.RigidBodyIndex, _header.RigidBodyIndexSize);
			ReadPMXIndex(filePtr, &anchor.VertexIndex, _header.VertexIndexSize);
			fread_s(&anchor.NearMode, sizeof(anchor.NearMode), sizeof(UINT8), 1, filePtr);
		}

		/*-------------------------------------------------------------------
		-             Load vertex count
		---------------------------------------------------------------------*/
		INT32 vertexCount = 0;
		fread_s(&vertexCount, sizeof(vertexCount), sizeof(INT32), 1, filePtr);

		/*-------------------------------------------------------------------
		-             Load vertex count
		---------------------------------------------------------------------*/
		softBody.VertexIndices.resize(vertexCount);
		for (auto& index : softBody.VertexIndices)
		{
			ReadPMXIndex(filePtr, &index, _header.VertexIndexSize);
		}
	}
	return true;
}

/****************************************************************************
*							ReadPMXString
*************************************************************************//**
*  @fn            bool PMXData::ReadPMXString(FILE* filePtr, std::string* string)
*  @brief         Load PMX Header
*  @param[in,out] FILE* filePtr 
*  @return 　    　bool 
*****************************************************************************/
bool PMXData::ReadPMXString(FILE* filePtr, std::string* string)
{
	using namespace pmx;

	UINT32 bufferSize = -1;
	fread_s(&bufferSize, sizeof(bufferSize), sizeof(UINT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Error Check
	---------------------------------------------------------------------*/
	if (bufferSize < 0) 
	{ 
		::OutputDebugString(L"cannot read buffer size."); 
		return false; 
	}

	/*-------------------------------------------------------------------
	-             Load String
	---------------------------------------------------------------------*/
	switch (_header.Encode)
	{
		case PMXEncode::UTF16:
		{
			std::u16string utf16String(bufferSize / 2, u'\0');
			fread_s(utf16String.data(), sizeof(char16_t) * utf16String.size(), sizeof(char16_t), utf16String.size(), filePtr);
			if (!unicode::ConvertU16ToU8(utf16String, *string)) { return false; }
			break;
		}
		case PMXEncode::UTF8:
		{
			std::u8string utf8String(bufferSize, '\0');
			fread_s(utf8String.data(), sizeof(char8_t) * utf8String.size(), sizeof(char8_t), utf8String.size(), filePtr);
			break;
		}
		default:
		{
			::OutputDebugString(L"don't substitute PMX header encoder.");
			return false;
		}
	}

	return true;
}

/****************************************************************************
*							LoadPMXHeader
*************************************************************************//**
*  @fn        bool PMXData::LoadPMXHeader(FILE* filePtr)
*  @brief     Load PMX Header
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::ReadPMXIndex(FILE* filePtr, INT32* pmxIndex, UINT8 indexSize)
{
	switch (indexSize)
	{
		case 1:
		{
			UINT8 index;
			fread_s(&index, sizeof(index), sizeof(UINT8), 1, filePtr);
			if (index != 0xFF)
			{
				*pmxIndex = (INT32)index;
			}
			else
			{
				*pmxIndex = -1;
			}
			break;
		}
		case 2:
		{
			UINT16 index;
			fread_s(&index, sizeof(index), sizeof(UINT16), 1, filePtr);
			if (index != 0xFFFF)
			{
				*pmxIndex = (INT32)index;
			}
			else
			{
				*pmxIndex = -1;
			}
			break;
		}
		case 4:
		{
			UINT32 index;
			fread_s(&index, sizeof(index), sizeof(UINT32), 1, filePtr);
			*pmxIndex = (INT32)index;
			break;
		}
		default:
		{
			return false;
		}
	}
	return true;
}

#pragma endregion Private Function