//=============================================================================
//
// 弾発射処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MAX_BULLET				(256)	// 弾最大数

#define	BULLET_ATTACK_SIZE		(5.0f)	// 当たり判定の大きさ

#include <memory>

#include "MathHelper.h"

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class BezierCurve;
struct ENEMY;

struct BULLET
{
	XMFLOAT3			pos;			// ポリゴンの位置
	XMFLOAT3			rot;			// ポリゴンの向き(回転)
	XMFLOAT3			scl;			// ポリゴンの大きさ(スケール)

	XMFLOAT4X4			mtxWorld;		// ワールドマトリックス

	BOOL				load;
	DX11_MODEL			model;			// モデル情報
	
	// 階層アニメーション用のメンバー変数
	INTERPOLATION_DATA* tbl_adr;		// アニメデータのテーブル先頭アドレス
	int					tbl_size;		// 登録したテーブルのレコード総数
	float				move_time;		// 実行時間

	// 親は、NULL、子供は親のアドレスを入れる
	BULLET*				parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	float				spd;			// 移動量
	int					shadowIdx;		// 影ID
	float				attackSize;		// 当たり判定の大きさ
	BOOL				use;			// 使用しているかどうか

	std::unique_ptr<BezierCurveQuadratic> curve = nullptr;
	float				flyingTime;
	float				hitTime;
	ENEMY*				target;
} ;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);
int SetBullet(const std::array<XMFLOAT3, 3>& controlPoints, float tHit, ENEMY* target);

BULLET *GetBullet(void);

