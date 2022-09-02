#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "building.h"

#include "light.h"
#include "meshfield.h"

#define	MODEL_BUILDING_LIGHTPOSTSINGLE			"data/MODEL/lightpostSingle.obj"			 
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

#define	VALUE_ROTATE			(XM_PI * 0.02f)				 

#define BUILDING_SHADOW_SIZE	(0.4f)						 
#define BUILDING_OFFSET_Y		(0.0f)						 

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

	light->Position = lightPos;
	light->Diffuse = { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f };
	light->Attenuation = 750.0f;
	light->Type = LIGHT_TYPE_POINT;
	light->Enable = TRUE;
	SetLightData(lightIdx, light);
}

static BUILDING			g_Building[MAX_BUILDING];

static BOOL				g_Load = FALSE;

HRESULT InitBuilding(void)
{
	for (auto& i : g_Building)
	{
		i.load = FALSE;

		i.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		i.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		i.scl = XMFLOAT3(0.0f, 0.0f, 0.0f);

		GetModelDiffuse(&g_Building[0].model, &g_Building[0].diffuse[0]);

		XMFLOAT3 pos = i.pos;
		pos.y -= (BUILDING_OFFSET_Y - 0.1f);

		i.move_time = 0.0f;

		i.tbl_adr = nullptr;
		i.tbl_size = 0;

		i.use = TRUE;
	}

	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[0].model);
	g_Building[0].load = TRUE;
	g_Building[0].pos = XMFLOAT3(40.0f, 0.0f, 300.0f);
	GetYOffset(g_Building[0].pos);
	g_Building[0].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[0].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[0], 1);

	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[1].model);
	g_Building[1].load = TRUE;
	g_Building[1].pos = XMFLOAT3(-40.0f, 0.0f, -300.0f);
	GetYOffset(g_Building[1].pos);
	g_Building[1].rot = XMFLOAT3(0.0f, -XM_PIDIV2, 0.0f);
	g_Building[1].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[1], 2);

	LoadModel(MODEL_BUILDING_LIGHTPOSTDOUBLE, &g_Building[2].model);
	g_Building[2].load = TRUE;
	g_Building[2].pos = XMFLOAT3(300.0f, 0.0f, -40.0f);
	GetYOffset(g_Building[2].pos);
	g_Building[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[2].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[2], 3);

	LoadModel(MODEL_BUILDING_LIGHTPOSTDOUBLE, &g_Building[14].model);
	g_Building[14].load = TRUE;
	g_Building[14].pos = XMFLOAT3(-300.0f, 0.0f, 40.0f);
	GetYOffset(g_Building[2].pos);
	g_Building[14].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	g_Building[14].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);
	SetStreetLight(g_Building[14], 4);

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

void UninitBuilding(void)
{
	if (g_Load == FALSE) return;

	for (auto& i : g_Building)
	{
		if (i.load)
		{
			UnloadModel(&i.model);
			i.load = FALSE;
		}
	}
	g_Load = FALSE;
}

void UpdateBuilding(void)
{
}

void DrawBuilding(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (auto& i : g_Building)
	{
		if (i.use == FALSE) continue;

		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(i.scl.x, i.scl.y, i.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(i.rot.x, i.rot.y + XM_PI, i.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(i.pos.x, i.pos.y, i.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&i.mtxWorld, mtxWorld);

		DrawModel(&i.model);
	}

}

BUILDING* GetBuilding()
{
	return &g_Building[0];
}