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

#include "MathHelper.h"
#include "meshfield.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_WEAPON_1						"data/MODEL/magicWand_body.obj"							// 読み込むモデル名 魔法の杖１
#define	MODEL_WEAPON_1_PARTS_CORE			"data/MODEL/magicWand_core.obj"							// 読み込むモデル名 魔法の杖１のパーツ
#define	MODEL_WEAPON_1_PARTS_MOVE			"data/MODEL/magicWand_move.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L1		"data/MODEL/magicWand_funnal_left1.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L2		"data/MODEL/magicWand_funnal_left2.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L3		"data/MODEL/magicWand_funnal_left3.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L4		"data/MODEL/magicWand_funnal_left4.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R1		"data/MODEL/magicWand_funnal_right1.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R2		"data/MODEL/magicWand_funnal_right2.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R3		"data/MODEL/magicWand_funnal_right3.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R4		"data/MODEL/magicWand_funnal_right4.obj"	

#define	VALUE_MOVE					(10.0f)															// 移動量
#define	VALUE_ROTATE				(XM_PI * 0.02f)													// 回転量


#define WEAPON_1_PARTS_MAX			(10)															// 魔法の杖1のパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static WEAPON		g_Weapon[MAX_WEAPON];							// 武器

static WEAPON		g_Weapon_1_Parts[WEAPON_1_PARTS_MAX];			// 武器１(魔法の杖１)のパーツ用

static BOOL			g_Load = FALSE;


// 武器の階層アニメーションデータ
static INTERPOLATION_DATA move_tbl_core[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.5f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_2PI, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, -0.5f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_2PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};


static INTERPOLATION_DATA move_tbl_funnal[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
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
		//g_Weapon_1_Parts[i].parent = &g_Weapon[0];		// ← ここに親のアドレスを入れる
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
	g_Weapon_1_Parts[0].parent = &g_Weapon[0];													// 親をセット
	g_Weapon_1_Parts[0].tbl_adr = move_tbl_core;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[0].tbl_size = sizeof(move_tbl_core) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[0].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_CORE, &g_Weapon_1_Parts[0].model);

	g_Weapon_1_Parts[1].use = TRUE;
	g_Weapon_1_Parts[1].parent = &g_Weapon[0];													// 親をセット
	g_Weapon_1_Parts[1].tbl_adr = move_tbl_move;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[1].tbl_size = sizeof(move_tbl_move) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[1].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_MOVE, &g_Weapon_1_Parts[1].model);

	g_Weapon_1_Parts[2].use = TRUE;
	g_Weapon_1_Parts[2].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[2].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[2].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[2].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L1, &g_Weapon_1_Parts[2].model);

	g_Weapon_1_Parts[3].use = TRUE;
	g_Weapon_1_Parts[3].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[3].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[3].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[3].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L2, &g_Weapon_1_Parts[3].model);

	g_Weapon_1_Parts[4].use = TRUE;
	g_Weapon_1_Parts[4].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[4].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[4].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[4].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L3, &g_Weapon_1_Parts[4].model);

	g_Weapon_1_Parts[5].use = TRUE;
	g_Weapon_1_Parts[5].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[5].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[5].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[5].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L4, &g_Weapon_1_Parts[5].model);

	g_Weapon_1_Parts[6].use = TRUE;
	g_Weapon_1_Parts[6].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[6].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[6].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[6].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R1, &g_Weapon_1_Parts[6].model);

	g_Weapon_1_Parts[7].use = TRUE;
	g_Weapon_1_Parts[7].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[7].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[7].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[7].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R2, &g_Weapon_1_Parts[7].model);

	g_Weapon_1_Parts[8].use = TRUE;
	g_Weapon_1_Parts[8].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[8].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[8].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[8].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R3, &g_Weapon_1_Parts[8].model);

	g_Weapon_1_Parts[9].use = TRUE;
	g_Weapon_1_Parts[9].parent = &g_Weapon_1_Parts[0];											// 親をセット
	g_Weapon_1_Parts[9].tbl_adr = move_tbl_funnal;												// 再生するアニメデータの先頭アドレスをセット
	g_Weapon_1_Parts[9].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Weapon_1_Parts[9].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R4, &g_Weapon_1_Parts[9].model);

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

	int boarderFlag = IsPlayerOutOfBoarder();

	for (auto& weapon : g_Weapon)
	{
		if (boarderFlag)
		{
			weapon.pos = GetWrapPosition(weapon.pos, boarderFlag);
		}
		auto target = XMLoadFloat3(&player->pos) + XMVECTOR{20.0f, 0.0f, -10.0f};
		XMVECTOR lerp = MathHelper::Lerp(XMLoadFloat3(&weapon.pos), target, 0.1f);
		weapon.rot = player->rot;
		XMStoreFloat3(&weapon.pos, lerp);
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
			g_Weapon_1_Parts[i].move_time += dt;						// アニメーションの合計時間に足す

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

	//{	// ポイントライトのテスト
	//	LIGHT* light = GetLightData(2);
	//	XMFLOAT3 pos = g_Weapon[0].pos;
	//	pos.y += 20.0f;

	//	light->Position = pos;
	//	light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Type = LIGHT_TYPE_POINT;
	//	light->Enable = TRUE;
	//	SetLightData(2, light);
	//}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("WeaponOffset:X:%f Y:%f Z:%f\n", g_Weapon[0].pos.x - player->pos.x, g_Weapon[0].pos.y - player->pos.y, g_Weapon[0].pos.z - player->pos.z);
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

