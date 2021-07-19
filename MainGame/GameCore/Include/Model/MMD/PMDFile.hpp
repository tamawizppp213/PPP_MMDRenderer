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
#include "GameCore/Include/Model/ModelFile.hpp"
#include "PMDConfig.hpp"
#include <vector>
#include <array>
#include <map>
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
	using PMDBoneNodeAddressList = PMDBoneNode*;
	using PMDBoneNodeIterator    = std::map<std::string, PMDBoneNode>::iterator;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Load3DModel(const std::wstring& filePath) override;
	
#pragma region Property
	inline const  PMDVertex*    GetVertex()              const { return _vertices.data(); }
	inline const  UINT16*       GetIndex()               const { return _indices.data();  }
	inline const  PMDMaterial*  GetMaterial()            const { return _materials.data(); }
	inline const  PMDTexture    GetTextureList(int index)const { return _textures[index]; }
	inline        PMDBoneNode*  GetBoneNode(const std::string& boneName){ return &_boneNodeTable.at(boneName); }
	inline        PMDBoneNodeIterator FindBoneNode(const std::string& boneName) { return _boneNodeTable.find(boneName); }
	inline        PMDBoneIK*    GetBoneIK()                    { return _boneIKs.data(); }
	inline        std::string*  GetBoneNames()                 { return _boneNames.data(); }
	inline const  UINT32*       GetKneeIndex()           const { return _kneeIndices.data(); }
	inline const  std::vector<UINT32>& GetKneeIndexList()const { return _kneeIndices; }
	inline const  std::vector<PMDBoneNode*>& GetBoneAddressList() { return _boneNodeAddress; }
	inline const  std::map<std::string, PMDBoneNode>& GetBoneNodeTable() const {return _boneNodeTable; }

	inline size_t GetVertexCount()              { return _vertices.size(); }
	inline size_t GetTotalIndexCount()          { return _indices.size();}
	inline size_t GetMaterialCount()            { return _materials.size(); }
	inline size_t GetBoneCount()                { return _boneNodeTable.size(); }
	inline size_t GetBoneIKCount()              { return _boneIKs.size(); }
	inline size_t GetKneeIndexCount()           { return _kneeIndices.size(); }
	inline size_t GetBoneNodeAddressListCount() { return _boneNodeAddress.size(); }

	inline UINT  GetIndexCountForMaterial(int index) { return _materials[index].PolygonNum; }

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
	bool LoadPMDTextures(const pmd::PMDMaterial& material, int index);
	bool DebugPMDBoneIKList();
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	pmd::PMDHeader                      _header;
	std::vector<PMDVertex>              _vertices;
	std::vector<UINT16>                 _indices;
	std::vector<PMDMaterial>            _materials;
	std::vector<PMDTexture>             _textures;
	std::map<std::string, PMDBoneNode>  _boneNodeTable;
	std::vector<std::string>            _boneNames;
	std::vector<PMDBoneNode*>           _boneNodeAddress;
	std::vector<UINT32>                 _kneeIndices;
	std::vector<PMDBoneIK>              _boneIKs;
	std::vector<pmd::PMDFaceExpression> _faces;
	std::vector<pmd::PMDFaceLabelIndex> _faceLabelIndices;
	std::vector<pmd::PMDBoneLabelList>  _boneLabelIndices;
	std::vector<pmd::PMDRigidBody>      _rigidBodies;
	std::vector<pmd::PMDJoint>          _jointList;
	std::array<std::string, 10>          _toonTextureNames;
	pmd::PMDLocalizeInfo                _localizeInfo;
	std::string _directory;
};

#endif