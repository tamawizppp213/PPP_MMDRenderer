//////////////////////////////////////////////////////////////////////////////////
///             @file   PMXConfig.cpp
///             @brief  PMX Config 
///             @author Toide Yutaro
///             @date   2021_06_16
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/PMXConfig.hpp"
#include "GameCore/Include/Model/MMD/PMXFile.hpp"
#include "GameCore/Include/Model/MMD/VMDFile.hpp"
#include "GameCore/Include/Model/MMD/PMXModel.hpp"
#include <btBulletDynamicsCommon.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define EPSILON (0.0005f)
#define ROOT_BONE_NAME ("全ての親")

inline void ConvertBtTransformToRotationAndTranslation(const btTransform& trans, Matrix3& m, Vector3& v)
{
    const auto& b = trans.getBasis();
    const auto& t = trans.getOrigin();
    m = Matrix3(Vector3(b[0][0], b[0][1], b[0][2]), Vector3(b[1][0], b[1][1], b[1][2]), Vector3(b[2][0], b[2][1], b[2][2]));
    v = Vector3(t.x(), t.y(), t.z());
}

inline void ConvertRotationAndTranslationToBtTransform(const Matrix3& m, const Vector3& v, btTransform& trans)
{
    trans.setBasis(btMatrix3x3(m.GetX().GetX(), m.GetX().GetY(), m.GetX().GetZ(), m.GetY().GetX(), m.GetY().GetY(), m.GetY().GetZ(), m.GetZ().GetX(), m.GetZ().GetY(), m.GetZ().GetZ()));
    trans.setOrigin(btVector3(v.GetX(), v.GetY(), v.GetZ()));
}
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*							     PMDVertex
*****************************************************************************/
const D3D12_INPUT_ELEMENT_DESC PMXVertex::InputElements[] =
{
    { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TANGENT" ,   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "BINORMAL" ,  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "BONE_NO",    0, DXGI_FORMAT_R32G32B32A32_SINT , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "WEIGHT"   ,  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    //{ "DEF_C"    ,  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    //{ "DEF_R_ZERO", 0, DXGI_FORMAT_R32G32B32_FLOAT ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    //{ "DEF_R_ONE",  0, DXGI_FORMAT_R32G32B32_FLOAT ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "WEIGHT_TYPE",0, DXGI_FORMAT_R32_UINT         ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(PMXVertex) == 92, "Vertex struct/layout mismatch");
const D3D12_INPUT_LAYOUT_DESC PMXVertex::InputLayout =
{
    PMXVertex::InputElements,
    PMXVertex::InputElementCount
};

void PMXBoneNode::RecursiveBoneMatrixMultiply(std::vector<gm::Matrix4>& boneMatrix, PMXBoneNode* boneNode, const gm::Matrix4& matrix)
{
    boneMatrix[(int)boneNode->GetBoneIndex()] *= matrix;

    for (auto& childNode : boneNode->_children)
    {
        RecursiveBoneMatrixMultiply(boneMatrix, childNode, boneMatrix[boneNode->GetBoneIndex()]);
    }
   
}

#pragma region Bone
void UpdateBoneMatrices(std::vector<gm::Matrix4>& boneMatrix, PMXBoneNode* boneNode)
{
    
}
/****************************************************************************
*                       Update Local Matrix
*************************************************************************//**
*  @fn        void PMXBoneNode::UpdateLocalMatrix()
*  @brief     Update bone matrix relative to the direct parent bone
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXBoneNode::UpdateLocalMatrix()
{
    Transform transform;
    transform.LocalPosition = AnimateTranslate();
    transform.LocalRotation = AnimateRotate();

    // Append bone node
    if (_isAppendTranslate)
    {
        transform.LocalPosition += _appendTransform.LocalPosition;
    }

    // ik node 
    if (_enableIK)
    {
        transform.LocalRotation = _ikRotation * transform.LocalRotation;
    }

    // append bone rotation
    if (_isAppendRotate)
    {
        transform.LocalRotation = transform.LocalRotation * _appendTransform.LocalRotation;
    }

    // final local matrix
    _localBoneMatrix = transform.GetMatrix();
}

/****************************************************************************
*                       Update Global Matrix
*************************************************************************//**
*  @fn        void PMXBoneNode::UpdateGlobalMatrix()
*  @brief     Update bone matrix to the direct root bone
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXBoneNode::UpdateGlobalMatrix()
{
    auto parent = MatrixIdentity();
    if (_parent != nullptr)
    {
        parent = _parent->GetGlobalMatrix();
    }


    _globalBoneMatrix = _localBoneMatrix * parent;
}

/****************************************************************************
*                       UpdateSelfandChildMatrices
*************************************************************************//**
*  @fn        void PMXBoneNode::UpdateSelfandChildMatrix
*  @brief     Update child bone matrices
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXBoneNode::UpdateSelfandChildMatrix()
{
    UpdateGlobalMatrix();
    for (auto child : _children)
    {
        child->UpdateSelfandChildMatrix();
    }
}
void PMXBoneNode::UpdateChildMatrix()
{
    for (auto child : _children)
    {
        child->UpdateSelfandChildMatrix();
    }
}

void PMXBoneNode::UpdateAppendMatrix()
{
    if (_appendNode == nullptr) { return; }

    if (_isAppendRotate)
    {
        Quaternion appendRotate;
        if (_isAppendLocal)
        { 
            appendRotate = _appendNode->AnimateRotate(); 
        }
        else
        {
            if (_appendNode->GetAppendNode() != nullptr)
            {
                appendRotate = _appendNode->GetAppendRotate();
                
            }
            else
            {
                appendRotate = _appendNode->AnimateRotate();

            }
        }

        if (_appendNode->_enableIK)
        {
            appendRotate = _appendNode->GetIKRotate() * appendRotate;
        }

        if (GetAppendWeight() != 0)
        {
            _appendTransform.LocalRotation = Slerp(Quaternion(), appendRotate, GetAppendWeight());
        }
        else
        {
            _appendTransform.LocalRotation = appendRotate;
        }
        
    }

    if (_isAppendTranslate)
    {
        Vector3 appendTranslate;
        if (_isAppendLocal) { appendTranslate = _appendNode->GetTranslate() - _appendNode->GetInitialTranslate(); }
        else
        {
            if (_appendNode->GetAppendNode() != nullptr)
            {
                appendTranslate = _appendNode->GetAppendTranslate();
            }
            else
            {
                appendTranslate = _appendNode->GetTranslate() - _appendNode->GetInitialTranslate();
            }
        }

        _appendTransform.LocalPosition = appendTranslate * GetAppendWeight();
    }

    UpdateLocalMatrix();
}

void PMXBoneNode::BeforeUpdateMatrix()
{
    LoadInitialSRT();
    _ikRotation = (Quaternion());
    _appendTransform.LocalPosition = Vector3(0, 0, 0);
    _appendTransform.LocalRotation = Quaternion();
}
#pragma endregion Bone

#pragma region BoneIK
void PMXBoneIK::SolveIK(int frameNo)
{
    /*-------------------------------------------------------------------
    -             Initialize Child Node
    ---------------------------------------------------------------------*/
    InitializeChildNode();
    
    float maxDist = FLT_MAX;
    for (int i = 0; i < _iterationCount; ++i)
    {
        SolveCore(i);

        float dist = Norm(_ikTargetBone->GetGlobalMatrix().GetW() - _ikBone->GetGlobalMatrix().GetW());
        if (dist < maxDist)
        {
            maxDist = dist;
            for (auto& chain : _ikChains)
            {
                chain.SaveIKRotation = chain.IKBone->GetIKRotate();
            }
        }
        else
        {
            for (auto& chain : _ikChains)
            {
                chain.IKBone->SetIKRotate(chain.SaveIKRotation);
                chain.IKBone->UpdateLocalMatrix();
                chain.IKBone->UpdateSelfandChildMatrix();
            }
            break;
        }
    }

}

void PMXBoneIK::SolveCore(int iteration)
{
    /*-------------------------------------------------------------------
    -                         Get node
    ---------------------------------------------------------------------*/
    PMXBoneNode* targetNode = _ikTargetBone;
    PMXBoneNode* ikNode     = _ikBone;

    Vector3 ikPosition = Vector3(ikNode->GetGlobalMatrix().GetW());
    for (int chainIndex = 0; chainIndex < _ikChainLength; ++chainIndex)
    {
        auto& chain = _ikChains[chainIndex];
        PMXBoneNode* chainNode = chain.IKBone;

        /*-------------------------------------------------------------------
        -        Pointer check between chain node and ikTarget
        ---------------------------------------------------------------------*/
        if (chainNode == _ikTargetBone) { continue; }

        /*-------------------------------------------------------------------
        -        Solve Plane Check
        ---------------------------------------------------------------------*/
        if (chain.EnableLimit)
        {
            if ((chain.AngleMin.x != 0 || chain.AngleMax.x != 0) &&
                (chain.AngleMin.y == 0 || chain.AngleMax.y == 0) &&
                (chain.AngleMin.z == 0 || chain.AngleMax.z == 0)
                )
            {
                SolvePlane(iteration, chainIndex, SolveAxis::X);
                continue;
            }
            else if ((chain.AngleMin.y != 0 || chain.AngleMax.y != 0) &&
                (chain.AngleMin.x == 0 || chain.AngleMax.x == 0) &&
                (chain.AngleMin.z == 0 || chain.AngleMax.z == 0)
                )
            {
                SolvePlane(iteration, chainIndex, SolveAxis::Y);
                continue;
            }
            else if ((chain.AngleMin.z != 0 || chain.AngleMax.z != 0) &&
                (chain.AngleMin.x == 0 || chain.AngleMax.x == 0) &&
                (chain.AngleMin.y == 0 || chain.AngleMax.y == 0)
                )
            {
                SolvePlane(iteration, chainIndex, SolveAxis::Z);
                continue;
            }
        }

        /*-------------------------------------------------------------------
        -        Prepare variables for CCD-IK
        ---------------------------------------------------------------------*/
        Vector3 targetPosition = Vector3(targetNode->GetGlobalMatrix().GetW());
        auto inverseChain      = Inverse(chainNode->GetGlobalMatrix());
        auto toIK              = Normalize(Vector3(inverseChain * ikPosition));
        auto toEnd             = Normalize(Vector3(inverseChain * targetPosition));
        if (Norm(toIK - toEnd) <= EPSILON) { continue; }

        /*-------------------------------------------------------------------
        -        Calculate  angle and cross vector
        ---------------------------------------------------------------------*/
        float dot          = Clamp(Dot(toEnd, toIK), -1.0f, 1.0f);
        float angle        = ACos(dot);
        angle              = Min(Max(angle, -_angleLimit), _angleLimit); // modify
        auto  cross        = Normalize(Cross(toEnd, toIK));
        auto rotation      = Quaternion(cross, angle);
        auto chainRotation = chainNode->GetIKRotate() * chainNode->AnimateRotate() * rotation;

        auto ikRotation = chainRotation * Inverse(chainNode->AnimateRotate());
        chainNode->SetIKRotate(ikRotation);

        /*-------------------------------------------------------------------
        -        Update chain node
        ---------------------------------------------------------------------*/
        chainNode->UpdateLocalMatrix();
        chainNode->UpdateSelfandChildMatrix();

    }
}

void PMXBoneIK::SolveCCDIK()
{
    /*-------------------------------------------------------------------
    -                         Get node
    ---------------------------------------------------------------------*/
    PMXBoneNode* targetNode = _ikTargetBone;
    PMXBoneNode* ikNode     = _ikBone;

    /*-------------------------------------------------------------------
    -                     Calculate CCD-IK
    ---------------------------------------------------------------------*/
    for (int cycle = 0; cycle < _iterationCount; ++cycle)
    {
        Vector3 ikPosition     = Vector3(ikNode->GetGlobalMatrix().GetW());
        for (int chainIndex = 0; chainIndex < _ikChainLength; ++chainIndex)
        {
            auto& chain            = _ikChains[chainIndex];
            PMXBoneNode* chainNode = chain.IKBone;

            /*-------------------------------------------------------------------
            -        Pointer check between chain node and ikTarget
            ---------------------------------------------------------------------*/
            if (chainNode == _ikTargetBone){ continue;}

            /*-------------------------------------------------------------------
            -        Solve Plane Check
            ---------------------------------------------------------------------*/
            if (chain.EnableLimit)
            {
                if ((chain.AngleMin.x != 0 || chain.AngleMax.x != 0) &&
                    (chain.AngleMin.y == 0 || chain.AngleMax.y == 0) &&
                    (chain.AngleMin.z == 0 || chain.AngleMax.z == 0)
                    )
                {
                    SolvePlane(cycle, chainIndex, SolveAxis::X);
                    continue;
                }
                else if ((chain.AngleMin.y != 0 || chain.AngleMax.y != 0) &&
                    (chain.AngleMin.x == 0 || chain.AngleMax.x == 0) &&
                    (chain.AngleMin.z == 0 || chain.AngleMax.z == 0)
                    )
                {
                    SolvePlane(cycle, chainIndex, SolveAxis::Y);
                    continue;
                }
                else if ((chain.AngleMin.z != 0 || chain.AngleMax.z != 0) &&
                    (chain.AngleMin.x == 0 || chain.AngleMax.x == 0) &&
                    (chain.AngleMin.y == 0 || chain.AngleMax.y == 0)
                    )
                {
                    SolvePlane(cycle, chainIndex, SolveAxis::Z);
                    continue;
                }
            }

            /*-------------------------------------------------------------------
            -        Prepare variables for CCD-IK
            ---------------------------------------------------------------------*/
            Vector3 targetPosition = Vector3(targetNode->GetGlobalMatrix().GetW());
            auto inverseChain      = Inverse(chainNode->GetGlobalMatrix());
            auto toIK  = Normalize(Vector3(inverseChain * ikPosition));
            auto toEnd = Normalize(Vector3(inverseChain * targetPosition));
            if (Norm(toIK - toEnd) <= EPSILON) { continue; }

            /*-------------------------------------------------------------------
            -        Calculate  angle and cross vector
            ---------------------------------------------------------------------*/
            float dot     = Clamp(Dot(toEnd, toIK), -1.0f, 1.0f);
            float angle   = ACos(dot);
            angle         = Min(Max(angle, -_angleLimit), _angleLimit); // modify
            auto  cross   = Normalize(Cross(toEnd, toIK));
            auto rotation = Quaternion(cross, angle);
            auto chainRotation = chainNode->GetIKRotate() * chainNode->AnimateRotate() * rotation;

            auto ikRotation = chainRotation * Inverse(chainNode->AnimateRotate());
            chainNode->SetIKRotate(ikRotation);

            /*-------------------------------------------------------------------
            -        Update chain node
            ---------------------------------------------------------------------*/
            chainNode->UpdateLocalMatrix();
            chainNode->UpdateSelfandChildMatrix();
        }
       
    }

}


void PMXBoneIK::SolvePlane(int cycle, int chainIndex, SolveAxis solveAxis)
{
    int rotateAxisIndex = 0; // xaxis
    Vector3 rotateAxis  = Vector3(1, 0, 0);
    Vector3 plane       = Vector3(0, 1, 1);

    /*-------------------------------------------------------------------
    -                     Set rotate axis and plane 
    ---------------------------------------------------------------------*/
    switch (solveAxis)
    {
        case SolveAxis::X:
        {
            rotateAxisIndex = 0;
            rotateAxis = Vector3(1, 0, 0);
            plane      = Vector3(0, 1, 1);
            break;
        }
        case SolveAxis::Y:
        {
            rotateAxisIndex = 1;
            rotateAxis = Vector3(0, 1, 0);
            plane      = Vector3(1, 0, 1);
            break;
        }
        case SolveAxis::Z:
        {
            rotateAxisIndex = 0;
            rotateAxis = Vector3(0, 0, 1);
            plane      = Vector3(1, 1, 0);
            break;
        }
        default:
        {
            break;
        }


    }

    /*-------------------------------------------------------------------
    -                     Set rotate axis and plane
    ---------------------------------------------------------------------*/
    auto& chain            = _ikChains[chainIndex];
    PMXBoneNode* targetNode = _ikTargetBone;
    PMXBoneNode* ikNode     = _ikBone;

    Vector3 ikPosition     = Vector3(ikNode->GetGlobalMatrix().GetW());
    Vector3 targetPosition = Vector3(targetNode->GetGlobalMatrix().GetW());
    auto inverseChain      = Inverse(chain.IKBone->GetGlobalMatrix());
    auto toIK              = Normalize(Vector3(inverseChain * ikPosition));
    auto toEnd             = Normalize(Vector3(inverseChain * targetPosition));

    float dot   = Clamp(Dot(toEnd, toIK), -1.0f, 1.0f);
    float angle = ACos(dot);
    angle       = Min(Max(angle, -_angleLimit), _angleLimit); // modify

    auto rotation1  = Quaternion(rotateAxis, angle);
    auto toTarget1  = rotation1 * toEnd;
    float dot1      = Dot(toTarget1, toIK);

    auto rotation2  = Quaternion(rotateAxis, -angle);
    auto toTarget2  = rotation2 * toEnd;
    float dot2      = Dot(toTarget2, toIK);

    auto newAngle = chain.PlaneModeAngle;
    if (dot1 > dot2) { newAngle += angle; }
    else             { newAngle -= angle; }

    if (cycle == 0)
    {
        float limitRotateMax = 0;
        float limitRotateMin = 0;
        if      (rotateAxisIndex == 0) { limitRotateMax = chain.AngleMax.x; limitRotateMin = chain.AngleMin.x; }
        else if (rotateAxisIndex == 1) { limitRotateMax = chain.AngleMax.y; limitRotateMin = chain.AngleMin.y; }
        else                           { limitRotateMax = chain.AngleMax.z; limitRotateMin = chain.AngleMin.z;; }

        if (newAngle < limitRotateMin || limitRotateMax < newAngle)
        {
            if (-newAngle < limitRotateMax && limitRotateMin < -newAngle)
            {
                newAngle *= -1;
            }
            else
            {
                float halfAngle = (limitRotateMax + limitRotateMin) * 0.5f;
                if (abs(halfAngle - newAngle) > abs(halfAngle + newAngle))
                {
                    newAngle *= -1;
                }
            }
        }
    }

    if      (rotateAxisIndex == 0) { newAngle = Clamp(newAngle, chain.AngleMin.x, chain.AngleMax.x); }
    else if (rotateAxisIndex == 1) { newAngle = Clamp(newAngle, chain.AngleMin.y, chain.AngleMax.y); }
    else                           { newAngle = Clamp(newAngle, chain.AngleMin.z, chain.AngleMax.z); }
    chain.PlaneModeAngle = newAngle;

    /*-------------------------------------------------------------------
    -        Update chain node
    ---------------------------------------------------------------------*/
    auto ikRotation = Quaternion(rotateAxis, newAngle) * Inverse(chain.IKBone->AnimateRotate());
    chain.IKBone->SetIKRotate(ikRotation);
    chain.IKBone->UpdateLocalMatrix();
    chain.IKBone->UpdateSelfandChildMatrix();
}
void PMXBoneIK::AddIKChain(PMXBoneNode* boneNode, bool axisLimit, const gm::Float3& limitMin, const gm::Float3& limitMax)
{
    PMXIKChain chain;
    chain.IKBone      = boneNode;
    chain.EnableLimit = axisLimit;
    chain.AngleMax    = limitMax;
    chain.AngleMin    = limitMin;
    AddIKChain(std::move(chain));
}
void PMXBoneIK::AddIKChain(PMXIKChain&& chain)
{
    _ikChains.emplace_back(chain);
    _ikChainLength = _ikChains.size();
}

void PMXBoneIK::InsertIKChain(int index, PMXBoneNode* boneNode, bool axisLimit, const gm::Float3& limitMin, const gm::Float3& limitMax)
{
    if (_ikChains.size() <= index) { return; }
    auto it = _ikChains.begin();
    it += index;

    PMXIKChain chain;
    chain.IKBone      = boneNode;
    chain.EnableLimit = axisLimit;
    chain.AngleMax    = limitMax;
    chain.AngleMin    = limitMin;
    _ikChains.insert(it, chain);
}

void PMXBoneIK::InitializeChildNode()
{
    if (GetChainLength() == 0) { return; }
    for (auto& chain : _ikChains)
    {
        chain.IKBone->SetIKRotate(Quaternion());
        chain.IKBone->UpdateLocalMatrix();
        chain.IKBone->UpdateSelfandChildMatrix();
        
    }
}
#pragma endregion BoneIK
#pragma region MotionState
class MMDMotionState : public btMotionState
{
public:
    virtual void Reset() = 0;
    virtual void ReflectGlobalTransform() = 0;
};

class DefaultMMDMotionState : public MMDMotionState
{
public:
    /****************************************************************************
    **                Public Function
    *****************************************************************************/
    void Reset() override { _transform = _initialTransform; }
    void ReflectGlobalTransform() override {};

    /****************************************************************************
    **                Public Member Variables
    *****************************************************************************/
    void getWorldTransform(btTransform& worldTransform) const override
    {
        worldTransform = _transform;
    }
    void setWorldTransform(const btTransform& worldTransform)override
    {
        _transform = worldTransform;
    }

    /****************************************************************************
    **                Constructor and Destructor
    *****************************************************************************/
    DefaultMMDMotionState(const btTransform& transform)
    {
        _transform = transform;
        _initialTransform = _transform;
    };
private:
    /****************************************************************************
    **                Private Function
    *****************************************************************************/

    /****************************************************************************
    **                Private Member Variables
    *****************************************************************************/
    btTransform _initialTransform;
    btTransform _transform;
};

class DynamicsMMDMotionState : public MMDMotionState
{
public:
    /****************************************************************************
    **                Public Function
    *****************************************************************************/
    void Reset() override
    {
        Matrix4 m = _offset * _boneNode->GetGlobalMatrix();

        _transform.setBasis(btMatrix3x3(m.GetX().GetX(), m.GetX().GetY(), m.GetX().GetZ(), m.GetY().GetX(), m.GetY().GetY(), m.GetY().GetZ(), m.GetZ().GetX(), m.GetZ().GetY(), m.GetZ().GetZ()));
        _transform.setOrigin(btVector3(m.GetW().GetX(), m.GetW().GetY(), m.GetW().GetZ()));
    }

    void ReflectGlobalTransform() override
    {
        if (_override)
        {
            Matrix3 basis;
            Vector3 trans;
            ConvertBtTransformToRotationAndTranslation(_transform, basis, trans);

            _boneNode->SetGlobalMatrix(_inverseOffset * Matrix4(basis, trans));
            _boneNode->UpdateChildMatrix();
        }
    }
    /****************************************************************************
    **                Public Member Variables
    *****************************************************************************/
    void getWorldTransform(btTransform& worldTransform) const override { worldTransform = _transform; }
    void setWorldTransform(const btTransform& worldTransform) override { _transform = worldTransform; }

    /****************************************************************************
    **                Constructor and Destructor
    *****************************************************************************/
    DynamicsMMDMotionState(PMXBoneNode* boneNode, const Matrix4& offset, bool isOverride = true)
        :_boneNode(boneNode), _offset(offset), _override(isOverride)
    {
        _inverseOffset = Inverse(offset);
        Reset();
    }

private:
    /****************************************************************************
    **                Private Function
    *****************************************************************************/

    /****************************************************************************
    **                Private Member Variables
    *****************************************************************************/
private: 
    PMXBoneNode*    _boneNode;
    Matrix4         _offset;
    Matrix4         _inverseOffset;
    btTransform _transform;
    bool            _override;

};

class DynamicAndBoneMergeMMDMotionState : public MMDMotionState
{
public:
    /****************************************************************************
    **                Public Function
    *****************************************************************************/
    void Reset() override
    {
        Matrix4 m = _offset * _boneNode->GetGlobalMatrix();
        ConvertRotationAndTranslationToBtTransform(m.Get3x3(), Vector3(m.GetW()), _transform);
    }
    void ReflectGlobalTransform() override
    {
        if (_isOverride)
        {
            Matrix3 basis;
            Vector3 trans;
            ConvertBtTransformToRotationAndTranslation(_transform, basis, trans);

            _boneNode->SetGlobalMatrix(_inverseOffset * Matrix4(basis, trans));
            _boneNode->UpdateChildMatrix();
        }
    }

    /****************************************************************************
    **                Public Member Variables
    *****************************************************************************/
    void getWorldTransform(btTransform& worldTransform) const override { worldTransform = _transform; }
    void setWorldTransform(const btTransform& worldTransform) override { _transform = worldTransform; }

    /****************************************************************************
    **                Constructor and Destructor
    *****************************************************************************/
    DynamicAndBoneMergeMMDMotionState(PMXBoneNode* node, const Matrix4& offset, bool isOverride = true)
        :_boneNode(node), _offset(offset), _isOverride(isOverride)
    {
        _inverseOffset = Inverse(offset);
        Reset();
    }
private:
    /****************************************************************************
    **                Private Function
    *****************************************************************************/

    /****************************************************************************
    **                Private Member Variables
    *****************************************************************************/
    PMXBoneNode*    _boneNode;
    Matrix4         _offset;
    Matrix4         _inverseOffset;
    btTransform     _transform;
    bool            _isOverride;
};

class KinematicMMDMotionState : public MMDMotionState
{
public:
    /****************************************************************************
    **                Public Function
    *****************************************************************************/
    void Reset() override {};
    void ReflectGlobalTransform() override {};

    /****************************************************************************
    **                Public Member Variables
    *****************************************************************************/
    void getWorldTransform(btTransform& worldTransform) const override
    {
        Matrix4 m;
        if (_boneNode != nullptr)
        {
            m = _offset * _boneNode->GetGlobalMatrix();
        }
        else
        {
            m = _offset;
        }

        ConvertRotationAndTranslationToBtTransform(m.Get3x3(), Vector3(m.GetW()), worldTransform);
    }
    void setWorldTransform(const btTransform& worldTransform) override {};

    /****************************************************************************
    **                Constructor and Destructor
    *****************************************************************************/
    KinematicMMDMotionState(PMXBoneNode* boneNode, const Matrix4& offset) : _boneNode(boneNode), _offset(offset) {};
private:
    /****************************************************************************
    **                Private Function
    *****************************************************************************/

    /****************************************************************************
    **                Private Member Variables
    *****************************************************************************/
    PMXBoneNode* _boneNode;
    Matrix4      _offset;
};
#pragma endregion MotionState
#pragma region RigidBody
PMXRigidBody::PMXRigidBody() : _rigidBodyType(RigidBodyType::Kinematic), _group(0), _groupMask(0), _boneNode(0), _offsetMatrix()
{
    
}
PMXRigidBody::~PMXRigidBody()
{

}
/****************************************************************************
*                       Create
*************************************************************************//**
*  @fn        bool PMXRigidBody::Create(const pmx::PMXRigidBody& pmxRigidBody, PMXModel* model, PMXBoneNode* node)
*  @brief     Create new rigidBody
*  @param[in] pmx::PMXRigidBody& pmxRigidBody
*  @param[in,out] PMXModel* model,
*  @param[in,out] PMXBoneNode* node
*  @return 　　void
*****************************************************************************/
bool PMXRigidBody::Create(const pmx::PMXRigidBody& pmxRigidBody, PMXModel* model, PMXBoneNode* node)
{
    Destroy();

    /*-------------------------------------------------------------------
    -                   Get Shape
    ---------------------------------------------------------------------*/
    switch (pmxRigidBody.Shape)
    {
        case pmx::PMXRigidBodyShape::Sphere:
        {
            _shape = std::make_unique<btSphereShape>(btScalar(pmxRigidBody.ShapeSize.x));
            break;
        }
        case pmx::PMXRigidBodyShape::Box:
        {
            _shape = std::make_unique<btBoxShape>(btVector3(pmxRigidBody.ShapeSize.x, pmxRigidBody.ShapeSize.y, pmxRigidBody.ShapeSize.z));
            break;
        }
        case pmx::PMXRigidBodyShape::Capsule:
        {
            _shape = std::make_unique<btCapsuleShape>(btScalar(pmxRigidBody.ShapeSize.x), btScalar(pmxRigidBody.ShapeSize.y));
            break;
        }
        default: { break; }
    }
    if (_shape == nullptr) { return false; }

    /*-------------------------------------------------------------------
   -                   Calculate local inertia
   ---------------------------------------------------------------------*/
    btScalar   mass         = 0.0f;
    btVector3 localInertia = btVector3(0, 0, 0);
    if (pmxRigidBody.RigidBodyOperation != pmx::PMXRigidBodyOperation::Static)
    {
        mass = pmxRigidBody.Mass;
    }
    if (mass != 0)
    {
        _shape->calculateLocalInertia(mass, localInertia);
    }

    /*-------------------------------------------------------------------
   -                   Rotation
   ---------------------------------------------------------------------*/
    Matrix4 rx = RotationAxis(Vector3(1, 0, 0), pmxRigidBody.Rotation.x);
    Matrix4 ry = RotationAxis(Vector3(0, 1, 0), pmxRigidBody.Rotation.y);
    Matrix4 rz = RotationAxis(Vector3(0, 0, 1), pmxRigidBody.Rotation.z);
    Matrix4 rotate = rz * rx * ry;
    Matrix4 rtMatrix = rotate * Translation(pmxRigidBody.Translation);
    if (node != nullptr)
    {
        rtMatrix *= node->GetGlobalMatrix();
    }
    else
    {
        rtMatrix *= model->GetRootBoneNode("全ての親")->GetGlobalMatrix();
    }

    /*-------------------------------------------------------------------
    -                   Set offset matrix and kinematicNode
    ---------------------------------------------------------------------*/
    PMXBoneNode* kinematicNode = nullptr;
    bool         overrideNode  = true;
    if (node != nullptr)
    {
        _offsetMatrix = rtMatrix * Inverse(node->GetGlobalMatrix());
        kinematicNode = node;
    }
    else
    {
        _offsetMatrix = rtMatrix * Inverse(model->GetRootBoneNode(ROOT_BONE_NAME)->GetGlobalMatrix());
        kinematicNode = model->GetRootBoneNode(ROOT_BONE_NAME);
        overrideNode = false;
    }

    /*-------------------------------------------------------------------
    -                   Set offset matrix and kinematicNode
    ---------------------------------------------------------------------*/
    btMotionState* motionState = nullptr;
    if (pmxRigidBody.RigidBodyOperation == pmx::PMXRigidBodyOperation::Static)
    {
        _kinematicMotionState = std::make_unique<KinematicMMDMotionState>(kinematicNode, _offsetMatrix);
        motionState           = _kinematicMotionState.get();
    }
    else
    {
        if (node != nullptr)
        {
            if (pmxRigidBody.RigidBodyOperation == pmx::PMXRigidBodyOperation::Dynamic)
            {
                _activeMotionState    = std::make_unique<DynamicsMMDMotionState>(kinematicNode, _offsetMatrix);
                _kinematicMotionState = std::make_unique<KinematicMMDMotionState>(kinematicNode, _offsetMatrix);
                motionState           = _activeMotionState.get();
            }
            else if (pmxRigidBody.RigidBodyOperation == pmx::PMXRigidBodyOperation::DynamicAndBoneMerge)
            {
                _activeMotionState    = std::make_unique<DynamicAndBoneMergeMMDMotionState>(kinematicNode, _offsetMatrix);
                _kinematicMotionState = std::make_unique<KinematicMMDMotionState>(kinematicNode, _offsetMatrix);
                motionState           = _activeMotionState.get();
            }
        }
        else
        {
            btTransform t;
            ConvertRotationAndTranslationToBtTransform(_offsetMatrix.Get3x3(), Vector3(_offsetMatrix.GetW()), t);
            _activeMotionState    = std::make_unique<DefaultMMDMotionState>(t);
            _kinematicMotionState = std::make_unique<KinematicMMDMotionState>(kinematicNode, _offsetMatrix);
            motionState = _activeMotionState.get();
        }
    }

    /*-------------------------------------------------------------------
   -                   Set RigidBodyInfomation
   ---------------------------------------------------------------------*/
    btRigidBody::btRigidBodyConstructionInfo rigidBodyInfomation(mass, motionState, _shape.get(), localInertia);
    rigidBodyInfomation.m_linearDamping     = btScalar(pmxRigidBody.DampingTranslation);
    rigidBodyInfomation.m_angularDamping    = btScalar(pmxRigidBody.DampingRotation);
    rigidBodyInfomation.m_restitution       = btScalar(pmxRigidBody.Repulsion);
    rigidBodyInfomation.m_friction          = btScalar(pmxRigidBody.Friction);
    rigidBodyInfomation.m_additionalDamping = true;

    /*-------------------------------------------------------------------
   -                   Create RigidBody
   ---------------------------------------------------------------------*/
    _rigidBody = std::make_unique<btRigidBody>(rigidBodyInfomation);
    _rigidBody->setUserPointer(this);
    _rigidBody->setSleepingThresholds(btScalar(0.01), btScalar(0.1 * GM_PI / 180.0));
    _rigidBody->setActivationState(DISABLE_DEACTIVATION);
    if (pmxRigidBody.RigidBodyOperation == pmx::PMXRigidBodyOperation::Static)
    {
        _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    }
    
    _rigidBodyType = (RigidBodyType)pmxRigidBody.RigidBodyOperation;
    _group         = pmxRigidBody.Group;
    _groupMask     = pmxRigidBody.CollisionGroup;
    _boneNode      = node;
    _name          = pmxRigidBody.Name;
    return true;
}
/****************************************************************************
*                       Destroy
*************************************************************************//**
*  @fn        void PMXRigidBody::Destroy()
*  @brief     Release shape
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXRigidBody::Destroy()
{
    _shape = nullptr;
}
/****************************************************************************
*                       ReflectGlobalMatrix
*************************************************************************//**
*  @fn        void PMXRigidBody::ReflectGlobalMatrix()
*  @brief     Reflect global matrix
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXRigidBody::ReflectGlobalMatrix()
{
    if (_activeMotionState != nullptr)
    {
        _activeMotionState->ReflectGlobalTransform();
    }
    if (_kinematicMotionState != nullptr)
    {
        _kinematicMotionState->ReflectGlobalTransform();
    }
}
/****************************************************************************
*                       CalculateLocalMatrix
*************************************************************************//**
*  @fn        void PMXRigidBody::CalculateLocalMatrix()
*  @brief     Caclulate local matrix
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXRigidBody::CalculateLocalMatrix()
{
    if (_boneNode != nullptr)
    {
        auto parent = _boneNode->GetParent();
        if (parent != nullptr)
        {
            auto local = _boneNode->GetGlobalMatrix() * Inverse(parent->GetGlobalMatrix());
            _boneNode->SetLocalMatrix(_boneNode->GetGlobalMatrix());
        }
        else
        {
            _boneNode->SetLocalMatrix(_boneNode->GetGlobalMatrix());
        }
    }
}
/****************************************************************************
*                       GetRigidBody
*************************************************************************//**
*  @fn        btRigidBody* PMXRigidBody::GetRigidBody() const
*  @brief     Return rigidbody pointer
*  @param[in] void
*  @return 　　RigidBody*
*****************************************************************************/
btRigidBody* PMXRigidBody::GetRigidBody() const
{
    return _rigidBody.get();
}
/****************************************************************************
*                       GetGroup
*************************************************************************//**
*  @fn        UINT16 PMXRigidBody::GetGroup()     const
*  @brief     Return UINT16 group
*  @param[in] void
*  @return 　　UINT16
*****************************************************************************/
UINT16 PMXRigidBody::GetGroup()     const
{
    return _group;
}
/****************************************************************************
*                       GetGroupMask
*************************************************************************//**
*  @fn        UINT16 PMXRigidBody::GetGroupMask()     const
*  @brief     Return UINT16 group
*  @param[in] void
*  @return 　　UINT16
*****************************************************************************/
UINT16 PMXRigidBody::GetGroupMask() const
{
    return _groupMask;
}
/****************************************************************************
*                       GetTransformMatrix
*************************************************************************//**
*  @fn        Matrix4 PMXRigidBody::GetTransformMatrix()
*  @brief     Get transform matrix
*  @param[in] void
*  @return 　　Matrix4
*****************************************************************************/
Matrix4 PMXRigidBody::GetTransformMatrix()
{
    btTransform transform = _rigidBody->getCenterOfMassTransform();
    Matrix3 r; Vector3 t;
    ConvertBtTransformToRotationAndTranslation(transform, r, t);
    return Matrix4(r,t);
}
/****************************************************************************
*                       SetActivation
*************************************************************************//**
*  @fn        void PMXRigidBody::SetActivation(bool activation)
*  @brief     Activate rigidBody and motionState, and collisionFlags
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXRigidBody::SetActivation(bool activation)
{
    if (_rigidBodyType != RigidBodyType::Kinematic)
    {
        if (activation)
        {
            _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
            _rigidBody->setMotionState(_activeMotionState.get());
        }
        else
        {
            _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
            _rigidBody->setMotionState(_kinematicMotionState.get());
        }
    }
    else
    {
        _rigidBody->setMotionState(_kinematicMotionState.get());
    }
}
/****************************************************************************
*                       ResetTransform
*************************************************************************//**
*  @fn        void PMXRigidBody::ResetTransform()
*  @brief     Reset transform
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXRigidBody::ResetTransform()
{
    if (_activeMotionState != nullptr)
    {
        _activeMotionState->Reset();
    }
}
/****************************************************************************
*                          Reset
*************************************************************************//**
*  @fn        void PMXRigidBody::Reset(MMDPhysics* physics)
*  @brief     Reset
*  @param[in,out] MMDPhysics* phisics 
*  @return 　　void
*****************************************************************************/
void PMXRigidBody::Reset(MMDPhysics* physics)
{
    auto cache = physics->GetDynamicsWorld()->getPairCache();
    if (cache != nullptr)
    {
        auto dispatcher = physics->GetDynamicsWorld()->getDispatcher();
        cache->cleanProxyFromPairs(_rigidBody->getBroadphaseHandle(), dispatcher);
    }
    _rigidBody->setAngularVelocity(btVector3(0,0,0));
    _rigidBody->setLinearVelocity(btVector3(0, 0, 0));
    _rigidBody->clearForces();
}
#pragma endregion RigidBody
#pragma region Joint

PMXJoint::PMXJoint()
{

}
PMXJoint::~PMXJoint()
{

}
/****************************************************************************
*                         CreateJoint
*************************************************************************//**
*  @fn        bool PMXJoint::CreateJoint(const pmx::PMXJoint& pmxJoint, PMXRigidBody* rigidBodyA, PMXRigidBody* rigidBodyB)
*  @brief     Create joint
*  @param[in] pmx::PMXJoint pmxJoint
*  @param[in,out] PMXRigidBody* rigidBodyA
*  @param[in,out] PMXRigidBody* rigidBodyB
*  @return 　　void
*****************************************************************************/
bool PMXJoint::CreateJoint(const pmx::PMXJoint& pmxJoint, PMXRigidBody* rigidBodyA, PMXRigidBody* rigidBodyB)
{
    Destroy();

    btMatrix3x3 rotation;
    rotation.setEulerZYX(pmxJoint.Rotation.x, pmxJoint.Rotation.y, pmxJoint.Rotation.z);
    
    /*-------------------------------------------------------------------
    -                      Set affineTransform
    ---------------------------------------------------------------------*/
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(pmxJoint.Translation.x, pmxJoint.Translation.y, pmxJoint.Translation.z));
    transform.setBasis(rotation);

    btTransform inverseA = rigidBodyA->GetRigidBody()->getWorldTransform().inverse();
    btTransform inverseB = rigidBodyB->GetRigidBody()->getWorldTransform().inverse();
    inverseA = inverseA * transform;
    inverseB = inverseB * transform;

    /*-------------------------------------------------------------------
    -                      Set Generic6DofSpringConstraint
    ---------------------------------------------------------------------*/
    auto constraint = std::make_unique<btGeneric6DofSpringConstraint>(*rigidBodyA->GetRigidBody(), *rigidBodyB->GetRigidBody(), inverseA, inverseB, true);
    constraint->setLinearLowerLimit(btVector3(pmxJoint.TranslationMin.x, pmxJoint.TranslationMin.y, pmxJoint.TranslationMin.z));
    constraint->setLinearUpperLimit(btVector3(pmxJoint.TranslationMax.x, pmxJoint.TranslationMax.y, pmxJoint.TranslationMax.z));
    constraint->setAngularLowerLimit(btVector3(pmxJoint.RotationMin.x, pmxJoint.RotationMin.y, pmxJoint.RotationMin.z));
    constraint->setAngularUpperLimit(btVector3(pmxJoint.RotationMax.x, pmxJoint.RotationMax.y, pmxJoint.RotationMax.z));

    /*-------------------------------------------------------------------
    -                      Set affineTransform
    ---------------------------------------------------------------------*/
    if (pmxJoint.SpringTranslationFactor.x != 0)
    {
        constraint->enableSpring(0, true);
        constraint->setStiffness(0, pmxJoint.SpringTranslationFactor.x);
    }
    if (pmxJoint.SpringTranslationFactor.y != 0)
    {
        constraint->enableSpring(1, true);
        constraint->setStiffness(1, pmxJoint.SpringTranslationFactor.y);
    }
    if (pmxJoint.SpringTranslationFactor.z != 0)
    {
        constraint->enableSpring(2, true);
        constraint->setStiffness(2, pmxJoint.SpringTranslationFactor.x);
    }
    if (pmxJoint.SpringRotationFactor.x != 0)
    {
        constraint->enableSpring(3, true);
        constraint->setStiffness(3, pmxJoint.SpringRotationFactor.x);
    }
    if (pmxJoint.SpringRotationFactor.y != 0)
    {
        constraint->enableSpring(4, true);
        constraint->setStiffness(4, pmxJoint.SpringRotationFactor.y);
    }
    if (pmxJoint.SpringRotationFactor.x != 0)
    {
        constraint->enableSpring(5, true);
        constraint->setStiffness(5, pmxJoint.SpringRotationFactor.z);
    }

    _constraint = std::move(constraint);
    return true;
}
/****************************************************************************
*                          Destroy
*************************************************************************//**
*  @fn        void PMXJoint::Destroy()
*  @brief     Release constraint
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void PMXJoint::Destroy()
{
    _constraint = nullptr;
}
/****************************************************************************
*                          GetConstraint
*************************************************************************//**
*  @fn        btTypedConstraint* PMXJoint::GetConstraint() const
*  @brief     return TypedConstraint pointer
*  @param[in] void
*  @return 　　TypedConstraint
*****************************************************************************/
btTypedConstraint* PMXJoint::GetConstraint() const
{
    return _constraint.get();
}
#pragma endregion Joint
#pragma region PMXPhysics

