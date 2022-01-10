//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXConfig.hpp
///             @brief  PMXFile Config
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef PMX_CONFIG_HPP
#define PMX_CONFIG_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "DirectX12/Include/Core/DirectX12VertexTypes.hpp"
#include "DirectX12/Include/Core/DirectX12Texture.hpp"
#include "GameMath/Include/GMTransform.hpp"
#include "GameMath/Include/GMQuaternion.hpp"
#include <Windows.h>
#include <array>
#include <vector>
#include <string>
#pragma warning(disable : 26495)
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
// 
// read pmx format https://github.com/hirakuni45/glfw3_app/blob/master/glfw3_app/docs/PMX_spec.txt
#define PMX_BONE_MATRIX_SIZE (512)
namespace pmx
{
	using namespace gm;
#pragma pack(1)
	enum class PMXEncode : UINT8 
	{
		UTF16 = 0,
		UTF8  = 1
	};

	struct PMXHeader
	{
		char      Signature[4];
		float     Version;            // ver 2.0 / 2.1
		UINT8     DataSize;
		PMXEncode Encode;             // 0: UTF16, 1: UTF8
		UINT8     AddUVCount;         // 0 ~ 4
		UINT8     VertexIndexSize;    // 1, 2, or 4
		UINT8     TextureIndexSize;   // 1, 2, or 4
		UINT8     MaterialIndexSize;  // 1, 2, or 4
		UINT8     BoneIndexSize;      // 1, 2, or 4
		UINT8     FaceIndexSize;      // 1, 2, or 4
		UINT8     RigidBodyIndexSize; // 1, 2, or 4
	};

	struct PMXInfo
	{
		std::string ModelName;
		std::string EngliseModelName;
		std::string Comment;
		std::string EnglishComment;
	};

	enum class PMXVertexWeight : UINT8
	{
		BDEF1, // boneIndices[0]
		BDEF2, // boneIndices[0-1] + boneWeights[0]  (PMD Format weight(0.0-1.0))
		BDEF4, // boneIndices[0-3] + boneWeights[0-3] (may not weight total = 1.0)
		SDEF,  // boneIndices[0-1] + boneWeights[0] + SDefC + SDefR0 + SDefR1
		QDEF   // boneIndices[0-3] + boneWeights[0-3]
	};

	struct PMXVertex
	{
		Float3          Position;
		Float3          Normal;
		Float2          UV;
		Float4          AddUV[4];      // mainly use for effect
		PMXVertexWeight WeightType;
		INT32           BoneIndices[4];
		float           BoneWeights[4];
		Float3          SDefC;
		Float3          SDefR0;
		Float3          SDefR1;

		float EdgeMagnitude;

	};

	struct PMXMesh
	{
		UINT32 Indices[3];
	};

	struct PMXTexture
	{
		std::string TextureName;
	};

	enum class PMXDrawModeFlags : UINT8
	{
		BothSurface       = 0x01,
		GroundShadow      = 0x02,
		CastSelfShadow    = 0x04, 
		ReceiveSelfShadow = 0x08,
		DrawEdge          = 0x10,
		VertexColor       = 0x20, // ver2.1 only
		DrawPoint         = 0x40, // ver2.1 only
		DrawLine          = 0x80  // ver2.1 only
	};
	
	enum class PMXSphereMapMode : UINT8
	{
		None, 
		Multiply,
		Addition, 
		SubTexture
	};

	enum class PMXToonTextureMode :UINT8
	{
		Separate,
		Common
	};

	struct PMXMaterial
	{
		std::string        MaterialName;
		std::string        EnglishName;
		Float4             Diffuse;
		float              SpecularPower;
		Float3             Specular;
		Float3             Ambient;
		PMXDrawModeFlags   DrawMode;
		Float4             EdgeColor;
		float              EdgeSize;
		INT32              TextureIndex;
		INT32              SphereMapTextureIndex;
		PMXSphereMapMode   SphereMapMode;
		PMXToonTextureMode ToonTextureMode;
		INT32              ToonTextureIndex;
		std::string        Memo;
		INT32              FaceIndicesCount;

	};

