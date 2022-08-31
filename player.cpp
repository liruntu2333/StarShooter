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

#include <queue>

#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "enemy.h"
#include "weapon.h"
#include "billboard.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	MODEL_PLAYER					"data/MODEL/player_body.obj"			// 読み込むモデル名 プレイヤーの本体

#define	MODEL_PLAYER_PARTS_BODY			"data/MODEL/player_body.obj"				// 読み込むモデル名	プレイヤーのパーツ
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

#define	VALUE_MOVE			(3.0f)							// 移動量
#define	VALUE_JUMP			(10)							// 移動量
#define	VALUE_SIDE_MOVE		(1.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(-4.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(11)							// プレイヤーのパーツの数

static constexpr float CONTROL_POINT_XY_RANGE = 100.0f;
static constexpr float CONTROL_POINT_Z_BIAS = 20.0f;
static constexpr float HIT_TIME = 0.8f;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void ReleaseMoveTable();
void BuildMoveTableIdle();
void BuildMoveTableRun();
void GetDecision();
void UpdateLockedTarget();
void ShootBullets(int nBullet, ENEMY& enemy, XMFLOAT3 p0, XMFLOAT3 p2, XMVECTOR front, XMVECTOR right, XMVECTOR up);
bool CheckCode(CommandCode code, ENEMY& enemy);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Player_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

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

static ENEMY*		g_LockedTarget = nullptr;

