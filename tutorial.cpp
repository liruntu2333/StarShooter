#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sprite.h"
#include "tutorial.h"

#define TEXTURE_WIDTH				(SCREEN_WIDTH)	 
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	

#define TEXTURE_WIDTH_TIPS			(500.0f)	 
#define TEXTURE_HEIGHT_TIPS			(200.0f)	

#define TEXTURE_TUTORIAL_MAX		(4)
#define TEXTURE_TIPS_MAX			(2)

#define TEXTURE_MAX					(TEXTURE_TUTORIAL_MAX + TEXTURE_TIPS_MAX)				 

static ID3D11Buffer* g_VertexBuffer = nullptr;					 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};		 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/TUTORIAL/tutorial1.png",
	"data/TEXTURE/TUTORIAL/tutorial2.png",
	"data/TEXTURE/TUTORIAL/tutorial3.png",
	"data/TEXTURE/TUTORIAL/tutorial4.png",
	"data/TEXTURE/TUTORIAL/tips.png",
	"data/TEXTURE/TUTORIAL/tips2.png",
};

static TUTORIAL	g_Tutorial[TEXTURE_TUTORIAL_MAX];

static TUTORIAL g_Tips;

static BOOL		g_Load = FALSE;

HRESULT InitTutorial(void)
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

	g_Tutorial[0].use = TRUE;
	g_Tutorial[0].alpha = 1.0f;
	g_Tutorial[0].width = TEXTURE_WIDTH;
	g_Tutorial[0].height = TEXTURE_HEIGHT;
	g_Tutorial[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Tutorial[0].texNo = TEXTURE_TUTORIAL_1;

	g_Tutorial[1].use = FALSE;
	g_Tutorial[1].alpha = 0.0f;
	g_Tutorial[1].width = TEXTURE_WIDTH;
	g_Tutorial[1].height = TEXTURE_HEIGHT;
	g_Tutorial[1].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Tutorial[1].texNo = TEXTURE_TUTORIAL_2;

	g_Tutorial[2].use = FALSE;
	g_Tutorial[2].alpha = 0.0f;
	g_Tutorial[2].width = TEXTURE_WIDTH;
	g_Tutorial[2].height = TEXTURE_HEIGHT;
	g_Tutorial[2].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Tutorial[2].texNo = TEXTURE_TUTORIAL_3;

	g_Tutorial[3].use = FALSE;
	g_Tutorial[3].alpha = 0.0f;
	g_Tutorial[3].width = TEXTURE_WIDTH;
	g_Tutorial[3].height = TEXTURE_HEIGHT;
	g_Tutorial[3].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Tutorial[3].texNo = TEXTURE_TUTORIAL_4;

	g_Tips.use = TRUE;
	g_Tips.alpha = 1.0f;
	g_Tips.width = TEXTURE_WIDTH_TIPS;
	g_Tips.height = TEXTURE_HEIGHT_TIPS;
	g_Tips.pos = XMFLOAT3(SCREEN_WIDTH - TEXTURE_WIDTH_TIPS, SCREEN_HEIGHT - TEXTURE_HEIGHT_TIPS, 0.0f);
	g_Tips.texNo = TEXTURE_TUTORIAL_TIPS_1;

	g_Load = TRUE;
	return S_OK;
}

void UninitTutorial(void)
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

void UpdateTutorial(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{ 
		SetFade(FADE_OUT, MODE_GAME);
	}

	if (GetKeyboardTrigger(DIK_RIGHT))
	{
		for (int i = 0; i < TEXTURE_TUTORIAL_MAX; i++)
		{
			if (g_Tutorial[i].use == TRUE)
			{
				if (i == 3)
				{
					continue;
				}
				else
				{
					g_Tutorial[i].use = FALSE;
					g_Tutorial[i + 1].use = TRUE;
					break;
				}
			}
		}
	}

	if (g_Tutorial[3].use == TRUE)
	{
		g_Tips.texNo = TEXTURE_TUTORIAL_TIPS_2;
	}

	for (auto& i : g_Tutorial)
	{
		if (i.use == TRUE)
		{
			i.alpha = 1.0f;
		}
		else
		{
			i.alpha = 0.0f;
		}
	}

	if (g_Tips.use == TRUE)
	{
		g_Tips.alpha -= 0.02f;
		if (g_Tips.alpha <= 0.0f)
		{
			g_Tips.alpha = 0.0f;
			g_Tips.use = FALSE;
		}
	}
	else
	{
		g_Tips.alpha += 0.02f;
		if (g_Tips.alpha >= 1.0f)
		{
			g_Tips.alpha = 1.0f;
			g_Tips.use = TRUE;
		}
	}

#ifdef _DEBUG	 

#endif
}

void DrawTutorial(void)
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

	{
		for (auto& i : g_Tutorial)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i.texNo]);

			SetSpriteColor_LeftTop(g_VertexBuffer, i.pos.x, i.pos.y, i.width, i.height, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f,
				                       i.alpha));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Tips.texNo]);

		SetSpriteColor_LeftTop(g_VertexBuffer, g_Tips.pos.x, g_Tips.pos.y, g_Tips.width, g_Tips.height, 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, g_Tips.alpha));

		GetDeviceContext()->Draw(4, 0);
	}
}