//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

#define	PLAYER_SIZE		(5.0f)				// 当たり判定の大きさ

#define PLAYER_HP_MAX	(5)

#define PLAYER_MP_MAX	(5)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;		// ポリゴンの位置
	XMFLOAT3			rot;		// ポリゴンの向き(回転)
	XMFLOAT3			scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4			mtxWorld;	// ワールドマトリックス

	BOOL				load;
	DX11_MODEL			model;		// モデル情報

	float				spd;		// 移動スピード
	float				dir;		// 向き
	float				size;		// 当たり判定の大きさ
	int					shadowIdx;	// 影のIndex
	BOOL				use;

	// 階層アニメーション用のメンバー変数
	INTERPOLATION_DATA	*tbl_adr;	// アニメデータのテーブル先頭アドレス
	int					tbl_size;	// 登録したテーブルのレコード総数
	float				move_time;	// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;	// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			quaternion;	// クォータニオン
	XMFLOAT3			upVector;	// 自分が立っている所

	// 属性
	int					HP;
	int					MP;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

/**
 * \brief Every object moving correspondingly to player should check this flag to decide
 * whether wrap their position in field or not.
 *
 * \return An int type flag, last four bits used for representing a player's position's state
 * to field's boarder.
 * e.g. 0b0000 represents isn't out of any boarder.
 *      0b0001 represents out of Z plus boarder.
 */
int IsPlayerEndOfBoarder();
