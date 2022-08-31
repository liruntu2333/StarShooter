//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include <vector>

#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY		(10)					// エネミーの数

#define	ENEMY_SIZE		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum EnemyBehaviorType : int;

struct ENEMY
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
	
	INTERPOLATION_DATA	*tbl_adr;			// アニメデータのテーブル先頭アドレス
	int					tbl_size;			// 登録したテーブルのレコード総数
	float				move_time;			// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	ENEMY*				parent;				// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	std::vector<int>	codes;
	int					compare_index;

	EnemyBehaviorType			type;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

