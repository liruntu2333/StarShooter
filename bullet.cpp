#include "main.h"
#include "model.h"
#include "light.h"
#include "bullet.h"

#include "enemy.h"
#include "sound.h"
#include "particle.h"

#define	MODEL_BULLET				"data/MODEL/bullet_star.obj"			 

#define	BULLET_SPEED				(5.0f)									 
#define PARTICLE_GENERATION_FRAME	2

static BULLET						g_Bullet[MAX_BULLET];

static BOOL							g_Load = FALSE;

HRESULT InitBullet(void)
{
	for (auto& i : g_Bullet)
	{
		i.pos = { 0.0f, 0.0f, 0.0f };
		i.rot = { 0.0f, 0.0f, 0.0f };
		i.scl = { 0.0f, 0.0f, 0.0f };

		i.attackSize = BULLET_ATTACK_SIZE;

		i.spd = 0.0f;
		i.use = FALSE;

		LoadModel(MODEL_BULLET, &i.model);
		i.load = TRUE;
	}

	g_Load = TRUE;
	return S_OK;
}

void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	for (auto& i : g_Bullet)
	{
		if (i.load)
		{
			UnloadModel(&i.model);
			i.load = FALSE;
		}
	}

	g_Load = FALSE;
}

void UpdateBullet(void)
{
	static int time = 0;

	for (auto& bullet : g_Bullet)
	{
		if (bullet.use)
		{
			if (bullet.curve != nullptr)
			{
				const float t = bullet.flyingTime / bullet.hitTime;
				if (bullet.target != nullptr)
				{
					bullet.curve->SetControlPointsBack(bullet.target->pos);
				}
				bullet.pos = bullet.curve->GetPosition(t);
				const XMFLOAT3 dir = bullet.curve->GetNormalizedDerivative(t);
				const XMVECTOR dirVec = XMLoadFloat3(&dir);
				constexpr XMVECTOR right = { -1.0f, 0.0f, 0.0f };
				constexpr XMVECTOR up = { 0.0f, -1.0f, 0.0f };
				constexpr XMVECTOR front = { 0.0f, 0.0f, -1.0f };
				bullet.rot.x = XMVectorGetX(XMVector3AngleBetweenNormals(dirVec, right));
				bullet.rot.y = XMVectorGetX(XMVector3AngleBetweenNormals(dirVec, up));
				bullet.rot.z = XMVectorGetX(XMVector3AngleBetweenNormals(dirVec, front));
				bullet.flyingTime += 1.0f / 60.0f;
			}
			else
			{
				bullet.pos.x -= sinf(bullet.rot.y) * bullet.spd;
				bullet.pos.z -= cosf(bullet.rot.y) * bullet.spd;
			}

			{
				if (time == PARTICLE_GENERATION_FRAME)
				{
					SetParticle(bullet.pos, {}, { 1.0f,1.0f,1.0f,1.0f }, 0.5f, 0.5f, 120);
				}
			}


			if (bullet.pos.x < MAP_LEFT
				|| bullet.pos.x > MAP_RIGHT
				|| bullet.pos.z < MAP_DOWN
				|| bullet.pos.z > MAP_TOP)
			{
				bullet.use = FALSE;
				bullet.curve.release();
			}
		}
	}

	time == PARTICLE_GENERATION_FRAME ? time = 0 : time++;
}

void DrawBullet(void)
{


	XMMATRIX mtxScl{}, mtxRot{}, mtxTranslate{}, mtxWorld{};

	for (auto& i : g_Bullet)
	{
		if (i.use)
		{
			mtxWorld = XMMatrixIdentity();

			mtxScl = XMMatrixScaling(i.scl.x, i.scl.y, i.scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			mtxRot = XMMatrixRotationRollPitchYaw(i.rot.x, i.rot.y, i.rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			mtxTranslate = XMMatrixTranslation(i.pos.x, i.pos.y, i.pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&i.mtxWorld, mtxWorld);

			DrawModel(&i.model);
		}
	}
}

int SetBullet(const std::array<XMFLOAT3, 3>& controlPoints, float tHit, ENEMY* target)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			auto& bullet = g_Bullet[nCntBullet];
			bullet.curve = std::make_unique<BezierCurveQuadratic>(controlPoints);
			bullet.flyingTime = 0.0f;
			bullet.hitTime = tHit;
			bullet.target = target;

			bullet.scl = { 5.0f, 5.0f, 5.0f };
			g_Bullet[nCntBullet].use = TRUE;

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

BULLET* GetBullet(void)
{
	return &(g_Bullet[0]);
}