	enum class PMXBoneFlag : UINT16
	{
		TargetShowMode     = 0x0001, // Connection destination PMD child bone specification) (0: coodinate offset specify, 1: bone specify)
		AllowRotate        = 0x0002, // allow rotation
		AllowTranslate     = 0x0004, // allow translation
		Visible            = 0x0008, // display
		AllowControl       = 0x0010, // allow control
		IKBone             = 0x0020, // IK
		AppendLocal        = 0x0080, // local axis(0: user deformation value / IK link / multiple assignment 1: parent's local deformation amount )
		AppendRotate       = 0x0100, // append rotation
		AppendTranslate    = 0x0200, // append translate
		FixedAxis          = 0x0400, // fixed axis
		LocalAxis          = 0x0800, // local axis
		DeformAfterPhisics = 0x1000, // deform after phisics
		DeformOuterParent  = 0x2000, // deform outer parent
	};

	struct PMXIKLink
	{
		INT32 IKBoneIndex;
		unsigned char EnableLimit;

		//When Enable Limit is 1, the variations is needed.
		Float3   AngleMin; // radian 
		Float3   AngleMax; // radian
	};

	struct PMXBone
	{
		std::string BoneName;
		std::string EnglishName;
		Float3      Position;
		INT32       ParentBoneIndex;
		INT32       DeformDepth;
		PMXBoneFlag BoneFlag;
		Float3      PositionOffset;
		INT32       LinkBoneIndex;
		INT32       AppendBoneIndex;
		float       AppendWeight;
		Float3      FixedAxis;         // PMXBoneFlag: FixedAxis is enabled
		Float3      LocalAxis_X;       // PMXBoneFlag: LocalAxis is enabled 
		Float3      LocalAxis_Z;       // PMXBoneFlag: LocalAxis is enabled
		INT32       KeyValue;          // PMXBoneFlag: DeformAfterPhisics is enabled
		INT32       IKTargetBoneIndex; // PMXBoneFlag: IKBone is enabled
		INT32       IKIterationCount;  // PMXBoneFlag: IKBone is enabled
		float       IKAngleLimit;      // PMXBoneFlag: IKBone is enabled (radian)

		std::vector<PMXIKLink> IKLinks;
	};

	enum class PMXMorphType : UINT8
	{
		Group,
		Position,
		Bone,
		UV,
		AddUV1,
		AddUV2,
		AddUV3,
		AddUV4,
		Material,
		Flip,     // ver 2,1 only
		Impulse   // ver 2.1 only
	};

	enum class PMXFacePart : UINT8
	{
		Base,
		Eyebrow,
		Eye,
		Mouth,
		Other,
	};

	struct PositionMorph
	{
		INT32    VertexIndex;
		Float3   Position;
	};

	struct UVMorph
	{
		INT32    VertexIndex;
		Float4   UV;
	};

	struct BoneMorph
	{
		INT32    BoneIndex;
		Float3   Position;
		Float4   Quaternion;
	};

	struct MaterialMorph
	{
		enum class OpType : UINT8
		{
			Mutiply,
			Addition
		};

		INT32    MaterialIndex;
		OpType   OpType;
		Float4   Diffuse;
		Float3   Specular;
		float    SpecularPower;
		Float3   Ambient;
		Float4   EdgeColor;
		float    EdgeSize;
		Float4   TextureFactor;
		Float4   SphereMapFactor;
		Float4   ToonTextureFactor;
	};

	struct GroupMorph
	{
		INT32 MorphIndex;
		float Weight;
	};

	struct FlipMorph
	{
		INT32 MorphIndex;
		float Weight;
	};

	struct ImpulseMorph
	{
		INT32    RigidBodyIndex;
		UINT8    LocalFlag;         // 0: off, 1: on
		Float3   TranslateVelocity;
		Float3   RotateTorque;
	};

	struct PMXFaceExpression
	{
		std::string  Name;
		std::string  EnglishName;
		PMXFacePart  FacePart;
		PMXMorphType MorphType;
		

		std::vector<PositionMorph> PositionMorphs;
		std::vector<UVMorph>       UVMorphs;
		std::vector<BoneMorph>     BoneMorphs;
		std::vector<MaterialMorph> MaterialMorphs;
		std::vector<GroupMorph>    GroupMorphs;
		std::vector<FlipMorph>     FlipMorphs;
		std::vector<ImpulseMorph>  ImpulseMorphs;
	};

