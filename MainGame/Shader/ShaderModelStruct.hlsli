//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderModelStruct.hlsli
//            Content:  3DModel Vertex 
//             Author:  Toide Yutaro
//             Create:  2020_12_23
//////////////////////////////////////////////////////////////////////////////////
struct VSInputPMDModel
{
    float4 Position   : POSITION;
    float4 Normal     : NORMAL;
    float2 UV         : TEXCOORD;
    min16uint2 BoneNo : BONE_NO;
    min16uint  Weight : WEIGHT;
    
};