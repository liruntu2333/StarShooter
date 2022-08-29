//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : 
//
//=============================================================================
#include "main.h"

#include "shadow.h"
#include "model.h"
#include "light.h"
#include "bullet.h"

#include "enemy.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BULLET				"data/MODEL/bullet_star.obj"			// 読み込むモデル名

#define	BULLET_SPEED				(5.0f)									// 弾の移動スピード

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************

static BULLET						g_Bullet[MAX_BULLET];	

static BOOL							g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	//LoadModel(MODEL_BULLET, &g_Bullet[0].model);
	//g_Bullet[0].load = TRUE;
	
	for (int i = 0; i < MAX_BULLET; i++)
	{
		g_Bullet[i].pos = { 0.0f, 0.0f, 0.0f };
		g_Bullet[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Bullet[i].scl = { 0.0f, 0.0f, 0.0f };

		g_Bullet[i].attackSize = BULLET_ATTACK_SIZE;

		g_Bullet[i].spd = 0.0f;			
		g_Bullet[i].use = FALSE;

		LoadModel(MODEL_BULLET, &g_Bullet[i].model);
		g_Bullet[i].load = TRUE;
	}
	

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].load)
		{
			UnloadModel(&g_Bullet[i].model);
			g_Bullet[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{

	for (auto& bullet : g_Bullet)
	{
		if (bullet.use)
		{
			if (bullet.curve != nullptr)
			{
				const float t = bullet.flyingTime / bullet.hitTime;
				if (bullet.target != nullptr)
				{
					bullet.curve->SetControlPoint2(bullet.target->pos);
				}
				bullet.pos = bullet.curve->GetPosition(t);
				const XMFLOAT3 dir = bullet.curve->GetNormalizedDerivative(t);
				const XMVECTOR dirVec = XMLoadFloat3(&dir);
				constexpr XMVECTOR right = { -1.0f, 0.0f, 0.0f };
				constexpr XMVECTOR up = { 0.0f, -1.0f, 0.0f };
				constexpr XMVECTOR front = { 0.0f, 0.0f, -1.0f };
				bullet.rot.x = XMVectorGetX(XMVector3AngleBetweenNormals(dirVec, right));
				bullet.rot.y = XMVectorGetX(XMVector3AngleBetweenNormals(dirVec , up));
				bullet.rot.z = XMVectorGetX(XMVector3AngleBetweenNormals(dirVec , front));
				//bullet.rot
				
				bullet.flyingTime += 1.0f / 60.0f;
			}
			else
			{
				bullet.pos.x -= sinf(bullet.rot.y) * bullet.spd;
				bullet.pos.z -= cosf(bullet.rot.y) * bullet.spd;
			}


			// 影の位置設定
			SetPositionShadow(bullet.shadowIdx, XMFLOAT3(bullet.pos.x, 0.1f, bullet.pos.z));


			// フィールドの外に出たら弾を消す処理
			if (bullet.pos.x < MAP_LEFT
				|| bullet.pos.x > MAP_RIGHT
				|| bullet.pos.z < MAP_DOWN
				|| bullet.pos.z > MAP_TOP)
			{
				bullet.use = FALSE;
				bullet.curve.release();
				ReleaseShadow(bullet.shadowIdx);
			}

		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y, g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);

			// モデル描画
			DrawModel(&g_Bullet[i].model);
		}
	}

	

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);



	//SetCullingMode(CULL_MODE_NONE);

	//XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	//// 頂点バッファ設定
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//// プリミティブトポロジ設定
	//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//for (int i = 0; i < MAX_BULLET; i++)
	//{
	//	if (g_Bullet[i].use)
	//	{
	//		// ワールドマトリックスの初期化
	//		mtxWorld = XMMatrixIdentity();

	//		// スケールを反映
	//		mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
	//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	//		// 回転を反映
	//		mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y + XM_PI, g_Bullet[i].rot.z);
	//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//		// 移動を反映
	//		mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
	//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//		// ワールドマトリックスの設定
	//		SetWorldMatrix(&mtxWorld);

	//		XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


	//		// マテリアル設定
	//		SetMaterial(g_Bullet[i].material);

	//		// テクスチャ設定
	//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	//		// ポリゴンの描画
	//		GetDeviceContext()->Draw(4, 0);
	//	}
	//}

	//// ライティングを有効に
	//SetLightEnable(TRUE);
	//SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// 弾のパラメータをセット
//=============================================================================
//int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot)
//{
//	int nIdxBullet = -1;
//
//	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
//	{
//		if (!g_Bullet[nCntBullet].use)
//		{
//			g_Bullet[nCntBullet].pos = pos;
//			g_Bullet[nCntBullet].rot = rot;
//			g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
//			g_Bullet[nCntBullet].use = TRUE;
//
//			// 影の設定
//			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);
//
//			nIdxBullet = nCntBullet;
//
//			// 発射音
//			PlaySound(SOUND_LABEL_SE_shot000);
//
//			break;
//		}
//	}
//
//	return nIdxBullet;
//}

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

			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// 弾の取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

