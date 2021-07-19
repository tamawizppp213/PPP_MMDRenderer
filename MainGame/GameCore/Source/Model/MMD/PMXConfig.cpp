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
    { "BONE_NO",    0, DXGI_FORMAT_R32G32B32A32_SINT , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "WEIGHT"   ,  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "DEF_C"    ,  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "DEF_R_ZERO", 0, DXGI_FORMAT_R32G32B32_FLOAT ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "DEF_R_ONE",  0, DXGI_FORMAT_R32G32B32_FLOAT ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "WEIGHT_TYPE",0, DXGI_FORMAT_R8_UINT         ,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

static_assert(sizeof(PMXVertex) == 101, "Vertex struct/layout mismatch");
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
    if (_isAppendTranslate)
    {
        transform.LocalPosition += _appendTransform.LocalPosition;
    }

    transform.LocalRotation = AnimateRotate();
    if (_enableIK)
    {
        transform.LocalRotation = transform.LocalRotation * _ikRotation;
    }
    if (_isAppendRotate)
    {
        transform.LocalRotation = _appendTransform.LocalRotation * transform.LocalRotation;
    }
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
*                       UpdateChildMatrices
*************************************************************************//**
*  @fn        void PMXBoneNode::UpdateChildMatrices()
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
            appendRotate =  appendRotate * _appendNode->GetIKRotate();
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
void PMXBoneIK::SolveIK(int frameNo, VMDFile* vmdFile)
{
    /*-------------------------------------------------------------------
    -             enable ik check
    ---------------------------------------------------------------------*/

    /*-------------------------------------------------------------------
    -             Current Frame IK Data
    ---------------------------------------------------------------------*/
    auto iterator = std::find_if(vmdFile->GetEnableIKData().rbegin(), vmdFile->GetEnableIKData().rend(),
        [frameNo](const VMDIK& ikEnable)
    {
        return ikEnable.FrameNo <= frameNo;
    });

    if (iterator != vmdFile->GetEnableIKData().rend())
    {
        auto ikEnableIterator = iterator->IKEnableTable.find(_ikBone->GetBoneName());
        if (ikEnableIterator != iterator->IKEnableTable.end())
        {
            if (!ikEnableIterator->second)
            {
                return;
            }
        }
    }

    /*-------------------------------------------------------------------
    -             Initialize Child Node
    ---------------------------------------------------------------------*/
    InitializeChildNode();

    /*-------------------------------------------------------------------
    -             Calculate IK Solver
    ---------------------------------------------------------------------*/
    auto childNodesCount = GetChainLength();
    switch (childNodesCount)
    {
    case 0:
        assert(0);
        return;
    /*case 1:
        SolveLookAt();
        break;
    case 2:
        SolveCosIK();
        break;*/
    default:
        SolveCCDIK();
        break;
    }
    

}


void PMXBoneIK::SolveIK(int frameNo, PMXData* pmxFile, const PMXBoneNodeAddressList& boneAddressList, VMDFile* vmdFile)
{
    auto iterator = std::find_if(vmdFile->GetEnableIKData().rbegin(), vmdFile->GetEnableIKData().rend(),
        [frameNo](const VMDIK& ikEnable)
    {
        return ikEnable.FrameNo <= frameNo;
    });
    
    for (int i = 0; i < pmxFile->GetBoneIKCount(); ++i)
    {
        auto& ik = pmxFile->GetBoneIK()[i];

        if (iterator != vmdFile->GetEnableIKData().rend())
        {
            auto ikEnableIterator = iterator->IKEnableTable.find(ik.GetIKBoneNode()->GetBoneName());
            if (ikEnableIterator != iterator->IKEnableTable.end())
            {
                if (!ikEnableIterator->second)
                {
                    continue;
                }
            }
        }

        auto childNodesCount = ik.GetChainLength();
        switch (childNodesCount)
        {
        case 0:
            assert(0);
            continue;
        case 1:
            SolveLookAt(ik, boneAddressList);
            break;
        case 2:
            SolveCosIK(ik, boneAddressList);
            break;
        default:
            //SolveCCDIK(ik, pmxFile->GetBoneAddressList());
            break;
        }

    }

    
}

void PMXBoneIK::SolveLookAt()
{

}
void PMXBoneIK::SolveLookAt(const PMXBoneIK& ik, const PMXBoneNodeAddressList& boneNodeAddressArray)
{
    auto rootBoneNode   = ik.GetChains()[0].IKBone;
    auto targetBoneNode = ik.GetTargetBoneNode();
    auto ikBoneNode     = ik.GetIKBoneNode();

    auto inverseRoot    = Inverse(rootBoneNode->GetGlobalMatrix());
    auto targetPosition = Vector3(targetBoneNode->GetGlobalMatrix().GetW());
    auto ikPosition     = Vector3(ikBoneNode->GetGlobalMatrix().GetW());
   
    Vector3 localTarget = TransformCoordinateVector3(targetPosition, inverseRoot);
    Vector3 localIK     = TransformCoordinateVector3(ikPosition, inverseRoot);

    Vector3 toTarget = Normalize(localTarget);
    Vector3 toIK     = Normalize(localIK);

    Vector3 axis  = Normalize(Cross(toIK, toTarget));
    float   theta = ACos(Dot(toTarget, toIK));

    rootBoneNode  ->SetRotate(Quaternion(axis, theta));

    rootBoneNode  ->UpdateGlobalMatrix();
    targetBoneNode->UpdateGlobalMatrix();
    ikBoneNode    ->UpdateGlobalMatrix();

}

void PMXBoneIK::SolveCosIK()
{

}
void PMXBoneIK::SolveCosIK(const PMXBoneIK& ik, const PMXBoneNodeAddressList& boneNodeAddressArray)
{
     /*-------------------------------------------------------------------
    -             Get bone nodes
    ---------------------------------------------------------------------*/
    auto targetNode       = ik.GetTargetBoneNode();
    auto ikNode           = ik.GetIKBoneNode();
    auto rootNode         = ik.GetChainsVector()[1].IKBone;
    auto intermediateNode = ik.GetChainsVector()[0].IKBone;

    /*-------------------------------------------------------------------
    -            Get Position in model space
    ---------------------------------------------------------------------*/
    auto inverseRoot = Inverse(rootNode->GetGlobalMatrix());
    auto intermediatePosition = Vector3(intermediateNode->GetGlobalMatrix().GetW());
    auto targetPosition       = Vector3(targetNode->GetGlobalMatrix().GetW());
    auto ikPosition           = Vector3(ikNode->GetGlobalMatrix().GetW());
    /*-------------------------------------------------------------------
    -            Get Position in world space
    ---------------------------------------------------------------------*/
    Vector3 localTarget       = TransformCoordinateVector3(targetPosition      , inverseRoot);
    Vector3 localIK           = TransformCoordinateVector3(ikPosition          , inverseRoot);
    Vector3 localIntermediate = TransformCoordinateVector3(intermediatePosition, inverseRoot);

    /*-------------------------------------------------------------------
    -            Save edge distance
    ---------------------------------------------------------------------*/
    Vector3 toTarget             = localTarget;
    Vector3 toIK                 = localIK;
    Vector3 rootToIntermediate   = localIntermediate;
    Vector3 intermediateToTarget = localTarget - localIntermediate;

                                                                                                     
    /*-------------------------------------------------------------------
    -            Calculate cosine theorem
    ---------------------------------------------------------------------*/
    float a = Norm(toTarget);
    float b = Norm(rootToIntermediate);
    float c = Norm(intermediateToTarget);

    float theta1 = ACos(min(1.0f, max(-1.0f, (a * a + b * b - c * c) / (2 * a * b))));
    float theta2 = ACos(min(1.0f, max(-1.0f, (b * b + c * c - a * a) / (2 * b * c))));

    /*-------------------------------------------------------------------
    -            Calculate axis 
    ---------------------------------------------------------------------*/
    Vector3 axis;
    auto vm = Normalize(toTarget);
    auto vt = Normalize(toIK);
    axis    = Cross(vt, vm);
    

    rootNode->SetRotate(Quaternion(axis, theta1));
    rootNode->SetRotate(Quaternion(axis, theta2));
    /*-------------------------------------------------------------------
    -            Calculate bone matrix
    ---------------------------------------------------------------------*/
    rootNode        ->UpdateGlobalMatrix();
    intermediateNode->UpdateGlobalMatrix();
    targetNode      ->UpdateGlobalMatrix();
    ikNode          ->UpdateGlobalMatrix();
  
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
            auto inverseChain  = Inverse(chainNode->GetGlobalMatrix());
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