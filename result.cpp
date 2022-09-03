#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

#define TEXTURE_MAX						(4)				 

#define TEXTURE_WIDTH_BG				(SCREEN_WIDTH)	 
#define TEXTURE_HEIGHT_BG				(SCREEN_HEIGHT)	

#define TEXTURE_WIDTH_TEXT				(300)
#define TEXTURE_HEIGHT_TEXT				(300)
#define TEXT_DISTANCE					(110)

#define BG_MAX							(3)

static ID3D11Buffer* g_VertexBuffer = nullptr;		 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};	 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/RESULT/resultbg_0.png",
	"data/TEXTURE/RESULT/resultbg_1.png",
	"data/TEXTURE/RESULT/resultbg_2.png",
	"data/TEXTURE/TITLE/text_English.png",
};

static RESULT		g_ResultBG[BG_MAX];

static RESULT		g_ResultText;

static int			g_ResultScore = 0;

static BOOL			g_Load = FALSE;

HRESULT InitResult(void)
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

	for (int i = 0; i < BG_MAX; i++)
	{
		g_ResultBG[i].w = TEXTURE_WIDTH_BG;
		g_ResultBG[i].h = TEXTURE_HEIGHT_BG;
		g_ResultBG[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ResultBG[i].texNo = i;
	}

	g_ResultText.w = TEXTURE_WIDTH_TEXT;
	g_ResultText.h = TEXTURE_HEIGHT_TEXT;
	g_ResultText.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_ResultText.texNo = 3;

	PlaySound(SOUND_LABEL_BGM_sample002);

	g_Load = TRUE;
	return S_OK;
}

void UninitResult(void)
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

void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{ 
		SetFade(FADE_OUT, MODE_GAME);
	}

#ifdef _DEBUG	 

#endif
}

void DrawResult(void)
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
		for (auto& i : g_ResultBG)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i.texNo]);

			SetSpriteColor_LeftTop(g_VertexBuffer,
			                       i.pos.x, i.pos.y, i.w, i.h,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultText.texNo]);

		const float px = g_ResultText.pos.x;
		const float py = g_ResultText.pos.y;
		const float pw = g_ResultText.w;
		const float ph = g_ResultText.h;

		const float tw = 1.0f / 17;
		const float th = 1.0f / 12;

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 8, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, py, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, py, pw, ph,
			tw * 3, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, py, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 8, py, pw, ph,
			tw * 1, th * 10, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultText.texNo]);

		int number = g_ResultScore;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			const float x = static_cast<float>(number % 10);

			number /= 10;

			const float px = 220.0f - i * TEXT_DISTANCE * 0.4f;	 
			const float py = 220.0f;						 
			const float pw = TEXTURE_WIDTH_TEXT * 0.4f;					 
			const float ph = TEXTURE_WIDTH_TEXT * 0.4f;					 

			const float tw = 1.0f / 17;				 
			const float th = 1.0f / 12;				 
			float tx = 0;
			if (x == 0)
			{
				tx = 10 * tw;
			}
			else
			{
				tx = x * tw;					 
			}
			const float ty = 6 * th;					 

			SetSpriteColor_LeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}
}

void SetResult(int score)
{
	g_ResultScore = score;
}