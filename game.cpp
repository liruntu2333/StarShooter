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
#include "DPSM.h"
#include "light.h"

void CheckHit(void);
void SetCameraFocus();

namespace 
{
	BOOL	g_bPause = TRUE;
	int g_focusMode = FocusMode::FOCUS_PLAYER;
	int g_gameMode = GAMEMODE_START;

	std::vector<std::unique_ptr<DPSM>> g_DPSMs;
}


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

	g_DPSMs.emplace_back(std::make_unique<DPSM>(GetDevice(), GetDeviceContext(),
		1024, 1024, 0));
	g_DPSMs.emplace_back(std::make_unique<DPSM>(GetDevice(), GetDeviceContext(),
		1024, 1024, 1));
	g_DPSMs.emplace_back(std::make_unique<DPSM>(GetDevice(), GetDeviceContext(),
		1024, 1024, 2));
	g_DPSMs.emplace_back(std::make_unique<DPSM>(GetDevice(), GetDeviceContext(),
		1024, 1024, 3));
	g_DPSMs.emplace_back(std::make_unique<DPSM>(GetDevice(), GetDeviceContext(),
		1024, 1024, 4));

	return S_OK;
}

void UninitGame(void)
{
	g_DPSMs.clear();

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

void DrawDepthTextures()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pDeviceContext->VSSetShader(DPSM::sVs.Get(), nullptr, 0);
	pDeviceContext->PSSetShader(DPSM::sPs.Get(), nullptr, 0);

	{
		ID3D11ShaderResourceView* nullViews[] = 
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
		};
		pDeviceContext->PSSetShaderResources(2, _countof(nullViews), nullViews);
	}
	SetCullingMode(CULL_MODE_NUM);
	for (const auto & dpsm : g_DPSMs)
	{
		const int lightIdx = dpsm->GetLightIndex();

		const LIGHT* light = GetLightData(lightIdx);
		const XMVECTOR lightPos = XMLoadFloat3(&light->Position);
		const XMVECTOR lightUp = { 0.0f, 0.0f, 1.0f };
		const XMVECTOR lightDir = XMLoadFloat3(&light->Direction);
		XMMATRIX view = XMMatrixLookToLH(lightPos, lightDir, lightUp);
		SetViewMatrix(&view);
		dpsm->SetView(view);
		dpsm->SetViewPort();

		ID3D11DepthStencilView* dsvFront = dpsm->GetFrontDsv();
		pDeviceContext->ClearDepthStencilView(dsvFront, D3D11_CLEAR_DEPTH, 1.0f, 0);
		pDeviceContext->OMSetRenderTargets(0, nullptr, dsvFront);

		SetFuchi(lightIdx + 1);
		{
			//DrawMeshFieldToDepthTex();
			DrawPlayerToDepthTex();
			DrawBuildingToDepthTex();
			DrawEnemyToDepthTex();
			DrawWeaponToDepthTex();
		}

		ID3D11DepthStencilView* dsvBack = dpsm->GetBackDsv();
		pDeviceContext->ClearDepthStencilView(dsvBack, D3D11_CLEAR_DEPTH, 1.0f, 0);
		pDeviceContext->OMSetRenderTargets(0, nullptr, dsvBack);

		SetFuchi(- (lightIdx + 1));
		{
			//DrawMeshFieldToDepthTex();
			DrawPlayerToDepthTex();
			DrawBuildingToDepthTex();
			DrawEnemyToDepthTex();
			DrawWeaponToDepthTex();
		}
	}
	SetCullingMode(CULL_MODE_BACK);

	pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	std::vector<ID3D11ShaderResourceView*> srvs;
	std::vector<XMMATRIX> viewMtx(5);

	for (const auto & dpsm : g_DPSMs)
	{
		srvs.push_back(dpsm->GetFrontSrv());
		srvs.push_back(dpsm->GetBackSrv());
		viewMtx[dpsm->GetLightIndex()] = dpsm->GetView();
	}

	SetLightViews(viewMtx);
	pDeviceContext->PSSetShaderResources(2, srvs.size(), srvs.data());
	ReturnToMainPass();
}

void DrawGame(void)
{
	DrawDepthTextures();

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