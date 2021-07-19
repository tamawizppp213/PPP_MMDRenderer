//////////////////////////////////////////////////////////////////////////////////
///             @file   PMDConfig.hpp
///             @brief  PMDFile Config
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMD_CONFIG_HPP
#define PMD_CONFIG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "GameCore/Include/Model/MMD/VMDFile.hpp"
#include "GameMath/Include/GMMatrix.hpp"
#include <Windows.h>
#include <vector>
#pragma warning(disable : 26495)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define PMD_BONE_MATRIX_SIZE (256)

//////////////////////////////////////////////////////////////////////////////////
//                              Struct
//////////////////////////////////////////////////////////////////////////////////
namespace pmd
{
	using namespace gm;
#pragma pack(1)
	struct PMDHeader
	{
		char  Signature[3];
		float Version;
		char  ModelName[20];
		char  Comment  [256];
	};

	struct PMDVertex
	{
		VertexPositionNormalTexture Vertex;
		UINT16  BoneIndex[2];
		UINT8   BoneWeight;
		UINT8   EdgeFlag;
	};

	struct PMDMaterial
	{
		Float4   Diffuse;
		float    SpecularPower;
		Float3   Specular;
		Float3   Ambient;
		UINT8    ToonID;
		UINT8    EdgeFlag;
		UINT32   PolygonNum;
		char     TextureFileName[20];
	};


	struct PMDBone
	{
		char     BoneName[20];
		UINT16   ParentBoneID; // If there is no Parent, this value needs to set 0xFFFF
		UINT16   ChildBoneID;
		UINT8    BoneType;
		UINT16   IKBoneID;     // If there is no Parent, this value needs to set 0;
		Float3   BoneHeadPosition;
		char     EnglishBoneName[20];
	};

	struct PMDBoneIK
	{
		using ChainList = std::vector<UINT16>;

		UINT16    IKBoneID;
		UINT16    IKTargetBoneID;
		UINT8     IKChainLength;
		UINT16    IterationCount;
		float     AngleLimit;
		ChainList Chains;
	};

	enum class FacePart : UINT8
	{
		Base,
		EyeBrow,
		Eye,
		Rip,
		Other
	};


	struct PMDFaceExpression
	{
		using FaceVertexList = std::vector<Float3>;
		using FaceIndexList  = std::vector<UINT32>;

		char           FaceExpressionName[20];
		UINT32         VertexNum;
		FacePart       FaceExpressionType;
		FaceVertexList Vertices;
		FaceIndexList  Indices;
		char           EnglishFaceExpressionName[20];
	};

	struct PMDFaceLabelIndex
	{
		UINT16 FaceLabelIndex;
	};

	struct PMDBoneLabelList
	{
		using DisplayList = std::vector<UINT16>;
		char        BoneName[50];
		DisplayList BoneLabelIndices;
		char        BoneEnglishName[50];
	};

    enum class PMDRigidBodyShape : UINT8
	{
		Sphere,
		Box,
		Capsule
	};

	enum class PMDRigidBodyOperation : UINT8
	{
		Static,
		Dynamic,
		DynamicAdjustBone
	};

	struct PMDRigidBody
	{
		char                  RigidBodyName[20];
		UINT16                RelationBoneIndex;
		UINT8                 GroupIndex;
		UINT16                GroupTarget;
		PMDRigidBodyShape     RigidBodyShapeType;
		Float3                BodyShape;
		Float3                Position;
		Float3                Rotation;
		float                 Mass;
		float                 DampingTranslate;
		float                 DampingRotation;
		float                 Elasticity;
		float                 Friction;
		PMDRigidBodyOperation RigidBodyType;
	};

	struct PMDJoint
	{
		enum { JointNameCount = 20 };

		char     JointName[20];
		UINT32   RigidBodyA;
		UINT32   RigidBodyB;
		Float3   JointPosition;
		Float3   JointRotation;
		Float3   LimitPosition[2];
		Float3   LimitRotation[2];
		Float3   SpringPosition;
		Float3   SpringRotation;
	};

	struct PMDLocalizeInfo
	{
		UINT8 LocalizeFlag;
		char ModelName      [20];
		char Comment        [256];
	};

	enum class BoneType
	{
		Rotate,
		RotateAndMove,
		IK,
		Undefined,
		IKChild,
		RotationChild,
		IKDestination,
		Invisible
	};
#pragma pack()
}

/****************************************************************************
*				  			PMDVertex
*************************************************************************//**
*  @class     PMD Vertex
*  @brief     PMD Vertex
*****************************************************************************/
#pragma pack(1)
class PMDVertex
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	VertexPositionNormalTexture Vertex;
	UINT16 BoneIndex[2];
	UINT8  BoneWeight;
	UINT8  EdgeFlag;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMDVertex()                            = default;
	PMDVertex(const PMDVertex&)            = default;
	PMDVertex& operator=(const PMDVertex&) = default;
	PMDVertex(PMDVertex&&)                 = default;
	PMDVertex& operator=(PMDVertex&&)      = default;
	PMDVertex(VertexPositionNormalTexture const& vertex, UINT16 boneIndex[2], UINT8 boneWeight, UINT8 edgeFlag) :
		Vertex(vertex), BoneWeight(boneWeight), EdgeFlag(edgeFlag)
	{
		BoneIndex[0] = boneIndex[0];
		BoneIndex[1] = boneIndex[1];
	}
	PMDVertex(pmd::PMDVertex const& pmdVertex) :Vertex(pmdVertex.Vertex), BoneWeight(pmdVertex.BoneWeight), EdgeFlag(pmdVertex.EdgeFlag)
	{
		BoneIndex[0] = pmdVertex.BoneIndex[0];
		BoneIndex[1] = pmdVertex.BoneIndex[1];
	}


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	static constexpr unsigned int InputElementCount = 6;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

