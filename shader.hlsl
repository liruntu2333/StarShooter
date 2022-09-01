#define TOON_SHADING

cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte‹«ŠE—p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Intensity[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
};

cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

struct VertexIn
{
    float4 Position : POSITION0;
	float4 Normal   : NORMAL0;
	float4 Alebedo  : COLOR0;
	float2 TexCoord : TEXCOORD0;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
	float4 Normal   : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Alebedo  : COLOR0;
	float4 WorldPos : POSITION0;
};

VertexOut VertexShaderPolygon(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0;

	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

    vout.Position = mul(vin.Position, wvp);
    vout.Normal   = normalize(mul(float4(vin.Normal.xyz, 0.0f), World));
    vout.TexCoord = vin.TexCoord;
    vout.WorldPos = mul(vin.Position, World);
    vout.Alebedo  = vin.Alebedo;

    return vout;
}

Texture2D		g_Texture : register( t0 );
TextureCube		g_SkyBoxTexture : register(t1);
SamplerState	g_SamplerState : register( s0 );


float4 PixelShaderPolygon(VertexOut pin) : SV_Target
{
    float4 vout = (float4) 0;

    float4 albedo = Material.noTexSampling == 0 ? 
		g_Texture.Sample(g_SamplerState, pin.TexCoord) * pin.Alebedo : pin.Alebedo;

    if (Light.Enable == 0)
    {
        vout = Material.Diffuse * albedo;
    }
    else
    {
        const float3 eyeDir = normalize(Camera.xyz - pin.WorldPos.xyz);

        vout += 0.1f * Material.Ambient; // Ambient
    	for (int i = 0; i < 5; i++)	// the sceen is lightened with 5 point lights 
        {
            float3 lightDir = normalize(Light.Position[i].xyz - pin.WorldPos.xyz);
            float3 halfVec = normalize(lightDir + eyeDir);
            float cosTheta = max(dot(lightDir, pin.Normal.xyz), 0.0f);

#ifdef TOON_SHADING
            cosTheta = cosTheta < 0.0f ? 0.1f : cosTheta < 0.3f ? 0.4f : cosTheta < 0.5f ? 0.6f : cosTheta < 0.7f ? 0.8f : 0.9f;
#endif
            float distance = length(pin.WorldPos.xyz - Light.Position[i].xyz);
            float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
            float4 diffuse = albedo * Material.Diffuse * cosTheta * Light.Intensity[i] * att;
            float4 specular = Material.Specular * Light.Intensity[i] *
				pow(max(.0f, dot(pin.Normal.xyz, halfVec)), 150.f) * att;

            vout += diffuse + specular;
        }

		// reflection of the sky box
		{
            float3 reflDir = reflect(eyeDir, pin.Normal.xyz);
            float4 reflCol = g_SkyBoxTexture.Sample(g_SamplerState, reflDir);

            vout += Material.Specular * reflCol;
        }

        vout.a = pin.Alebedo.a * Material.Diffuse.a;
    }

    return vout;
}

struct SkyBoxVSIn
{
    float4 Position : POSITION0;
    float4 Normal : NORMAL0;
    float4 Alebedo : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct SkyBoxVSOut    //output structure for skymap vertex shader
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};


SkyBoxVSOut SkyBoxVS(SkyBoxVSIn vin)
{
    SkyBoxVSOut vout = (SkyBoxVSOut) 0;

    //Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);

    vin.Position.w = 1.0f;
    vout.Position = mul(vin.Position, wvp).xyww;
    vout.TexCoord = vin.Position.xyz;

    return vout;
}

float4 SkyBoxPS(SkyBoxVSOut pin) : SV_Target
{
    return g_SkyBoxTexture.Sample(g_SamplerState, pin.TexCoord);
}