	struct PMXDisplayFrame
	{
		std::string Name;
		std::string EnglishName;

		enum class TargetType : UINT8
		{
			BoneIndex,
			MorphIndex
		};

		struct Target
		{
			TargetType Type;
			INT32      Index;
		};

		enum class FrameType : UINT8
		{
			DefaultFrame,
			SpecialFrame
		};

		FrameType           Flag;
		std::vector<Target> Targets;
	};

	enum class PMXRigidBodyShape : UINT8
	{
		Sphere,
		Box,
		Capsule
	};

	enum class PMXRigidBodyOperation : UINT8
	{
		Static,
		Dynamic,
		DynamicAndBoneMerge
	};

	struct PMXRigidBody
	{
		std::string           Name;
		std::string           EnglishName;
		INT32                 BoneIndex;
		UINT8                 Group;
		UINT16                CollisionGroup;
		PMXRigidBodyShape     Shape;
		Float3                ShapeSize;
		Float3                Translation;
		Float3                Rotation;            // raduan
		float                 Mass;
		float                 DampingTranslation;
		float                 DampingRotation;
		float                 Repulsion;
		float                 Friction;
		PMXRigidBodyOperation RigidBodyOperation;
	};

	enum class PMXJointType : UINT8
	{
		SpringDOF6,
		DOF6,
		P2P,
		ConeTwist,
		Slider,
		Hinge
	};

	struct PMXJoint
	{
		std::string  Name;
		std::string  EnglishName;
		PMXJointType JointType;
		INT32        RigidBodyIndex_A;
		INT32        RigidBodyIndex_B;
		Float3       Translation;
		Float3       Rotation;
		Float3       TranslationMin;
		Float3       TranslationMax;
		Float3       RotationMin;             // radian
		Float3       RotationMax;             // radian
		Float3       SpringTranslationFactor;
		Float3       SpringRotationFactor;
	};

	enum class PMXSoftBodyType : UINT8
	{
		TriangleMesh,
		Rope
	};

	enum class PMXSoftBodyMask : UINT8
	{
		BLink      = 0x01,
		Cluster    = 0x02,
		HybridLink = 0x04
	};

	enum class PMXAeroModel : INT32
	{
		kAeroModelV_TwoSided,
		kAeroModelV_OneSided,
		kAeroModelF_TwoSided,
		kAeroModelF_OneSided
	};

	struct PMXSoftBodyConfig
	{
		float VCF; // Velocities correction factor (default V_point);
		float DP;  // Damping coefficient factor;
		float DG;  // Drag coefficient;
		float LF;  // Lift coefficent;
		float PR;  // Pressure coefficient;
		float VC;  // Volume conversation coefficient;
		float DF;  // Dynamic friction coefficient;
		float MT;  // Pose matching coefficient;
		float CHR; // Rigid contacts hardness;
		float KHR; // Kinetic contacts hardness;
		float SHR; // Soft contacts hardness;
		float AHR; // Anchors hardness;
	};

	struct PMXSoftBodyCluster
	{
		float SRHR_CL;    // Soft vs rigid hardness      [0,1];
		float SKHR_CL;    // Soft vs kinetic hardness    [0,1];
		float SSHR_CL;    // Soft vs soft hardness       [0,1]
		float SR_SPLT_CL; // Soft vs rigid impulse split [0,1];
		float SK_SPLT_CL; // Soft vs rigid impulse split [0,1];
		float SS_SPLT_CL; // Soft vs rigid impulse split [0,1];
	};

	struct PMXSoftBodyIteration
	{
		INT32 V_Iterations;
		INT32 P_Iterations;
		INT32 D_Iterations;
		INT32 C_Iterations;
	};

	struct PMXSoftBodyMaterial
	{
		float LST;
		float AST;
		float VST;
	};

	struct PMXSoftBodyAnchorRigidBody
	{
		INT32 RigidBodyIndex;
		INT32 VertexIndex;
		UINT8 NearMode;
	};

