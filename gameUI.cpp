//=============================================================================
//
// ゲーム中のUI処理 [GameUI.cpp]
// Author : 
//
//=============================================================================
#include "gameUI.h"
#include "sprite.h"
#include "player.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
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

#define TEXTURE_PATTERN_DIVIDE_X			(1)														// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y			(1)														// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM					(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)		// アニメーションパターン数
#define ANIM_WAIT							(1)														// アニメーションの切り替わるWait値
//
//#define PLAYER_HP_MAX						(5)
//#define PLAYER_MP_MAX						(5)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;					// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };		// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/GAME/hp_1.png",
	"data/TEXTURE/GAME/hp_0.png",
	"data/TEXTURE/GAME/mp_1.png",
	"data/TEXTURE/GAME/mp_0.png",
	"data/TEXTURE/GAME/DIK_A_KEYON.png",
	"data/TEXTURE/GAME/DIK_D_KEYON.png",
};

static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

static GameUI_Box		g_UIBox[BOX_MAX];

static GameUI_HP		g_UIHP[PLAYER_HP_MAX];

static GameUI_MP		g_UIMP[PLAYER_MP_MAX];

static GameUI_Box		g_UILeft;

static GameUI_Box		g_UIRight;

static GameUI_Box		g_UIShift;

// 初期化処理
//=============================================================================
HRESULT InitGameUI(void)
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

	// ボックス
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_UIBox[i].w = TEXTURE_HEIGHT_BOX;
		g_UIBox[i].h = TEXTURE_WIDTH_BOX;
		g_UIBox[i].pos = XMFLOAT3(TEXTURE_WIDTH_BOX * 0.5f, SCREEN_HEIGHT - TEXTURE_HEIGHT_BOX * i, 0.0f);
		//g_UIBox[i].texNo = TEXTURE_UIBOX;
	}


	// HP
	for (int i = 0; i < PLAYER_HP_MAX; i++)
	{
		g_UIHP[i].w = TEXTURE_WIDTH_HP;
		g_UIHP[i].h = TEXTURE_HEIGHT_HP;
		g_UIHP[i].pos = XMFLOAT3(g_UIBox[BOX_HP].pos.x + i * TEXTURE_WIDTH_HP * 0.5f,
			g_UIBox[BOX_HP].pos.y - TEXTURE_HEIGHT_HP,
			0.0f);
		g_UIHP[i].texNo = TEXTURE_HP_1;
	}

	// MP
	for (int i = 0; i < PLAYER_MP_MAX; i++)
	{
		g_UIMP[i].w = TEXTURE_WIDTH_MP;
		g_UIMP[i].h = TEXTURE_HEIGHT_MP;
		g_UIMP[i].pos = XMFLOAT3(g_UIBox[BOX_MP].pos.x + i * TEXTURE_WIDTH_MP * 0.5f,
			g_UIBox[BOX_MP].pos.y - TEXTURE_HEIGHT_MP,
			0.0f);
		g_UIMP[i].texNo = TEXTURE_MP_1;
	}

	// Left_button
	g_UILeft.w = TEXTURE_WIDTH_LEFT;
	g_UILeft.h = TEXTURE_HEIGHT_LEFT;
	g_UILeft.pos = XMFLOAT3(SCREEN_WIDTH * 0.5f - TEXTURE_WIDTH_LEFT * 2.0f, SCREEN_HEIGHT - TEXTURE_HEIGHT_LEFT, 0.0f);
	g_UILeft.texNo = TEXTURE_BUTTON_LEFT;

	// Right_button
	g_UIRight.w = TEXTURE_WIDTH_RIGHT;
	g_UIRight.h = TEXTURE_HEIGHT_RIGHT;
	g_UIRight.pos = XMFLOAT3(SCREEN_WIDTH * 0.5f + TEXTURE_WIDTH_RIGHT * 2.0f, SCREEN_HEIGHT - TEXTURE_HEIGHT_LEFT, 0.0f);
	g_UIRight.texNo = TEXTURE_BUTTON_RIGHT;

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameUI(void)
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
void UpdateGameUI(void)
{


	PLAYER* player = GetPlayer();
	// HPの更新
	{

		for (int i = PLAYER_HP_MAX - 1; i > player->HP - 1; i--)
		{
			g_UIHP[i].texNo = TEXTURE_HP_0;
		}


		for (int i = 0; i < player->HP ; i++)
		{
			g_UIHP[i].texNo = TEXTURE_HP_1;
		}
	}


	// MPの更新
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


#ifdef _DEBUG	// デバッグ情報を表示する

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameUI(void)
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


	// GameUIのboxを描画
	//{
	//	for (int i = 0; i < BOX_MAX; i++)
	//	{
	//		// テクスチャ設定
	//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIBox[i].texNo]);

	//		// UIの位置やテクスチャー座標を反映
	//		float px = g_UIBox[i].pos.x;
	//		float py = g_UIBox[i].pos.y;
	//		float pw = g_UIBox[i].w;
	//		float ph = g_UIBox[i].h;

	//		float tw = 1.0f;
	//		float th = 1.0f;
	//		float tx = 0.0f;
	//		float ty = 0.0f;

	//		// １枚のポリゴンの頂点とテクスチャ座標を設定
	//		SetSpriteLTColor(g_VertexBuffer,
	//			px, py, pw, ph,
	//			tx, ty, tw, th,
	//			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//		// ポリゴン描画
	//		GetDeviceContext()->Draw(4, 0);
	//	}
	//}



	// HPを描画
	{
		for (int i = 0; i < PLAYER_HP_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIHP[i].texNo]);

			// UIの位置やテクスチャー座標を反映
			float px = g_UIHP[i].pos.x;
			float py = g_UIHP[i].pos.y;
			float pw = g_UIHP[i].w;
			float ph = g_UIHP[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}


	// MPを描画
	{
		for (int i = 0; i < PLAYER_MP_MAX; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIMP[i].texNo]);

			// UIの位置やテクスチャー座標を反映
			float px = g_UIMP[i].pos.x;
			float py = g_UIMP[i].pos.y;
			float pw = g_UIMP[i].w;
			float ph = g_UIMP[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}


	// Left_Buttonを描画
	{
		
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UILeft.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_UILeft.pos.x;
		float py = g_UILeft.pos.y;
		float pw = g_UILeft.w;
		float ph = g_UILeft.h;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
		

	}


	// Right_Buttonを描画
	{

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIRight.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_UIRight.pos.x;
		float py = g_UIRight.pos.y;
		float pw = g_UIRight.w;
		float ph = g_UIRight.h;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 0.0f;
		float ty = 0.0f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);


	}

}







