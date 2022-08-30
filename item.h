//=============================================================================
//
// アイテムモデル処理 [item.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ITEM		(10)					// itemの数
#define MAX_ITEM_GOOD	(6)
#define MAX_ITEM_WORSE	(MAX_ITEM - MAX_ITEM_GOOD)	

#define	ITEM_SIZE		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum ItemType : int;

struct ITEM
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)
	XMFLOAT3			velocity;

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード

	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号

	INTERPOLATION_DATA* tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	ITEM* parent;				// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	ItemType			type;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

ITEM* GetItem(void);

