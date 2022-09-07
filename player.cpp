#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"

#include <queue>
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "enemy.h"
#include "weapon.h"
#include "billboard.h"
#include "game.h"

#define	MODEL_PLAYER_PARTS_BODY			"data/MODEL/player_body.obj"				 	
#define	MODEL_PLAYER_PARTS_LEG_LEFT_1	"data/MODEL/player_leg_left_1.obj"
#define	MODEL_PLAYER_PARTS_LEG_LEFT_2	"data/MODEL/player_leg_left_2.obj"
#define	MODEL_PLAYER_PARTS_LEG_LEFT_3	"data/MODEL/player_leg_left_3.obj"
#define	MODEL_PLAYER_PARTS_FOOT_LEFT	"data/MODEL/player_foot_left.obj"
#define	MODEL_PLAYER_PARTS_PANTS_LEFT	"data/MODEL/player_pants_left.obj"

#define	MODEL_PLAYER_PARTS_LEG_RIGHT_1	"data/MODEL/player_leg_right_1.obj"
#define	MODEL_PLAYER_PARTS_LEG_RIGHT_2	"data/MODEL/player_leg_right_2.obj"
#define	MODEL_PLAYER_PARTS_LEG_RIGHT_3	"data/MODEL/player_leg_right_3.obj"
#define	MODEL_PLAYER_PARTS_FOOT_RIGHT	"data/MODEL/player_foot_right.obj"
#define	MODEL_PLAYER_PARTS_PANTS_RIGHT	"data/MODEL/player_pants_right.obj"

#define	VALUE_MOVE			(3.0f)							 
#define	VALUE_JUMP			(10)							 
#define	VALUE_SIDE_MOVE		(1.0f)							 
#define	VALUE_ROTATE		(XM_PI * 0.02f)					 

#define PLAYER_SHADOW_SIZE	(1.0f)							 
#define PLAYER_OFFSET_Y		(-4.0f)							 

#define PLAYER_PARTS_MAX	(11)							 

static constexpr float CONTROL_POINT_XY_RANGE = 100.0f;
static constexpr float CONTROL_POINT_Z_BIAS = 20.0f;
static constexpr float HIT_TIME = 0.8f;

void ReleaseMoveTable();
void BuildMoveTableIdle();
void BuildMoveTableRun();
void GetDecision();
void UpdateLockedTarget();
void ShootBullets(int nBullet, ENEMY& enemy, XMFLOAT3 p0, XMFLOAT3 p2, XMVECTOR front, XMVECTOR right, XMVECTOR up);
bool CheckCode(CommandCode code, ENEMY& enemy);

static PLAYER		g_Player;						 

static PLAYER		g_Player_Parts[PLAYER_PARTS_MAX];		 

static BOOL			g_Load = FALSE;
/**
 * \brief An int type flag, last four bits used for representing a player's position's state
 * to field's boarder.
 * e.g. 0b0000 represents isn't out of any boarder.
 *      0b0001 represents out of Z plus boarder.
 */
static int			g_BoarderSignal = EndOfNone;
static bool			g_AtConjunction = false;
static bool			g_MadeDecision = false;
static float		g_FieldProgress = 0.0f;
static bool			g_Rampaging = false;

static ENEMY* g_LockedTarget = nullptr;

