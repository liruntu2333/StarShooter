//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX						(4)				// テクスチャの数

#define TEXTURE_WIDTH_BG				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT_BG				(SCREEN_HEIGHT)	// 

#define TEXTURE_WIDTH_TEXT				(300)
#define TEXTURE_HEIGHT_TEXT				(300)
#define TEXT_DISTANCE					(110)

#define BG_MAX							(3)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/RESULT/resultbg_0.png",
	"data/TEXTURE/RESULT/resultbg_1.png",
	"data/TEXTURE/RESULT/resultbg_2.png",
	"data/TEXTURE/TITLE/text_English.png",
};


static RESULT		g_ResultBG[BG_MAX];

static RESULT		g_ResultText;

static int			g_ResultScore = 0;

static BOOL			g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
	ID3D11Device *pDevice = GetDevice();

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

	for (int i = 0; i < BG_MAX; i++)
	{
		g_ResultBG[i].w = TEXTURE_WIDTH_BG;
		g_ResultBG[i].h = TEXTURE_HEIGHT_BG;
		g_ResultBG[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ResultBG[i].texNo = i;
	}


	// 文字の描画の初期化
	g_ResultText.w = TEXTURE_WIDTH_TEXT;
	g_ResultText.h = TEXTURE_HEIGHT_TEXT;
	g_ResultText.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_ResultText.texNo = 3;

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_sample002);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_GAME);
	}
	


#ifdef _DEBUG	// デバッグ情報を表示する
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	// リザルトの背景を描画
	{
		for (int i = 0; i < BG_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultBG[i].texNo]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor_LeftTop(g_VertexBuffer,
				g_ResultBG[i].pos.x, g_ResultBG[i].pos.y, g_ResultBG[i].w, g_ResultBG[i].h,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}

	


	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultText.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_ResultText.pos.x;
		float py = g_ResultText.pos.y;
		float pw = g_ResultText.w;
		float ph = g_ResultText.h;

		float tw = 1.0f / 17;
		float th = 1.0f / 12;

		//C
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//O
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 8, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//N
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//I
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//N
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, py, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//U
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, py, pw, ph,
			tw * 3, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//E
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, py, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//?
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 8, py, pw, ph,
			tw * 1, th * 10, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 最終スコア表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultText.texNo]);

		// 桁数分処理する
		int number = g_ResultScore;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// 次の桁へ
			number /= 10;

			// スコアの位置やテクスチャー座標を反映
			float px = 220.0f - i*TEXT_DISTANCE*0.4f;	// スコアの表示位置X
			float py = 220.0f;						// スコアの表示位置Y
			float pw = TEXTURE_WIDTH_TEXT*0.4f;					// スコアの表示幅
			float ph = TEXTURE_WIDTH_TEXT*0.4f;					// スコアの表示高さ

			float tw = 1.0f / 17;				// テクスチャの幅
			float th = 1.0f / 12;				// テクスチャの高さ
			float tx = 0;
			if (x == 0)
			{
				tx = 10 * tw;
			}
			else
			{
				tx = x * tw;					// テクスチャの左上X座標
			}
			float ty = 6 * th;					// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor_LeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

	}



}




