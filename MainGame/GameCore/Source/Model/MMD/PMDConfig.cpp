//////////////////////////////////////////////////////////////////////////////////
///             @file   PMDConfig.cpp
///             @brief  PMDFile 
///             @author Toide Yutaro
///             @date   2021_02_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Model/MMD/PMDConfig.hpp"
#include "GameCore/Include/Model/MMD/PMDFile.hpp"
#include <array>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
using namespace gm;
static Matrix4 LookAtMatrix(const Vector3& lookAt, Vector3& up, Vector3& right);
static Matrix4 LookAtMatrix(const Vector3& origin, const Vector3& lookAt, Vector3& up, Vector3& right);

#define EPSILON (0.0005f)
//////////////////////////////////////////////////////////////////////////////////
//                             Implement
//////////////////////////////////////////////////////////////////////////////////
/****************************************************************************
*							     PMDVertex
*****************************************************************************/
const D3D12_INPUT_ELEMENT_DESC PMDVertex::InputElements[] =
{
    { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "BONE_NO",    0, DXGI_FORMAT_R16G16_UINT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "WEIGHT"   ,  0, DXGI_FORMAT_R8_UINT,            0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "EDGE_FLAG",  0, DXGI_FORMAT_R8_UINT,            0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

static_assert(sizeof(PMDVertex) == 38, "Vertex struct/layout mismatch");

const D3D12_INPUT_LAYOUT_DESC PMDVertex::InputLayout =
{
    PMDVertex::InputElements,
    PMDVertex::InputElementCount
};

void PMDBoneNode::RecursiveBoneMatrixMultiply(std::vector<gm::Matrix4>& boneMatrix, PMDBoneNode* boneNode, const gm::Matrix4& matrix)
{
    boneMatrix[(int)boneNode->GetBoneIndex()] *= matrix;

    for (auto& childNode : boneNode->_children)
    {
        RecursiveBoneMatrixMultiply(boneMatrix, childNode, boneMatrix[boneNode->GetBoneIndex()]);
    }

}

#pragma region IK
/****************************************************************************
*							SolveIK
*************************************************************************//**
*  @fn        void PMDBoneIK::SolveIK(int frameNo, const std::vector<PMDBoneIK>& iks)
*  @brief     Move IK to next frame position (Change the process according to the type of bone.)
*  @param[in] int frameNo
*  @param[in] std::vector<PMDBoneIK> iks
*  @param[in] std::vector<PMDBoneNode*>& boneNodeAddressArray (note! The bone node address array need to be lined up in the order of ik)
*  @return 　　void
*****************************************************************************/
void PMDBoneIK::SolveIK(int frameNo, PMDData* pmdFile, std::vector<gm::Matrix4>& boneMatrices, VMDFile* vmdFile)
{
    auto iterator = std::find_if(vmdFile->GetEnableIKData().rbegin(), vmdFile->GetEnableIKData().rend(),
        [frameNo](const VMDIK& ikEnable)
    {
        return ikEnable.FrameNo <= static_cast<UINT>(frameNo);
    });

    for (int i = 0; i < pmdFile->GetBoneIKCount(); ++i)
    {
        auto& ik = pmdFile->GetBoneIK()[i];
        
        if (iterator != vmdFile->GetEnableIKData().rend())
        {
            auto ikEnableIterator = iterator->IKEnableTable.find(pmdFile->GetBoneNames()[ik.GetIKBoneID()]);
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
            SolveLookAt(ik, pmdFile->GetBoneAddressList(), boneMatrices);
            break;
        case 2:
            SolveCosIK(ik, pmdFile->GetBoneAddressList(), boneMatrices, pmdFile->GetKneeIndexList());
            break;
        default:
            SolveCCDIK(ik, pmdFile->GetBoneAddressList(), boneMatrices, pmdFile->GetKneeIndexList());
            break;
        }
    }
}

/****************************************************************************
*							SolveLookAt
*************************************************************************//**
*  @fn        void PMDBoneIK::SolveLookAt(const PMDBoneIK& ik)
*  @brief     Move IK to next frame position (Change the process according to the type of bone.)
*  @param[in] PMDBoneIK ik
*  @param[in] std::vector<PMDBoneNode*>& boneNodeAddressArray
*  @param[out] std::vector<gm::Matrix4>& boneMatrices
*  @return 　　void
*****************************************************************************/
void PMDBoneIK::SolveLookAt(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices)
{
    auto rootBoneNode   = boneNodeAddressArray[ik.GetChains()[0]];
    auto targetBoneNode = boneNodeAddressArray[ik.GetTargetBoneID()];
    auto ikBoneNode     = boneNodeAddressArray[ik.GetIKBoneID()];

    Vector3 modelRoot   = Vector3(rootBoneNode->GetBasePosition());
    Vector3 modelTarget = Vector3(targetBoneNode->GetBasePosition());
    Vector3 modelIK     = Vector3(ikBoneNode->GetBasePosition());

    Vector3 worldRoot   = TransformCoordinateVector3(modelRoot  , boneMatrices[ik.GetChains()[0]]);
    Vector3 worldTarget = TransformCoordinateVector3(modelTarget, boneMatrices[ik.GetTargetBoneID()]);
    Vector3 worldIK     = TransformCoordinateVector3(modelIK    , boneMatrices[ik.GetIKBoneID()]);

    Vector3 toTarget = Normalize(worldTarget - worldRoot);
    Vector3 toIK     = Normalize(worldIK - worldRoot);

    Vector3 axis  = Normalize(Cross(toIK, toTarget));
    float   theta = ACos(Dot(toTarget, toIK));

    Matrix4 matrix =
        Translation(-worldRoot) *
        RotationQuaternion(Quaternion(axis, theta)) * 
        Translation(worldRoot);


    boneMatrices[ik.GetChains()[0]] *= matrix;
}

/****************************************************************************
*							SolveCosIk (root - medium - end (in total three nodes))
*************************************************************************//**
*  @fn        void PMDBoneIK::SolveCosIK(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices, const std::vector<UINT32>& kneeIndices)
*  @brief     Move IK to next frame position (Change the process according to the type of bone.)
*  @param[in] PMDBoneIK ik
*  @param[in]  const PMDBoneNodeAddressList& boneNodeAddressArray
*  @param[out] std::vector<gm::Matrix4>& boneMatrices
*  @param[in]  const std::vector<UINT32>& kneeIndices
*  @return 　　void
*****************************************************************************/
void PMDBoneIK::SolveCosIK(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices, const std::vector<UINT32>& kneeIndices)
{
   
    /*-------------------------------------------------------------------
    -             Get bone nodes
    ---------------------------------------------------------------------*/
    auto& targetNode       = boneNodeAddressArray[ik.GetTargetBoneID()];
    auto& ikNode           = boneNodeAddressArray[ik.GetIKBoneID()];
    auto& rootNode         = boneNodeAddressArray[ik.GetChainsVector()[1]];
    auto& intermediateNode = boneNodeAddressArray[ik.GetChainsVector()[0]];

    /*-------------------------------------------------------------------
    -            Get Position in model space
    ---------------------------------------------------------------------*/
    Vector3 modelTarget       = Vector3(targetNode      ->GetBasePosition());
    Vector3 modelIK           = Vector3(ikNode          ->GetBasePosition());
    Vector3 modelRoot         = Vector3(rootNode        ->GetBasePosition());
    Vector3 modelIntermediate = Vector3(intermediateNode->GetBasePosition());

    /*-------------------------------------------------------------------
    -            Get Position in world space
    ---------------------------------------------------------------------*/
    Vector3 worldTarget       = TransformCoordinateVector3(modelTarget      , boneMatrices[ik.GetTargetBoneID()]);
    Vector3 worldIK           = TransformCoordinateVector3(modelIK          , boneMatrices[ik.GetIKBoneID()]);
    Vector3 worldRoot         = TransformCoordinateVector3(modelRoot        , boneMatrices[ik.GetChainsVector()[1]]);
    Vector3 worldIntermediate = TransformCoordinateVector3(modelIntermediate, boneMatrices[ik.GetChainsVector()[0]]);

    /*-------------------------------------------------------------------
    -            Save edge distance
    ---------------------------------------------------------------------*/
    Vector3 toTarget             = worldTarget - worldRoot;
    Vector3 toIK                 = worldIK     - worldRoot;
    Vector3 rootToIntermediate   = worldIntermediate - worldRoot;
    Vector3 intermediateToTarget = worldTarget - worldIntermediate;

                                                                                                       
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
    if (std::find(kneeIndices.begin(), kneeIndices.end(), ik.GetChainsVector()[0]) == kneeIndices.end())
    {
        auto vm = Normalize(toTarget);
        auto vt = Normalize(toIK);
        axis    = Cross(vt, vm);
    }
    else
    {
        auto right = Vector3(1.0f, 0.0f, 0.0f);
        auto vt    = Normalize(toIK);
        axis       = Normalize(Cross(vt, Cross(right, vt)));
    }

    /*-------------------------------------------------------------------
    -            Calculate bone matrix
    ---------------------------------------------------------------------*/
    auto matrix1 = 
        Translation(-worldRoot) * 
        RotationQuaternion(Quaternion(axis, theta1)) * 
        Translation(worldRoot);
     
    auto matrix2 = 
        Translation(-worldIntermediate) * 
        RotationQuaternion(Quaternion(axis, theta2 - DirectX::XM_PI)) * 
        Translation(worldIntermediate);


    boneMatrices[ik.GetChainsVector()[1]]   *= matrix1;
    boneMatrices[ik.GetChainsVector()[0]]   = matrix2 * boneMatrices[ik.GetChainsVector()[1]];
    //boneMatrices[ik.GetTargetBoneID()]      = boneMatrices[ik.GetChainsVector()[0]];
}

/****************************************************************************
*							SolveCCDIK (root - medium .... - end (over four nodes))
*************************************************************************//**
*  @fn        void PMDBoneIK::SolveCosIK(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices)
*  @brief     Move IK to next frame position (Change the process according to the type of bone.)
*  @param[in] PMDBoneIK ik
*  @param[in]  const PMDBoneNodeAddressList& boneNodeAddressArray
*  @param[out] std::vector<gm::Matrix4>& boneMatrices
*  @param[in]  const std::vector<UINT32>& kneeIndices
*  @return 　　void
*****************************************************************************/
#pragma region Test
void PMDBoneIK::SolveCCDIK(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices, const std::vector<UINT32>& kneeIndices)
{
    /*-------------------------------------------------------------------
    -                         Get node
    ---------------------------------------------------------------------*/
    auto& targetNode = boneNodeAddressArray[ik.GetTargetBoneID()];
    auto& ikNode     = boneNodeAddressArray[ik.GetIKBoneID()];
    auto& rootNode   = boneNodeAddressArray[ik.GetChainsVector()[ik.GetChainLength() - 1]];

    /*-------------------------------------------------------------------
    -                     Get positions in model space
    ---------------------------------------------------------------------*/
    Vector3 modelIK      = Vector3(ikNode->GetBasePosition());
    Vector3 modelTarget  = Vector3(boneNodeAddressArray[ik.GetTargetBoneID()]->GetBasePosition());
    std::vector<Vector3> modelChains;
    for (auto& index : ik.GetChainsVector())
    {
        modelChains.emplace_back(boneNodeAddressArray[index]->GetBasePosition());
    }
    
    /*-------------------------------------------------------------------
    -                     Get parent
    ---------------------------------------------------------------------*/
    auto parentMatrix = boneMatrices[boneNodeAddressArray[ik.GetIKBoneID()]->GetIKParentBone()];
    Vector4 determinant;
    Matrix4 inverseParentMatrix = Inverse(determinant, parentMatrix);
    Matrix4 invBoneMatrix       = boneMatrices[ik.GetIKBoneID()] * inverseParentMatrix;
    Vector3 targetNextPosition  = TransformCoordinateVector3(modelIK, invBoneMatrix);

   
    std::vector<Matrix4> matrices(modelChains.size());
    std::fill(matrices.begin(), matrices.end(), MatrixIdentity());

    /*-------------------------------------------------------------------
    -            Calculate CCD IK
    ---------------------------------------------------------------------*/
    float angleLimit = ik.GetAngleLimit() * DirectX::XM_PI;
    for (int cycle = 0; cycle < ik.GetIterationCount(); ++cycle)
    {
        /*-------------------------------------------------------------------
        -            Exit the loop when the target position and the end position almost match.
        ---------------------------------------------------------------------*/
        if (Norm(modelTarget - targetNextPosition) <= EPSILON) { break; }

        for (int boneIndex = 0; boneIndex < modelChains.size(); ++boneIndex)
        {
            const auto& position = modelChains[boneIndex];

            // Create vectors ① from current node to end node ② from current node to target node
            auto toEnd    = Normalize(modelTarget - position);
            auto toIK     = Normalize(targetNextPosition - position);

            if (Norm(toEnd - toIK) <= EPSILON) { continue; }

            /*-------------------------------------------------------------------
            -       Calcurate Cross, Angle
            ---------------------------------------------------------------------*/
            auto  cross      = Normalize(Cross(toIK, toEnd));
            float angle      = ACos(Dot(toEnd, toIK));
            angle            = min(max(angle, -angleLimit), angleLimit); // correction

            /*-------------------------------------------------------------------
           　-       Rotate around position
           　---------------------------------------------------------------------*/
            if (std::find(kneeIndices.begin(), kneeIndices.end(), ik.GetChainsVector()[0]) == kneeIndices.end())
            {

            }
            else
            {
                auto right = Vector3(1.0f, 0.0f, 0.0f);
                auto vt = Normalize(toIK);
                cross = Normalize(Cross(vt, Cross(right, vt)));
            }

            Matrix4 matrix = 
                Translation(-position) *
                RotationQuaternion(Quaternion(cross, angle).Normalize()) * 
                Translation(position);
            matrices[boneIndex] *= matrix;

            /*-------------------------------------------------------------------
           　-       Rotate all points
           　---------------------------------------------------------------------*/
            for (int i = boneIndex - 1; i >= 0; --i)
            {
                modelChains[i] = TransformCoordinateVector3(modelChains[i], matrix);
            }
            modelTarget = TransformCoordinateVector3(modelTarget, matrix);
            if (Norm(modelTarget - targetNextPosition) <= EPSILON) { break; }
        }
    }

    for (int i = 0; i < ik.GetChainLength(); ++i)
    {
        boneMatrices[ik.GetChains()[i]] = matrices[i];
    }
    PMDBoneNode* node = boneNodeAddressArray[ik.GetChainsVector().back()];
    PMDBoneNode::RecursiveBoneMatrixMultiply(boneMatrices, node, parentMatrix);
}
#pragma endregion test
//void PMDBoneIK::SolveCCDIK(const PMDBoneIK& ik, const PMDBoneNodeAddressList& boneNodeAddressArray, std::vector<gm::Matrix4>& boneMatrices, const std::vector<UINT32>& kneeIndices)
//{
//    /*-------------------------------------------------------------------
//    -            Get target bone position and next position
//    ---------------------------------------------------------------------*/
//    auto targetBoneNode    = boneNodeAddressArray[ik.GetIKBoneID()];
//    Vector3 targetPosition = Vector3(targetBoneNode->GetBasePosition());
//
//    auto parentMatrix = boneMatrices[boneNodeAddressArray[ik.GetIKBoneID()]->GetIKParentBone()];
//    
//    // Invalidate rotation matrix of ParentIKBone.
//    Vector4 determinant;
//    Matrix4 inverseParentMatrix = Inverse(determinant, parentMatrix);
//    Matrix4 invBoneMatrix       = boneMatrices[ik.GetIKBoneID()] * inverseParentMatrix;
//    Vector3 targetNextPosition  = TransformVector3(targetPosition, invBoneMatrix);
//
//    /*-------------------------------------------------------------------
//    -            Get IK Bone Positions
//    ---------------------------------------------------------------------*/
//    std::vector<Vector3> bonePositions;
//    Vector3 endPosition = Vector3(boneNodeAddressArray[ik.GetTargetBoneID()]->GetBasePosition());
//    for (auto& index : ik.GetChainsVector())
//    {
//        bonePositions.push_back(boneNodeAddressArray[index]->GetBasePosition());
//    }
//
//    std::vector<Matrix4> matrices(ik.GetChainLength());
//    std::fill(matrices.begin(), matrices.end(), MatrixIdentity());
//
//    /*-------------------------------------------------------------------
//    -            Calculate CCD IK
//    ---------------------------------------------------------------------*/
//    float angleLimit = ik.GetAngleLimit() * DirectX::XM_PI;
//    for (int cycle = 0; cycle < ik.GetIterationCount(); ++cycle)
//    {
//        /*-------------------------------------------------------------------
//        -            Exit the loop when the target position and the end position almost match.
//        ---------------------------------------------------------------------*/
//        if (Norm(endPosition - targetNextPosition) <= EPSILON) { break; }
//
//        for (int boneIndex = 0; boneIndex < bonePositions.size(); ++boneIndex)
//        {
//            const auto& position = bonePositions[boneIndex];
//
//            // Create vectors ① from current node to end node ② from current node to target node
//            auto toEnd    = (endPosition - position);
//            auto toTarget = (targetNextPosition - position);
//
//            if (Norm(toEnd - toTarget) <= EPSILON) { continue; }
//
//            // normalize
//            auto toEndVector    = Normalize(toEnd);
//            auto toTargetVector = Normalize(toTarget);
//
//            /*-------------------------------------------------------------------
//            -       Calcurate Angle
//            ---------------------------------------------------------------------*/
//            float dot    = Dot(toEndVector, toTargetVector);
//            dot          = min(1.0f, max(-1.0f, dot));
//            float radian = acosf(dot);
//            if (radian < EPSILON) { continue; }
//            radian       = min(angleLimit, max(-angleLimit, radian)); // angle limitation
//
//            /*-------------------------------------------------------------------
//            -       Calcurate Axis
//            ---------------------------------------------------------------------*/
//            auto cross = Normalize(Cross(toTargetVector, toEndVector));
//            if (std::find(kneeIndices.begin(), kneeIndices.end(), ik.GetChainsVector()[boneIndex]) != kneeIndices.end())
//            {
//                float   temp;
//                DirectX::XMVECTOR tempVector;
//                AxisAngle(Quaternion(cross, radian), &temp, &tempVector);
//                cross = Vector3(temp, 0, 0);
//            }
//
//            Matrix4 rotation = RotationQuaternion(Quaternion(cross, radian));
//
//            /*-------------------------------------------------------------------
//           　-       Rotate around position
//           　---------------------------------------------------------------------*/
//            Matrix4 matrix = 
//                Translation(-position) *
//                rotation *
//                Translation(position);
//            matrices[boneIndex] *= matrix;
//
//            /*-------------------------------------------------------------------
//            -       Update Bone Position
//            ---------------------------------------------------------------------*/
//            for (int i = boneIndex - 1; i >= 0; --i)
//            {
//                bonePositions[i] = TransformVector3(bonePositions[i], matrix);
//            }
//            endPosition = TransformVector3(endPosition, matrix);
//            if (Norm(endPosition - targetNextPosition) <= EPSILON) { break; }
//        }
//    }
//
//    for (int i = 0; i < ik.GetChainLength(); ++i)
//    {
//        boneMatrices[ik.GetChains()[i]] = matrices[i];
//    }
//    PMDBoneNode* node = boneNodeAddressArray[ik.GetChainsVector().back()];
//    PMDBoneNode::RecursiveBoneMatrixMultiply(boneMatrices, node, parentMatrix);
//}
#pragma endregion IK

#pragma region TEMP
static Matrix4 LookAtMatrix(const Vector3& lookAt, Vector3& up, Vector3& right)
{
    Vector3 vz = Normalize(lookAt);         // direction yo want to go(z axis)
    Vector3 vy = Normalize(up);  // temp y vector
    Vector3 vx = Normalize(Cross(vy, vz));
    vy = Normalize(Cross(vz, vx));

    if (std::abs(Dot(vy, vz)) == 1.0f)
    {
        vx = Normalize(right);
        vy = Normalize(Cross(vz, vx));
        vx = Normalize(Cross(vy, vz));
    }

    Matrix4 result = MatrixIdentity();
    result.Set3x3(Matrix3(vx, vy, vz));

    return result;
}

static Matrix4 LookAtMatrix(const Vector3& origin, const Vector3& lookAt, Vector3& up, Vector3& right)
{
    //LookAtLH(origin, lookAt, up);
    Matrix4 result = Transpose(LookAtMatrix(origin, up, right)) * LookAtMatrix(lookAt, up, right);
    return result;
}
#pragma endregion TEMP
