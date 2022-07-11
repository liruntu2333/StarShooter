//=============================================================================
//
// 武器のモデル処理 [weapon.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "light.h"
#include "bullet.h"
#include "weapon.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_WEAPON_1				"data/MODEL/magicWand1.obj"		// 読み込むモデル名 魔法の杖１
#define	MODEL_WEAPON_1_PARTS		"data/MODEL/torus.obj"			// 読み込むモデル名 魔法の杖１のパーツ

#define	VALUE_MOVE					(10.0f)							// 移動量
#define	VALUE_ROTATE				(XM_PI * 0.02f)					// 回転量


#define WEAPON_1_PARTS_MAX			(2)										// 魔法の杖1のパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static WEAPON		g_Weapon[MAX_WEAPON];							// 武器

static WEAPON		g_Weapon_1_Parts[WEAPON_1_PARTS_MAX];			// 武器１(魔法の杖１)のパーツ用

static BOOL			g_Load = FALSE;


// プレイヤーの階層アニメーションデータ
// プレイヤーの頭を左右に動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitWeapon(void)
{
	LoadModel(MODEL_WEAPON_1, &g_Weapon[0].model);
	g_Weapon[0].load = TRUE;

	PLAYER *player = GetPlayer();


	for (int i = 0; i < MAX_WEAPON; i++)
	{
		g_Weapon[i].pos = player->pos;
		g_Weapon[i].rot = { 0.0f, XM_PI, 0.0f };
		g_Weapon[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Weapon[i].spd = 0.0f;			// 移動スピードクリア
	}
	

	g_Weapon[0].use = TRUE;


	// 階層アニメーション用の初期化処理
	g_Weapon[0].parent = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < WEAPON_1_PARTS_MAX; i++)
	{
		g_Weapon_1_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Weapon_1_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Weapon_1_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Weapon_1_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Weapon_1_Parts[i].parent = &g_Weapon[0];		// ← ここに親のアドレスを入れる
	//	g_Parts[腕].parent= &g_Player;					// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].parent= &g_Paerts[腕];				// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Weapon_1_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Weapon_1_Parts[i].move_time = 0.0f;	// 実行時間をクリア
		g_Weapon_1_Parts[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Weapon_1_Parts[i].load = FALSE;
	}

	g_Weapon_1_Parts[0].use = TRUE;
	g_Weapon_1_Parts[0].parent = &g_Weapon[0];		// 親をセット
	g_Weapon_1_Parts[0].tbl_adr = move_tbl_right;	// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[0].tbl_size = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[0].load = 1;
	LoadModel(MODEL_WEAPON_1_PARTS, &g_Weapon_1_Parts[0].model);

	g_Weapon_1_Parts[1].use = TRUE;
	g_Weapon_1_Parts[1].parent = &g_Weapon[0];		// 親をセット
	g_Weapon_1_Parts[1].tbl_adr = move_tbl_left;	// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[1].tbl_size = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[1].load = 1;
	LoadModel(MODEL_WEAPON_1_PARTS, &g_Weapon_1_Parts[1].model);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitWeapon(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	for (int i = 0; i < MAX_WEAPON; i++)
	{
		if (g_Weapon[i].load)
		{
			UnloadModel(&g_Weapon[i].model);
			g_Weapon[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateWeapon(void)
{
	// 移動処理
	PLAYER* player = GetPlayer();

	for (int i = 0; i < MAX_WEAPON; i++)
	{
		g_Weapon[i].pos.x += (player->pos.x - g_Weapon[i].pos.x) * 0.05f;
		g_Weapon[i].pos.y += (player->pos.y - g_Weapon[i].pos.y) * 0.05f;
		g_Weapon[i].pos.z += (player->pos.z - g_Weapon[i].pos.z) * 0.05f;
	}
	

	// 弾発射処理
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
	}

	// 階層アニメーション
	for (int i = 0; i < WEAPON_1_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Weapon_1_Parts[i].use == TRUE) && (g_Weapon_1_Parts[i].tbl_adr != NULL))
		{
			// 移動処理
			int		index = (int)g_Weapon_1_Parts[i].move_time;
			float	time = g_Weapon_1_Parts[i].move_time - index;
			int		size = g_Weapon_1_Parts[i].tbl_size;

			float dt = 1.0f / g_Weapon_1_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Weapon_1_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Weapon_1_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Weapon_1_Parts[i].pos, p0 + vec * time);

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR r1 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 1].rot);	// 次の角度
			XMVECTOR r0 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 0].rot);	// 現在の角度
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Weapon_1_Parts[i].rot, r0 + rot * time);

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			XMVECTOR s1 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 1].scl);	// 次のScale
			XMVECTOR s0 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 0].scl);	// 現在のScale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Weapon_1_Parts[i].scl, s0 + scl * time);

		}
	}



	{	// ポイントライトのテスト
		LIGHT* light = GetLightData(1);
		XMFLOAT3 pos = g_Weapon[0].pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	



#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawWeapon(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Weapon[0].scl.x, g_Weapon[0].scl.y, g_Weapon[0].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon[0].rot.x, g_Weapon[0].rot.y + XM_PI, g_Weapon[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Weapon[0].pos.x, g_Weapon[0].pos.y, g_Weapon[0].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Weapon[0].mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Weapon[0].model);



	// パーツの階層アニメーション
	for (int i = 0; i < WEAPON_1_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Weapon_1_Parts[i].scl.x, g_Weapon_1_Parts[i].scl.y, g_Weapon_1_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon_1_Parts[i].rot.x, g_Weapon_1_Parts[i].rot.y, g_Weapon_1_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Weapon_1_Parts[i].pos.x, g_Weapon_1_Parts[i].pos.y, g_Weapon_1_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Weapon_1_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Weapon_1_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Weapon_1_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Weapon_1_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Weapon_1_Parts[i].model);

	}



	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
WEAPON* GetWeapon(void)
{
	return &g_Weapon[0];
}

