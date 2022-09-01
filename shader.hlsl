#define TOON_SHADING

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
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

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
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
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
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
	float4 Diffuse  : COLOR0;
	float2 TexCoord : TEXCOORD0;
};

struct VertexOut
{
    float4 Position : SV_POSITION;
	float4 Normal   : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Diffuse  : COLOR0;
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
    vout.Diffuse  = vin.Diffuse;

    return vout;
}

Texture2D		g_Texture : register( t0 );
TextureCube		g_SkyBoxTexture : register(t1);
SamplerState	g_SamplerState : register( s0 );


float4 PixelShaderPolygon(VertexOut pin) : SV_Target
{
    float4 color = (float4) 0;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, pin.TexCoord);

		color *= pin.Diffuse;
	}
	else
	{
		color = pin.Diffuse;
	}

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
        float3 view_dir = normalize(Camera.xyz - pin.WorldPos.xyz);

        outColor += 0.1f * Material.Ambient;	// Ambient

        for (int i = 0; i < 5; i++)
        {
            float3 lightDir = 0;
            float light = 0; // Ambient

            //if (Light.Flags[i].y == 1)
            {
//                if (Light.Flags[i].x == 1)
//                {
//                    lightDir = normalize(-Light.Direction[i].xyz);
//                    light = max(dot(lightDir, pin.Normal.xyz), 0.0f);
//                    float3 half_vec = normalize(lightDir + view_dir);
//                    float4 specular = Material.Specular *
//						pow(max(.0f, dot(normalize(pin.Normal.xyz), half_vec)), 150.f);
					
//#ifdef TOON_SHADING
//                    light = light < 0.0f ? 0.1f : light < 0.3f ? 0.4f : light < 0.5f ? 0.6f : light < 0.7f ? 0.8f : 0.9f;
//#endif
//					//light = 0.5 - 0.5 * light;
//                    tempColor += color * Material.Diffuse * light * Light.Diffuse[i];
//                    tempColor += color * Material.Specular * specular * light;

//                }
//                else if (Light.Flags[i].x == 2)
                {
                    lightDir = normalize(Light.Position[i].xyz - pin.WorldPos.xyz);
                    light = max(dot(lightDir, pin.Normal.xyz), 0.2f);
                    float3 half_vec = normalize(lightDir + view_dir);
                    float4 specular = Material.Specular *
						pow(max(.0f, dot(normalize(pin.Normal.xyz), half_vec)), 150.f);

#ifdef TOON_SHADING
                    light = light < 0.0f ? 0.1f : light < 0.3f ? 0.4f : light < 0.5f ? 0.6f : light < 0.7f ? 0.8f : 0.9f;
#endif
                    tempColor += color * Material.Diffuse * light * Light.Diffuse[i];
                    tempColor += color * Material.Specular * specular * light;

                    float distance = length(pin.WorldPos - Light.Position[i]);
                    float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
                    tempColor *= att;
                }
                //else
                //{
                //    tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
                //}

                outColor += tempColor;
            }
        }


		// reflection
		{
            float3 incident = view_dir;
            float3 reflectionVec = reflect(incident, pin.Normal.xyz);
            float4 reflCol = g_SkyBoxTexture.Sample(g_SamplerState, reflectionVec);

            outColor += Material.Specular * reflCol;
        }

		color = outColor;
		color.a = pin.Diffuse.a * Material.Diffuse.a;
	}

	if (fuchi == 1)
	{
		float angle = dot(normalize(pin.WorldPos.xyz - Camera.xyz), normalize(pin.Normal.xyz));
		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.3f)
		{
			pin.Diffuse.rb  = 1.0f;
			pin.Diffuse.g = 0.0f;			
		}
	}

    return color;
}

struct SkyBoxVSIn
{
    float4 Position : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
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