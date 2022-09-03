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

#define	MODEL_WEAPON_1						"data/MODEL/magicWand_body.obj"							  
#define	MODEL_WEAPON_1_PARTS_CORE			"data/MODEL/magicWand_core.obj"							  
#define	MODEL_WEAPON_1_PARTS_MOVE			"data/MODEL/magicWand_move.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L1		"data/MODEL/magicWand_funnal_left1.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L2		"data/MODEL/magicWand_funnal_left2.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L3		"data/MODEL/magicWand_funnal_left3.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L4		"data/MODEL/magicWand_funnal_left4.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R1		"data/MODEL/magicWand_funnal_right1.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R2		"data/MODEL/magicWand_funnal_right2.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R3		"data/MODEL/magicWand_funnal_right3.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R4		"data/MODEL/magicWand_funnal_right4.obj"

#define	VALUE_MOVE					(10.0f)															 
#define	VALUE_ROTATE				(XM_PI * 0.02f)													 

#define WEAPON_1_PARTS_MAX			(10)															 

static WEAPON		g_Weapon[MAX_WEAPON];							 

static WEAPON		g_Weapon_1_Parts[WEAPON_1_PARTS_MAX];			 

static BOOL			g_Load = FALSE;

static INTERPOLATION_DATA move_tbl_core[] = {	    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.5f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_2PI, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, -0.5f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_move[] = {	    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_2PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_funnal[] = {	    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

HRESULT InitWeapon(void)
{
	LoadModel(MODEL_WEAPON_1, &g_Weapon[0].model);
	g_Weapon[0].load = TRUE;

	const PLAYER* player = GetPlayer();

	for (auto& i : g_Weapon)
	{
		i.pos = player->pos;
		i.rot = { 0.0f, XM_PI, 0.0f };
		i.scl = { 1.0f, 1.0f, 1.0f };

		i.spd = 0.0f;			 
	}

	g_Weapon[0].use = TRUE;

	g_Weapon[0].parent = nullptr;			 

	for (auto& g_Weapon_1_Part : g_Weapon_1_Parts)
	{
		g_Weapon_1_Part.use = FALSE;

		g_Weapon_1_Part.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Weapon_1_Part.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Weapon_1_Part.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Weapon_1_Part.tbl_adr = nullptr;		 
		g_Weapon_1_Part.move_time = 0.0f;	 
		g_Weapon_1_Part.tbl_size = 0;		 

		g_Weapon_1_Part.load = FALSE;
	}

	g_Weapon_1_Parts[0].use = TRUE;
	g_Weapon_1_Parts[0].parent = &g_Weapon[0];													 
	g_Weapon_1_Parts[0].tbl_adr = move_tbl_core;												 
	g_Weapon_1_Parts[0].tbl_size = sizeof(move_tbl_core) / sizeof(INTERPOLATION_DATA);			 
	g_Weapon_1_Parts[0].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_CORE, &g_Weapon_1_Parts[0].model);

	g_Weapon_1_Parts[1].use = TRUE;
	g_Weapon_1_Parts[1].parent = &g_Weapon[0];													 
	g_Weapon_1_Parts[1].tbl_adr = move_tbl_move;												 
	g_Weapon_1_Parts[1].tbl_size = sizeof(move_tbl_move) / sizeof(INTERPOLATION_DATA);			 
	g_Weapon_1_Parts[1].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_MOVE, &g_Weapon_1_Parts[1].model);

	g_Weapon_1_Parts[2].use = TRUE;
	g_Weapon_1_Parts[2].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[2].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[2].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[2].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L1, &g_Weapon_1_Parts[2].model);

	g_Weapon_1_Parts[3].use = TRUE;
	g_Weapon_1_Parts[3].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[3].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[3].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[3].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L2, &g_Weapon_1_Parts[3].model);

	g_Weapon_1_Parts[4].use = TRUE;
	g_Weapon_1_Parts[4].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[4].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[4].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[4].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L3, &g_Weapon_1_Parts[4].model);

	g_Weapon_1_Parts[5].use = TRUE;
	g_Weapon_1_Parts[5].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[5].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[5].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[5].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L4, &g_Weapon_1_Parts[5].model);

	g_Weapon_1_Parts[6].use = TRUE;
	g_Weapon_1_Parts[6].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[6].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[6].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[6].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R1, &g_Weapon_1_Parts[6].model);

	g_Weapon_1_Parts[7].use = TRUE;
	g_Weapon_1_Parts[7].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[7].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[7].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[7].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R2, &g_Weapon_1_Parts[7].model);

	g_Weapon_1_Parts[8].use = TRUE;
	g_Weapon_1_Parts[8].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[8].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[8].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[8].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R3, &g_Weapon_1_Parts[8].model);

	g_Weapon_1_Parts[9].use = TRUE;
	g_Weapon_1_Parts[9].parent = &g_Weapon_1_Parts[0];											 
	g_Weapon_1_Parts[9].tbl_adr = move_tbl_funnal;												 
	g_Weapon_1_Parts[9].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		 
	g_Weapon_1_Parts[9].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R4, &g_Weapon_1_Parts[9].model);

	g_Load = TRUE;
	return S_OK;
}

void UninitWeapon(void)
{
	if (g_Load == FALSE) return;

	for (auto& i : g_Weapon)
	{
		if (i.load)
		{
			UnloadModel(&i.model);
			i.load = FALSE;
		}
	}

	g_Load = FALSE;
}

