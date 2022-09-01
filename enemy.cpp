#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "player.h"
#include "MathHelper.h"
#include "meshfield.h"
#include "billboard.h"

#define	MODEL_ENEMY						"data/MODEL/enemy_body.obj"			 

#define	MODEL_ENEMY_PARTS_WINGS_LEFT	"data/MODEL/enemy_wings_left.obj"	 	
#define	MODEL_ENEMY_PARTS_WINGS_RIGHT	"data/MODEL/enemy_wings_right.obj"
#define	MODEL_ENEMY_PARTS_EYE_LEFT		"data/MODEL/enemy_eye_left.obj"
#define	MODEL_ENEMY_PARTS_EYE_RIGHT		"data/MODEL/enemy_eye_right.obj"

#define ENEMY_PARTS_MAX					(4)									 

#define	VALUE_MOVE			(1.0f)						 
#define	VALUE_AMP			(50.0f)						 
#define	VALUE_ROTATE		(XM_PI * 0.02f)				 

#define ENEMY_SHADOW_SIZE	(0.4f)						 
#define ENEMY_OFFSET_Y		(7.0f)						 

enum EnemyBehaviorType : int
{
	Obstacle = 0,
	GoalKeeper = 1,
	Flyable = 2,
};

static ENEMY			g_Enemy[MAX_ENEMY];				 

static BOOL				g_Load = FALSE;

static ENEMY			g_Enemy_Parts[MAX_ENEMY][ENEMY_PARTS_MAX];

static INTERPOLATION_DATA move_tbl_wings[] = {	    
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,		0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y + 3.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y ,		0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y - 3.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y,		0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
};

HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = VALUE_MOVE;			 
		g_Enemy[i].size = ENEMY_SIZE;	 

		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);

		g_Enemy[i].move_time = 0.0f;
		g_Enemy[i].tbl_adr = nullptr;
		g_Enemy[i].tbl_size = 0;

		g_Enemy[i].parent = nullptr;

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

			g_Enemy_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Enemy_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			g_Enemy_Parts[i][j].parent = &g_Enemy[i];		  
			g_Enemy_Parts[i][j].tbl_adr = nullptr;		 
			g_Enemy_Parts[i][j].move_time = 0.0f;		 
			g_Enemy_Parts[i][j].tbl_size = 0;			 

			g_Enemy_Parts[i][j].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		g_Enemy_Parts[i][0].use = TRUE;
		g_Enemy_Parts[i][0].parent = &g_Enemy[i];													 
		g_Enemy_Parts[i][0].tbl_adr = move_tbl_wings;												 
		g_Enemy_Parts[i][0].tbl_size = sizeof(move_tbl_wings) / sizeof(INTERPOLATION_DATA);		 
		g_Enemy_Parts[i][0].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_WINGS_LEFT, &g_Enemy_Parts[i][0].model);

		g_Enemy_Parts[i][1].use = TRUE;
		g_Enemy_Parts[i][1].parent = &g_Enemy[i];													 
		g_Enemy_Parts[i][1].tbl_adr = move_tbl_wings;												 
		g_Enemy_Parts[i][1].tbl_size = sizeof(move_tbl_wings) / sizeof(INTERPOLATION_DATA);		 
		g_Enemy_Parts[i][1].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_WINGS_RIGHT, &g_Enemy_Parts[i][1].model);

		g_Enemy_Parts[i][2].use = TRUE;
		g_Enemy_Parts[i][2].parent = &g_Enemy[i];													 
		g_Enemy_Parts[i][2].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_EYE_LEFT, &g_Enemy_Parts[i][2].model);

		g_Enemy_Parts[i][3].use = TRUE;
		g_Enemy_Parts[i][3].parent = &g_Enemy[i];													 
		g_Enemy_Parts[i][3].load = TRUE;
		LoadModel(MODEL_ENEMY_PARTS_EYE_RIGHT, &g_Enemy_Parts[i][3].model);
	}

	InitBillboard();

	g_Load = TRUE;
	return S_OK;
}

void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (auto& i : g_Enemy)
	{
		if (i.load)
		{
			UnloadModel(&i.model);
			i.load = FALSE;
		}
	}

	ShutdownBillboard();

	g_Load = FALSE;
}

void UpdateEnemy(void)
{
	const XMFLOAT3 playerPos = GetPlayer()->pos;
	const float playerProgress = GetPlayerFieldProgress();

	bool boarderFlag = IsPlayerOutOfBoarder();

	if (boarderFlag)
	{
		for (auto& enemy : g_Enemy)
		{
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
				float vx = VALUE_MOVE * static_cast<float>(rand()) / RAND_MAX;
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
		}
	}

	for (auto& g_Enemy_Part : g_Enemy_Parts)
	{
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			if ((g_Enemy_Part[j].use == TRUE) && (g_Enemy_Part[j].tbl_adr != nullptr))
			{
				int		index = static_cast<int>(g_Enemy_Part[j].move_time);
				float	time = g_Enemy_Part[j].move_time - index;
				int		size = g_Enemy_Part[j].tbl_size;

				float dt = 1.0f / g_Enemy_Part[j].tbl_adr[index].frame;	 
				g_Enemy_Part[j].move_time += dt;					 

				if (index > (size - 2))	 
				{
					g_Enemy_Part[j].move_time = 0.0f;
					index = 0;
				}

				XMVECTOR p1 = XMLoadFloat3(&g_Enemy_Part[j].tbl_adr[index + 1].pos);	 
				XMVECTOR p0 = XMLoadFloat3(&g_Enemy_Part[j].tbl_adr[index + 0].pos);	 
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Enemy_Part[j].pos, p0 + vec * time);

				XMVECTOR r1 = XMLoadFloat3(&g_Enemy_Part[j].tbl_adr[index + 1].rot);	 
				XMVECTOR r0 = XMLoadFloat3(&g_Enemy_Part[j].tbl_adr[index + 0].rot);	 
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Enemy_Part[j].rot, r0 + rot * time);

				XMVECTOR s1 = XMLoadFloat3(&g_Enemy_Part[j].tbl_adr[index + 1].scl);	 
				XMVECTOR s0 = XMLoadFloat3(&g_Enemy_Part[j].tbl_adr[index + 0].scl);	 
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Enemy_Part[j].scl, s0 + scl * time);
			}
		}
	}
}

void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		mtxWorld = XMMatrixIdentity();

		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		DrawModel(&g_Enemy[i].model);

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			mtxWorld = XMMatrixIdentity();

			mtxScl = XMMatrixScaling(g_Enemy_Parts[i][j].scl.x, g_Enemy_Parts[i][j].scl.y, g_Enemy_Parts[i][j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy_Parts[i][j].rot.x, g_Enemy_Parts[i][j].rot.y, g_Enemy_Parts[i][j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			mtxTranslate = XMMatrixTranslation(g_Enemy_Parts[i][j].pos.x, g_Enemy_Parts[i][j].pos.y, g_Enemy_Parts[i][j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Enemy_Parts[i][j].parent != nullptr)	 
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Enemy_Parts[i][j].parent->mtxWorld));
			}

			XMStoreFloat4x4(&g_Enemy_Parts[i][j].mtxWorld, mtxWorld);

			if (g_Enemy_Parts[i][j].use == FALSE) continue;

			SetWorldMatrix(&mtxWorld);

			DrawModel(&g_Enemy_Parts[i][j].model);
		}
	}

	for (auto& enemy : g_Enemy)
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
		const XMFLOAT3 scl{ fScl, fScl, fScl };

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

ENEMY* GetEnemy()
{
	return &g_Enemy[0];
}