static INTERPOLATION_DATA move_tbl_body_idle[] = {					    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 3.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_left_idle[] = {				    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_right_idle[] = {			    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_body_run[] = {					    
	{ XMFLOAT3(0.0f, 3.0f, 0.0f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 5.0f, 0.0f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 3.0f, 0.0f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_left_run[] = {				    
	{ XMFLOAT3(0.0f, -0.5f, -0.5f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_right_run[] = {				    
	{ XMFLOAT3(0.0f, -0.5f, -0.5f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_pants_left[] = {					    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_pants_right[] = {				    
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

HRESULT InitPlayer(void)
{
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, 0.0f, -600.0f };
	g_Player.pos.y = PLAYER_OFFSET_Y;
	g_Player.rot = { 0.0f, XM_PI, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = VALUE_MOVE;			 
	g_Player.dir = XM_PI;
	g_Player.size = PLAYER_SIZE;	 

	g_Player.use = TRUE;

	g_Player.HP = PLAYER_HP_MAX;
	g_Player.MP = PLAYER_MP_MAX;

	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.parent = nullptr;			 

	for (auto& g_Player_Part : g_Player_Parts)
	{
		g_Player_Part.use = FALSE;

		g_Player_Part.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player_Part.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player_Part.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Player_Part.tbl_adr = nullptr;		 
		g_Player_Part.move_time = 0.0f;		 
		g_Player_Part.tbl_size = 0;			 

		g_Player_Part.load = FALSE;
	}

	g_Player_Parts[0].use = TRUE;
	g_Player_Parts[0].parent = &g_Player;														 
	g_Player_Parts[0].tbl_adr = move_tbl_body_run;												 
	g_Player_Parts[0].tbl_size = sizeof(move_tbl_body_run) / sizeof(INTERPOLATION_DATA);		 
	g_Player_Parts[0].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_BODY, &g_Player_Parts[0].model);

	g_Player_Parts[1].use = TRUE;
	g_Player_Parts[1].parent = &g_Player_Parts[0];											 
	g_Player_Parts[1].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_LEFT_1, &g_Player_Parts[1].model);

	g_Player_Parts[2].use = TRUE;
	g_Player_Parts[2].parent = &g_Player_Parts[1];											   
	g_Player_Parts[2].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_LEFT_2, &g_Player_Parts[2].model);

	g_Player_Parts[3].use = TRUE;
	g_Player_Parts[3].parent = &g_Player_Parts[2];											   
	g_Player_Parts[3].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_LEFT_3, &g_Player_Parts[3].model);

	g_Player_Parts[4].use = TRUE;
	g_Player_Parts[4].parent = &g_Player_Parts[3];											   
	g_Player_Parts[4].tbl_adr = move_tbl_foot_left_run;										 
	g_Player_Parts[4].tbl_size = sizeof(move_tbl_foot_left_run) / sizeof(INTERPOLATION_DATA);	 
	g_Player_Parts[4].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_FOOT_LEFT, &g_Player_Parts[4].model);

	g_Player_Parts[5].use = TRUE;
	g_Player_Parts[5].parent = &g_Player_Parts[0];											 
	g_Player_Parts[5].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_RIGHT_1, &g_Player_Parts[5].model);

	g_Player_Parts[6].use = TRUE;
	g_Player_Parts[6].parent = &g_Player_Parts[5];											   
	g_Player_Parts[6].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_RIGHT_2, &g_Player_Parts[6].model);

	g_Player_Parts[7].use = TRUE;
	g_Player_Parts[7].parent = &g_Player_Parts[6];											   
	g_Player_Parts[7].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_RIGHT_3, &g_Player_Parts[7].model);

	g_Player_Parts[8].use = TRUE;
	g_Player_Parts[8].parent = &g_Player_Parts[7];											   
	g_Player_Parts[8].tbl_adr = move_tbl_foot_right_run;										 
	g_Player_Parts[8].tbl_size = sizeof(move_tbl_foot_right_run) / sizeof(INTERPOLATION_DATA);	 
	g_Player_Parts[8].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_FOOT_RIGHT, &g_Player_Parts[8].model);

	g_Player_Parts[9].use = TRUE;
	g_Player_Parts[9].parent = &g_Player_Parts[0];											 
	g_Player_Parts[9].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_PANTS_LEFT, &g_Player_Parts[9].model);

	g_Player_Parts[10].use = TRUE;
	g_Player_Parts[10].parent = &g_Player_Parts[0];											 
	g_Player_Parts[10].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_PANTS_RIGHT, &g_Player_Parts[10].model);

	g_Load = TRUE;
	return S_OK;
}

void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

void UpdatePlayer(void)
{
	float& dir = g_Player.dir;

	g_BoarderSignal = FALSE;

	g_AtConjunction = IsAtConjunction(g_Player.pos.x, g_Player.pos.z, dir);
	g_FieldProgress = GetFieldProgress(g_Player.pos.x, g_Player.pos.z, dir);
	bool cameraAtPlayer = GetFocusMode() == FOCUS_PLAYER;
	g_Rampaging = GetKeyboardPress(DIK_G) ? true : false;

	if (g_Rampaging)
	{
		g_Player.spd = VALUE_MOVE * 2.0f;
	}
	else if (!g_AtConjunction)
	{
		g_MadeDecision = false;
		g_Player.spd = VALUE_MOVE;
		BuildMoveTableRun();
	}
	else if (!g_MadeDecision)
	{
		g_Player.spd = VALUE_MOVE * 0.1f;
		ReleaseMoveTable();
		BuildMoveTableIdle();

		// Decision of road's branch.
		GetDecision();

		//return;
	}

	if (!cameraAtPlayer && g_LockedTarget && g_Player.MP > 0)
	{
		g_Player.spd = VALUE_MOVE * 0.1f;
		static int t = 0;
		if (++t == 60)
		{
			g_Player.MP--;
			t = 0;
		}
		if (g_Player.MP == 0)
		{
			SetFocusMode(FOCUS_PLAYER);
		}
	}
	else
	{
		if (g_Player.MP < PLAYER_MP_MAX)
		{
			static int t = 0;
			if (++t == 60)
			{
				g_Player.MP++;
				t = 0;
			}
		}
	}

	// x pass
	if (!g_AtConjunction || g_MadeDecision)
	{
		XMFLOAT3 target = g_Player.pos;

		if (GetKeyboardPress(DIK_A))
		{
			target.x -= VALUE_SIDE_MOVE * cosf(dir);
			target.z -= VALUE_SIDE_MOVE * -sinf(dir);
		}
		if (GetKeyboardPress(DIK_D))
		{
			target.x += VALUE_SIDE_MOVE * cosf(dir);
			target.z += VALUE_SIDE_MOVE * -sinf(dir);
		}

		if (IsPositionValid(target.x, target.z))
		{
			g_Player.pos.x = target.x;
			g_Player.pos.z = target.z;
		}
	}

	// z pass
	{
		g_Player.rot.y = dir + XM_PI;

		g_Player.pos.z += g_Player.spd * cosf(dir);
		g_Player.pos.x += g_Player.spd * sinf(dir);

		g_BoarderSignal = IsOutOfBoarder(g_Player.pos.x, g_Player.pos.z);
		if (g_BoarderSignal)
		{
			g_Player.pos = GetWrapPosition(g_Player.pos, g_BoarderSignal);
		}
	}

	// y pass
	{
		static bool inAir = false;
		static int vertSpd = 0;

		if (!inAir)
		{
			if (GetKeyboardTrigger(DIK_SPACE))
			{
				inAir = true;
				vertSpd = VALUE_JUMP;
			}
		}
		else
		{
			g_Player.pos.y += static_cast<float>(vertSpd);
			vertSpd -= 1;
			if (g_Player.pos.y < PLAYER_OFFSET_Y)
			{
				inAir = false;
				vertSpd = 0;
				g_Player.pos.y = PLAYER_OFFSET_Y;
			}
		}
	}

	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	XMFLOAT3 wandPos = GetWeapon()->pos;
	wandPos.y += 25.0f;

	XMVECTOR front = { sinf(dir), 0.0f, cosf(dir) };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f };
	XMVECTOR right = XMVector3Cross(front, up);

	if (g_Rampaging)
	{
		static unsigned timer = 0;
		int i = rand() % MAX_ENEMY;
		ENEMY& enemy = *(GetEnemy() + i);
		if (++timer == 10)
		{
			ShootBullets(3, enemy, wandPos, enemy.pos, front, right, up);
			timer = 0;
		}
	}

	CommandCode cmd =
		GetKeyboardTrigger(DIK_UP) ? Up :
		GetKeyboardTrigger(DIK_DOWN) ? Down :
		GetKeyboardTrigger(DIK_LEFT) ? Left :
		GetKeyboardTrigger(DIK_RIGHT) ? Right : None;

	if (cameraAtPlayer)
	{
		for (int i = 0; i < MAX_ENEMY; ++i)
		{
			ENEMY& enemy = *(GetEnemy() + i);
			if (CheckCode(cmd, enemy))
			{
				ShootBullets(6, enemy, wandPos, enemy.pos, front, right, up);
			}
		}
	}
	else
	{
		UpdateLockedTarget();

		if (g_LockedTarget != nullptr)
		{
			ENEMY& enemy = *g_LockedTarget;
			if (CheckCode(cmd, enemy))
			{
				ShootBullets(6, enemy, wandPos, enemy.pos, front, right, up);
			}
		}
	}

	for (auto& g_Player_Part : g_Player_Parts)
	{
		if ((g_Player_Part.use == TRUE) && (g_Player_Part.tbl_adr != nullptr))
		{
			int		index = static_cast<int>(g_Player_Part.move_time);
			float	time = g_Player_Part.move_time - index;
			int		size = g_Player_Part.tbl_size;

			float dt = 1.0f / g_Player_Part.tbl_adr[index].frame;	 
			g_Player_Part.move_time += dt;					 

			if (index > (size - 2))	 
			{
				g_Player_Part.move_time = 0.0f;
				index = 0;
			}

			XMVECTOR p1 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 1].pos);	 
			XMVECTOR p0 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 0].pos);	 
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Player_Part.pos, p0 + vec * time);

			XMVECTOR r1 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 1].rot);	 
			XMVECTOR r0 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 0].rot);	 
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Player_Part.rot, r0 + rot * time);

			XMVECTOR s1 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 1].scl);	 
			XMVECTOR s0 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 0].scl);	 
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Player_Part.scl, s0 + scl * time);
		}
	}

