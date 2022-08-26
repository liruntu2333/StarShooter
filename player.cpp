//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS	"data/MODEL/torus.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)							// 移動量
#define	VALUE_JUMP			(10)							// 移動量
#define	VALUE_SIDE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(2)								// プレイヤーのパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static BOOL			g_Load = FALSE;

/**
 * \brief An int type flag, last four bits used for representing a player's position's state
 * to field's boarder.
 * e.g. 0b0000 represents isn't out of any boarder.
 *      0b0001 represents out of Z plus boarder.
 */
static int			g_OutOfBoarder = EndOfNone;
static bool			g_AtConjunction = false;
static bool			g_MadeDecision = false;


// プレイヤーの階層アニメーションデータ
// プレイヤーの頭を左右に動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_right[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(20.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_left[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(XM_PI / 2, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 180 },
	{ XMFLOAT3(-20.0f, 10.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),         XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = {0.0f, 0.0f, -600.0f};
	g_Player.pos.y = PLAYER_OFFSET_Y;
	g_Player.rot = { 0.0f, XM_PI, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = VALUE_MOVE;			// 移動スピードクリア
	g_Player.dir = XM_PI;
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// 階層アニメーション用の初期化処理
	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる

	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
	//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i].move_time = 0.0f;	// 実行時間をクリア
		g_Parts[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}

	//g_Parts[0].use = TRUE;
	//g_Parts[0].parent   = &g_Player;		// 親をセット
	//g_Parts[0].tbl_adr  = move_tbl_right;	// 再生するアニメデータの先頭アドレスをセット
	//g_Parts[0].tbl_size = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	//g_Parts[0].load = 1;
	//LoadModel(MODEL_PLAYER_PARTS, &g_Parts[0].model);

	//g_Parts[1].use = TRUE;
	//g_Parts[1].parent   = &g_Player;		// 親をセット
	//g_Parts[1].tbl_adr  = move_tbl_left;	// 再生するアニメデータの先頭アドレスをセット
	//g_Parts[1].tbl_size = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	//g_Parts[1].load = 1;
	//LoadModel(MODEL_PLAYER_PARTS, &g_Parts[1].model);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	float& dir = g_Player.dir;

	if (g_OutOfBoarder)
	{
		g_OutOfBoarder = FALSE;
	}

	g_AtConjunction = IsAtConjunction(g_Player.pos.x, g_Player.pos.z, dir);

	if (g_MadeDecision && !g_AtConjunction)
	{
		g_MadeDecision = false;
	}

	if (g_AtConjunction && !g_MadeDecision)
	{
		g_Player.spd = VALUE_MOVE * 0.1f;

		// Decision of road's branch.
		int i = 0; //rand() % 4;

		if (GetKeyboardPress(DIK_A))
		{
			i = -1;
			g_MadeDecision = true;
		}
		else if (GetKeyboardPress(DIK_D))
		{
			i = 1;
			g_MadeDecision = true;
		}
		else if (GetKeyboardPress(DIK_W))
		{
			i = 0;
			g_MadeDecision = true;
		}
		g_Player.dir += XM_PIDIV2 * i;

		if (g_Player.dir > XM_2PI - 0.01f)	g_Player.dir -= XM_2PI;
		if (g_Player.dir < 0.0f)	g_Player.dir += XM_2PI;

		if(g_MadeDecision) 
			g_Player.spd = VALUE_MOVE;
	}

	if (!g_AtConjunction)
	{
		g_Player.spd = VALUE_MOVE;
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = dir = 0.0f;
		g_Player.spd = VALUE_MOVE;
	}
	if (GetKeyboardPress(DIK_P))
	{
		g_Player.spd = g_Player.spd < 0.01f ? VALUE_MOVE : 0.0f;
	}
#endif

	// x pass
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

		if (CheckFieldValid(target.x, target.z))
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

		g_OutOfBoarder = IsOutOfBoarder(g_Player.pos.x, g_Player.pos.z);
		if (g_OutOfBoarder)
		{
			g_Player.pos = GetWrapPosition(g_Player.pos, g_OutOfBoarder);
			// TODO: make player set on the right offset of road
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

	// レイキャストして足元の高さを求める
	//XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
	//XMFLOAT3 hitPosition;								// 交点
	//hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// 外れた時用に初期化しておく
	//bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	//g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	////g_Player.pos.y = PLAYER_OFFSET_Y;

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// 弾発射処理
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
	}

	//g_Player.spd *= 0.5f;


	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Parts[i].use == TRUE)&&(g_Parts[i].tbl_adr != NULL))
		{
			// 移動処理
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// 次の角度
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// 現在の角度
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// 次のScale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// 現在のScale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}



	{	// ポイントライトのテスト
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// ２つのベクトルの外積を取って任意の回転軸を求める
	g_Player.upVector = {0.0f, 1.0f, 0.0f};
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	// 求めた回転軸からクォータニオンを作り出す
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	// 今回のクォータニオンの結果を保存する
	XMStoreFloat4(&g_Player.quaternion, quat);



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	PrintDebugProc("Player: dir:%f\n", dir);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// クォータニオンを反映
	//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Player.model);



	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Parts[i].model);

	}



	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

int IsPlayerEndOfBoarder()
{
	return g_OutOfBoarder;
}

