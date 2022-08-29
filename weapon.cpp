//=============================================================================
//
// ����̃��f������ [weapon.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "light.h"
#include "bullet.h"
#include "weapon.h"

#include "MathHelper.h"
#include "meshfield.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_WEAPON_1						"data/MODEL/magicWand_body.obj"							// �ǂݍ��ރ��f���� ���@�̏�P
#define	MODEL_WEAPON_1_PARTS_CORE			"data/MODEL/magicWand_core.obj"							// �ǂݍ��ރ��f���� ���@�̏�P�̃p�[�c
#define	MODEL_WEAPON_1_PARTS_MOVE			"data/MODEL/magicWand_move.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L1		"data/MODEL/magicWand_funnal_left1.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L2		"data/MODEL/magicWand_funnal_left2.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L3		"data/MODEL/magicWand_funnal_left3.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_L4		"data/MODEL/magicWand_funnal_left4.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R1		"data/MODEL/magicWand_funnal_right1.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R2		"data/MODEL/magicWand_funnal_right2.obj"	
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R3		"data/MODEL/magicWand_funnal_right3.obj"
#define	MODEL_WEAPON_1_PARTS_FUNNAL_R4		"data/MODEL/magicWand_funnal_right4.obj"	

#define	VALUE_MOVE					(10.0f)															// �ړ���
#define	VALUE_ROTATE				(XM_PI * 0.02f)													// ��]��


#define WEAPON_1_PARTS_MAX			(10)															// ���@�̏�1�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static WEAPON		g_Weapon[MAX_WEAPON];							// ����

static WEAPON		g_Weapon_1_Parts[WEAPON_1_PARTS_MAX];			// ����P(���@�̏�P)�̃p�[�c�p

static BOOL			g_Load = FALSE;


// ����̊K�w�A�j���[�V�����f�[�^
static INTERPOLATION_DATA move_tbl_core[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.5f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_2PI, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, -0.5f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),     XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),      XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_DATA move_tbl_move[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_2PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, XM_PI, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};


