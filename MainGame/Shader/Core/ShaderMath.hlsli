//////////////////////////////////////////////////////////////////////////////////
//              Title:  ShaderStaticMath.hlsli
//            Content:  Static Sa,pler
//             Author:  Toide Yutaro
//             Create:  2020_06_23
//////////////////////////////////////////////////////////////////////////////////
#define QUATERNION_IDENTITY float4(0,0,0,1)
#define ZERO_TOLERANCE (1e-6f)
float Dot_Quaternion(float4 left, float4 right)
{
    return (left.x * right.x) + (left.y * right.y) + (left.z * right.z) + (left.w * right.w);
}

float4 Slerp_Quaternion(float4 start, float4 end, float t)
{
    float4 result = QUATERNION_IDENTITY;
    
    float opposite;
    float inverse;
    float dt = Dot_Quaternion(start, end);
    
    if (abs(dt) > 1.0f - ZERO_TOLERANCE)
    {
        inverse  = 1.0f - t;
        opposite = t * sign(dt);
    }
    else
    {
        float acs = acos(abs(dt));
        float inverseSine = 1.0f / sign(dt);
        inverse = sin((1.0f - t) * acs) * inverseSine;
        opposite = sin(t * acs) * inverseSine * sign(dt);
    }

    result = inverse * start + opposite * end;
    return result;
}

float4x4 QuaternionToMatrix(float4 quat)
{
    float4x4 m = float4x4(float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0));
    
    float xx = quat.x * quat.x;
    float yy = quat.y * quat.y;
    float zz = quat.z * quat.z;
    float xy = quat.x * quat.y;
    float zw = quat.z * quat.w;
    float zx = quat.z * quat.x;
    float yw = quat.y * quat.w;
    float yz = quat.y * quat.z;
    float xw = quat.x * quat.w;
    
    m[0][0] = 1.0f - (2.0f * (yy + zz));
    m[0][1] = 2.0f * (xy + zw);
    m[0][2] = 2.0f * (zx - yw);
    m[0][3] = 0.0f;
    m[1][0] = 2.0f * (xy - zw);
    m[1][1] = 1.0f - (2.0f * (zz + xx));
    m[1][2] = 2.0f * (yz + xw);
    m[1][3] = 0.0f;
    m[2][0] = 2.0f * (zx + yw);
    m[2][1] = 2.0f * (yz - xw);
    m[2][2] = 1.0f - (2.0f * (yy + xx));
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
    
    return m;
}