	struct PMXSoftBody
	{
		std::string          Name;
		std::string          EnglishName;
		PMXSoftBodyType      SoftBodyType;
		INT32                MaterialIndex;
		UINT8                Group;
		UINT16               CollisionGroup;
		PMXSoftBodyMask      MaskFlag;
		INT32                BoneLinkLength;
		INT32                ClustersCount;
		float                TotalMass;
		float                CollisionMargin;
		PMXAeroModel         AeroModel;
		PMXSoftBodyConfig    Config;
		PMXSoftBodyCluster   Cluster;
		PMXSoftBodyIteration Iteration;
		PMXSoftBodyMaterial  Material;
		std::vector<PMXSoftBodyAnchorRigidBody> Anchor;
		std::vector<INT32>   VertexIndices;
	};
#pragma pack()

}

/****************************************************************************
*				  			PMXBDefVertex
*************************************************************************//**
*  @class     PMXBDefVertex
*  @brief     PMX Format Vertex (for Bone Define)
*****************************************************************************/
#pragma pack(1)
class PMXVertex
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/


	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	VertexPositionNormalTextureTangent Vertex;
	std::array<INT32, 4>        BoneIndices;
	std::array<float, 4>        BoneWeights;
	//gm::Float3                  SDefC;
	//gm::Float3                  SDefR0;
	//gm::Float3                  SDefR1;
	UINT32                      WeightType;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXVertex()                            = default;
	PMXVertex(const PMXVertex&)            = default;
	PMXVertex& operator=(const PMXVertex&) = default;
	PMXVertex(PMXVertex&&)                 = default;
	PMXVertex& operator=(PMXVertex&&)      = default;
	inline PMXVertex(pmx::PMXVertex& vertex)
	{
		Vertex = VertexPositionNormalTextureTangent(vertex.Position, vertex.Normal, vertex.UV, gm::Float3(0,0,0), gm::Float3(0, 0, 0));
		for (int i = 0; i < 4; ++i)
		{
			BoneIndices[i] = vertex.BoneIndices[i];
			BoneWeights[i] = vertex.BoneWeights[i];
		}
		//SDefC  = vertex.SDefC;
		//SDefR0 = vertex.SDefR0;
		//SDefR1 = vertex.SDefR1;
		WeightType = (UINT8)vertex.WeightType;
	}


private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	static constexpr unsigned int InputElementCount = 8;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

/****************************************************************************
*				  			PMDMaterial
*************************************************************************//**
*  @class     PMDMaterial
*  @brief     PMD Material Data
*****************************************************************************/
class PMXMaterial
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
	UINT32     PolygonNum;
	

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXMaterial()                                = default;
	~PMXMaterial()                               = default;
	PMXMaterial(const PMXMaterial&)              = default;
	PMXMaterial& operator=(const PMXMaterial&)   = default;
	PMXMaterial(PMXMaterial&&)                   = default;
	PMXMaterial& operator=(PMXMaterial &&)       = default;
	PMXMaterial(const pmx::PMXMaterial& material)
	{
		this->Diffuse       = material.Diffuse;
		this->SpecularPower = material.SpecularPower;
		this->Specular      = material.Specular;
		this->Ambient       = material.Ambient;
		this->PolygonNum    = material.FaceIndicesCount;
	}

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

using SphereMapMultiply  = Texture;
using SphereMapAddition  = Texture;
using SpehreMapSubstract = Texture;
using ToonTexture        = Texture;
class PMXTexture
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
	PMXTexture()  = default;
	~PMXTexture()
	{
		Texture.Resource = nullptr;
		SphereMultiply.Resource = nullptr;
		SphereAddition.Resource = nullptr;
		ToonTexture.Resource = nullptr;
	}

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

