#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "particle.h"
#include "player.h"
#include "MathHelper.h"

#define TEXTURE_MAX			(1)			 

#define	PARTICLE_SIZE_X		(10.0f)		 
#define	PARTICLE_SIZE_Y		(10.0f)		 
#define	VALUE_MOVE_PARTICLE	(5.0f)		 

#define	MAX_PARTICLE		(512)		 
#define PARTICLE_INTENSITY	(0.6f)

#define	DISP_SHADOW						 
typedef struct
{
	XMFLOAT3		pos;			 
	XMFLOAT3		rot;			 
	XMFLOAT3		scale;			 
	XMFLOAT3		move;			 
	MATERIAL		material;		 
	float			fSizeX;			 
	float			fSizeY;			 
	int				nLife;			 
	BOOL			bUse;			 
} PARTICLE;

HRESULT MakeVertexParticle(void);

static ID3D11Buffer* g_VertexBuffer = nullptr;		 

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 
static int							g_TexNo;					 

static PARTICLE					g_aParticle[MAX_PARTICLE];		 
static XMFLOAT3					g_posBase;						 
static float					g_fWidthBase = 5.0f;			 
static float					g_fHeightBase = 10.0f;			 
static float					g_roty = 0.0f;					 
static float					g_spd = 0.0f;					 

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/doge.jpg",
};

static BOOL						g_Load = FALSE;

HRESULT InitParticle(void)
{
	MakeVertexParticle();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			nullptr,
			nullptr,
			&g_Texture[i],
		nullptr);
	}

	g_TexNo = 0;

	for (auto& nCntParticle : g_aParticle)
	{
		nCntParticle.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		nCntParticle.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		nCntParticle.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		nCntParticle.move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&nCntParticle.material, sizeof(nCntParticle.material));
		nCntParticle.material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		nCntParticle.fSizeX = PARTICLE_SIZE_X;
		nCntParticle.fSizeY = PARTICLE_SIZE_Y;
		nCntParticle.nLife = 0;
		nCntParticle.bUse = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	for (auto& nCntTex : g_Texture)
	{
		if (nCntTex != nullptr)
		{
			nCntTex->Release();
			nCntTex = nullptr;
		}
	}

	if (g_VertexBuffer != nullptr)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}

	g_Load = FALSE;
}

void UpdateParticle(void)
{
	const bool refresh = IsPlayerOutOfBoarder();

	if (refresh)
	{
		for (auto& particle : g_aParticle)
		{
			particle.bUse = FALSE;
		}
		return;
	}

	for (auto& nCntParticle : g_aParticle)
	{
		if (nCntParticle.bUse)
		{
			nCntParticle.nLife--;
			if (nCntParticle.nLife <= 0)
			{
				nCntParticle.bUse = FALSE;
			}
			else
			{
				auto& particle = nCntParticle;
				const float t = 1.0f - static_cast<float>(particle.nLife) / 120;
				float r = 0.0f;
				float g = 0.0f;
				float b = 0.0f;
				if (t < 0.5f)
				{
					const float r2 = PARTICLE_INTENSITY * (1.0f - 2.0f * t);
					r = sqrtf(r2);
					g = sqrtf(PARTICLE_INTENSITY - r2);
				}
				else
				{
					const float g2 = PARTICLE_INTENSITY * (2.0f - 2.0f * t);
					g = sqrtf(g2);
					b = sqrtf(PARTICLE_INTENSITY - g2);
				}
				auto& diffuse = particle.material.Diffuse;
				diffuse.x = r;
				diffuse.y = g;
				diffuse.z = b;
				diffuse.w = 1.0f - t;
			}
		}
	}
}

void DrawParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	const CAMERA* cam = GetCamera();

	SetLightEnable(FALSE);

	SetBlendState(BLEND_MODE_ADD);

	SetDepthEnable(FALSE);

	SetFogEnable(FALSE);

	const UINT stride = sizeof(VERTEX_3D);
	const UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (auto& nCntParticle : g_aParticle)
	{
		if (nCntParticle.bUse)
		{
			mtxWorld = XMMatrixIdentity();

			mtxView = XMLoadFloat4x4(&cam->mtxView);

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			mtxScl = XMMatrixScaling(nCntParticle.scale.x, nCntParticle.scale.y, nCntParticle.scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			mtxTranslate = XMMatrixTranslation(nCntParticle.pos.x, nCntParticle.pos.y, nCntParticle.pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			SetWorldMatrix(&mtxWorld);

			SetMaterial(nCntParticle.material);

			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetLightEnable(TRUE);

	SetBlendState(BLEND_MODE_ALPHABLEND);

	SetDepthEnable(TRUE);

	SetFogEnable(FALSE);
}

HRESULT MakeVertexParticle(void)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		const auto vertex = static_cast<VERTEX_3D*>(msr.pData);

		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

void SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	g_aParticle[nIdxParticle].material.Diffuse = col;
}

int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_aParticle[nCntParticle].bUse)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot = { 0.0f, 0.0f, 0.0f };
			g_aParticle[nCntParticle].scale = { 1.0f, 1.0f, 1.0f };
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].material.Diffuse = col;
			g_aParticle[nCntParticle].fSizeX = fSizeX;
			g_aParticle[nCntParticle].fSizeY = fSizeY;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].bUse = TRUE;

			nIdxParticle = nCntParticle;

			break;
		}
	}

	return nIdxParticle;
}