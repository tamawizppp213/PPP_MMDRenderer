//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderModelStruct.hlsli
//            Content:  3DModel Vertex 
//             Author:  Toide Yutaro
//             Create:  2021_06_19
//////////////////////////////////////////////////////////////////////////////////
struct VSInputPMDModel
{
    float4 Position     : POSITION;
    float4 Normal       : NORMAL;
    float2 UV           : TEXCOORD;
    min16uint2 BoneNo   : BONE_NO;
    min16uint  Weight   : WEIGHT;
    min16uint  EdgeFlag : EDGE_FLAG;
};


struct VSOutputPMDModel
{
    float4 Position   : SV_POSITION;
    float4 Normal     : NORMAL0;
    float4 ViewNormal : NORMAL1;
    float2 UV         : TEXCOORD;
    
};



struct PMDMaterial
{
    float4    Diffuse;
    float     SpecularPower;
    float3    Specular;
    float3    Ambient;
    min16uint ToonID;
    min16uint EdgeFlag;
    min16uint Padding;
    uint      PolygonNum;
};

struct VSInputPMXModel
{
    float4    Position   : POSITION;
    float4    Normal     : NORMAL;
    float2    UV         : TEXCOORD;
    float4    Tangent    : TANGENT;
    float4    BiNormal   : BINORMAL;
    int4      BoneNo     : BONE_NO;
    float4    Weight     : WEIGHT;
    //float3    Def_C      : DEF_C;
    //float3    Def_R0     : DEF_R_ZERO;
    //float3    Def_R1     : DEF_R_ONE;
    uint   WeightType : WEIGHT_TYPE;
    
};


struct VSOutputPMXModel
{
    float4 Position   : SV_POSITION;
    float4 Normal     : NORMAL0;
    float4 ViewNormal : NORMAL1;
    float4 Tangent    : TANGENT;
    float4 BiNormal   : BINORMAL;
    float2 UV         : TEXCOORD;
    float4 WorldPosition : POSITION;
};

struct PMXMaterial
{
    float4 Diffuse;
    float  SpecularPower;
    float3 Specular;
    float3 Ambient;
    uint   PolygonNum;
};

struct VSOutputPrimitiveModel
{
    float4 Position      : SV_POSITION;
    float4 Normal        : NORMAL0;
    float4 ViewNormal    : NORMAL1;
    float2 UV            : TEXCOORD;
    float4 WorldPosition : POSITION;
};


ConstantBuffer<PMDMaterial> pmdMaterial : register(b2);
ConstantBuffer<PMXMaterial> pmxMaterial : register(b2);


cbuffer BoneMatrices : register(b4)
{
    matrix bones[256];
}

cbuffer PMXBoneMatrices : register(b4)
{
    matrix pmxBones[512];
}


Texture2D<float4> DiffuseMap          : register(t0);
Texture2D<float4> SphereMapMultiply   : register(t1);
Texture2D<float4> SphereMapAddition   : register(t2);
Texture2D<float4> ToonTextureDiffuse  : register(t3);
Texture2D<float4> AmbientMap          : register(t4);

StructuredBuffer<uint> PointLightList : register(t10);
StructuredBuffer<uint> SpotLightList : register(t11);