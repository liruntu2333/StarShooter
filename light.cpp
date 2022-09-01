#include "main.h"
#include "renderer.h"

static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

void InitLight(void)
{
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Light[i].Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
		g_Light[i].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		g_Light[i].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		g_Light[i].Attenuation = 100.0f;	 
		g_Light[i].Type = LIGHT_TYPE_NONE;	 
		g_Light[i].Enable = FALSE;			   
		SetLight(i, &g_Light[i]);
	}

	g_Fog.FogStart = 100.0f;									 
	g_Fog.FogEnd = 1250.0f;									 
	g_Fog.FogColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);		 
	SetFog(&g_Fog);
	SetFogEnable(FALSE);		  
}

void UpdateLight(void)
{
}

void SetLightData(int index, LIGHT* light)
{
	SetLight(index, light);
}

LIGHT* GetLightData(int index)
{
	assert(index < LIGHT_MAX && "Light index out of range");
	return(&g_Light[index]);
}

void SetFogData(FOG* fog)
{
	SetFog(fog);
}