/****************************************************************************
*				  			PMDMaterial
*************************************************************************//**
*  @class     PMDMaterial
*  @brief     PMD Material Data
*****************************************************************************/
class PMDMaterial
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	gm::Float4 Diffuse;
	float      SpecularPower;
	gm::Float3 Specular;
	gm::Float3 Ambient;
	UINT8      ToonID;
	UINT8      EdgeFlag;
	UINT16     Padding;
	UINT32     PolygonNum;
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMDMaterial()                                = default;
	~PMDMaterial()                               = default;
	PMDMaterial(const PMDMaterial&)              = default;
	PMDMaterial& operator=(const PMDMaterial&)   = default;
	PMDMaterial(PMDMaterial&&)                   = default;
	PMDMaterial& operator=(PMDMaterial &&)       = default;
	PMDMaterial(const pmd::PMDMaterial& material)
	{
		this->Diffuse       = material.Diffuse;
		this->SpecularPower = material.SpecularPower;
		this->Specular      = material.Specular;
		this->Ambient       = material.Ambient;
		this->ToonID        = material.ToonID;
		this->EdgeFlag      = material.EdgeFlag;
		this->PolygonNum    = material.PolygonNum;
	}

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  			PMDTexture
*************************************************************************//**
*  @class     PMD Texture
*  @brief     PMD Texture Data
*****************************************************************************/
using SphereMapMultiply = Texture;
using SphereMapAddition = Texture;
using ToonTexture       = Texture;
class PMDTexture
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	Texture           Texture;
	SphereMapMultiply SphereMultiply;
	SphereMapAddition SphereAddition;
	ToonTexture       ToonTexture;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMDTexture()  = default;
	~PMDTexture() = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

/****************************************************************************
*				  			PMDBoneNode
*************************************************************************//**
*  @class     PMD Bone Node
*  @brief     PMD Bone Data
*****************************************************************************/
class PMDBoneNode
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void AddChild(PMDBoneNode* boneNode) { _children.emplace_back(boneNode); }
	
	static void RecursiveBoneMatrixMultiply(std::vector<gm::Matrix4>& boneMatrix, PMDBoneNode* boneNode, const gm::Matrix4& matrix);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline void SetBoneIndex   (int boneIndex)                  { this->_boneIndex    = boneIndex; }
	inline void SetBasePosition(const gm::Float3& basePosition) { this->_basePosition = basePosition; }
	inline void SetParent      (PMDBoneNode* parent)            { this->_parent = parent; }
	inline void SetBoneType(UINT32 type)                         { this->_boneType    = type; }
	inline void SetIKParentBone(UINT32 ikParentIndex)           { this->_ikParentBone = ikParentIndex; }

	inline int GetBoneIndex()                   { return _boneIndex; }
	inline gm::Float3 GetBasePosition()         { return _basePosition; }
	inline  PMDBoneNode* GetParent() { return _parent; }
	inline int GetIKParentBone()                { return _ikParentBone; }
 
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMDBoneNode () = default;
	~PMDBoneNode() = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	// bone config
	int         _boneIndex = 0;
	gm::Float3  _basePosition;

	// node 
	PMDBoneNode*              _parent;
	std::vector<PMDBoneNode*> _children;

	// boneIK
	UINT32 _boneType;
	UINT32 _ikParentBone;

};

class PMDData;
class VMDFile;
/****************************************************************************
*				  			PMDBoneNode
*************************************************************************//**
*  @class     PMD Bone Node
*  @brief     PMD Bone Data
*****************************************************************************/
class PMDBoneIK
{
	using ChainList = std::vector<UINT16>;
	using PMDBoneNodeAddressList = std::vector<PMDBoneNode*>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	static void SolveIK(int frameNo, PMDData* pmdFile, std::vector<gm::Matrix4>& boneMatrices, VMDFile* vmdFile);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline UINT16 GetIKBoneID()       const { return _ikBoneID; }
	inline UINT16 GetTargetBoneID()   const { return _ikTargetBoneID; }
	inline UINT8  GetChainLength()    const { return _ikChainLength; }
	inline UINT16 GetIterationCount() const { return _iterationCount; }
	inline float  GetAngleLimit()     const { return _angleLimit; }
	inline const UINT16* GetChains()  const { return _chains.data(); }
	inline const std::vector<UINT16>& GetChainsVector()  const { return _chains; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMDBoneIK()  = default;
	PMDBoneIK(pmd::PMDBoneIK& ik)
	{
		_ikBoneID       = ik.IKBoneID;
		_ikTargetBoneID = ik.IKTargetBoneID;
		_ikChainLength  = ik.IKChainLength;
		_iterationCount = ik.IterationCount;
		_angleLimit     = ik.AngleLimit;
		_chains         = std::move(ik.Chains);
	}
	~PMDBoneIK() = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	static void SolveCCDIK (const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices, const std::vector<UINT32>& kneeIndices);
	static void SolveCosIK (const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices, const std::vector<UINT32>& kneeIndices);
	static void SolveLookAt(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices);
	
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	UINT16    _ikBoneID;       // IK Bone (target bone position)
	UINT16    _ikTargetBoneID; // Index of bones to get closer to the target.
	UINT8     _ikChainLength;  // bone node count between IK bone
	UINT16    _iterationCount; 
	float     _angleLimit;     // Rotation limit per 1 Frame
	ChainList _chains;
};

struct BoneParameter
{
	gm::Matrix4 BoneMatrices[PMD_BONE_MATRIX_SIZE];
};
#pragma pack()
#endif