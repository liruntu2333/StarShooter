#include "gameUI.h"
#include "sprite.h"
#include "player.h"
#include "debugproc.h"

#define TEXTURE_WIDTH_BOX					(100.0f)
#define TEXTURE_HEIGHT_BOX					(50.0f)

#define TEXTURE_WIDTH_HP					(40.0f)
#define TEXTURE_HEIGHT_HP					(40.0f)

#define TEXTURE_WIDTH_MP					(40.0f)
#define TEXTURE_HEIGHT_MP					(40.0f)

#define TEXTURE_WIDTH_LEFT					(150.0f)
#define TEXTURE_HEIGHT_LEFT					(150.0f)

#define TEXTURE_WIDTH_RIGHT					(150.0f)
#define TEXTURE_HEIGHT_RIGHT				(150.0f)

#define TEXTURE_WIDTH_SHIFT					(200.0f)
#define TEXTURE_HEIGHT_SHIFT				(40.0f)

#define TEXTURE_PATTERN_DIVIDE_X			(1)														 
#define TEXTURE_PATTERN_DIVIDE_Y			(1)														 
#define ANIM_PATTERN_NUM					(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)		 
#define ANIM_WAIT							(1)														 
static ID3D11Buffer* g_VertexBuffer = nullptr;					 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};		 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/GAME/hp_1.png",
	"data/TEXTURE/GAME/hp_0.png",
	"data/TEXTURE/GAME/mp_1.png",
	"data/TEXTURE/GAME/mp_0.png",
	"data/TEXTURE/GAME/DIK_A_KEYON.png",
	"data/TEXTURE/GAME/DIK_D_KEYON.png",
};

static BOOL	g_Load = FALSE;		 

static GameUI_Box		g_UIBox[BOX_MAX];

static GameUI_HP		g_UIHP[PLAYER_HP_MAX];

static GameUI_MP		g_UIMP[PLAYER_MP_MAX];

static GameUI_Box		g_UILeft;

static GameUI_Box		g_UIRight;

static GameUI_Box		g_UIShift;

HRESULT InitGameUI(void)
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

	for (int i = 0; i < BOX_MAX; i++)
	{
		g_UIBox[i].w = TEXTURE_HEIGHT_BOX;
		g_UIBox[i].h = TEXTURE_WIDTH_BOX;
		g_UIBox[i].pos = XMFLOAT3(TEXTURE_WIDTH_BOX * 0.5f, SCREEN_HEIGHT - TEXTURE_HEIGHT_BOX * i, 0.0f);
	}

	for (int i = 0; i < PLAYER_HP_MAX; i++)
	{
		g_UIHP[i].w = TEXTURE_WIDTH_HP;
		g_UIHP[i].h = TEXTURE_HEIGHT_HP;
		g_UIHP[i].pos = XMFLOAT3(g_UIBox[BOX_HP].pos.x + i * TEXTURE_WIDTH_HP * 0.5f,
			g_UIBox[BOX_HP].pos.y - TEXTURE_HEIGHT_HP,
			0.0f);
		g_UIHP[i].texNo = TEXTURE_HP_1;
	}

	for (int i = 0; i < PLAYER_MP_MAX; i++)
	{
		g_UIMP[i].w = TEXTURE_WIDTH_MP;
		g_UIMP[i].h = TEXTURE_HEIGHT_MP;
		g_UIMP[i].pos = XMFLOAT3(g_UIBox[BOX_MP].pos.x + i * TEXTURE_WIDTH_MP * 0.5f,
			g_UIBox[BOX_MP].pos.y - TEXTURE_HEIGHT_MP,
			0.0f);
		g_UIMP[i].texNo = TEXTURE_MP_1;
	}

	g_UILeft.w = TEXTURE_WIDTH_LEFT;
	g_UILeft.h = TEXTURE_HEIGHT_LEFT;
	g_UILeft.pos = XMFLOAT3(SCREEN_WIDTH * 0.5f - TEXTURE_WIDTH_LEFT * 2.0f, SCREEN_HEIGHT - TEXTURE_HEIGHT_LEFT, 0.0f);
	g_UILeft.texNo = TEXTURE_BUTTON_LEFT;

	g_UIRight.w = TEXTURE_WIDTH_RIGHT;
	g_UIRight.h = TEXTURE_HEIGHT_RIGHT;
	g_UIRight.pos = XMFLOAT3(SCREEN_WIDTH * 0.5f + TEXTURE_WIDTH_RIGHT * 2.0f, SCREEN_HEIGHT - TEXTURE_HEIGHT_LEFT, 0.0f);
	g_UIRight.texNo = TEXTURE_BUTTON_RIGHT;

	g_Load = TRUE;	 
	return S_OK;
}

void UninitGameUI(void)
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

void UpdateGameUI(void)
{
	const PLAYER* player = GetPlayer();
	{
		for (int i = PLAYER_HP_MAX - 1; i > player->HP - 1; i--)
		{
			g_UIHP[i].texNo = TEXTURE_HP_0;
		}

		for (int i = 0; i < player->HP; i++)
		{
			g_UIHP[i].texNo = TEXTURE_HP_1;
		}
	}

	{
		for (int i = PLAYER_MP_MAX - 1; i > player->MP - 1; i--)
		{
			g_UIMP[i].texNo = TEXTURE_MP_0;
		}

		for (int i = 0; i < player->MP; i++)
		{
			g_UIMP[i].texNo = TEXTURE_MP_1;
		}
	}

#ifdef _DEBUG	 

#endif
}

void DrawGameUI(void)
{
	const UINT stride = sizeof(VERTEX_3D);
	const UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	SetWorldViewProjection2D();

	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	{
		for (auto& i : g_UIHP)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i.texNo]);

			const float px = i.pos.x;
			const float py = i.pos.y;
			const float pw = i.w;
			const float ph = i.h;

			const float tw = 1.0f;
			const float th = 1.0f;
			const float tx = 0.0f;
			const float ty = 0.0f;

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	{
		for (auto& i : g_UIMP)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i.texNo]);

			const float px = i.pos.x;
			const float py = i.pos.y;
			const float pw = i.w;
			const float ph = i.h;

			const float tw = 1.0f;
			const float th = 1.0f;
			const float tx = 0.0f;
			const float ty = 0.0f;

			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UILeft.texNo]);

		const float px = g_UILeft.pos.x;
		const float py = g_UILeft.pos.y;
		const float pw = g_UILeft.w;
		const float ph = g_UILeft.h;

		const float tw = 1.0f;
		const float th = 1.0f;
		const float tx = 0.0f;
		const float ty = 0.0f;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIRight.texNo]);

		const float px = g_UIRight.pos.x;
		const float py = g_UIRight.pos.y;
		const float pw = g_UIRight.w;
		const float ph = g_UIRight.h;

		const float tw = 1.0f;
		const float th = 1.0f;
		const float tx = 0.0f;
		const float ty = 0.0f;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}