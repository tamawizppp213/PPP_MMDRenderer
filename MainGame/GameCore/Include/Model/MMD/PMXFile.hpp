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
#include <map>
#include <array>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                              Class
//////////////////////////////////////////////////////////////////////////////////
class PMXData final : public ModelData
{
	using PMXBoneNodeIterator = std::map<std::string, PMXBoneNode>::iterator;
	using PMXMorphIterator    = std::map<std::string, PMXMorph>::iterator;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Load3DModel(const std::wstring& filePath) override;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline const PMXVertex*          GetVertex()     const     { return _vertices.data(); }
	inline const UINT32*             GetIndex()      const     { return _indices.data(); }
	inline const PMXMaterial*        GetMaterial()   const     { return _materials.data(); }
	inline const PMXTexture          GetTextureList(int index) { return _textures[index]; }
	inline       PMXBoneNode*        GetBoneNode (const std::string& boneName) { return &_boneNodeTable.at(boneName); }
	inline       PMXBoneNodeIterator FindBoneNode(const std::string& boneName) { return _boneNodeTable.find(boneName); }
	inline       PMXMorphIterator    FindMorph(const std::string& morphName)   { return _morphingMap.find(morphName); }
	inline       PMXBoneIK*          GetBoneIK()                               { return _boneIKs.data(); }
	inline       std::string*        GetBoneNames()                            { return _boneNames.data(); }
	inline const std::vector<PMXBoneNode*>& GetBoneAddressList()               { return _boneNodeAddress; }
	std::map<std::string, PMXBoneNode>& GetBoneNodeTable()  { return _boneNodeTable; }
	std::map<std::string, PMXMorph>   & GetMorphingMap()    { return _morphingMap; }
	std::map<std::string, PMXBoneNode> CopyBoneNodeTable()  { return _boneNodeTable; }

	inline size_t GetVertexCount()   { return _vertices.size(); }
	inline size_t GetIndexCount()    { return _indices.size(); }
	inline size_t GetMaterialCount() { return _materials.size(); }
	inline size_t GetBoneCount()     { return _boneNodeTable.size(); }
	inline size_t GetBoneIKCount()   { return _boneIKs.size(); }
	inline size_t GetBoneNodeAddressListCount()      { return _boneNodeAddress.size(); }
	inline UINT  GetIndexCountForMaterial(int index) { return _materials[index].PolygonNum; }
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
	bool LoadPMXMeshIndex      (FILE* filePtr);
	bool LoadPMXTextureName    (FILE* filePtr);
	bool LoadPMXMaterial       (FILE* filePtr);
	bool LoadPMXBone           (FILE* filePtr);
	bool LoadPMXFaceExpression (FILE* filePtr);
	bool LoadPMXDisplayFrame   (FILE* filePtr);
	bool LoadPMXRigidBody      (FILE* filePtr);
	bool LoadPMXJoint          (FILE* filePtr);
	bool LoadPMXSoftBody       (FILE* filePtr);
	bool LoadPMXTextures(const pmx::PMXMaterial& material, int index);

	bool ReadPMXString(FILE* filePtr, std::string* string);
	bool ReadPMXIndex (FILE* filePtr, INT32* pmxIndex, UINT8 indexSize);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	pmx::PMXHeader                           _header;
	pmx::PMXInfo                             _info;
	std::vector<PMXVertex>                   _vertices;
	std::vector<UINT32>                      _indices;
	std::vector<std::string>                 _texturePathList;
	std::vector<PMXTexture>                  _textures;
	std::vector<PMXMaterial>                 _materials;
	std::map<std::string, PMXBoneNode>       _boneNodeTable;
	std::vector<std::string>                 _boneNames;
	std::vector<PMXBoneNode*>                _boneNodeAddress;
	std::vector<PMXBoneIK>                   _boneIKs;
	std::map<std::string, PMXMorph>          _morphingMap;
	std::vector<pmx::PMXDisplayFrame>        _displayFrames;
	std::vector<pmx::PMXRigidBody>           _rigidBodies;
	std::vector<pmx::PMXJoint>               _joints;
	std::vector<pmx::PMXSoftBody>            _softBodies;
	std::string _directory;
};
#endif