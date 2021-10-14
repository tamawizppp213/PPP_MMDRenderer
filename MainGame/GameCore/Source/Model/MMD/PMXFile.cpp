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
#include <iomanip>
#include <filesystem>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define INVALID_ID (-1)

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
	if (!LoadPMXMeshIndex(filePtr)) { MessageBox(NULL, L" PMXFace cannot be read.", L"Warning", MB_ICONWARNING); };
	
	/*-------------------------------------------------------------------
	-             Load Texture Data
	---------------------------------------------------------------------*/
	if (!LoadPMXTextureName(filePtr)) { MessageBox(NULL, L" PMXTexture cannot be read.", L"Warning", MB_ICONWARNING); };
	
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
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Load Vertex Count
	---------------------------------------------------------------------*/
	INT32 vertexCount = 0;
	fread_s(&vertexCount, sizeof(vertexCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Vertex Data
	---------------------------------------------------------------------*/
	std::vector<pmx::PMXVertex> vertices;
	vertices.resize(vertexCount);
	for (auto& vertex : vertices)
	{
		/*-------------------------------------------------------------------
		-             Load Position , Normal, UV, and Add UV
		---------------------------------------------------------------------*/
		fread_s(&vertex.Position, sizeof(vertex.Position), sizeof(Float3), 1, filePtr);
		fread_s(&vertex.Normal  , sizeof(vertex.Normal  ), sizeof(Float3), 1, filePtr);
		fread_s(&vertex.UV      , sizeof(vertex.UV      ), sizeof(Float2), 1, filePtr);

		for (UINT8 i = 0; i < _header.AddUVCount; ++i)
		{
			fread_s(&vertex.AddUV[i], sizeof(vertex.AddUV[i]), sizeof(Float4), 1, filePtr);
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
				fread_s(&vertex.BoneWeights[0], sizeof(vertex.BoneWeights), sizeof(float), 1, filePtr);
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
				fread_s(&vertex.SDefC         , sizeof(vertex.SDefC)         , sizeof(Float3), 1, filePtr);
				fread_s(&vertex.SDefR0        , sizeof(vertex.SDefR0)        , sizeof(Float3), 1, filePtr);
				fread_s(&vertex.SDefR1        , sizeof(vertex.SDefR1)        , sizeof(Float3), 1, filePtr);
				
				auto w0 = vertex.BoneWeights[0];
				auto w1 = 1.0f - w0;
				auto center = Vector3(vertex.SDefC);
				auto r0     = Vector3(vertex.SDefR0);
				auto r1     = Vector3(vertex.SDefR1);

				auto rw = r0 * w0 + r1 * w1;
				r0 = center + r0 - rw;
				r1 = center + r1 - rw;
				auto cr0 = (center + r0) * 0.5f;
				auto cr1 = (center + r1) * 0.5f;

				vertex.SDefR0 = cr0.ToFloat3();
				vertex.SDefR1 = cr1.ToFloat3();
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

		_vertices.emplace_back(vertex);
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
bool PMXData::LoadPMXMeshIndex(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Face Count
	---------------------------------------------------------------------*/
	INT32 indexCount = 0;
	fread_s(&indexCount, sizeof(indexCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Face Data
	---------------------------------------------------------------------*/
	_indices.resize(indexCount);
	switch (_header.VertexIndexSize)
	{
		case 1:
		{
			std::vector<UINT8> indices(indexCount);
			fread_s(indices.data(), sizeof(UINT8) * indices.size(), sizeof(UINT8), indices.size(), filePtr);
			for (int i = 0; i < indexCount; ++i)
			{
				_indices[i] = indices[i];
			}
			break;
		}
		case 2:
		{
			std::vector<UINT16> indices(indexCount);
			fread_s(indices.data(), sizeof(UINT16) * indices.size(), sizeof(UINT16), indices.size(), filePtr);
			for (int i = 0; i < indexCount; ++i)
			{
				_indices[i] = indices[i];
			}
			break;
		}
		case 4:
		{
			std::vector<UINT32> indices(indexCount);
			fread_s(indices.data(), sizeof(UINT32) * indices.size(), sizeof(UINT32), indices.size(), filePtr);
			_indices = std::move(indices);
			break;
		}
		default:
		{
			return false;
		}
	}

	/*-------------------------------------------------------------------
	-             Calculate Tangent Data
	---------------------------------------------------------------------*/
	for (int i = 0; i < _indices.size() / 3; ++i)
	{
		auto& vertex0 = _vertices[_indices[i]];
		auto& vertex1 = _vertices[_indices[i + 1]];
		auto& vertex2 = _vertices[_indices[i + 2]];

		Vector3 cp0[] =
		{
			{vertex0.Vertex.Position.x, vertex0.Vertex.UV.x, vertex0.Vertex.UV.y},
			{vertex0.Vertex.Position.y, vertex0.Vertex.UV.x, vertex0.Vertex.UV.y},
			{vertex0.Vertex.Position.z, vertex0.Vertex.UV.x, vertex0.Vertex.UV.y}
		};

		Vector3 cp1[] =
		{
			{vertex1.Vertex.Position.x, vertex1.Vertex.UV.x, vertex1.Vertex.UV.y},
			{vertex1.Vertex.Position.y, vertex1.Vertex.UV.x, vertex1.Vertex.UV.y},
			{vertex1.Vertex.Position.z, vertex1.Vertex.UV.x, vertex1.Vertex.UV.y}
		};

		Vector3 cp2[] =
		{
			{vertex2.Vertex.Position.x, vertex2.Vertex.UV.x, vertex2.Vertex.UV.y},
			{vertex2.Vertex.Position.y, vertex2.Vertex.UV.x, vertex2.Vertex.UV.y},
			{vertex2.Vertex.Position.z, vertex2.Vertex.UV.x, vertex2.Vertex.UV.y}
		};

		float tangent[3];
		float binormal[3];
		for (int i = 0; i < 3; ++i)
		{
			Vector3 V1 = cp1[i] - cp0[i];
			Vector3 V2 = cp2[i] - cp1[i];
			Vector3 ABC = Cross(V1, V2);
			
			if (ABC.GetX() == 0.0f)
			{
				tangent[i]  = 0.0f;
				binormal[i] = 0.0f;
			}
			else
			{
				tangent[i]  = -ABC.GetY() / ABC.GetX();
				binormal[i] = -ABC.GetZ() / ABC.GetX();
			}
		}
		Float3 tangentAfter  = Float3(tangent);
		Float3 binormalAfter = Float3(binormal);
		tangentAfter  = Normalize(tangentAfter).ToFloat3();
		binormalAfter = Normalize(binormalAfter).ToFloat3();
		
		//https://edom18.hateblo.jp/entry/2018/08/30/131859
		vertex0.Vertex.Tangent  = Vector3(Vector3(vertex0.Vertex.Tangent) + Vector3(tangentAfter)).ToFloat3();
		vertex1.Vertex.Tangent  = Vector3(Vector3(vertex1.Vertex.Tangent) + Vector3(tangentAfter)).ToFloat3();
		vertex2.Vertex.Tangent  = Vector3(Vector3(vertex2.Vertex.Tangent) + Vector3(tangentAfter)).ToFloat3();
		vertex0.Vertex.Binormal = Vector3(Vector3(vertex0.Vertex.Binormal) + Vector3(tangentAfter)).ToFloat3();
		vertex1.Vertex.Binormal = Vector3(Vector3(vertex1.Vertex.Binormal) + Vector3(tangentAfter)).ToFloat3();
		vertex2.Vertex.Binormal = Vector3(Vector3(vertex2.Vertex.Binormal) + Vector3(tangentAfter)).ToFloat3();
	}

	for (int i = 0; i < _vertices.size(); ++i)
	{
		_vertices[i].Vertex.Tangent = Normalize(_vertices[i].Vertex.Tangent).ToFloat3();
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
bool PMXData::LoadPMXTextureName(FILE* filePtr)
{
	/*-------------------------------------------------------------------
	-             Load Texture Count
	---------------------------------------------------------------------*/
	INT32 textureCount = 0;
	fread_s(&textureCount, sizeof(textureCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Texture Data
	---------------------------------------------------------------------*/
	_texturePathList.resize(textureCount);
	for (auto& texture : _texturePathList)
	{
		ReadPMXString(filePtr, &texture);
		texture = _directory + "/" + texture;
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
	/*-------------------------------------------------------------------
	-             Load Material Count
	---------------------------------------------------------------------*/
	INT32 materialCount = 0;
	fread_s(&materialCount, sizeof(materialCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Material Data
	---------------------------------------------------------------------*/
	std::vector<pmx::PMXMaterial>materials;
	materials.resize(materialCount);
	for (auto& material : materials)
	{
		/*-------------------------------------------------------------------
		-             Material Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &material.MaterialName);
		ReadPMXString(filePtr, &material.EnglishName);

		/*-------------------------------------------------------------------
		-             Material Config
		---------------------------------------------------------------------*/
		fread_s(&material.Diffuse      , sizeof(material.Diffuse)      , sizeof(Float4), 1, filePtr);
		fread_s(&material.Specular     , sizeof(material.Specular)     , sizeof(Float3), 1, filePtr);
		fread_s(&material.SpecularPower, sizeof(material.SpecularPower), sizeof(float) , 1, filePtr);
		fread_s(&material.Ambient      , sizeof(material.Ambient)      , sizeof(Float3), 1, filePtr);
		fread_s(&material.DrawMode     , sizeof(material.DrawMode)     , sizeof(UINT8) , 1, filePtr);
		fread_s(&material.EdgeColor    , sizeof(material.EdgeColor)    , sizeof(Float4), 1, filePtr);
		fread_s(&material.EdgeSize     , sizeof(material.EdgeSize)     , sizeof(float) , 1, filePtr);

		ReadPMXIndex(filePtr, &material.TextureIndex, _header.TextureIndexSize);
		ReadPMXIndex(filePtr, &material.SphereMapTextureIndex, _header.TextureIndexSize);

		fread_s(&material.SphereMapMode , sizeof(material.SphereMapMode), sizeof(UINT8), 1, filePtr);
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

	_materials.resize(materialCount);
	_textures.resize(materialCount);
	for (int i = 0; i < _materials.size(); ++i)
	{
		_materials[i] = PMXMaterial(materials[i]);
		LoadPMXTextures(materials[i], i);
	}
	return true;
}

/****************************************************************************
*							LoadPMDTexture
*************************************************************************//**
*  @fn        bool PMDData::LoadPMDTextures(const pmd::PMDMaterial& material, int index)
*  @brief     Load PMD Textures Data
*  @param[in] void
*  @return 　　bool
*****************************************************************************/
bool PMXData::LoadPMXTextures(const pmx::PMXMaterial& material, int index)
{
	TextureLoader textureLoader;

	std::string textureName = "";
	std::string sphName = "";
	std::string spaName = "";
	std::string toonName = "";

	/*-------------------------------------------------------------------
	-             Check Texture Path
	---------------------------------------------------------------------*/
	if (material.TextureIndex != INVALID_ID) { textureName = _texturePathList[material.TextureIndex]; }
	else                                     { textureName = "Resources/Texture/Default/White.png";}

	/*-------------------------------------------------------------------
	-             Check Sphere Map Path
	---------------------------------------------------------------------*/
	switch (material.SphereMapMode)
	{
		case pmx::PMXSphereMapMode::Addition:
		{
			if (material.SphereMapTextureIndex != INVALID_ID) { spaName = _texturePathList[material.SphereMapTextureIndex]; }
			//else { ::OutputDebugString(L"Couldn't read addition type textures\n"); }
			break;
		}
		case pmx::PMXSphereMapMode::Multiply:
		{
			if (material.SphereMapTextureIndex != INVALID_ID) { sphName = _texturePathList[material.SphereMapTextureIndex]; }
			//else { ::OutputDebugString(L"Couldn't read multiply type textures\n"); }
			break;
		}
		case pmx::PMXSphereMapMode::SubTexture:
		{
			
			break;
		}
		case pmx::PMXSphereMapMode::None:
		{
			break;
		}
		default:
		{
			::OutputDebugString(L"Couldn't read textures");
			return false;
		}
	}

	/*-------------------------------------------------------------------
	-             Check Toon Texture Path
	---------------------------------------------------------------------*/
	if (material.ToonTextureIndex != INVALID_ID) { toonName = _texturePathList[material.ToonTextureIndex]; }

	/*-------------------------------------------------------------------
	-             NULL String Check
	---------------------------------------------------------------------*/
	if (sphName == "")  { sphName  = "Resources/Texture/Default/White.png"; }
	if (spaName == "")  { spaName  = "Resources/Texture/Default/Black.png"; }
	if (toonName == "") { toonName = "Resources/Texture/Default/White.png"; }

	/*-------------------------------------------------------------------
	-             LoadTextures
	---------------------------------------------------------------------*/
	textureLoader.LoadTexture(file::AnsiToWString(textureName), _textures[index].Texture);
	textureLoader.LoadTexture(file::AnsiToWString(sphName), _textures[index].SphereMultiply);
	textureLoader.LoadTexture(file::AnsiToWString(spaName), _textures[index].SphereAddition);
	textureLoader.LoadTexture(file::AnsiToWString(toonName), _textures[index].ToonTexture);
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
	using namespace pmx;

	/*-------------------------------------------------------------------
	-             Load Bone Count
	---------------------------------------------------------------------*/
	INT32 boneCount = 0;
	fread_s(&boneCount, sizeof(boneCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Bone Data
	---------------------------------------------------------------------*/
	std::vector<pmx::PMXBone> bones;
	bones.resize(boneCount);
	for (auto& bone : bones)
	{
		/*-------------------------------------------------------------------
		-             Bone Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &bone.BoneName);
		ReadPMXString(filePtr, &bone.EnglishName);

		/*-------------------------------------------------------------------
		-             Bone Config
		---------------------------------------------------------------------*/
		fread_s(&bone.Position   , sizeof(bone.Position)   , sizeof(Float3)   , 1, filePtr);
		ReadPMXIndex(filePtr     , &bone.ParentBoneIndex   , _header.BoneIndexSize);
		fread_s(&bone.DeformDepth, sizeof(bone.DeformDepth), sizeof(INT32)      , 1, filePtr);
		fread_s(&bone.BoneFlag   , sizeof(bone.BoneFlag)   , sizeof(PMXBoneFlag), 1, filePtr);
		/*-------------------------------------------------------------------
		-             Bone Flag: TargetShowMode
		---------------------------------------------------------------------*/
		if (((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::TargetShowMode) == 0)
		{
			fread_s(&bone.PositionOffset, sizeof(bone.PositionOffset), sizeof(Float3), 1, filePtr);
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
			fread_s(&bone.FixedAxis, sizeof(bone.FixedAxis), sizeof(Float3), 1, filePtr);
		}

		/*-------------------------------------------------------------------
		-             Bone Flag: LocalAxis
		---------------------------------------------------------------------*/
		if ((UINT16)bone.BoneFlag & (UINT16)PMXBoneFlag::LocalAxis)
		{
			fread_s(&bone.LocalAxis_X, sizeof(bone.LocalAxis_X), sizeof(Float3), 1, filePtr);
			fread_s(&bone.LocalAxis_Z, sizeof(bone.LocalAxis_Z), sizeof(Float3), 1, filePtr);
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
					fread_s(&ikLink.AngleMin, sizeof(ikLink.AngleMin), sizeof(Float3), 1, filePtr);
					fread_s(&ikLink.AngleMax, sizeof(ikLink.AngleMax), sizeof(Float3), 1, filePtr);
				}
			}
		}

	}

	/*-------------------------------------------------------------------
	-             Load Bone Node
	---------------------------------------------------------------------*/
	_boneNames.resize(boneCount);
	_boneNodeAddress.resize(boneCount);

	for (int i = 0; i < boneCount; ++i)
	{
		auto boneName = file::WStringToString(unicode::ToWString(bones[i].BoneName));
		_boneNames[i] = boneName;

		auto& boneNode = _boneNodeTable[_boneNames[i]]; // acquire bone node
		boneNode.SetBoneName(boneName);
		boneNode.SetBoneIndex(i);

		_boneNodeAddress[i] = &boneNode;
	}

	// build parent  - child relationships
	for (auto& bone : bones)
	{
		// overload check: parent index
		if (bone.ParentBoneIndex >= bones.size()) { continue; }

		auto parentName = _boneNames[bone.ParentBoneIndex];
		auto boneName   = file::WStringToString(unicode::ToWString(bone.BoneName));
		_boneNodeTable[boneName].SetParent(&_boneNodeTable[parentName]); // set parent
		_boneNodeTable[parentName].AddChild(&_boneNodeTable[boneName]);  // set child
	}

	/*-------------------------------------------------------------------
	-             Load Bone Node Config
	---------------------------------------------------------------------*/
	for (int i = 0; i < boneCount; ++i)
	{
		auto& boneNode = _boneNodeAddress[i];

		if (boneNode->GetParent() != nullptr)
		{
			auto localPosition = Vector3(bones[i].Position) - Vector3(bones[bones[i].ParentBoneIndex].Position);
			boneNode->SetTransform(Transform(localPosition));
		}
		else
		{
			auto localPosition = Vector3(bones[i].Position);
			boneNode->SetTransform(Transform(localPosition));
		}

		boneNode->SetDeformationDepth(bones[i].DeformDepth);
		boneNode->SetGlobalMatrix    (Transform(Vector3(bones[i].Position)).GetMatrix());
		boneNode->SetInverseBindMatrix(Inverse(Translation(bones[i].Position)));
		boneNode->SaveInitialSRT();

		bool deformAfterPhysics = !!((uint16_t)bones[i].BoneFlag & (uint16_t)PMXBoneFlag::DeformAfterPhisics);
		bool appendRotate       = ((uint16_t)bones[i].BoneFlag & (uint16_t)PMXBoneFlag::AppendRotate)    != 0;
		bool appendTranslate    = ((uint16_t)bones[i].BoneFlag & (uint16_t)PMXBoneFlag::AppendTranslate) != 0;
		boneNode->EnableDeformationAfterPhysics(deformAfterPhysics);
		boneNode->EnableAppendRotate(appendRotate);
		boneNode->EnableAppendTranslate(appendTranslate);

		if ((appendRotate || appendTranslate) && (bones[i].AppendBoneIndex != -1))
		{
			bool appendLocal = ((uint16_t)bones[i].BoneFlag & (uint16_t)PMXBoneFlag::AppendLocal) != 0;
			auto appendNode = _boneNodeTable[file::WStringToString(unicode::ToWString(bones[bones[i].AppendBoneIndex].BoneName))];
			boneNode->EnableAppendLocal(appendLocal);
			boneNode->SetAppendNode    (&appendNode);

			boneNode->SetAppendWeight  (bones[i].AppendWeight);
		
		}

	}
	
	
	/*-------------------------------------------------------------------
	-             Load IK Bone
	---------------------------------------------------------------------*/
	int boneIndex = 0;
	for (auto& ik : bones)
	{
		if ((UINT16)ik.BoneFlag & (UINT16)PMXBoneFlag::IKBone)
		{
			std::vector<PMXIKChain> ikChain;
			// set ik link bone (chain)
			for (const auto& ikLink : ik.IKLinks)
			{
				_boneNodeAddress[ikLink.IKBoneIndex]->EnableIK(true);
				PMXIKChain chain;
				chain.IKBone      = _boneNodeAddress[ikLink.IKBoneIndex];
				chain.EnableLimit = ikLink.EnableLimit;
				chain.AngleMax    = ikLink.AngleMax;
				chain.AngleMin    = ikLink.AngleMin;
				ikChain.emplace_back(chain);
			}

			// add bone ik
			PMXBoneIK boneIK;
			boneIK.SetIKChains(ikChain);
			boneIK.SetIKBone        (_boneNodeAddress[boneIndex]);
			boneIK.SetIKParentBone  (_boneNodeAddress[ik.LinkBoneIndex]);
			boneIK.SetIterationCount(ik.IKIterationCount);
			boneIK.SetLimitAngle    (ik.IKAngleLimit / 4.0f); // as pmd value
			boneIK.SetTargetBone    (_boneNodeAddress[ik.IKTargetBoneIndex]);
			
			auto& boneNode = _boneNodeAddress[boneIndex];
			boneNode->SetBoneIK(&boneIK);
			boneNode->EnableIK(true);
			_boneIKs.emplace_back(boneIK);
		}
		boneIndex++;
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

	/*-------------------------------------------------------------------
	-             Load Morph Count
	---------------------------------------------------------------------*/
	INT32 morphCount = 0;
	fread_s(&morphCount, sizeof(morphCount), sizeof(INT32), 1, filePtr);

	/*-------------------------------------------------------------------
	-             Load Face Morph Data
	---------------------------------------------------------------------*/
	std::vector<pmx::PMXFaceExpression> faceExpressions;
	faceExpressions.resize(morphCount);
	for (auto& faceExpression : faceExpressions)
	{
		/*-------------------------------------------------------------------
		-             Load Name
		---------------------------------------------------------------------*/
		ReadPMXString(filePtr, &faceExpression.Name);
		ReadPMXString(filePtr, &faceExpression.EnglishName);

		faceExpression.Name = file::WStringToString(unicode::ToWString(faceExpression.Name));
		
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
					fread_s(&positionMorph.Position, sizeof(positionMorph.Position), sizeof(Float3), 1, filePtr);
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
					fread_s(&boneMorph.Position  , sizeof(boneMorph.Position)  , sizeof(Float3), 1, filePtr);
					fread_s(&boneMorph.Quaternion, sizeof(boneMorph.Quaternion), sizeof(Float4), 1, filePtr);
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
					fread_s(&materialMorph.Diffuse          , sizeof(materialMorph.Diffuse)          , sizeof(Float4), 1, filePtr);
					fread_s(&materialMorph.Specular         , sizeof(materialMorph.Specular)         , sizeof(Float3), 1, filePtr);
					fread_s(&materialMorph.SpecularPower    , sizeof(materialMorph.SpecularPower)    , sizeof(float)   , 1, filePtr);
					fread_s(&materialMorph.Ambient          , sizeof(materialMorph.Ambient)          , sizeof(Float3), 1, filePtr);
					fread_s(&materialMorph.EdgeColor        , sizeof(materialMorph.EdgeColor)        , sizeof(Float4), 1, filePtr);
					fread_s(&materialMorph.EdgeSize         , sizeof(materialMorph.EdgeSize)         , sizeof(float)   , 1, filePtr);
					fread_s(&materialMorph.TextureFactor    , sizeof(materialMorph.TextureFactor)    , sizeof(Float4), 1, filePtr);
					fread_s(&materialMorph.SphereMapFactor  , sizeof(materialMorph.SphereMapFactor)  , sizeof(Float4), 1, filePtr);
					fread_s(&materialMorph.ToonTextureFactor, sizeof(materialMorph.ToonTextureFactor), sizeof(Float4), 1, filePtr);
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
					fread_s(&impulseMorph.TranslateVelocity, sizeof(impulseMorph.TranslateVelocity), sizeof(Float3), 1, filePtr);
					fread_s(&impulseMorph.RotateTorque     , sizeof(impulseMorph.RotateTorque)     , sizeof(Float3), 1, filePtr);
				}
				break;
			}
			default:
			{
				return false;
			}
			
		}
		
		_morphingMap[faceExpression.Name] = faceExpression;
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
		fread_s(&rigidBody.ShapeSize         , sizeof(rigidBody.ShapeSize)         , sizeof(Float3)  , 1, filePtr);
		fread_s(&rigidBody.Translation       , sizeof(rigidBody.Translation)       , sizeof(Float3)  , 1, filePtr);
		fread_s(&rigidBody.Rotation          , sizeof(rigidBody.Rotation)          , sizeof(Float3)  , 1, filePtr);
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
		fread_s(&joint.Translation            , sizeof(joint.Translation)            , sizeof(Float3), 1, filePtr);
		fread_s(&joint.Rotation               , sizeof(joint.Rotation)               , sizeof(Float3), 1, filePtr);
		fread_s(&joint.TranslationMin         , sizeof(joint.TranslationMin)         , sizeof(Float3), 1, filePtr);
		fread_s(&joint.TranslationMax         , sizeof(joint.TranslationMax)         , sizeof(Float3), 1, filePtr);
		fread_s(&joint.RotationMin            , sizeof(joint.RotationMin)            , sizeof(Float3), 1, filePtr);
		fread_s(&joint.RotationMax            , sizeof(joint.RotationMax)            , sizeof(Float3), 1, filePtr);
		fread_s(&joint.SpringTranslationFactor, sizeof(joint.SpringTranslationFactor), sizeof(Float3), 1, filePtr);
		fread_s(&joint.SpringRotationFactor   , sizeof(joint.SpringRotationFactor)   , sizeof(Float3), 1, filePtr);

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