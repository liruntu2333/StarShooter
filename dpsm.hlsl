cbuffer WorldBuffer : register(b0)
{
	matrix World;
}

cbuffer ViewBuffer : register(b1)
{
	matrix View;
}

struct LIGHT
{
	float4 Direction[5];
	float4 Position[5];
	float4 Intensity[5];
	float4 Ambient[5];
	float4 Attenuation[5];
	int4 Flags[5];
	int Enable;
};

cbuffer LightBuffer : register(b4)
{
	LIGHT Light;
}

cbuffer FlagBuffer : register(b6)
{
	int g_LightFlag;    // 1, -1, 2, -2, 3, -3 ...
};

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
    float  ClipDepth : TEXCOORD1;
};

VertexOut ShadowVS(VertexIn vin)
{
	VertexOut vout;

	vout.PositionH = mul(mul(float4(vin.PositionL.xyz, 1.0f), World), View);
	vout.PositionH /= vout.PositionH.w;

	if (g_LightFlag < 0)
	{
		vout.PositionH.z = -vout.PositionH.z;
	}
	int i = abs(g_LightFlag) - 1;

	// because the origin is at 0 the proj-vector
	// matches the vertex-position
	const float fLength = length(vout.PositionH.xyz);
	vout.PositionH /= fLength;
    vout.ClipDepth = vout.PositionH.z;

	// calc "normal" on intersection, by adding the 
	// reflection-vector(0,0,1) and divide through 
	// his z to get the texture coords
	vout.PositionH.xy /= vout.PositionH.z + 1.0f;

    const float fFar = Light.Attenuation[i].x;
	// set z for z-buffering and neutralize w
	const float g_Near = 0.1f;
	vout.PositionH.z = (fLength - g_Near) / (fFar - g_Near);
	vout.PositionH.w = 1.0f;

	return vout;
}

void ShadowPS(VertexOut pin)
{
    clip(pin.ClipDepth);
}