/****************************************************************************
*				  			TemplateStruct
*************************************************************************//**
*  @struct     TemplateStruct
*  @brief     temp
*****************************************************************************/
struct MMDFilterCallback : public btOverlapFilterCallback
{
public:
    bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override;

    std::vector<btBroadphaseProxy*> NonFilterProxy;

};
bool MMDFilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
    auto findIterator = std::find_if(NonFilterProxy.begin(), NonFilterProxy.end(), [proxy0, proxy1](const auto& x) { return x == proxy0 || x == proxy1; });
    if (findIterator != NonFilterProxy.end())
    {
        return true;
    }

    bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
    collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);
    return collides;
}

MMDPhysics::MMDPhysics() :_fps(60.0f), _maxSubStepCount(10)
{

}
MMDPhysics::~MMDPhysics()
{
    Destroy();
}
/****************************************************************************
*                       Create
*************************************************************************//**
*  @fn        bool MMDPhysics::Create()
*  @brief     initialize MMD physics. allocate memory
*  @param[in] test
*  @return 　　bool 
*****************************************************************************/
bool MMDPhysics::Create()
{
    _broadPhase      = std::make_unique<btDbvtBroadphase>();
    _collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    _dispatcher      = std::make_unique<btCollisionDispatcher>(_collisionConfig.get());
    _solver          = std::make_unique<btSequentialImpulseConstraintSolver>();
    _world           = std::make_unique<btDiscreteDynamicsWorld>(_dispatcher.get(), _broadPhase.get(), _solver.get(), _collisionConfig.get());
    _world->setGravity(btVector3(0, 9.8 * 10.0, 0));
    _groundShape     = std::make_unique<btStaticPlaneShape>(btVector3(0, 1, 0), 0.0);
    btTransform groundTransform;
    groundTransform.setIdentity();
    _groundMotionState = std::make_unique<btDefaultMotionState>(groundTransform);

    /*-------------------------------------------------------------------
    -                  Add rigid body to collision world
    ---------------------------------------------------------------------*/
    btRigidBody::btRigidBodyConstructionInfo groundInfomation(0, _groundMotionState.get(), _groundShape.get(), btVector3(0, 0, 0));
    _groundRigidBody = std::make_unique<btRigidBody>(groundInfomation);
    _world->addRigidBody(_groundRigidBody.get());

    auto filterCallback = std::make_unique<MMDFilterCallback>();
    filterCallback->NonFilterProxy.push_back(_groundRigidBody->getBroadphaseProxy());
    _world->getPairCache()->setOverlapFilterCallback(filterCallback.get());
    _filterCallback = std::move(filterCallback);
    return true;
}
/****************************************************************************
*                       Update
*************************************************************************//**
*  @fn        bool MMDPhysics::Update(float time)
*  @brief     Calculate physics simulation
*  @param[in] float time
*  @return 　　bool
*****************************************************************************/
bool MMDPhysics::Update(float deltaTime)
{
    if (_world != nullptr)
    {
        _fps = 1.0f/ deltaTime;
        _world->stepSimulation(deltaTime, _maxSubStepCount, 1.0f / _fps);
    }
    return true;
}
/****************************************************************************
*                       Destroy
*************************************************************************//**
*  @fn        void MMDPhysics::Destroy()
*  @brief     Remove rigidbody and unique_ptr Release 
*  @param[in] void
*  @return 　　void
*****************************************************************************/
void MMDPhysics::Destroy()
{
    if (_world != nullptr && _groundRigidBody != nullptr)
    {
        _world->removeRigidBody(_groundRigidBody.get());
    }

    _broadPhase        = nullptr;
    _collisionConfig   = nullptr;
    _dispatcher        = nullptr;
    _solver            = nullptr;
    _world             = nullptr;
    _groundShape       = nullptr;
    _groundMotionState = nullptr;
    _groundRigidBody   = nullptr;
}
/****************************************************************************
*                       AddRigidBody
*************************************************************************//**
*  @fn        void MMDPhysics::AddRigidBody(PMXRigidBody* rigidBody)
*  @brief     Add rigidbody
*  @param[in,out] PMXRigidBody* rigidBody
*  @return 　　void
*****************************************************************************/
void MMDPhysics::AddRigidBody(PMXRigidBody* rigidBody)
{
    _world->addRigidBody(rigidBody->GetRigidBody(), 1 << rigidBody->GetGroup(), rigidBody->GetGroupMask());
}
/****************************************************************************
*                       RemoveRigidBody
*************************************************************************//**
*  @fn        void MMDPhysics::RemoveRigidBody(PMXRigidBody* rigidBody)
*  @brief     Remove rigidbody
*  @param[in,out] PMXRigidBody* rigidBody
*  @return 　　void
*****************************************************************************/
void MMDPhysics::RemoveRigidBody(PMXRigidBody* rigidBody)
{
    _world->removeRigidBody(rigidBody->GetRigidBody());
}
/****************************************************************************
*                       AddJoint
*************************************************************************//**
*  @fn        void MMDPhysics::AddJoint(PMXJoint* joint)
*  @brief     Add joint 
*  @param[in,out] PMXJoint* joint
*  @return 　　void
*****************************************************************************/
void MMDPhysics::AddJoint(PMXJoint* joint)
{
    if (joint->GetConstraint() != nullptr)
    {
        _world->addConstraint(joint->GetConstraint());
    }
}
/****************************************************************************
*                       RemoveJoint
*************************************************************************//**
*  @fn        void MMDPhysics::RemoveJoint(PMXJoint* joint)
*  @brief     Remove joint
*  @param[in,out] PMXJoint* joint
*  @return 　　void
*****************************************************************************/
void MMDPhysics::RemoveJoint(PMXJoint* joint)
{
    if (joint->GetConstraint() != nullptr)
    {
        _world->removeConstraint(joint->GetConstraint());
    }
}
/****************************************************************************
*                       GetFPS
*************************************************************************//**
*  @fn        float MMDPhysics::GetFPS() const
*  @brief     Get FPS
*  @param[in] void
*  @return 　　float
*****************************************************************************/
float MMDPhysics::GetFPS() const
{
    return _fps;
}
/****************************************************************************
*                       GetMaxSubStepCount
*************************************************************************//**
*  @fn        int MMDPhysics::GetMaxSubStepCount() const
*  @brief     Get maxSubStepCount
*  @param[in] void
*  @return 　　int
*****************************************************************************/
int MMDPhysics::GetMaxSubStepCount() const
{
    return _maxSubStepCount;
}
/****************************************************************************
*                       GetDynamicsWorld
*************************************************************************//**
*  @fn        DiscreteDynamicsWorld* MMDPhysics::GetDynamicsWorld() const
*  @brief     Return world pointer
*  @param[in] void
*  @return 　　DiscreteDynamicsWorld*
*****************************************************************************/
btDiscreteDynamicsWorld* MMDPhysics::GetDynamicsWorld() const
{
    return _world.get();
}
/****************************************************************************
*                       SetFPS
*************************************************************************//**
*  @fn        void MMDPhysics::SetFPS(float fps)
*  @brief     Set FPS
*  @param[in] float fps
*  @return 　　void
*****************************************************************************/
void MMDPhysics::SetFPS(float fps)
{
    _fps = fps;
}
/****************************************************************************
*                      SetMaxSubStepCount
*************************************************************************//**
*  @fn        void  MMDPhysics::SetMaxSubStepCount(int numSteps)
*  @brief     Set max substep count
*  @param[in] int numStep
*  @return 　　void
*****************************************************************************/
void  MMDPhysics::SetMaxSubStepCount(int numSteps)
{
    _maxSubStepCount = numSteps;
}
#pragma endregion PMXPhysics