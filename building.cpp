//=============================================================================
//
// 建物モデル処理 [building.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "building.h"

#include "light.h"
#include "shadow.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BUILDING_LIGHTPOSTSINGLE			"data/MODEL/lightpostSingle.obj"			// 読み込むモデル名
#define MODEL_BUILDING_LIGHTPOSTDOUBLE			"data/MODEL/lightpostDouble.obj"

#define MODEL_BUILDING_HANGAR_LARGEA			"data/MODEL/hangar_largeA.obj"
#define MODEL_BUILDING_HANGAR_LARGEB			"data/MODEL/hangar_largeB.obj"
#define MODEL_BUILDING_HANGAR_SMALLA			"data/MODEL/hangar_smallA.obj"
#define MODEL_BUILDING_HANGAR_SMALLB			"data/MODEL/hangar_smallB.obj"
#define MODEL_BUILDING_HANGAR_ROUNDA			"data/MODEL/hangar_roundA.obj"
#define MODEL_BUILDING_HANGAR_ROUNDB			"data/MODEL/hangar_roundB.obj"

#define MODEL_BUILDING_CRAFT_MINER				"data/MODEL/craft_miner.obj"

#define MODEL_BUILDING_TURRET_SINGLE			"data/MODEL/turret_single.obj"
#define MODEL_BUILDING_TURRET_DOUBLE			"data/MODEL/turret_double.obj"

#define MODEL_BUILDING_STRUCTURE				"data/MODEL/structure.obj"
#define MODEL_BUILDING_STRUCTURE_CLOSED			"data/MODEL/structure_closerd.obj"
#define MODEL_BUILDING_STRUCTURE_DETAILED		"data/MODEL/structure_detailed.obj"
#define MODEL_BUILDING_STRUCTURE_DIAGONAL		"data/MODEL/structure_diagonal.obj"

#define	VALUE_ROTATE			(XM_PI * 0.02f)				// 回転量

#define BUILDING_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define BUILDING_OFFSET_Y		(0.0f)						// BUILDINGの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
enum BuildingType : int
{
	
};

void GetYOffset(XMFLOAT3& pos)
{
	pos.y = GetFieldHeight(pos.x, pos.z);
}

void SetStreetLight(const BUILDING& building, int lightIdx)
{
	LIGHT* light = GetLightData(lightIdx);
	XMFLOAT3 lightPos{ building.pos };
	lightPos.y += 20.0f;

	light->Position    = lightPos;
	light->Diffuse = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
	//light->Ambient     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light->Attenuation = 200.0f;
	light->Type        = LIGHT_TYPE_POINT;
	light->Enable      = TRUE;
	SetLightData(lightIdx, light);
}

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BUILDING			g_Building[MAX_BUILDING];		