#ifdef _DEBUG	 
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	PrintDebugProc("Player: dir:%f progress : %f\n", dir, g_FieldProgress);
	PrintDebugProc("Player hp:%d\n", g_Player.HP);
	PrintDebugProc("Player mp:%d\n", g_Player.MP);
	PrintDebugProc("g_AtConjunction:%d\n", g_AtConjunction);
	PrintDebugProc("g_MadeDecision:%d\n", g_MadeDecision);
#endif
}

void DrawPlayer(void)
{
	XMMATRIX mtxWorld = XMMatrixIdentity();

	XMMATRIX mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMMATRIX mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	DrawModel(&g_Player.model);

	for (auto& g_Player_Part : g_Player_Parts)
	{
		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(g_Player_Part.scl.x, g_Player_Part.scl.y, g_Player_Part.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(g_Player_Part.rot.x, g_Player_Part.rot.y, g_Player_Part.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(g_Player_Part.pos.x, g_Player_Part.pos.y, g_Player_Part.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Player_Part.parent != nullptr)	 
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Player_Part.parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Player_Part.mtxWorld, mtxWorld);

		if (g_Player_Part.use == FALSE) continue;

		SetWorldMatrix(&mtxWorld);

		DrawModel(&g_Player_Part.model);
	}
}

void DrawPlayerToDepthTex(void)
{
	XMMATRIX mtxWorld = XMMatrixIdentity();

	XMMATRIX mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	XMMATRIX mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	DrawModelWithoutMat(&g_Player.model);

	for (auto& g_Player_Part : g_Player_Parts)
	{
		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(g_Player_Part.scl.x, g_Player_Part.scl.y, g_Player_Part.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(g_Player_Part.rot.x, g_Player_Part.rot.y, g_Player_Part.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(g_Player_Part.pos.x, g_Player_Part.pos.y, g_Player_Part.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Player_Part.parent != nullptr)
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Player_Part.parent->mtxWorld));
		}

		XMStoreFloat4x4(&g_Player_Part.mtxWorld, mtxWorld);

		if (g_Player_Part.use == FALSE) continue;

		SetWorldMatrix(&mtxWorld);

		DrawModelWithoutMat(&g_Player_Part.model);
	}
}