class PMXBoneIK;
/****************************************************************************
*				  			PMDBoneNode
*************************************************************************//**
*  @class     PMD Bone Node
*  @brief     PMD Bone Data
*****************************************************************************/
class PMXBoneNode
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void SaveInitialSRT() { _initialTransform = _transform; } // initialize
	void LoadInitialSRT() { _transform = _initialTransform; }; // reset (per frame)

	void UpdateLocalMatrix();
	void UpdateGlobalMatrix();
	void UpdateSelfandChildMatrix();
	void UpdateChildMatrix();
	void UpdateAppendMatrix(); // use before global matrix 
	void BeforeUpdateMatrix();

	void AddChild(PMXBoneNode* boneNode) { _children.emplace_back(boneNode); }
	static void RecursiveBoneMatrixMultiply(std::vector<gm::Matrix4>& boneMatrix, PMXBoneNode* boneNode, const gm::Matrix4& matrix);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	/*-------------------------------------------------------------------
	-							Transform
	---------------------------------------------------------------------*/
	inline void SetTransform       (const gm::Transform&  transform)  { _transform = transform; }
	inline void SetTranslate       (const gm::Vector3&    translate)  { _transform.LocalPosition        = translate; }
	inline void SetScale           (const gm::Vector3&    scale)      { _transform.LocalScale           = scale; }
	inline void SetRotate          (const gm::Quaternion  quaternion) { _transform.LocalRotation        = quaternion; }
	inline void SetAnimateTransform(const gm::Transform&  transform)  { _animateTransform               = transform; }
	inline void SetAnimateTranslate(const gm::Vector3&    translate)  { _animateTransform.LocalPosition = translate; }
	inline void SetAnimateRotate   (const gm::Quaternion& rotate)     { _animateTransform.LocalRotation = rotate; }
	inline void SetAppendTransform (const gm::Transform&  transform)  { _appendTransform                = transform; }
	inline void SetAppendRotate    (const gm::Vector3&    translate)  { _appendTransform.LocalPosition  = translate; }
	inline void SetIKRotate        (const gm::Quaternion& quaternion) { _ikRotation = quaternion; }
	inline gm::Transform  GetTransform       ()       { return _transform; }
	inline gm::Vector3    GetTranslate       () const { return _transform.LocalPosition; }
	inline gm::Vector3    GetScale           () const { return _transform.LocalScale; }
	inline gm::Quaternion GetRotate          () const { return _transform.LocalRotation; }
	inline gm::Transform  GetInitialTransform() const { return _initialTransform; }
	inline gm::Vector3    GetInitialTranslate() const { return _initialTransform.LocalPosition; }
	inline gm::Vector3    GetInitialScale    () const { return _initialTransform.LocalScale; }
	inline gm::Quaternion GetInitialRotate   () const { return _initialTransform.LocalRotation; }
	inline gm::Transform  GetAppendTransform () const { return _appendTransform; }
	inline gm::Vector3    GetAppendTranslate () const { return _appendTransform.LocalPosition; }
	inline gm::Quaternion GetAppendRotate    () const { return _appendTransform.LocalRotation; }
	inline gm::Quaternion GetIKRotate        () const { return _ikRotation; }
	inline gm::Vector3    AnimateTranslate   () const { return _animateTransform.LocalPosition + _transform.LocalPosition; }
	inline gm::Quaternion AnimateRotate      () const { return _animateTransform.LocalRotation * _transform.LocalRotation; }

	/*-------------------------------------------------------------------
	-							Bone Matrix
	---------------------------------------------------------------------*/
	inline void SetLocalMatrix      (const gm::Matrix4& matrix) { _localBoneMatrix   = matrix; }
	inline void SetGlobalMatrix     (const gm::Matrix4& matrix) { _globalBoneMatrix  = matrix; }
	inline void SetInverseBindMatrix(const gm::Matrix4& matrix) { _inverseBindMatrix = matrix; }
	const  gm::Matrix4& GetLocalMatrix      () const { return _localBoneMatrix; }
	const  gm::Matrix4& GetGlobalMatrix     () const { return _globalBoneMatrix; }
	const  gm::Matrix4& GetInverseBindMatrix() const { return _inverseBindMatrix; }
	
	/*-------------------------------------------------------------------
	-							Property
	---------------------------------------------------------------------*/
	inline void SetBoneName(const std::string& name)   { _name             = name; }
	inline void SetBoneIndex       (int   boneIndex)   { _boneIndex        = boneIndex; }
	inline void SetDeformationDepth(int   deformDepth) { _deformationDepth = deformDepth; }
	inline void SetAppendWeight    (float weight)      { _appendWeight     = weight; }

	inline float GetAppendWeight()     { return _appendWeight; }
	inline int   GetBoneIndex()        { return _boneIndex; }
	inline int   GetDeformationDepth() { return _deformationDepth; }

	/*-------------------------------------------------------------------
	-							Bone flag
	---------------------------------------------------------------------*/
	void EnableIK             (bool enable) { _enableIK = enable; }
	void EnableAppendRotate   (bool enable) { _isAppendRotate    = enable; }
	void EnableAppendTranslate(bool enable) { _isAppendTranslate = enable; }
	void EnableAppendLocal    (bool enable) { _isAppendLocal     = enable; }
	void EnableDeformationAfterPhysics(bool enable) { _isDeformationAfterPhysics = enable; }
	bool IsDeformationAfterPhysics()                { return _isDeformationAfterPhysics; }

	/*-------------------------------------------------------------------
	-							Bone Node
	---------------------------------------------------------------------*/
	inline void         SetParent(PMXBoneNode* parent)          { _parent          = parent; }
	inline void         SetChild(PMXBoneNode* child, int index) { _children[index] = child; }
	inline void         SetAppendNode(PMXBoneNode* boneNode)    { _appendNode      = boneNode; }
	inline PMXBoneNode* GetAppendNode() const                   { return _appendNode; }
	inline PMXBoneNode* GetParent()                             { return _parent; }
	inline std::vector<PMXBoneNode*>& GetChildren() { return _children; }
	inline std::string GetBoneName() { return _name; }
 
	inline void        SetBoneIK(PMXBoneIK* ik) { _boneIK = ik; }
	inline PMXBoneIK*  GetBoneIK()              { return _boneIK; }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXBoneNode () = default;
	~PMXBoneNode() = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	/*-------------------------------------------------------------------
	-							Transform 
	---------------------------------------------------------------------*/
	gm::Transform _initialTransform; // A or T poses transform 
	gm::Transform _transform;        // 
	gm::Transform _appendTransform;  // append bone node transform
	gm::Transform _animateTransform; // bone node animation per frame. 

	/*-------------------------------------------------------------------
	-							Bone matrix
	---------------------------------------------------------------------*/
	gm::Matrix4 _localBoneMatrix;   // Relative coordinates to the direct parent bone
	gm::Matrix4 _globalBoneMatrix;  // Global matrix to the root bone.
	gm::Matrix4 _inverseBindMatrix; // inverse bind pose convertion

	/*-------------------------------------------------------------------
	-							Bone Config
	---------------------------------------------------------------------*/
	int         _boneIndex;
	int         _deformationDepth;
	float       _appendWeight;
	gm::Quaternion _ikRotation;

	/*-------------------------------------------------------------------
	-							Bone flag
	---------------------------------------------------------------------*/
	bool _enableIK;
	bool _isAppendRotate;
	bool _isAppendTranslate;
	bool _isAppendLocal;
	bool _isDeformationAfterPhysics;

	/*-------------------------------------------------------------------
	-							Bone node
	---------------------------------------------------------------------*/
	std::string               _name;
	PMXBoneNode*              _parent;
	std::vector<PMXBoneNode*> _children;
	PMXBoneNode*              _appendNode;

	PMXBoneIK* _boneIK = nullptr;
};


