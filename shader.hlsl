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
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

struct VertexIn
{
    float4 PositionL : POSITION0;
	float4 NormalL   : NORMAL0;
	float4 Albedo  : COLOR0;
	float2 TexCoord : TEXCOORD0;
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
	float4 Normal   : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Albedo  : COLOR0;
	float4 PositionW : POSITION0;
};

VertexOut VertexShaderPolygon(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0;

    matrix wvp = mul(World, View);
	wvp = mul(wvp, Projection);

    vout.PositionH = mul(vin.PositionL, wvp);
    vout.Normal   = normalize(mul(float4(vin.NormalL.xyz, 0.0f), World));
    vout.TexCoord = vin.TexCoord;
    vout.PositionW = mul(vin.PositionL, World);
    vout.Albedo  = vin.Albedo;

    return vout;
}

Texture2D		g_Texture : register( t0 );
TextureCube		g_SkyBoxTexture : register(t1);
SamplerState	g_SamplerState : register( s0 );

void ComputePointLight(uint iLight, const float3 posW, const float3 normW, const float3 eyeDir, const float4 diffuseAlbedo,
						out float4 diffuse, out float4 specular)
{
    const float3 lightDir = normalize(Light.Position[iLight].xyz - posW);
    const float lightDis = length(posW - Light.Position[iLight].xyz);
    if (lightDis > Light.Attenuation[iLight].x) return;

    const float3 halfVec = normalize(lightDir + eyeDir);
    float cosTheta = max(dot(lightDir, normW), 0.0f);
    
#ifdef TOON_SHADING
    cosTheta = cosTheta < 0.1f ? 0.2f : cosTheta < 0.3f ? 0.4f : cosTheta < 0.5f ? 0.6f : cosTheta < 0.7f ? 0.8f : 0.9f;
#endif
    const float att = saturate((Light.Attenuation[iLight].x - lightDis) / Light.Attenuation[iLight].x);

    diffuse = diffuseAlbedo * Material.Diffuse * cosTheta * Light.Intensity[iLight] * att;
    specular = Material.Specular * pow(max(.0f, dot(normW, halfVec)), 150.f) * Light.Intensity[iLight] * att;
}

float4 PixelShaderPolygon(VertexOut pin) : SV_Target
{
    float4 vout = 0;

    const float4 diffuseAlbedo = Material.noTexSampling == 0 ? 
	                          g_Texture.Sample(g_SamplerState, pin.TexCoord) * pin.Albedo : pin.Albedo;
    [branch]
    if (Light.Enable == 0)
    {
        vout = Material.Diffuse * diffuseAlbedo;
    }
    else
    {
        const float3 eyeDir = normalize(Camera.xyz - pin.PositionW.xyz);

        vout += 0.1f * Material.Ambient; // Ambient

		[unroll]   // avoid branching
        for (uint i = 0; i < 5; i++)	// the scene is lightened with 5 point lights 
        {
            float4 diffuse = 0;
            float4 specular = 0;

            ComputePointLight(i, pin.PositionW.xyz, pin.Normal.xyz, eyeDir, diffuseAlbedo,
								diffuse, specular);

            vout += diffuse + specular;
        }

		// reflection of the sky box
		{
			const float3 reflDir = reflect(eyeDir, pin.Normal.xyz);
            const float4 reflCol = g_SkyBoxTexture.Sample(g_SamplerState, reflDir);

            vout += Material.Specular * reflCol;
        }

        vout.a = pin.Albedo.a * Material.Diffuse.a;
    }

    return vout;
}

struct SkyBoxVsIn
{
    float4 Position : POSITION0;
    float4 Normal : NORMAL0;
    float4 Albedo : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct SkyBoxVsOut    //output structure for skymap vertex shader
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
};


SkyBoxVsOut SkyBoxVS(SkyBoxVsIn vin)
{
    SkyBoxVsOut vout = (SkyBoxVsOut) 0;

    matrix wvp = mul(World, View);
    wvp = mul(wvp, Projection);

    vin.Position.w = 1.0f;
    vout.Position = mul(vin.Position, wvp).xyww;
    vout.TexCoord = vin.Position.xyz;

    return vout;
}

float4 SkyBoxPS(SkyBoxVsOut pin) : SV_Target
{
    return g_SkyBoxTexture.Sample(g_SamplerState, pin.TexCoord);
}