void UpdateWeapon(void)
{
	PLAYER* player = GetPlayer();

	int boarderFlag = IsPlayerOutOfBoarder();

	for (auto& weapon : g_Weapon)
	{
		if (boarderFlag)
		{
			weapon.pos = GetWrapPosition(weapon.pos, boarderFlag);
		}
		auto target = XMLoadFloat3(&player->pos) + XMVECTOR{ 20.0f, 0.0f, -10.0f };
		XMVECTOR lerp = MathHelper::Lerp(XMLoadFloat3(&weapon.pos), target, 0.1f);
		weapon.rot = player->rot;
		XMStoreFloat3(&weapon.pos, lerp);
	}

	if (GetKeyboardTrigger(DIK_SPACE))
	{
	}

	for (auto& g_Weapon_1_Part : g_Weapon_1_Parts)
	{
		if ((g_Weapon_1_Part.use == TRUE) && (g_Weapon_1_Part.tbl_adr != nullptr))
		{
			int		index = static_cast<int>(g_Weapon_1_Part.move_time);
			float	time = g_Weapon_1_Part.move_time - index;
			int		size = g_Weapon_1_Part.tbl_size;

			float dt = 1.0f / g_Weapon_1_Part.tbl_adr[index].frame;	 
			g_Weapon_1_Part.move_time += dt;						 

			if (index > (size - 2))	 
			{
				g_Weapon_1_Part.move_time = 0.0f;
				index = 0;
			}

			XMVECTOR p1 = XMLoadFloat3(&g_Weapon_1_Part.tbl_adr[index + 1].pos);	 
			XMVECTOR p0 = XMLoadFloat3(&g_Weapon_1_Part.tbl_adr[index + 0].pos);	 
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Weapon_1_Part.pos, p0 + vec * time);

			XMVECTOR r1 = XMLoadFloat3(&g_Weapon_1_Part.tbl_adr[index + 1].rot);	 
			XMVECTOR r0 = XMLoadFloat3(&g_Weapon_1_Part.tbl_adr[index + 0].rot);	 
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Weapon_1_Part.rot, r0 + rot * time);

			XMVECTOR s1 = XMLoadFloat3(&g_Weapon_1_Part.tbl_adr[index + 1].scl);	 
			XMVECTOR s0 = XMLoadFloat3(&g_Weapon_1_Part.tbl_adr[index + 0].scl);	 
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Weapon_1_Part.scl, s0 + scl * time);
		}
	}

	{	 
		LIGHT* light = GetLightData(0);

		XMFLOAT3 lightPos{ g_Weapon[0].pos };
		lightPos.y += 40.0f;

		light->Position = lightPos;
		light->Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Attenuation = 400.0f;
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(0, light);
	}

#ifdef _DEBUG	 
	PrintDebugProc("WeaponOffset:X:%f Y:%f Z:%f\n", g_Weapon[0].pos.x - player->pos.x, g_Weapon[0].pos.y - player->pos.y, g_Weapon[0].pos.z - player->pos.z);
#endif
}

void DrawWeapon(void)
{
	XMMATRIX mtxWorld = XMMatrixIdentity();

	XMMATRIX mtxScl = XMMatrixScaling(g_Weapon[0].scl.x, g_Weapon[0].scl.y, g_Weapon[0].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon[0].rot.x, g_Weapon[0].rot.y + XM_PI, g_Weapon[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMMATRIX mtxTranslate = XMMatrixTranslation(g_Weapon[0].pos.x, g_Weapon[0].pos.y, g_Weapon[0].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Weapon[0].mtxWorld, mtxWorld);

	DrawModel(&g_Weapon[0].model);

	for (auto& g_Weapon_1_Part : g_Weapon_1_Parts)
	{
		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(g_Weapon_1_Part.scl.x, g_Weapon_1_Part.scl.y, g_Weapon_1_Part.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon_1_Part.rot.x, g_Weapon_1_Part.rot.y, g_Weapon_1_Part.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(g_Weapon_1_Part.pos.x, g_Weapon_1_Part.pos.y, g_Weapon_1_Part.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Weapon_1_Part.parent != nullptr)	 
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Weapon_1_Part.parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Weapon_1_Part.mtxWorld, mtxWorld);

		if (g_Weapon_1_Part.use == FALSE) continue;

		SetWorldMatrix(&mtxWorld);

		DrawModel(&g_Weapon_1_Part.model);
	}
}

void DrawWeaponToDepthTex(void)
{
	XMMATRIX mtxWorld = XMMatrixIdentity();

	XMMATRIX mtxScl = XMMatrixScaling(g_Weapon[0].scl.x, g_Weapon[0].scl.y, g_Weapon[0].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon[0].rot.x, g_Weapon[0].rot.y + XM_PI, g_Weapon[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMMATRIX mtxTranslate = XMMatrixTranslation(g_Weapon[0].pos.x, g_Weapon[0].pos.y, g_Weapon[0].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Weapon[0].mtxWorld, mtxWorld);

	DrawModelWithoutMat(&g_Weapon[0].model);

	for (auto& g_Weapon_1_Part : g_Weapon_1_Parts)
	{
		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(g_Weapon_1_Part.scl.x, g_Weapon_1_Part.scl.y, g_Weapon_1_Part.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon_1_Part.rot.x, g_Weapon_1_Part.rot.y, g_Weapon_1_Part.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(g_Weapon_1_Part.pos.x, g_Weapon_1_Part.pos.y, g_Weapon_1_Part.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Weapon_1_Part.parent != nullptr)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Weapon_1_Part.parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Weapon_1_Part.mtxWorld, mtxWorld);

		if (g_Weapon_1_Part.use == FALSE) continue;

		SetWorldMatrix(&mtxWorld);

		DrawModelWithoutMat(&g_Weapon_1_Part.model);
	}
}

WEAPON* GetWeapon(void)
{
	return &g_Weapon[0];
}