struct PMXBoneParameter
{
	gm::Matrix4 BoneMatrices[PMX_BONE_MATRIX_SIZE];
};

struct PMXBoneQuaternion
{
	gm::Float4 BoneQuaternions[PMX_BONE_MATRIX_SIZE];
};

struct PMXBoneLocalPosition
{
	gm::Float3 BonePositions[PMX_BONE_MATRIX_SIZE];
};
/****************************************************************************
*				  			PMDBoneNode
*************************************************************************//**
*  @class     PMD Bone Node
*  @brief     PMD Bone Data
*****************************************************************************/
class PMXMorph
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	std::vector<pmx::PositionMorph> PositionMorphs;
	std::vector<pmx::UVMorph>       UVMorphs;
	std::vector<pmx::BoneMorph>     BoneMorphs;
	std::vector<pmx::MaterialMorph> MaterialMorphs;
	std::vector<pmx::GroupMorph>    GroupMorphs;
	std::vector<pmx::FlipMorph>     FlipMorphs;
	std::vector<pmx::ImpulseMorph>  ImpulseMorphs;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXMorph()  = default;
	PMXMorph(pmx::PMXFaceExpression& face)
	{
		PositionMorphs = std::move(face.PositionMorphs);
		UVMorphs       = std::move(face.UVMorphs);
		BoneMorphs     = std::move(face.BoneMorphs);
		MaterialMorphs = std::move(face.MaterialMorphs);
		GroupMorphs    = std::move(face.GroupMorphs);
		FlipMorphs     = std::move(face.FlipMorphs);
		ImpulseMorphs  = std::move(face.ImpulseMorphs);
	}
	~PMXMorph() = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/


	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
};

