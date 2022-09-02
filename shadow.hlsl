#ifndef SHADOW_HLSL
#define SHADOW_HLSL

cbuffer CB : register(b0)
{
    matrix g_WorldViewProj;
}

struct VertexIn
{
    float4 PositionL : POSITION0;
    float4 NormalL : NORMAL0;
    float4 Albedo : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

Texture2D g_DiffuseMap : register(t0);
SamplerState g_SamplerState : register(s0);

VertexOut ShadowVS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0;

    vout.PositionH = mul(vin.PositionL, g_WorldViewProj);
    vout.TexCoord = vin.TexCoord;

    return vout;
}

void ShadowPS(VertexOut pin)
{
    float4 diffuse = g_DiffuseMap.Sample(g_SamplerState, pin.TexCoord);
    clip(diffuse.a - 0.1f);
}

#endif