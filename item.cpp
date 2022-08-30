//=============================================================================
//
// アイテムモデル処理 [item.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "item.h"
#include "shadow.h"
#include "player.h"
#include "MathHelper.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ITEM_HP_HEAL					"data/MODEL/apple.obj"			// 読み込むモデル名
#define	MODEL_ITEM_HP_KILL					"data/MODEL/fishBones.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(1.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ITEM_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ITEM_OFFSET_Y		(0.0f)						// アイテムの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ITEM				g_Item[MAX_ITEM];				

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitItem(void)
{
	// hpHealのモデルを導入（0,1,2番は）
	for (int i = 0; i < MAX_ITEM_GOOD; i++)
	{
		LoadModel(MODEL_ITEM_HP_HEAL, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].type = hpHeal;
	}

	// hpKillのモデルを導入（3,4番は）
	for (int i = MAX_ITEM_GOOD; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM_HP_KILL, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].type = hpKill;
	}

	for (int i = 0; i < MAX_ITEM; i++)
	{
		
		g_Item[i].pos = XMFLOAT3(0.0f, ITEM_OFFSET_Y, 0.0f);
		g_Item[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Item[i].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

		g_Item[i].spd = VALUE_MOVE;			// 移動スピードクリア
		g_Item[i].size = ITEM_SIZE;			// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);

		XMFLOAT3 pos = g_Item[i].pos;
		pos.y -= (ITEM_OFFSET_Y - 0.1f);
		g_Item[i].shadowIdx = CreateShadow(pos, ITEM_SHADOW_SIZE, ITEM_SHADOW_SIZE);

		g_Item[i].move_time = 0.0f;
		g_Item[i].tbl_adr = NULL;
		g_Item[i].tbl_size = 0;

		// 階層アニメーション用の初期化処理
		g_Item[i].parent = NULL;

		g_Item[i].use = TRUE;

	}

	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].load)
		{
			UnloadModel(&g_Item[i].model);
			g_Item[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateItem(void)
{
	const XMFLOAT3 playerPos = GetPlayer()->pos;
	const float playerProgress = GetPlayerFieldProgress();

	bool boarderFlag = IsPlayerOutOfBoarder();

	if (boarderFlag)
	{
		for (auto& item : g_Item)
		{
			//item.type = static_cast<itemBehaviorType>(rand() % 3);
			item.use = true;
			item.pos = GetRandomValidPositionAtConjuction();
			item.pos.y += ITEM_OFFSET_Y;


			/*if (item.type == Obstacle)
			{

			}*/

			
		}
		return;
	}

	for (auto& item : g_Item)
	{
		if (item.use == TRUE)
		{
			/*if (item.type == Obstacle)
			{
				continue;
			}*/

			XMFLOAT3 pos = item.pos;
			pos.y -= (ITEM_OFFSET_Y - 0.1f);
			SetPositionShadow(item.shadowIdx, pos);

		}
	}



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawItem(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	//SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Item[i].scl.x, g_Item[i].scl.y, g_Item[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Item[i].rot.x, g_Item[i].rot.y + XM_PI, g_Item[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Item[i].pos.x, g_Item[i].pos.y, g_Item[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Item[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Item[i].model);

	}




	// カリング設定を戻す
	//SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ITEM* GetItem()
{
	return &g_Item[0];
}