PLAYER* GetPlayer(void)
{
	return &g_Player;
}

int IsPlayerOutOfBoarder()
{
	return g_BoarderSignal;
}

float GetPlayerFieldProgress()
{
	return g_FieldProgress;
}

ENEMY* GetPlayerLockedTarget()
{
	return g_LockedTarget;
}

bool IsPlayerRampage()
{
	return g_Rampaging;
}

void ReleaseMoveTable()
{
	g_Player.tbl_adr = nullptr;
	for (auto& part : g_Player_Parts)
	{
		part.tbl_adr = nullptr;
	}
}

void BuildMoveTableIdle()
{
	g_Player_Parts[0].tbl_adr = move_tbl_body_idle;

	g_Player_Parts[4].tbl_adr = move_tbl_foot_left_idle;

	g_Player_Parts[8].tbl_adr = move_tbl_foot_right_idle;
}

void BuildMoveTableRun()
{
	g_Player_Parts[0].tbl_adr = move_tbl_body_run;

	g_Player_Parts[4].tbl_adr = move_tbl_foot_left_run;

	g_Player_Parts[8].tbl_adr = move_tbl_foot_right_run;
}

void GetDecision()
{
	float nPiDiv2 = 0.0f;

	if (GetKeyboardPress(DIK_A))
	{
		nPiDiv2 = -1.0f;
		g_MadeDecision = true;
	}
	else if (GetKeyboardPress(DIK_D))
	{
		nPiDiv2 = 1.0f;
		g_MadeDecision = true;
	}
	else if (GetKeyboardPress(DIK_W))
	{
		nPiDiv2 = 0.0f;
		g_MadeDecision = true;
	}
	g_Player.dir += XM_PIDIV2 * nPiDiv2;

	if (g_MadeDecision)
	{
		g_Player.spd = VALUE_MOVE;
		BuildMoveTableRun();
	}
}

