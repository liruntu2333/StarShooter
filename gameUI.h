//=============================================================================
//
// ゲーム中のUI処理 [GameUI.cpp]
// Author : GP12B183 11 黄源
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum
{
	TEXTURE_HP_1,
	TEXTURE_HP_0,
	TEXTURE_MP_1,
	TEXTURE_MP_0,
	TEXTURE_BUTTON_LEFT,
	TEXTURE_BUTTON_RIGHT,
	TEXTURE_BUTTON_SHIFT,
	TEXTURE_MAX,
};

enum
{
	BOX_HP,
	BOX_MP,
	BOX_MAX,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct GameUI_Box
{
	XMFLOAT3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号

};

struct GameUI_HP
{
	XMFLOAT3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
};

struct GameUI_MP
{
	XMFLOAT3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGameUI(void);
void UninitGameUI(void);
void UpdateGameUI(void);
void DrawGameUI(void);