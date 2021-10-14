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
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
#define EPSILON (0.0005f)
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
    { "DEF_C"    ,  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "DEF_R_ZERO", 0, DXGI_FORMAT_R32G32B32_FLOAT ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "DEF_R_ONE",  0, DXGI_FORMAT_R32G32B32_FLOAT ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "WEIGHT_TYPE",0, DXGI_FORMAT_R8_UINT         ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(PMXVertex) == 125, "Vertex struct/layout mismatch");
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