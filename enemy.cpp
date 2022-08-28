//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "player.h"
#include "MathHelper.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/enemy.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(1.0f)						// 移動量
#define	VALUE_AMP			(50.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(7.0f)						// エネミーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum EnemyBehaviorType : int
{
	Obstacle = 0,
	GoalKeeper = 1,
	Flyable = 2,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

static BOOL				g_Load = FALSE;


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = VALUE_MOVE;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	
		g_Enemy[i].tbl_adr = NULL;		
		g_Enemy[i].tbl_size = 0;		

		g_Enemy[i].type = Flyable;
		g_Enemy[i].use = TRUE;			
		
	}


	// 0番だけ線形補間で動かしてみる
	g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Enemy[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
	g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	const XMFLOAT3 playerPos = GetPlayer()->pos;
	const float playerProgress = GetPlayerFieldProgress();

	bool boarderFlag = IsPlayerOutOfBoarder();

	if (boarderFlag)
	{
		for (auto & enemy : g_Enemy)
		{
			enemy.type = static_cast<EnemyBehaviorType>(rand() % 3);
			enemy.use = true;
			enemy.pos = GetRandomValidPosition();
			enemy.pos.y += ENEMY_OFFSET_Y;

			if (enemy.type == Obstacle)
			{

			}

			if (enemy.type == GoalKeeper)
			{
				enemy.pos = GetRandomValidPosition();
				enemy.pos.y += ENEMY_OFFSET_Y;
				float vx = VALUE_MOVE * (float)rand() / RAND_MAX;
				float vz = sqrtf(VALUE_MOVE * VALUE_MOVE - vx * vx);
				enemy.velocity = { vx,0.0f,vz };
			}

			if (enemy.type == Flyable)
			{
				enemy.pos = GetRandomPosition();
				float amp = VALUE_AMP * (float)rand() / RAND_MAX;
				float phase = XM_2PI * (float)rand() / RAND_MAX;
				enemy.velocity = { phase, amp, phase };
			}
		}
		return;
	}

	for (auto& enemy : g_Enemy)
	{
		if (enemy.use == TRUE)
		{
			if (enemy.type == Obstacle)
			{
				continue;
			}

			if (enemy.type == GoalKeeper)
			{
				const XMVECTOR target = XMLoadFloat3(&enemy.pos) + XMLoadFloat3(&enemy.velocity);
				const float& tarX = target.m128_f32[0];
				const float& tarZ = target.m128_f32[2];

				if (IsPositionValid(tarX, tarZ))
				{
					XMStoreFloat3(&enemy.pos, target);
				}
				else
				{
					XMStoreFloat3(&enemy.velocity, -XMLoadFloat3(&enemy.velocity));
				}
			}

			if (enemy.type == Flyable)
			{
				const float amp = enemy.velocity.y;
				const float phase = enemy.velocity.x;
				enemy.pos.y = 30.0f + amp * sinf(playerProgress * XM_PI * 15.0f + phase);
			}

			{
				XMFLOAT3 enemyDir = playerPos - enemy.pos;
				XMVECTOR dirVec = XMLoadFloat3(&enemyDir);
				dirVec = XMVector3Normalize(dirVec);
				const auto& x = dirVec.m128_f32[0];
				const auto& z = dirVec.m128_f32[2];
				enemy.rot = { 0.0f, atan2f(x, z) + XM_PI,0.0f };
			}

			XMFLOAT3 pos = enemy.pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(enemy.shadowIdx, pos);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}
