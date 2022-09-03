#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"

#define TEXTURE_WIDTH				(32)	 
#define TEXTURE_HEIGHT				(64)	
#define TEXTURE_MAX					(1)		 

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/number16x32.png",
};

static BOOL						g_Use;						   
static float					g_w, g_h;					 
static XMFLOAT3					g_Pos;						 
static int						g_TexNo;					 

static int						g_Score;					 

static BOOL						g_Load = FALSE;

HRESULT InitScore(void)
{
	ID3D11Device* pDevice = GetDevice();

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = nullptr;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			nullptr,
			nullptr,
			&g_Texture[i],
		nullptr);
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { SCREEN_WIDTH * 0.5f + TEXTURE_WIDTH * 0.5f , TEXTURE_HEIGHT, 0.0f };
	g_TexNo = 0;

	g_Score = 0;	 

	g_Load = TRUE;
	return S_OK;
}

void UninitScore(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = nullptr;
	}

	for (auto& i : g_Texture)
	{
		if (i)
		{
			i->Release();
			i = nullptr;
		}
	}

	g_Load = FALSE;
}

void UpdateScore(void)
{
#ifdef _DEBUG	 
#endif
}

void DrawScore(void)
{
	const UINT stride = sizeof(VERTEX_3D);
	const UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	SetWorldViewProjection2D();

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material{};
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	int number = g_Score;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		const float x = static_cast<float>(number % 10);

		const float px = g_Pos.x - g_w * i;	 
		const float py = g_Pos.y;			 
		const float pw = g_w;				 
		const float ph = g_h;				 

		const float tw = 1.0f / 10;		 
		const float th = 1.0f / 1;		 
		const float tx = x * tw;			 
		const float ty = 0.0f;			 

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		number /= 10;
	}
}

void AddScore(int add)
{
	g_Score += add;
	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}
}

int GetScore(void)
{
	return g_Score;
}