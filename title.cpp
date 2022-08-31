//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
//#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)			// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)			// 
#define TEXTURE_MAX					(TEXTURE_TITLE_MAX)		// テクスチャの数

#define TEXTURE_WIDTH_MOON			(350)					// 月のサイズ
#define TEXTURE_HEIGHT_MOON			(350)

#define TEXTURE_WIDTH_BUTTON		(250)					//	ボタンサイズ
#define TEXTURE_HEIGHT_BUTTON		(70)
#define BUTTON_DISTANCE				(60)

#define TEXTURE_WIDTH_TEXT			(70)
#define TEXTURE_HEIGHT_TEXT			(70)
#define TEXT_DISTANCE				(25)

#define TEXTURE_WIDTH_CONFIRM		(50)
#define TEXTURE_HEIGHT_CONFIRM		(50)

#define TEXTURE_PATTERN_DIVIDE_X	(50)														// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)															// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)			// アニメーションパターン数
#define ANIM_WAIT					(8)															// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;								// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };		// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/TITLE/background.png",
	"data/TEXTURE/TITLE/moon.png",
	"data/TEXTURE/TITLE/choose.png",
	"data/TEXTURE/TITLE/text_English.png",
	"data/TEXTURE/TITLE/star.png",
};


static TITLE	g_Title;			// 背景の初期量

static MOON		g_Moon;				// 月の初期量

static TITLE	g_Choose[3];

static TITLE	g_Text;

static CONFIRM	g_Confirm;

static BOOL		g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	// backgroundの描画
	g_Title.w = TEXTURE_WIDTH;
	g_Title.h = TEXTURE_HEIGHT;
	g_Title.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Title.texNo = TEXTURE_TITLE_BG;

	// 地球の描画の初期化
	g_Moon.w = TEXTURE_WIDTH_MOON;
	g_Moon.h = TEXTURE_HEIGHT_MOON;
	g_Moon.pos = XMFLOAT3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.3f, 0.0f);
	g_Moon.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Moon.texNo = TEXTURE_TITLE_MOON;
	g_Moon.countAnim = 0;					//アニメカウント
	g_Moon.patternAnim = 0;					//アニメパターン番号

	// 選択ボタンの描画の初期化
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

	// 文字の描画の初期化
	g_Text.w = TEXTURE_WIDTH_TEXT;
	g_Text.h = TEXTURE_HEIGHT_TEXT;
	g_Text.pos = XMFLOAT3(g_Choose[0].pos.x, g_Choose[0].pos.y, 0.0f);
	g_Text.texNo = TEXTURE_TITLE_TEXT;

	// 選択アイコンの描画の初期化
	g_Confirm.w = TEXTURE_WIDTH_CONFIRM;
	g_Confirm.h = TEXTURE_HEIGHT_CONFIRM;
	g_Confirm.pos = XMFLOAT3(g_Choose[0].pos.x - 60, g_Choose[0].pos.y + 5, 0.0f);
	g_Confirm.texNo = TEXTRUE_TITLE_STAR;
	g_Confirm.choose = 0;


	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_sample000);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		// Enter押したら、ステージを切り替える
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
			// セーブデータをロードする
			//SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}

	}

	// アニメーション  
	g_Moon.countAnim += 1.0f;
	if (g_Moon.countAnim > ANIM_WAIT)
	{
		g_Moon.countAnim = 0.0f;
		// パターンの切り替え
		g_Moon.patternAnim = (g_Moon.patternAnim + 1) % ANIM_PATTERN_NUM;
	}


	// 選択ボタンを移動させる
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

#ifdef _DEBUG	// デバッグ情報を表示する


#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			g_Title.pos.x, g_Title.pos.y, g_Title.w, g_Title.h,
			0.0f, 0.0f, 15.0f, 10.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 地球を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Moon.texNo]);

		// 地球の位置やテクスチャー座標を反映
		float px = g_Moon.pos.x;	// プレイヤーの表示位置X
		float py = g_Moon.pos.y;	// プレイヤーの表示位置Y
		float pw = g_Moon.w;		// プレイヤーの表示幅
		float ph = g_Moon.h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = (float)(g_Moon.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Moon.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
			g_Moon.rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 選択ボタンを描画
	{
		for (int i = 0; i < 3; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Choose[i].texNo]);

			// UIの位置やテクスチャー座標を反映
			float px = g_Choose[i].pos.x;
			float py = g_Choose[i].pos.y;
			float pw = g_Choose[i].w;
			float ph = g_Choose[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 1.0f;
			float ty = 1.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor_LeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// 文字(Text)を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Text.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_Text.pos.x;
		float py = g_Text.pos.y;
		float pw = g_Text.w;
		float ph = g_Text.h;

		float tw = 1.0f / 17;
		float th = 1.0f / 12;

		//S
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//A
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 11, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//R
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 14, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//***************************************************//
		//C
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, g_Choose[1].pos.y, pw, ph,
			tw * 16, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//O
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[1].pos.y, pw, ph,
			tw * 8, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//N
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[1].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//I
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, g_Choose[1].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//N
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//U
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//E
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//***************************************************//
		//E
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, g_Choose[2].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//X
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[2].pos.y, pw, ph,
			tw * 9, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//I
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[2].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[2].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 選択アイコンを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Confirm.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_Confirm.pos.x;
		float py = g_Confirm.pos.y;
		float pw = g_Confirm.w;
		float ph = g_Confirm.h;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 1.0f;
		float ty = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

