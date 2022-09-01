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
#include "billboard.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY						"data/MODEL/enemy_body.obj"			// 読み込むモデル名

#define	MODEL_ENEMY_PARTS_WINGS_LEFT	"data/MODEL/enemy_wings_left.obj"	// 読み込むモデル名	エネミーのパーツ		
#define	MODEL_ENEMY_PARTS_WINGS_RIGHT	"data/MODEL/enemy_wings_right.obj"
#define	MODEL_ENEMY_PARTS_EYE_LEFT		"data/MODEL/enemy_eye_left.obj"
#define	MODEL_ENEMY_PARTS_EYE_RIGHT		"data/MODEL/enemy_eye_right.obj"

#define ENEMY_PARTS_MAX					(4)									// エネミーのパーツの数


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

static ENEMY			g_Enemy_Parts[MAX_ENEMY][ENEMY_PARTS_MAX];


// エネミーの階層アニメーションデータ

static INTERPOLATION_DATA move_tbl_wings[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,		0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y + 3.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y ,		0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y - 3.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,		0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
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

		// 階層アニメーション用の初期化処理
		g_Enemy[i].parent = NULL;

		g_Enemy[i].type = Flyable;
		g_Enemy[i].use = TRUE;

		g_Enemy[i].codes.clear();
		const int len = rand() % 5 + 1;
		for (int i = 0; i < len; ++i)
		{
			g_Enemy[i].codes.push_back(rand() % 4);
		}
		g_Enemy[i].compare_index = 0;

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Enemy_Parts[i][j].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Enemy_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Enemy_Parts[i][j].parent = &g_Enemy[i];		// ← ここに親のアドレスを入れる
			//g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
			//g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

			// 階層アニメーション用のメンバー変数の初期化
			g_Enemy_Parts[i][j].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
			g_Enemy_Parts[i][j].move_time = 0.0f;		// 実行時間をクリア
			g_Enemy_Parts[i][j].tbl_size = 0;			// 再生するアニメデータのレコード数をセット

			// パーツの読み込みはまだしていない
			g_Enemy_Parts[i][j].load = FALSE;
		}
	}



	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy_Parts[i][0].use = TRUE;
		g_Enemy_Parts[i][0].parent = &g_Enemy[i];													// 親(enemy_body)をセット
		g_Enemy_Parts[i][0].tbl_adr = move_tbl_wings;												// 再生するアニメデータの先頭アドレスをセット
		g_Enemy_Parts[i][0].tbl_size = sizeof(move_tbl_wings) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Enemy_Parts[i][0].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_WINGS_LEFT, &g_Enemy_Parts[i][0].model);

		g_Enemy_Parts[i][1].use = TRUE;
		g_Enemy_Parts[i][1].parent = &g_Enemy[i];													// 親(enemy_body)をセット
		g_Enemy_Parts[i][1].tbl_adr = move_tbl_wings;												// 再生するアニメデータの先頭アドレスをセット
		g_Enemy_Parts[i][1].tbl_size = sizeof(move_tbl_wings) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Enemy_Parts[i][1].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_WINGS_RIGHT, &g_Enemy_Parts[i][1].model);

		g_Enemy_Parts[i][2].use = TRUE;
		g_Enemy_Parts[i][2].parent = &g_Enemy[i];													// 親(enemy_body)をセット
		//g_Enemy_Parts[i][2].tbl_adr = move_tbl_wings;												// 再生するアニメデータの先頭アドレスをセット
		//g_Enemy_Parts[i][2].tbl_size = sizeof(move_tbl_wings) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Enemy_Parts[i][2].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_EYE_LEFT, &g_Enemy_Parts[i][2].model);

		g_Enemy_Parts[i][3].use = TRUE;
		g_Enemy_Parts[i][3].parent = &g_Enemy[i];													// 親(enemy_body)をセット
		//g_Enemy_Parts[i][3].tbl_adr = move_tbl_wings;												// 再生するアニメデータの先頭アドレスをセット
		//g_Enemy_Parts[i][3].tbl_size = sizeof(move_tbl_wings) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Enemy_Parts[i][3].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_EYE_RIGHT, &g_Enemy_Parts[i][3].model);
	}



	InitBillboard();

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

	ShutdownBillboard();

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
			//enemy.type = static_cast<EnemyBehaviorType>(rand() % 3);
			enemy.use = true;
			enemy.pos = GetRandomValidPosition();
			enemy.pos.y += ENEMY_OFFSET_Y;

			enemy.codes.clear();
			const int len = rand() % 5 + 1;
			for (int i = 0; i < len; ++i)
			{
				enemy.codes.push_back(rand() % 4);
			}
			enemy.compare_index = 0;

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
				float amp = VALUE_AMP * MathHelper::RandF();
				float phase = XM_2PI * MathHelper::RandF();
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


	// パーツの階層アニメーション
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			if ((g_Enemy_Parts[i][j].use == TRUE) && (g_Enemy_Parts[i][j].tbl_adr != NULL))
			{
				int		index = (int)g_Enemy_Parts[i][j].move_time;
				float	time = g_Enemy_Parts[i][j].move_time - index;
				int		size = g_Enemy_Parts[i][j].tbl_size;

				float dt = 1.0f / g_Enemy_Parts[i][j].tbl_adr[index].frame;	// 1フレームで進める時間
				g_Enemy_Parts[i][j].move_time += dt;					// アニメーションの合計時間に足す

				if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
				{
					g_Enemy_Parts[i][j].move_time = 0.0f;
					index = 0;
				}

				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				XMVECTOR p1 = XMLoadFloat3(&g_Enemy_Parts[i][j].tbl_adr[index + 1].pos);	// 次の場所
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy_Parts[i][j].tbl_adr[index + 0].pos);	// 現在の場所
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy_Parts[i][j].pos, p0 + vec * time);

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				XMVECTOR r1 = XMLoadFloat3(&g_Enemy_Parts[i][j].tbl_adr[index + 1].rot);	// 次の角度
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy_Parts[i][j].tbl_adr[index + 0].rot);	// 現在の角度
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy_Parts[i][j].rot, r0 + rot * time);

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				XMVECTOR s1 = XMLoadFloat3(&g_Enemy_Parts[i][j].tbl_adr[index + 1].scl);	// 次のScale
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy_Parts[i][j].tbl_adr[index + 0].scl);	// 現在のScale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy_Parts[i][j].scl, s0 + scl * time);

			}
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
	//SetCullingMode(CULL_MODE_NONE);

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

		// パーツの階層アニメーション
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Enemy_Parts[i][j].scl.x, g_Enemy_Parts[i][j].scl.y, g_Enemy_Parts[i][j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy_Parts[i][j].rot.x, g_Enemy_Parts[i][j].rot.y, g_Enemy_Parts[i][j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Enemy_Parts[i][j].pos.x, g_Enemy_Parts[i][j].pos.y, g_Enemy_Parts[i][j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Enemy_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Enemy_Parts[i][j].parent->mtxWorld));
				// ↑
				// g_Player.mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Enemy_Parts[i][j].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Enemy_Parts[i][j].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Enemy_Parts[i][j].model);

		}
	}

	


	// カリング設定を戻す
	//SetCullingMode(CULL_MODE_BACK);

	for (auto & enemy : g_Enemy)
	{
		if (enemy.use == FALSE) continue;

		auto vPos = XMLoadFloat3(&enemy.pos);
		const float dis = XMVectorGetX(XMVector3Length(XMLoadFloat3(&GetPlayer()->pos) - vPos));
		const float tScl = MathHelper::Clamp((dis - 20.0f) / 980.0f, 0.0f, 1.0f);
		const float fScl = MathHelper::Lerp(1.0f, 5.0f, tScl);

		vPos.m128_f32[1] += enemy.size + 5.0f;
		const float angle = enemy.rot.y + XM_PIDIV2;
		const float xIncrease = sinf(angle) * Billboard_WIDTH * fScl;
		const float zIncrease = cosf(angle) * Billboard_WIDTH * fScl;
		const XMVECTOR increase =
		{
			xIncrease,
			0.0f,
			zIncrease,
		};
		vPos -= static_cast<float>(enemy.codes.size() - 1) * 0.5f * increase;

		int cmdIdx = 0;
		XMFLOAT3 pos{};
		XMFLOAT3 scl{ fScl, fScl, fScl };

		for (; cmdIdx < enemy.compare_index; ++cmdIdx)
		{
			XMStoreFloat3(&pos, vPos);
			DrawBillboard(static_cast<CommandCode>(enemy.codes[cmdIdx]), pos, scl, true);
			vPos += increase;
		}
		for (; cmdIdx < enemy.codes.size(); ++cmdIdx)
		{
			XMStoreFloat3(&pos, vPos);
			DrawBillboard(static_cast<CommandCode>(enemy.codes[cmdIdx]), pos, scl, false);
			vPos += increase;
		}
	}
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}
