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
#include <DirectXMath.h>
#include <Windows.h>
#include <vector>
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#pragma warning(disable : 26495)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
namespace pmd
{
	using namespace DirectX;
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
		XMFLOAT4 Diffuse;
		float    SpecularPower;
		XMFLOAT3 Specular;
		XMFLOAT3 Ambient;
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
		XMFLOAT3 BoneHeadPosition;
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
		using FaceVertexList = std::vector<XMFLOAT3>;
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
		XMFLOAT3              BodyShape;
		XMFLOAT3              Position;
		XMFLOAT3              Rotation;
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
		XMFLOAT3 JointPosition;
		XMFLOAT3 JointRotation;
		XMFLOAT3 LimitPosition[2];
		XMFLOAT3 LimitRotation[2];
		XMFLOAT3 SpringPosition;
		XMFLOAT3 SpringRotation;
	};

	struct PMDLocalizeInfo
	{
		UINT8 LocalizeFlag;
		char ModelName      [20];
		char Comment        [256];
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
#pragma pack()
#endif