class PMXData;
class VMDFile;
class PMXIKChain
{
public:
	PMXBoneNode* IKBone;
	unsigned char EnableLimit;

	//When Enable Limit is 1, the variations is needed.
	gm::Float3   AngleMin; // radian 
	gm::Float3   AngleMax; // radian

	gm::Quaternion SaveIKRotation;
	gm::Float3     PreviousAngle;
	float          PlaneModeAngle;

	PMXIKChain() = default;
	PMXIKChain(const pmx::PMXIKLink& ikLink)
	{
		IKBone      = nullptr;
		EnableLimit = ikLink.EnableLimit;
		AngleMin    = ikLink.AngleMin;
		AngleMax    = ikLink.AngleMax;
	}
	PMXIKChain(const PMXIKChain& ikLink)
	{
		IKBone      = ikLink.IKBone;
		EnableLimit = ikLink.EnableLimit;
		AngleMin    = ikLink.AngleMin;
		AngleMax    = ikLink.AngleMax;
	}
};

enum class SolveAxis
{
	X,
	Y,
	Z
};
/****************************************************************************
*				  			PMXBoneIK
*************************************************************************//**
*  @class     PMD Bone IK
*  @brief     PMD Bone IK
*****************************************************************************/
class PMXBoneIK
{
	using PMXBoneNodeAddressList = std::vector<PMXBoneNode*>;
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void InitializeChildNode();
	void SolveIK(int frameNo);
	void AddIKChain(PMXBoneNode* boneNode, bool axisLimit, const gm::Float3& limitMin, const gm::Float3& limitMax);
	void SetIKChains(std::vector<PMXIKChain>& ikChains) 
	{
		_ikChains = std::move(ikChains);
		_ikChainLength = _ikChains.size();
	}
	void InsertIKChain(int index, PMXBoneNode* boneNode, bool axisLimit, const gm::Float3& limitMin, const gm::Float3& limitMax);

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	inline PMXBoneNode* GetIKBoneNode()         const { return _ikBone; }
	inline PMXBoneNode* GetIKParentBoneNode()   const { return _ikParent; }
	inline PMXBoneNode* GetTargetBoneNode()     const { return _ikTargetBone; }
	inline size_t       GetChainLength()        const { return _ikChainLength; }
	inline size_t       GetIterationCount()     const { return _iterationCount; }
	inline float        GetAngleLimit()         const { return _angleLimit; }
	std::string GetIKName() const
	{
		if (_ikBone != nullptr) { return _ikBone->GetBoneName(); }
		else { return ""; }
	}

	void SetIKBone      (PMXBoneNode* boneNode) { _ikBone       = boneNode; }
	void SetIKParentBone(PMXBoneNode* boneNode) { _ikParent     = boneNode; }
	void SetTargetBone  (PMXBoneNode* boneNode) { _ikTargetBone = boneNode; }
	void SetChainNode(int index, PMXBoneNode* boneNode) { _ikChains.at(index).IKBone = boneNode; }
	void SetChainNode(int index, PMXBoneNode* boneNode, bool axisLimit, const gm::Float3& limitMin, const gm::Float3& limitMax)
	{
		_ikChains.at(index).IKBone = boneNode;
		_ikChains.at(index).EnableLimit = axisLimit;
		_ikChains.at(index).AngleMax = limitMax;
		_ikChains.at(index).AngleMin = limitMin;
	}
	void SetIterationCount(size_t count) { _iterationCount = static_cast<UINT16>(count); }
	void SetLimitAngle    (float angle)  { _angleLimit = angle; }

	inline const std::vector<PMXIKChain>& GetChainsVector()  const { return _ikChains; }
	inline const PMXIKChain* GetChains()  const { return _ikChains.data(); }
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXBoneIK()  = default;
	~PMXBoneIK() = default;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	void SolveCore(int iteration);
	void SolveCCDIK();
	void SolvePlane(int cycle, int chainIndex, SolveAxis solveAxis);
	
	
	void AddIKChain(PMXIKChain&& chain);
	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	PMXBoneNode*    _ikBone;       // IK Bone (target bone position)
	PMXBoneNode*    _ikTargetBone; // Index of bones to get closer to the target.
	PMXBoneNode*    _ikParent;
	UINT64          _ikChainLength;  // bone node count between IK bone
	UINT16          _iterationCount; 
	float           _angleLimit;     // Rotation limit per 1 Frame
	std::vector<PMXIKChain> _ikChains;
};