// プレイヤーの階層アニメーションデータ
// idleのアニメ
static INTERPOLATION_DATA move_tbl_body_idle[] = {					// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 3.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_left_idle[] = {				// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_right_idle[] = {			// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};


// runのアニメ
static INTERPOLATION_DATA move_tbl_body_run[] = {					// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 3.0f, 0.0f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 5.0f, 0.0f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 3.0f, 0.0f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_left_run[] = {				// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -0.5f, -0.5f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_foot_right_run[] = {				// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, -0.5f, -0.5f),	XMFLOAT3(0.5f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_pants_left[] = {					// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_pants_right[] = {				// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	//LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = {0.0f, 0.0f, -600.0f};
	g_Player.pos.y = PLAYER_OFFSET_Y;
	g_Player.rot = { 0.0f, XM_PI, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = VALUE_MOVE;			// 移動スピードクリア
	g_Player.dir = XM_PI;
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.use = TRUE;

	// 属性の初期化
	g_Player.HP = PLAYER_HP_MAX;
	g_Player.MP = PLAYER_MP_MAX;

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
		g_Player_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Player_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		//g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
		//g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
		//g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

		// 階層アニメーション用のメンバー変数の初期化
		g_Player_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Player_Parts[i].move_time = 0.0f;		// 実行時間をクリア
		g_Player_Parts[i].tbl_size = 0;			// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Player_Parts[i].load = FALSE;
	}

	// MODEL_PLAYER_PARTS_BODY
	g_Player_Parts[0].use = TRUE;
	g_Player_Parts[0].parent = &g_Player;														// 親(player)をセット
	g_Player_Parts[0].tbl_adr = move_tbl_body_run;												// 再生するアニメデータの先頭アドレスをセット
	g_Player_Parts[0].tbl_size = sizeof(move_tbl_body_run) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Player_Parts[0].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_BODY, &g_Player_Parts[0].model);

	// MODEL_PLAYER_PARTS_LEG_LEFT
	g_Player_Parts[1].use = TRUE;
	g_Player_Parts[1].parent   = &g_Player_Parts[0];											// 親(body)をセット
	//g_Player_Parts[1].tbl_adr  = move_tbl_leg_left;											// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[1].tbl_size = sizeof(move_tbl_leg_left) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Player_Parts[1].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_LEFT_1, &g_Player_Parts[1].model);

	g_Player_Parts[2].use = TRUE;
	g_Player_Parts[2].parent   = &g_Player_Parts[1];											// 親(left leg 1)をセット 
	//g_Player_Parts[2].tbl_adr  = move_tbl_null;												// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[2].tbl_size = sizeof(move_tbl_null) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	g_Player_Parts[2].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_LEFT_2, &g_Player_Parts[2].model);

	g_Player_Parts[3].use = TRUE;
	g_Player_Parts[3].parent   = &g_Player_Parts[2];											// 親(left leg 2)をセット 
	//g_Player_Parts[3].tbl_adr  = move_tbl_null;												// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[3].tbl_size = sizeof(move_tbl_null) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	g_Player_Parts[3].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_LEFT_3, &g_Player_Parts[3].model);

	g_Player_Parts[4].use = TRUE;
	g_Player_Parts[4].parent   = &g_Player_Parts[3];											// 親(left leg 3)をセット 
	g_Player_Parts[4].tbl_adr  = move_tbl_foot_left_run;										// 再生するアニメデータの先頭アドレスをセット
	g_Player_Parts[4].tbl_size = sizeof(move_tbl_foot_left_run) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Player_Parts[4].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_FOOT_LEFT, &g_Player_Parts[4].model);

	// MODEL_PLAYER_PARTS_LEG_RIGHT
	g_Player_Parts[5].use = TRUE;
	g_Player_Parts[5].parent   = &g_Player_Parts[0];											// 親(body)をセット
	//g_Player_Parts[5].tbl_adr  = move_tbl_leg_right;											// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[5].tbl_size = sizeof(move_tbl_leg_right) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Player_Parts[5].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_RIGHT_1, &g_Player_Parts[5].model);

	g_Player_Parts[6].use = TRUE;
	g_Player_Parts[6].parent   = &g_Player_Parts[5];											// 親(right leg 1)をセット 
	//g_Player_Parts[6].tbl_adr  = move_tbl_null;												// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[6].tbl_size = sizeof(move_tbl_null) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	g_Player_Parts[6].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_RIGHT_2, &g_Player_Parts[6].model);

	g_Player_Parts[7].use = TRUE;
	g_Player_Parts[7].parent   = &g_Player_Parts[6];											// 親(right leg 2)をセット 
	//g_Player_Parts[7].tbl_adr  = move_tbl_null;												// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[7].tbl_size = sizeof(move_tbl_null) / sizeof(INTERPOLATION_DATA);			// 再生するアニメデータのレコード数をセット
	g_Player_Parts[7].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_LEG_RIGHT_3, &g_Player_Parts[7].model);

	g_Player_Parts[8].use = TRUE;
	g_Player_Parts[8].parent   = &g_Player_Parts[7];											// 親(right leg 3)をセット 
	g_Player_Parts[8].tbl_adr  = move_tbl_foot_right_run;										// 再生するアニメデータの先頭アドレスをセット
	g_Player_Parts[8].tbl_size = sizeof(move_tbl_foot_right_run) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Player_Parts[8].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_FOOT_RIGHT, &g_Player_Parts[8].model);

	// MODEL_PLAYER_PANTS
	g_Player_Parts[9].use = TRUE;
	g_Player_Parts[9].parent   = &g_Player_Parts[0];											// 親(body)をセット
	//g_Player_Parts[9].tbl_adr  = move_tbl_pants_left;											// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[9].tbl_size = sizeof(move_tbl_pants_left) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Player_Parts[9].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_PANTS_LEFT, &g_Player_Parts[9].model);

	g_Player_Parts[10].use = TRUE;
	g_Player_Parts[10].parent   = &g_Player_Parts[0];											// 親(body)をセット
	//g_Player_Parts[10].tbl_adr  = move_tbl_pants_right;										// 再生するアニメデータの先頭アドレスをセット
	//g_Player_Parts[10].tbl_size = sizeof(move_tbl_pants_right) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Player_Parts[10].load = TRUE;
	LoadModel(MODEL_PLAYER_PARTS_PANTS_RIGHT, &g_Player_Parts[10].model);

	

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

	g_BoarderSignal = FALSE;

	g_AtConjunction = IsAtConjunction(g_Player.pos.x, g_Player.pos.z, dir);
	g_FieldProgress = GetFieldProgress(g_Player.pos.x, g_Player.pos.z, dir);

	if (!g_AtConjunction)
	{
		g_MadeDecision = false;
		g_Player.spd = VALUE_MOVE;
		BuildMoveTableRun();
	}
	else if (!g_MadeDecision)
	{
		g_Player.spd = VALUE_MOVE * 0.0f;
		ReleaseMoveTable();
		BuildMoveTableIdle();

		// Decision of road's branch.
		GetDecision();

		//return;
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

	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);
	XMFLOAT3 wandPos = GetWeapon()->pos;
	wandPos.y += 20.0f;

	ENEMY* pEnemy = GetEnemy();
	XMVECTOR front = { sinf(dir), 0.0f, cosf(dir) };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f };
	XMVECTOR right = XMVector3Cross(front, up);

	// auto shooting mode
	//if (GetKeyboardTrigger(DIK_J))
	//{
	//	for (int i = 0; i < MAX_ENEMY; ++i)
	//	{
	//		ENEMY& enemy = *(pEnemy + i);
	//		XMVECTOR enemyPos = XMLoadFloat3(&enemy.pos);
	//		XMVECTOR enemyDir = XMVector3Normalize(enemyPos - XMLoadFloat3(&wandPos));
	//		float enemyDis = (enemyDir / enemyPos).m128_f32[0];

	//		static const float cos45 = cosf(XM_PIDIV4);

	//		if (enemy.use && 
	//			XMVector3Dot(enemyDir, front).m128_f32[0] > cos45)
	//		{
	//			// generate control point p1 on +y semi-circle, with a range of 20.0f
	//			float theta = XM_PI * (float)rand() / (float)RAND_MAX;
	//			XMFLOAT3 p1{};
	//			XMVECTOR target = XMLoadFloat3(&g_Player.pos);

	//			target += front * CONTROL_POINT_Z_BIAS;
	//			target += right * cosf(theta) * CONTROL_POINT_XY_RANGE;
	//			target += up * sinf(theta) * CONTROL_POINT_XY_RANGE;
	//			XMStoreFloat3(&p1, target);
	//			std::array<XMFLOAT3, 3> points =
	//			{
	//				wandPos,
	//				p1,
	//				enemy.pos
	//			};
	//			SetBullet(points, HIT_TIME, &enemy);
	//			//SetBullet(g_Player.pos, g_Player.rot);
	//			break;
	//		}
	//	}
	//}

	CommandCode cmd =
		GetKeyboardTrigger(DIK_UP) ? Up :
		GetKeyboardTrigger(DIK_DOWN) ? Down :
		GetKeyboardTrigger(DIK_LEFT) ? Left :
		GetKeyboardTrigger(DIK_RIGHT) ? Right : None;

	if (GetFocusMode() == FOCUS_PLAYER)
	{
		for (int i = 0; i < MAX_ENEMY; ++i)
		{
			ENEMY& enemy = *(GetEnemy() + i);
			if (CheckCode(cmd, enemy))
			{
				ShootBullets(5, enemy, wandPos, enemy.pos, front, right, up);
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
				ShootBullets(5, enemy, wandPos, enemy.pos, front, right, up);
			}
		}
	}

	for (auto& g_Player_Part : g_Player_Parts)
	{
		if ((g_Player_Part.use == TRUE)&&(g_Player_Part.tbl_adr != NULL))
		{
			int		index = (int)g_Player_Part.move_time;
			float	time = g_Player_Part.move_time - index;
			int		size = g_Player_Part.tbl_size;

			float dt = 1.0f / g_Player_Part.tbl_adr[index].frame;	// 1フレームで進める時間
			g_Player_Part.move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Player_Part.move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Player_Part.pos, p0 + vec * time);

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR r1 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 1].rot);	// 次の角度
			XMVECTOR r0 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 0].rot);	// 現在の角度
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Player_Part.rot, r0 + rot * time);

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			XMVECTOR s1 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 1].scl);	// 次のScale
			XMVECTOR s0 = XMLoadFloat3(&g_Player_Part.tbl_adr[index + 0].scl);	// 現在のScale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Player_Part.scl, s0 + scl * time);

		}
	}



	//{
	//	LIGHT *light = GetLightData(1);
	//	XMFLOAT3 pos = g_Player.pos;
	//	pos.y += 20.0f;

	//	light->Position = pos;
	//	light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Type = LIGHT_TYPE_POINT;
	//	light->Enable = TRUE;
	//	SetLightData(1, light);
	//}

		//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	//XMVECTOR vx, nvx, up;
	//XMVECTOR quat;
	//float len, angle;

	//// ２つのベクトルの外積を取って任意の回転軸を求める
	//g_Player.upVector = {0.0f, 1.0f, 0.0f};
	//up = { 0.0f, 1.0f, 0.0f, 0.0f };
	//vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	//// 求めた回転軸からクォータニオンを作り出す
	//nvx = XMVector3Length(vx);
	//XMStoreFloat(&len, nvx);
	//nvx = XMVector3Normalize(vx);
	//angle = asinf(len);
	//quat = XMQuaternionRotationNormal(nvx, angle);

	//// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
	//quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	//// 今回のクォータニオンの結果を保存する
	//XMStoreFloat4(&g_Player.quaternion, quat);

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	PrintDebugProc("Player: dir:%f progress : %f\n", dir, g_FieldProgress);
	PrintDebugProc("Player hp:%d\n", g_Player.HP);
	
	PrintDebugProc("g_AtConjunction:%d\n", g_AtConjunction);
	PrintDebugProc("g_MadeDecision:%d\n", g_MadeDecision);
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
		mtxScl = XMMatrixScaling(g_Player_Parts[i].scl.x, g_Player_Parts[i].scl.y, g_Player_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Player_Parts[i].rot.x, g_Player_Parts[i].rot.y, g_Player_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Player_Parts[i].pos.x, g_Player_Parts[i].pos.y, g_Player_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Player_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Player_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Player_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Player_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Player_Parts[i].model);

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