void UpdateLockedTarget()
{
	const float dir = g_Player.dir;
	const XMVECTOR playerPos = XMLoadFloat3(&g_Player.pos);
	const XMVECTOR front = { sinf(dir), 0.0f, cosf(dir) };

	// No need to use heap, we just want the min distance one.
	//auto disCmp = [&](const ENEMY* e1, const ENEMY* e2)
	//{
	//	const float dis1 = XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&e1->pos) - playerPos));
	//	const float dis2 = XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&e2->pos) - playerPos));
	//	return dis1 > dis2;
	//};

	//std::priority_queue<ENEMY*, std::vector<ENEMY*>, decltype(disCmp)> pq(disCmp);
	ENEMY* pEnemies = GetEnemy();
	ENEMY* target = nullptr;
	float minDis = D3D11_FLOAT32_MAX;
	for (int i = 0; i < MAX_ENEMY; ++i)
	{
		ENEMY& enemy = *(pEnemies + i);
		if (enemy.use == FALSE)   
			continue;

		const XMVECTOR enemyPos = XMLoadFloat3(&enemy.pos);
		const float enemyDis = XMVectorGetX(XMVector3Length(enemyPos - playerPos));
		const XMVECTOR enemyDir = XMVector3Normalize(enemyPos - playerPos);

		const float angle = XMVectorGetX(XMVector3AngleBetweenNormalsEst(enemyDir, front));
		if ((angle) < XM_PIDIV2 && enemyDis < minDis)       
		{
			//pq.emplace(&enemy);
			target = &enemy;
			minDis = enemyDis;
		}
	}
	g_LockedTarget = target;
	/*if (pq.empty()) g_LockedTarget = nullptr;
	else g_LockedTarget = pq.top();*/
}

void ShootBullets(int nBullet, ENEMY& enemy, XMFLOAT3 p0, XMFLOAT3 p2, XMVECTOR front, XMVECTOR right, XMVECTOR up)
{
	for (int i = 0; i < nBullet; ++i)
	{
		const float theta = XM_PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		XMFLOAT3 p1{};
		XMVECTOR target = XMLoadFloat3(&g_Player.pos);

		target += front * CONTROL_POINT_Z_BIAS;
		target += right * cosf(theta) * CONTROL_POINT_XY_RANGE;
		target += up * sinf(theta) * CONTROL_POINT_XY_RANGE;
		XMStoreFloat3(&p1, target);
		std::array<XMFLOAT3, 3> points =
		{
			p0,
			p1,
			p2
		};
		SetBullet(points, HIT_TIME, &enemy);
	}
}

bool CheckCode(CommandCode code, ENEMY& enemy)
{
	int& idx = enemy.compare_index;

	if (enemy.use == false || idx == enemy.codes.size())
		return false;

	if (enemy.codes[idx] == code)
		idx++;

	else if (code != None)
	{
		idx = 0;
		return false;
	}

	if (idx == enemy.codes.size())
		return true;

	return false;
}