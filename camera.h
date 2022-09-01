//=============================================================================
//
// カメラ処理 [camera.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct CAMERA
{
	XMFLOAT4X4			mtxView;		// ビューマトリックス
	XMFLOAT4X4			mtxInvView;		// ビューマトリックス
	XMFLOAT4X4			mtxProjection;	// プロジェクションマトリックス
	
	XMFLOAT3			pos;			// カメラの視点(位置)
	XMFLOAT3			at;				// カメラの注視点
	XMFLOAT3			up;				// カメラの上方向ベクトル
	float				theta;
	float				phi;
	
	float				lenPlayer;			// カメラの視点と注視点の距離(プレーヤーの場合)
	float				lenMenu;			// カメラの視点と注視点の距離(メニューの場合)
};


enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_NONE,

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA *GetCamera(void);

void LerpCameraPosition(XMFLOAT3 pos, float dir, float tPos = 1.0f);
//void SetCameraAtMenu(XMFLOAT3 pos, float dir, float t = 1.0f);
void LerpCameraPositionAt(XMFLOAT3 playerPos, XMFLOAT3 enemyPos, float dir, float tPos, float tAt);
void LerpCameraViewAngle(float angle, float t);
