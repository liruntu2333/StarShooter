#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "item.h"
#include "player.h"
#include "MathHelper.h"
#include "meshfield.h"

#define	MODEL_ITEM_HP_HEAL					"data/MODEL/apple.obj"			 
#define	MODEL_ITEM_HP_KILL					"data/MODEL/fishBones.obj"		 

#define	VALUE_MOVE			(1.0f)						 
#define	VALUE_ROTATE		(XM_PI * 0.02f)				 

#define ITEM_SHADOW_SIZE	(0.4f)						 
#define ITEM_OFFSET_Y		(0.0f)						 

static ITEM				g_Item[MAX_ITEM];

static BOOL				g_Load = FALSE;

HRESULT InitItem(void)
{
	for (int i = 0; i < MAX_ITEM_GOOD; i++)
	{
		LoadModel(MODEL_ITEM_HP_HEAL, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].type = hpHeal;
	}

	for (int i = MAX_ITEM_GOOD; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM_HP_KILL, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].type = hpKill;
	}

	for (auto& i : g_Item)
	{
		i.pos = XMFLOAT3(0.0f, ITEM_OFFSET_Y, 0.0f);
		i.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		i.scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

		i.spd = VALUE_MOVE;			 
		i.size = ITEM_SIZE;			 

		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);

		XMFLOAT3 pos = i.pos;
		pos.y -= (ITEM_OFFSET_Y - 0.1f);

		i.move_time = 0.0f;
		i.tbl_adr = nullptr;
		i.tbl_size = 0;

		i.parent = nullptr;

		i.use = TRUE;
	}

	g_Load = TRUE;
	return S_OK;
}

void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (auto& i : g_Item)
	{
		if (i.load)
		{
			UnloadModel(&i.model);
			i.load = FALSE;
		}
	}

	g_Load = FALSE;
}

void UpdateItem(void)
{
	const XMFLOAT3 playerPos = GetPlayer()->pos;
	const float playerProgress = GetPlayerFieldProgress();

	const bool boarderFlag = IsPlayerOutOfBoarder();

	if (boarderFlag)
	{
		for (auto& item : g_Item)
		{
			item.use = true;
			item.pos = GetRandomValidPosition();
			item.pos.y += ITEM_OFFSET_Y;

		}
		return;
	}

	for (const auto& item : g_Item)
	{
		if (item.use == TRUE)
		{
			XMFLOAT3 pos = item.pos;
			pos.y -= (ITEM_OFFSET_Y - 0.1f);
		}
	}
}

void DrawItem(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (auto& i : g_Item)
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

ITEM* GetItem()
{
	return &g_Item[0];
}