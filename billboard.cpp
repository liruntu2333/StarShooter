#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "billboard.h"


#define TEXTURE_MAX				(4)
#define	MAX_Billboard			(4)

typedef struct
{
	XMFLOAT3	pos;			 
	XMFLOAT3	scl;			 
	MATERIAL	material;		 
	float		fWidth;			 
	float		fHeight;		 
	int			nIdxShadow;

} Billboard;

HRESULT MakeVertexBillboard(void);

static ID3D11Buffer* g_VertexBuffer = NULL;	 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	 

static Billboard			g_aBillboard[MAX_Billboard];	 
static int					g_TexNo;			 
static BOOL					g_bAlpaTest;		 
static BOOL					g_Load = FALSE;


static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/GAME/DIK_UP_KEYON.png",
	"data/TEXTURE/GAME/DIK_DOWN_KEYON.png",
	"data/TEXTURE/GAME/DIK_LEFT_KEYON.png",
	"data/TEXTURE/GAME/DIK_RIGHT_KEYON.png",
};

HRESULT InitBillboard(void)
{
	MakeVertexBillboard();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	for (int i = 0; i < MAX_Billboard; i++)
	{
		ZeroMemory(&g_aBillboard[i].material, sizeof(g_aBillboard[i].material));
		g_aBillboard[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aBillboard[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aBillboard[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aBillboard[i].fWidth = Billboard_WIDTH;
		g_aBillboard[i].fHeight = Billboard_HEIGHT;
	}

	g_bAlpaTest = TRUE;

	g_Load = TRUE;
	return S_OK;
}

void ShutdownBillboard(void)
{
	if (g_Load == FALSE) return;

	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{ 
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{ 
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

void DrawBillboard(const CommandCode code, const XMFLOAT3 position, const XMFLOAT3 scale, const bool isTriggered)
{
	const auto& billboard = g_aBillboard[static_cast<int> (code)];

	if (g_bAlpaTest == TRUE)
	{
		SetAlphaTestEnable(TRUE);
	}

	SetLightEnable(FALSE);

	XMMATRIX mtxScl{}, mtxTranslate{}, mtxWorld{}, mtxView{};
	CAMERA* cam = GetCamera();

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

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

		mtxScl = XMMatrixScaling(scale.x, scale.y, scale.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxTranslate = XMMatrixTranslation(position.x, position.y, position.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		SetWorldMatrix(&mtxWorld);

		auto material = billboard.material;
		material.Diffuse = isTriggered ? 
			XMFLOAT4{ 0.5f, 0.5f, 0.5f, 1.0f } :
			XMFLOAT4{1.5f, 1.5f, 1.5f, 1.0f};

		SetMaterial(material);

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[static_cast<int> (code)]);

		GetDeviceContext()->Draw(4, 0);
	}

	SetLightEnable(TRUE);

	SetAlphaTestEnable(FALSE);
}

HRESULT MakeVertexBillboard(void)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 20.0f;
	float fHeight = 20.0f;

	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}