static BOOL				g_Load = FALSE;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBuilding(void)
{

	for (int i = 0; i < MAX_BUILDING; i++)
	{
		g_Building[i].load = FALSE;

		g_Building[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Building[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Building[i].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);


		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Building[0].model, &g_Building[0].diffuse[0]);

		XMFLOAT3 pos = g_Building[i].pos;
		pos.y -= (BUILDING_OFFSET_Y - 0.1f);
		g_Building[i].shadowIdx = CreateShadow(pos, BUILDING_SHADOW_SIZE, BUILDING_SHADOW_SIZE);

		g_Building[i].move_time = 0.0f;

		g_Building[i].tbl_adr = NULL;
		g_Building[i].tbl_size = 0;

		g_Building[i].use = TRUE;

	}

	// lightspot
	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[0].model);
	g_Building[0].load = TRUE;
	g_Building[0].pos = XMFLOAT3(40.0f, 0.0f, 300.0f);
	GetYOffset(g_Building[0].pos);
	g_Building[0].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[0].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[0], 2);

	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[1].model);
	g_Building[1].load = TRUE;
	g_Building[1].pos = XMFLOAT3(-40.0f, 0.0f, -300.0f);
	GetYOffset(g_Building[1].pos);
	g_Building[1].rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);
	g_Building[1].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[1], 3);

	LoadModel(MODEL_BUILDING_LIGHTPOSTDOUBLE, &g_Building[2].model);
	g_Building[2].load = TRUE;
	g_Building[2].pos = XMFLOAT3(300.0f, 0.0f, -40.0f);
	GetYOffset(g_Building[2].pos);
	g_Building[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[2].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[2], 4);

	LoadModel(MODEL_BUILDING_HANGAR_LARGEB, &g_Building[3].model);
	g_Building[3].load = TRUE;
	g_Building[3].pos = XMFLOAT3(300.0f, 0.0f, -300.0f);
	GetYOffset(g_Building[3].pos);
	g_Building[3].rot = XMFLOAT3(0.0f, -XM_PIDIV4, 0.0f);
	g_Building[3].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_HANGAR_LARGEB, &g_Building[5].model);
	g_Building[5].load = TRUE;
	g_Building[5].pos = XMFLOAT3(-300.0f, 0.0f, 300.0f);
	GetYOffset(g_Building[5].pos);
	g_Building[5].rot = XMFLOAT3(0.0f, -XM_PIDIV4, 0.0f);
	g_Building[5].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_HANGAR_ROUNDA, &g_Building[4].model);
	g_Building[4].load = TRUE;
	g_Building[4].pos = XMFLOAT3(200.0f, 0.0f, -400.0f);
	GetYOffset(g_Building[4].pos);
	g_Building[4].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Building[4].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_HANGAR_ROUNDB, &g_Building[5].model);
	g_Building[5].load = TRUE;
	g_Building[5].pos = XMFLOAT3(-200.0f, 0.0f, -300.0f);
	GetYOffset(g_Building[5].pos);
	g_Building[5].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Building[5].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_HANGAR_LARGEA, &g_Building[6].model);
	g_Building[6].load = TRUE;
	g_Building[6].pos = XMFLOAT3(-400.0f, 0.0f, -500.0f);
	GetYOffset(g_Building[6].pos);
	g_Building[6].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Building[6].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);


	LoadModel(MODEL_BUILDING_STRUCTURE, &g_Building[7].model);
	g_Building[7].load = TRUE;
	g_Building[7].pos = XMFLOAT3(-100.0f, 0.0f, -100.0f);
	GetYOffset(g_Building[7].pos);
	g_Building[7].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[7].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	
	LoadModel(MODEL_BUILDING_TURRET_SINGLE, &g_Building[8].model);
	g_Building[8].load = TRUE;
	g_Building[8].pos = XMFLOAT3(-200.0f, 0.0f, -150.0f);
	GetYOffset(g_Building[8].pos);
	g_Building[8].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Building[8].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_HANGAR_SMALLA, &g_Building[9].model);
	g_Building[9].load = TRUE;
	g_Building[9].pos = XMFLOAT3(200.0f, 0.0f, 150.0f);
	GetYOffset(g_Building[9].pos);
	g_Building[9].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[9].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	
	LoadModel(MODEL_BUILDING_STRUCTURE_DIAGONAL, &g_Building[10].model);
	g_Building[10].load = TRUE;
	g_Building[10].pos = XMFLOAT3(100.0f, 0.0f, 400.0f);
	GetYOffset(g_Building[10].pos);
	g_Building[10].rot = XMFLOAT3(0.0f, XM_PIDIV4, 0.0f);
	g_Building[10].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_HANGAR_SMALLB, &g_Building[11].model);
	g_Building[11].load = TRUE;
	g_Building[11].pos = XMFLOAT3(-100.0f, 0.0f, 300.0f);
	GetYOffset(g_Building[11].pos);
	g_Building[11].rot = XMFLOAT3(0.0f, -XM_PIDIV4, 0.0f);
	g_Building[11].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_STRUCTURE, &g_Building[12].model);
	g_Building[12].load = TRUE;
	g_Building[12].pos = XMFLOAT3(-150.0f, 0.0f, 150.0f);
	GetYOffset(g_Building[12].pos);
	g_Building[12].rot = XMFLOAT3(0.0f, XM_PIDIV4, 0.0f);
	g_Building[12].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

	LoadModel(MODEL_BUILDING_STRUCTURE, &g_Building[13].model);
	g_Building[13].load = TRUE;
	g_Building[13].pos = XMFLOAT3(-400.0f, 0.0f, 400.0f);
	GetYOffset(g_Building[13].pos);
	g_Building[13].rot = XMFLOAT3(0.0f, XM_PIDIV4, 0.0f);
	g_Building[13].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);
	

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBuilding(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_BUILDING; i++)
	{
		if (g_Building[i].load)
		{
			UnloadModel(&g_Building[i].model);
			g_Building[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBuilding(void)
{

	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBuilding(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	//SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_BUILDING; i++)
	{
		if (g_Building[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Building[i].scl.x, g_Building[i].scl.y, g_Building[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Building[i].rot.x, g_Building[i].rot.y + XM_PI, g_Building[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Building[i].pos.x, g_Building[i].pos.y, g_Building[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Building[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Building[i].model);


	}




	// カリング設定を戻す
	//SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
BUILDING* GetBuilding()
{
	return &g_Building[0];
}
