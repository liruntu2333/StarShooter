#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sprite.h"
#include "title.h"

#define TEXTURE_WIDTH				(SCREEN_WIDTH)			 
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)			
#define TEXTURE_MAX					(TEXTURE_TITLE_MAX)		 

#define TEXTURE_WIDTH_MOON			(350)					 
#define TEXTURE_HEIGHT_MOON			(350)

#define TEXTURE_WIDTH_BUTTON		(250)						
#define TEXTURE_HEIGHT_BUTTON		(70)
#define BUTTON_DISTANCE				(60)

#define TEXTURE_WIDTH_TEXT			(70)
#define TEXTURE_HEIGHT_TEXT			(70)
#define TEXT_DISTANCE				(25)

#define TEXTURE_WIDTH_CONFIRM		(50)
#define TEXTURE_HEIGHT_CONFIRM		(50)

#define TEXTURE_PATTERN_DIVIDE_X	(50)														 
#define TEXTURE_PATTERN_DIVIDE_Y	(2)															 
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)			 
#define ANIM_WAIT					(8)															 

static ID3D11Buffer* g_VertexBuffer = nullptr;								 
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = {nullptr};		 

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/TITLE/background.png",
	"data/TEXTURE/TITLE/moon.png",
	"data/TEXTURE/TITLE/choose.png",
	"data/TEXTURE/TITLE/text_English.png",
	"data/TEXTURE/TITLE/star.png",
};

static TITLE	g_Title;			 

static MOON		g_Moon;				 

static TITLE	g_Choose[3];

static TITLE	g_Text;

static CONFIRM	g_Confirm;

static BOOL		g_Load = FALSE;

HRESULT InitTitle(void)
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

	g_Title.w = TEXTURE_WIDTH;
	g_Title.h = TEXTURE_HEIGHT;
	g_Title.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Title.texNo = TEXTURE_TITLE_BG;

	g_Moon.w = TEXTURE_WIDTH_MOON;
	g_Moon.h = TEXTURE_HEIGHT_MOON;
	g_Moon.pos = XMFLOAT3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.3f, 0.0f);
	g_Moon.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Moon.texNo = TEXTURE_TITLE_MOON;
	g_Moon.countAnim = 0;					
	g_Moon.patternAnim = 0;					

	g_Choose[0].w = TEXTURE_WIDTH_BUTTON;
	g_Choose[0].h = TEXTURE_HEIGHT_BUTTON;
	g_Choose[0].pos = XMFLOAT3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.6f, 0.0f);
	g_Choose[0].texNo = TEXTURE_TITLE_BOX;

	g_Choose[1].w = TEXTURE_WIDTH_BUTTON;
	g_Choose[1].h = TEXTURE_HEIGHT_BUTTON;
	g_Choose[1].pos = XMFLOAT3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.6f + TEXTURE_HEIGHT_BUTTON, 0.0f);
	g_Choose[1].texNo = TEXTURE_TITLE_BOX;

	g_Choose[2].w = TEXTURE_WIDTH_BUTTON;
	g_Choose[2].h = TEXTURE_HEIGHT_BUTTON;
	g_Choose[2].pos = XMFLOAT3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.6f + TEXTURE_HEIGHT_BUTTON * 2.0f, 0.0f);
	g_Choose[2].texNo = TEXTURE_TITLE_BOX;

	g_Text.w = TEXTURE_WIDTH_TEXT;
	g_Text.h = TEXTURE_HEIGHT_TEXT;
	g_Text.pos = XMFLOAT3(g_Choose[0].pos.x, g_Choose[0].pos.y, 0.0f);
	g_Text.texNo = TEXTURE_TITLE_TEXT;

	g_Confirm.w = TEXTURE_WIDTH_CONFIRM;
	g_Confirm.h = TEXTURE_HEIGHT_CONFIRM;
	g_Confirm.pos = XMFLOAT3(g_Choose[0].pos.x - 60, g_Choose[0].pos.y + 5, 0.0f);
	g_Confirm.texNo = TEXTRUE_TITLE_STAR;
	g_Confirm.choose = 0;

	g_Load = TRUE;
	return S_OK;
}

void UninitTitle(void)
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

void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		if (g_Confirm.choose == 0)
		{
			SetFade(FADE_OUT, MODE_TUTORIAL);
		}
		else if (g_Confirm.choose == 2)
		{
			SetFade(FADE_OUT, MODE_MAX);
		}
		else if (g_Confirm.choose == 1)
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
	}

	g_Moon.countAnim += 1.0f;
	if (g_Moon.countAnim > ANIM_WAIT)
	{
		g_Moon.countAnim = 0.0f;
		g_Moon.patternAnim = (g_Moon.patternAnim + 1) % ANIM_PATTERN_NUM;
	}

	int i = g_Confirm.choose;

	if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S))
	{
		i = i + 1;
		if (i < 3)
		{
			g_Confirm.pos.y = g_Choose[i].pos.y + 5;
			g_Confirm.choose = i;
		}
		else
		{
			g_Confirm.pos.y = g_Choose[0].pos.y + 5;
			g_Confirm.choose = 0;
		}
	}
	else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W))
	{
		i = i - 1;
		if (i >= 0)
		{
			g_Confirm.pos.y = g_Choose[i].pos.y + 5;
			g_Confirm.choose = i;
		}
		else
		{
			g_Confirm.pos.y = g_Choose[2].pos.y + 5;
			g_Confirm.choose = 2;
		}
	}

#ifdef _DEBUG	 

#endif
}

void DrawTitle(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			g_Title.pos.x, g_Title.pos.y, g_Title.w, g_Title.h,
			0.0f, 0.0f, 15.0f, 10.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Moon.texNo]);

		const float px = g_Moon.pos.x;	 
		const float py = g_Moon.pos.y;	 
		const float pw = g_Moon.w;		 
		const float ph = g_Moon.h;		 

		const float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	 
		const float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	 
		const float tx = static_cast<float>(g_Moon.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	 
		const float ty = static_cast<float>(g_Moon.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	 

		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_Moon.rot.z);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		for (auto& i : g_Choose)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i.texNo]);

			const float px = i.pos.x;
			const float py = i.pos.y;
			const float pw = i.w;
			const float ph = i.h;

			const float tw = 1.0f;
			const float th = 1.0f;
			const float tx = 1.0f;
			const float ty = 1.0f;

			SetSpriteColor_LeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			GetDeviceContext()->Draw(4, 0);
		}
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Text.texNo]);

		const float px = g_Text.pos.x;
		const float py = g_Text.pos.y;
		const float pw = g_Text.w;
		const float ph = g_Text.h;

		const float tw = 1.0f / 17;
		const float th = 1.0f / 12;

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 11, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 14, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, g_Choose[1].pos.y, pw, ph,
			tw * 16, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[1].pos.y, pw, ph,
			tw * 8, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[1].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, g_Choose[1].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, g_Choose[2].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[2].pos.y, pw, ph,
			tw * 9, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[2].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[2].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Confirm.texNo]);

		const float px = g_Confirm.pos.x;
		const float py = g_Confirm.pos.y;
		const float pw = g_Confirm.w;
		const float ph = g_Confirm.h;

		const float tw = 1.0f;
		const float th = 1.0f;
		const float tx = 1.0f;
		const float ty = 1.0f;

		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		GetDeviceContext()->Draw(4, 0);
	}
}