enum class RigidBodyType
{
	Kinematic,
	Dynamic,
	Aligned
};

class btRigidBody;
class btCollisionShape;
class btTypedConstraint;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btMotionState;
struct btOverlapFilterCallback;
class MMDPhysics;
class MMDMotionState;
class PMXModel;
class PMXRigidBody
{
	enum class RigidBodyType
	{
		Kinematic,
		Dynamic,
		Aligned
	};
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Create(const pmx::PMXRigidBody& pmxRigidBody, PMXModel* model, PMXBoneNode* node);
	void Destroy();

	void ReflectGlobalMatrix();
	void CalculateLocalMatrix();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	btRigidBody* GetRigidBody() const;
	UINT16     GetGroup()     const;
	UINT16     GetGroupMask() const;
	gm::Matrix4 GetTransformMatrix();

	void SetActivation(bool activation);
	void ResetTransform();
	void Reset(MMDPhysics* physics);
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXRigidBody();
	~PMXRigidBody();
	PMXRigidBody(const PMXRigidBody& rigidBody)              = delete;
	PMXRigidBody& operator = (const PMXRigidBody& rigidBody) = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::string   _name;
	gm::AffineTransform _transform;
	
	std::unique_ptr<btCollisionShape> _shape;
	std::unique_ptr<MMDMotionState>  _activeMotionState;
	std::unique_ptr<MMDMotionState>  _kinematicMotionState;
	std::unique_ptr<btRigidBody>       _rigidBody;
	RigidBodyType                    _rigidBodyType;

	UINT16 _group;
	UINT16 _groupMask;

	PMXBoneNode* _boneNode;
	gm::Matrix4      _offsetMatrix;
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class PMXJoint
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool CreateJoint(const pmx::PMXJoint& pmxJoint, PMXRigidBody* rigidBodyA, PMXRigidBody* rigidBodyB);
	void Destroy();

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	btTypedConstraint* GetConstraint() const;

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	PMXJoint();
	~PMXJoint();
	PMXJoint(const PMXJoint& rhs)              = delete;
	PMXJoint& operator = (const PMXJoint& rhs) = delete;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::unique_ptr<btTypedConstraint> _constraint;
};

/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     TemplateClass
*  @brief     temp
*****************************************************************************/
class MMDPhysics
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool Create();
	bool Update(float time);
	void Destroy();

	void AddRigidBody(PMXRigidBody* rigidBody);
	void RemoveRigidBody(PMXRigidBody* rigidBody);
	void AddJoint(PMXJoint* joint);
	void RemoveJoint(PMXJoint* joint);
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	float GetFPS() const;
	int   GetMaxSubStepCount() const;
	btDiscreteDynamicsWorld* GetDynamicsWorld() const;

	void  SetFPS(float fps);
	void  SetMaxSubStepCount(int numSteps);

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	MMDPhysics();
	~MMDPhysics();
	MMDPhysics(const MMDPhysics& rhs)              = delete; // prohibit copy
	MMDPhysics& operator = (const MMDPhysics& rhs) = delete;

private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::unique_ptr<btBroadphaseInterface> _broadPhase;
	std::unique_ptr<btDefaultCollisionConfiguration>     _collisionConfig;
	std::unique_ptr<btCollisionDispatcher>               _dispatcher;
	std::unique_ptr<btSequentialImpulseConstraintSolver> _solver;
	std::unique_ptr<btDiscreteDynamicsWorld>             _world;
	std::unique_ptr<btCollisionShape>                   _groundShape;
	std::unique_ptr<btMotionState>                      _groundMotionState;
	std::unique_ptr<btRigidBody>                         _groundRigidBody;
	std::unique_ptr<btOverlapFilterCallback>             _filterCallback;

	float  _fps;
	int    _maxSubStepCount;
};
#pragma pack()
#endif