static INTERPOLATION_DATA move_tbl_funnal[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitWeapon(void)
{
	LoadModel(MODEL_WEAPON_1, &g_Weapon[0].model);
	g_Weapon[0].load = TRUE;

	PLAYER *player = GetPlayer();


	for (int i = 0; i < MAX_WEAPON; i++)
	{
		g_Weapon[i].pos = player->pos;
		g_Weapon[i].rot = { 0.0f, XM_PI, 0.0f };
		g_Weapon[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Weapon[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
	}
	

	g_Weapon[0].use = TRUE;


	// �K�w�A�j���[�V�����p�̏���������
	g_Weapon[0].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	for (int i = 0; i < WEAPON_1_PARTS_MAX; i++)
	{
		g_Weapon_1_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Weapon_1_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Weapon_1_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Weapon_1_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		//g_Weapon_1_Parts[i].parent = &g_Weapon[0];		// �� �����ɐe�̃A�h���X������
	//	g_Parts[�r].parent= &g_Player;					// �r��������e�͖{�́i�v���C���[�j
	//	g_Parts[��].parent= &g_Paerts[�r];				// �w���r�̎q���������ꍇ�̗�

		// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		g_Weapon_1_Parts[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Weapon_1_Parts[i].move_time = 0.0f;	// ���s���Ԃ��N���A
		g_Weapon_1_Parts[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Weapon_1_Parts[i].load = FALSE;
	}

	g_Weapon_1_Parts[0].use = TRUE;
	g_Weapon_1_Parts[0].parent = &g_Weapon[0];													// �e���Z�b�g
	g_Weapon_1_Parts[0].tbl_adr = move_tbl_core;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[0].tbl_size = sizeof(move_tbl_core) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[0].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_CORE, &g_Weapon_1_Parts[0].model);

	g_Weapon_1_Parts[1].use = TRUE;
	g_Weapon_1_Parts[1].parent = &g_Weapon[0];													// �e���Z�b�g
	g_Weapon_1_Parts[1].tbl_adr = move_tbl_move;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[1].tbl_size = sizeof(move_tbl_move) / sizeof(INTERPOLATION_DATA);			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[1].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_MOVE, &g_Weapon_1_Parts[1].model);

	g_Weapon_1_Parts[2].use = TRUE;
	g_Weapon_1_Parts[2].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[2].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[2].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[2].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L1, &g_Weapon_1_Parts[2].model);

	g_Weapon_1_Parts[3].use = TRUE;
	g_Weapon_1_Parts[3].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[3].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[3].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[3].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L2, &g_Weapon_1_Parts[3].model);

	g_Weapon_1_Parts[4].use = TRUE;
	g_Weapon_1_Parts[4].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[4].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[4].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[4].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L3, &g_Weapon_1_Parts[4].model);

	g_Weapon_1_Parts[5].use = TRUE;
	g_Weapon_1_Parts[5].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[5].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[5].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[5].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_L4, &g_Weapon_1_Parts[5].model);

	g_Weapon_1_Parts[6].use = TRUE;
	g_Weapon_1_Parts[6].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[6].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[6].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[6].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R1, &g_Weapon_1_Parts[6].model);

	g_Weapon_1_Parts[7].use = TRUE;
	g_Weapon_1_Parts[7].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[7].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[7].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[7].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R2, &g_Weapon_1_Parts[7].model);

	g_Weapon_1_Parts[8].use = TRUE;
	g_Weapon_1_Parts[8].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[8].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[8].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[8].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R3, &g_Weapon_1_Parts[8].model);

	g_Weapon_1_Parts[9].use = TRUE;
	g_Weapon_1_Parts[9].parent = &g_Weapon_1_Parts[0];											// �e���Z�b�g
	g_Weapon_1_Parts[9].tbl_adr = move_tbl_funnal;												// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Weapon_1_Parts[9].tbl_size = sizeof(move_tbl_funnal) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Weapon_1_Parts[9].load = TRUE;
	LoadModel(MODEL_WEAPON_1_PARTS_FUNNAL_R4, &g_Weapon_1_Parts[9].model);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitWeapon(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	for (int i = 0; i < MAX_WEAPON; i++)
	{
		if (g_Weapon[i].load)
		{
			UnloadModel(&g_Weapon[i].model);
			g_Weapon[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateWeapon(void)
{
	// �ړ�����
	PLAYER* player = GetPlayer();

	int boarderFlag = IsPlayerOutOfBoarder();

	for (auto& weapon : g_Weapon)
	{
		if (boarderFlag)
		{
			weapon.pos = GetWrapPosition(weapon.pos, boarderFlag);
		}
		auto target = XMLoadFloat3(&player->pos) + XMVECTOR{20.0f, 0.0f, -10.0f};
		XMVECTOR lerp = MathHelper::Lerp(XMLoadFloat3(&weapon.pos), target, 0.1f);
		weapon.rot = player->rot;
		XMStoreFloat3(&weapon.pos, lerp);
	}
	

	// �e���ˏ���
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
	}

	// �K�w�A�j���[�V����
	for (int i = 0; i < WEAPON_1_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Weapon_1_Parts[i].use == TRUE) && (g_Weapon_1_Parts[i].tbl_adr != NULL))
		{
			// �ړ�����
			int		index = (int)g_Weapon_1_Parts[i].move_time;
			float	time = g_Weapon_1_Parts[i].move_time - index;
			int		size = g_Weapon_1_Parts[i].tbl_size;

			float dt = 1.0f / g_Weapon_1_Parts[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Weapon_1_Parts[i].move_time += dt;						// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
			{
				g_Weapon_1_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			XMVECTOR p1 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
			XMVECTOR p0 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Weapon_1_Parts[i].pos, p0 + vec * time);

			// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			XMVECTOR r1 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 1].rot);	// ���̊p�x
			XMVECTOR r0 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Weapon_1_Parts[i].rot, r0 + rot * time);

			// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			XMVECTOR s1 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 1].scl);	// ����Scale
			XMVECTOR s0 = XMLoadFloat3(&g_Weapon_1_Parts[i].tbl_adr[index + 0].scl);	// ���݂�Scale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Weapon_1_Parts[i].scl, s0 + scl * time);

		}
	}

	//{	// �|�C���g���C�g�̃e�X�g
	//	LIGHT* light = GetLightData(2);
	//	XMFLOAT3 pos = g_Weapon[0].pos;
	//	pos.y += 20.0f;

	//	light->Position = pos;
	//	light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Type = LIGHT_TYPE_POINT;
	//	light->Enable = TRUE;
	//	SetLightData(2, light);
	//}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("WeaponOffset:X:%f Y:%f Z:%f\n", g_Weapon[0].pos.x - player->pos.x, g_Weapon[0].pos.y - player->pos.y, g_Weapon[0].pos.z - player->pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawWeapon(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Weapon[0].scl.x, g_Weapon[0].scl.y, g_Weapon[0].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon[0].rot.x, g_Weapon[0].rot.y + XM_PI, g_Weapon[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Weapon[0].pos.x, g_Weapon[0].pos.y, g_Weapon[0].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Weapon[0].mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&g_Weapon[0].model);



	// �p�[�c�̊K�w�A�j���[�V����
	for (int i = 0; i < WEAPON_1_PARTS_MAX; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Weapon_1_Parts[i].scl.x, g_Weapon_1_Parts[i].scl.y, g_Weapon_1_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Weapon_1_Parts[i].rot.x, g_Weapon_1_Parts[i].rot.y, g_Weapon_1_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Weapon_1_Parts[i].pos.x, g_Weapon_1_Parts[i].pos.y, g_Weapon_1_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Weapon_1_Parts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Weapon_1_Parts[i].parent->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Weapon_1_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
		if (g_Weapon_1_Parts[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);


		// ���f���`��
		DrawModel(&g_Weapon_1_Parts[i].model);

	}



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
WEAPON* GetWeapon(void)
{
	return &g_Weapon[0];
}

