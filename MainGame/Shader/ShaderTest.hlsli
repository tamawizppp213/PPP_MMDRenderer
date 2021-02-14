Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
cbuffer RootConstants : register(b0)
{
    matrix projectionViewMatrix;
}