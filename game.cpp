#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"
#include "result.h"

#include "player.h"
#include "enemy.h"
#include "weapon.h"
#include "item.h"
#include "building.h"
#include "menu.h"
#include "gameUI.h"
#include "meshfield.h"
#include "meshwall.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

void CheckHit(void);
void SetCameraFocus();

static BOOL	g_bPause = TRUE;	 

static int g_focusMode = FocusMode::FOCUS_PLAYER;

static int g_gameMode = GAMEMODE_START;

HRESULT InitGame(void)
{
	const unsigned seed = time(nullptr);
	srand(seed);

	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	InitBuilding();

	InitPlayer();

	InitWeapon();

	InitItem();

	InitEnemy();

	InitMenu();

	InitMeshWall(XMFLOAT3(0.0f, -80.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, -80.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, -80.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, -80.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	InitMeshWall(XMFLOAT3(0.0f, -80.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, -80.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, -80.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, -80.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);


	InitBullet();

	InitScore();

	InitGameUI();

	InitParticle();

	return S_OK;
}

void UninitGame(void)
{
	UninitParticle();

	UninitGameUI();

	UninitScore();

	UninitBullet();

	UninitMeshWall();

	UninitMeshField();

	UninitBuilding();

	UninitMenu();

	UninitEnemy();

	UninitItem();

	UninitWeapon();

	UninitPlayer();
}

void UpdateGame(void)
{
#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

#endif

	if (g_bPause == FALSE)
		return;

	if (GetKeyboardTrigger(DIK_LSHIFT))
	{
		g_focusMode = (g_focusMode + 1) % 2;
	}

	UpdateMeshField();

	UpdateBuilding();

	UpdatePlayer();

	UpdateWeapon();

	UpdateItem();

	UpdateEnemy();

	UpdateMenu();

	UpdateMeshWall();

	UpdateBullet();

	UpdateParticle();

	CheckHit();

	UpdateGameUI();

	UpdateScore();
}

void DrawGame(void)
{
	SetCameraFocus();

	UpdateSkyBox(GetCamera()->pos);

	DrawMeshField();

	DrawBuilding();

	DrawEnemy();

	DrawItem();

	DrawPlayer();

	DrawWeapon();

	DrawMenu();

	DrawBullet();

	DrawMeshWall();

	DrawSkyBox();

	DrawParticle();

	SetDepthEnable(FALSE);

	SetLightEnable(FALSE);

	DrawScore();

	DrawGameUI();

	SetLightEnable(TRUE);

	SetDepthEnable(TRUE);
}

int GetFocusMode()
{
	return g_focusMode;
}

void SetFocusMode(int mode)
{
	g_focusMode = mode;
}

void CheckHit(void)
{
	ENEMY* enemy = GetEnemy();		 
	PLAYER* player = GetPlayer();		 
	BULLET* bullet = GetBullet();		 
	ITEM* item = GetItem();				 

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (bullet[i].use == FALSE)
			continue;

		for (int j = 0; j < MAX_ENEMY; j++)
		{
			if (enemy[j].use == FALSE)
				continue;

			if (CollisionBC(bullet[i].pos, enemy[j].pos, bullet[i].attackSize, enemy[j].size))
			{
				bullet[i].use = FALSE;
				enemy[j].use = FALSE;
				AddScore(10);

				PlaySound(SOUND_LABEL_SE_shot000);
			}
		}
	}

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (item[i].use == FALSE)
			continue;

		if (CollisionBC(player->pos, item[i].pos, player->size, item[i].size))
		{
			item[i].use = FALSE;

			PlaySound(SOUND_LABEL_SE_lockon000);

			if (item[i].type == hpHeal)
			{
				if (player->HP < PLAYER_HP_MAX)
				{
					player->HP += 1;
				}
			}
			else if (item[i].type == hpKill)
			{
				if (player->HP > 0)
				{
					player->HP -= 1;
				}
			}
		}
	}

	if (player->HP == 0)
	{
		SetResult(GetScore());	 

		SetFade(FADE_OUT, MODE_RESULT);
	}

}

void SetCameraFocus()
{
	XMFLOAT3 playerPos = GetPlayer()->pos;
	const float playerDir = GetPlayer()->dir;
	playerPos.y += 35.f;        
	float tPos;
	const bool playerRampaging = IsPlayerRampage();

	const auto target = GetPlayerLockedTarget();

	switch (g_focusMode)
	{
	case FOCUS_PLAYER:
		tPos = IsPlayerOutOfBoarder() ? 1.0f : 0.5f;
		if (playerRampaging) LerpCameraViewAngle(XM_PIDIV2 * 0.8f, 0.5f);
		else LerpCameraViewAngle(XM_PIDIV4, 0.5f);
		LerpCameraPosition(playerPos, playerDir, tPos);
		SetCamera();
		break;

	case FOCUS_ENEMY:
		tPos = IsPlayerOutOfBoarder() ? 1.0f : 0.5f;
		if (playerRampaging) LerpCameraViewAngle(XM_PIDIV2 * 0.8f, 0.5f);
		else LerpCameraViewAngle(XM_PI / 6.0f, 0.5f);

		if (target != nullptr)
			LerpCameraPositionAt(playerPos, target->pos, playerDir, tPos, 0.4f);
		else
			LerpCameraPosition(playerPos, playerDir, tPos);

		SetCamera();
		break;

	default: break;
	}
}