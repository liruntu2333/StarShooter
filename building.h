//=============================================================================
//
// 建物モデル処理 [building.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_BUILDING		(25)					// 建物の数

#define	ENEMY_SIZE			(5.0f)					// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum BuildingType : int;

struct BUILDING
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	XMFLOAT3			pos_center;

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	int					shadowIdx;			// 影のインデックス番号

	INTERPOLATION_DATA* tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	BUILDING*			parent;				// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	BuildingType		type;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBuilding(void);
void UninitBuilding(void);
void UpdateBuilding(void);
void DrawBuilding(void);

BUILDING* GetBuilding(void);

