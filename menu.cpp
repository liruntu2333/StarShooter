#include "main.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "light.h"
#include "bullet.h"
#include "player.h"
#include "menu.h"

#define	MODEL_MENU					"data/MODEL/menu.obj"			  
#define	MODEL_MENU_PARTS			"data/MODEL/torus.obj"			  

#define	VALUE_MOVE					(10.0f)							 

#define	VALUE_ROTATE				(XM_PI * 0.02f)					 

#define MENU_PARTS_MAX			(2)									 

static MENU			g_Menu[MAX_MENU];							 

static MENU			g_Menu_Parts[MENU_PARTS_MAX];				 

static BOOL			g_Load = FALSE;

static INTERPOLATION_DATA move_tbl_right[] = {	    
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_left[] = {	    
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

HRESULT InitMenu(void)
{
	LoadModel(MODEL_MENU, &g_Menu[0].model);
	g_Menu[0].load = TRUE;

	const PLAYER* player = GetPlayer();

	for (auto& i : g_Menu)
	{
		i.pos = player->pos;
		i.rot = { 0.0f, XM_PI, 0.0f };
		i.scl = { 5.0f, 5.0f, 1.0f };

		i.spd = 0.0f;			 
		i.isDisplay = FALSE;	 
	}

	g_Menu[0].use = TRUE;

	g_Menu[0].parent = nullptr;			 

	g_Load = TRUE;
	return S_OK;
}

void UninitMenu(void)
{
	if (g_Load == FALSE) return;

	for (auto& i : g_Menu)
	{
		if (i.load)
		{
			UnloadModel(&i.model);
			i.load = FALSE;
		}
	}

	g_Load = FALSE;
}

void UpdateMenu(void)
{
	PLAYER* player = GetPlayer();

	for (auto& i : g_Menu)
	{
		i.pos.x += (player->pos.x + 0.0f - i.pos.x) * 0.05f;
		i.pos.y += (player->pos.y + 70.0f - i.pos.y) * 0.05f;
		i.pos.z += (player->pos.z + 50.0f - i.pos.z) * 0.05f;
	}

	if (GetKeyboardTrigger(DIK_H))
	{
		if (g_Menu[0].isDisplay == FALSE)
		{
			g_Menu[0].isDisplay = TRUE;
		}
		else
		{
			g_Menu[0].isDisplay = FALSE;
		}
	}

	if (GetKeyboardTrigger(DIK_SPACE))
	{
	}

	for (auto& g_Menu_Part : g_Menu_Parts)
	{
		if ((g_Menu_Part.use == TRUE) && (g_Menu_Part.tbl_adr != nullptr))
		{
			int		index = static_cast<int>(g_Menu_Part.move_time);
			float	time = g_Menu_Part.move_time - index;
			int		size = g_Menu_Part.tbl_size;

			float dt = 1.0f / g_Menu_Part.tbl_adr[index].frame;	 
			g_Menu_Part.move_time += dt;					 

			if (index > (size - 2))	 
			{
				g_Menu_Part.move_time = 0.0f;
				index = 0;
			}

			XMVECTOR p1 = XMLoadFloat3(&g_Menu_Part.tbl_adr[index + 1].pos);	 
			XMVECTOR p0 = XMLoadFloat3(&g_Menu_Part.tbl_adr[index + 0].pos);	 
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Menu_Part.pos, p0 + vec * time);

			XMVECTOR r1 = XMLoadFloat3(&g_Menu_Part.tbl_adr[index + 1].rot);	 
			XMVECTOR r0 = XMLoadFloat3(&g_Menu_Part.tbl_adr[index + 0].rot);	 
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Menu_Part.rot, r0 + rot * time);

			XMVECTOR s1 = XMLoadFloat3(&g_Menu_Part.tbl_adr[index + 1].scl);	 
			XMVECTOR s0 = XMLoadFloat3(&g_Menu_Part.tbl_adr[index + 0].scl);	 
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Menu_Part.scl, s0 + scl * time);
		}
	}

#ifdef _DEBUG	 
#endif
}

void DrawMenu(void)
{
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	mtxWorld = XMMatrixIdentity();

	mtxScl = XMMatrixScaling(g_Menu[0].scl.x, g_Menu[0].scl.y, g_Menu[0].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	mtxRot = XMMatrixRotationRollPitchYaw(g_Menu[0].rot.x, g_Menu[0].rot.y + XM_PI, g_Menu[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	mtxTranslate = XMMatrixTranslation(g_Menu[0].pos.x, g_Menu[0].pos.y, g_Menu[0].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Menu[0].mtxWorld, mtxWorld);

	if (g_Menu[0].isDisplay == TRUE)
	{
		DrawModel(&g_Menu[0].model);

		for (auto& g_Menu_Part : g_Menu_Parts)
		{
			mtxWorld = XMMatrixIdentity();

			mtxScl = XMMatrixScaling(g_Menu_Part.scl.x, g_Menu_Part.scl.y, g_Menu_Part.scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			mtxRot = XMMatrixRotationRollPitchYaw(g_Menu_Part.rot.x, g_Menu_Part.rot.y, g_Menu_Part.rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			mtxTranslate = XMMatrixTranslation(g_Menu_Part.pos.x, g_Menu_Part.pos.y, g_Menu_Part.pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Menu_Part.parent != nullptr)	 
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Menu_Part.parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Menu_Part.mtxWorld, mtxWorld);

			if (g_Menu_Part.use == FALSE) continue;

			SetWorldMatrix(&mtxWorld);

			DrawModel(&g_Menu_Part.model);
		}
	}

	SetCullingMode(CULL_MODE_BACK);
}

MENU* GetMenu(void)
{
	return &g_Menu[0];
}