void ReleaseMoveTable()
{
	g_Player.tbl_adr = nullptr;
	for (auto & part : g_Player_Parts)
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

	auto disCmp = [&](const ENEMY* e1, const ENEMY* e2)
	{
		const float dis1 = XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&e1->pos) - playerPos));
		const float dis2 = XMVectorGetX(XMVector3LengthEst(XMLoadFloat3(&e2->pos) - playerPos));
		return dis1 > dis2;
	};

	std::priority_queue<ENEMY*, std::vector<ENEMY*>, decltype(disCmp)> pq(disCmp);
	ENEMY* pEnemies = GetEnemy();
	for (int i = 0; i < MAX_ENEMY; ++i)
	{
		ENEMY& enemy = *(pEnemies + i);
		if (enemy.use == FALSE || enemy.compare_index == enemy.codes.size()) 
			continue;

		const XMVECTOR enemyPos = XMLoadFloat3(&enemy.pos);
		const XMVECTOR enemyDir = XMVector3Normalize(enemyPos - playerPos);

		const float angle = XMVectorGetX(XMVector3AngleBetweenNormalsEst(enemyDir, front));
		if ((angle) < XM_PIDIV2 ) // only deal with enemy in front
		{
			pq.emplace(&enemy);
		}
	}

	if (pq.empty()) g_LockedTarget = nullptr;
	else g_LockedTarget = pq.top();
}

void ShootBullets(int nBullet, ENEMY& enemy, XMFLOAT3 p0, XMFLOAT3 p2, XMVECTOR front, XMVECTOR right, XMVECTOR up)
{
	for (int i = 0; i < nBullet; ++i)
	{
		// generate control point p1 on +y semi-circle, with a range of 20.0f
		float theta = XM_PI * (float)rand() / (float)RAND_MAX;
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
