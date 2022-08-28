//=============================================================================
//
// ���j���[�̃��f������ [menu.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "light.h"
#include "bullet.h"
#include "weapon.h"
#include "menu.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_MENU					"data/MODEL/menu.obj"			// �ǂݍ��ރ��f���� ���j���[
#define	MODEL_MENU_PARTS			"data/MODEL/torus.obj"			// �ǂݍ��ރ��f���� ���j���[�̃p�[�c

#define	VALUE_MOVE					(10.0f)							// �ړ���
#define	VALUE_ROTATE				(XM_PI * 0.02f)					// ��]��


#define MENU_PARTS_MAX			(2)									// ���j���[�̃p�[�c�̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static MENU			g_Menu[MAX_MENU];							// ����

static MENU			g_Menu_Parts[MENU_PARTS_MAX];				// ����P(���@�̏�P)�̃p�[�c�p

static BOOL			g_Load = FALSE;


// �v���C���[�̊K�w�A�j���[�V�����f�[�^
// �v���C���[�̓������E�ɓ������Ă���A�j���f�[�^
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
// ����������
//=============================================================================
HRESULT InitMenu(void)
{
	LoadModel(MODEL_MENU, &g_Menu[0].model);
	g_Menu[0].load = TRUE;

	WEAPON* weapon = GetWeapon();


	for (int i = 0; i < MAX_MENU; i++)
	{
		g_Menu[i].pos = weapon->pos;
		g_Menu[i].rot = { 0.0f, XM_PI, 0.0f };
		g_Menu[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Menu[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Menu[i].isDisplay = FALSE;	// �������͕\�����Ȃ�
	}


	g_Menu[0].use = TRUE;


	// �K�w�A�j���[�V�����p�̏���������
	g_Menu[0].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

	// �p�[�c�̏�����
	//for (int i = 0; i < MENU_PARTS_MAX; i++)
	//{
	//	g_Menu_Parts[i].use = FALSE;

	//	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	//	g_Menu_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	g_Menu_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	g_Menu_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	//	// �e�q�֌W
	//	g_Menu_Parts[i].parent = &g_Menu[0];		// �� �����ɐe�̃A�h���X������
	////	g_Parts[�r].parent= &g_Player;					// �r��������e�͖{�́i�v���C���[�j
	////	g_Parts[��].parent= &g_Paerts[�r];				// �w���r�̎q���������ꍇ�̗�

	//	// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
	//	g_Menu_Parts[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//	g_Menu_Parts[i].move_time = 0.0f;	// ���s���Ԃ��N���A
	//	g_Menu_Parts[i].tbl_size = 0;		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	//	// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
	//	g_Menu_Parts[i].load = FALSE;
	//}

	//g_Menu_Parts[0].use = TRUE;
	//g_Menu_Parts[0].parent = &g_Menu[0];		// �e���Z�b�g
	//g_Menu_Parts[0].tbl_adr = move_tbl_right;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Menu_Parts[0].tbl_size = sizeof(move_tbl_right) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	//g_Menu_Parts[0].load = 1;
	//LoadModel(MODEL_MENU_PARTS, &g_Menu_Parts[0].model);

	//g_Menu_Parts[1].use = TRUE;
	//g_Menu_Parts[1].parent = &g_Menu[0];		// �e���Z�b�g
	//g_Menu_Parts[1].tbl_adr = move_tbl_left;	// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	//g_Menu_Parts[1].tbl_size = sizeof(move_tbl_left) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	//g_Menu_Parts[1].load = 1;
	//LoadModel(MODEL_MENU_PARTS, &g_Menu_Parts[1].model);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMenu(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
	for (int i = 0; i < MAX_WEAPON; i++)
	{
		if (g_Menu[i].load)
		{
			UnloadModel(&g_Menu[i].model);
			g_Menu[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMenu(void)
{
	// �ړ�����
	WEAPON* weapon = GetWeapon();

	for (int i = 0; i < MAX_WEAPON; i++)
	{
		g_Menu[i].pos.x += (weapon->pos.x + 20.0f - g_Menu[i].pos.x) * 0.05f;
		g_Menu[i].pos.y += (weapon->pos.y + 10.0f - g_Menu[i].pos.y) * 0.05f;
		g_Menu[i].pos.z += (weapon->pos.z - 10.0f - g_Menu[i].pos.z) * 0.05f;
	}


	// ���j���[�̕\������
	if (GetKeyboardTrigger(DIK_H))
	{
		if (g_Menu[0].isDisplay == FALSE)
		{
			g_Menu[0].isDisplay = TRUE;
		}
		else
		{
			g_Menu[0].isDisplay = FALSE;
		}
	}

	// �e���ˏ���
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		//SetBullet(g_Player.pos, g_Player.rot);
	}

	// �K�w�A�j���[�V����
	for (int i = 0; i < MENU_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		if ((g_Menu_Parts[i].use == TRUE) && (g_Menu_Parts[i].tbl_adr != NULL))
		{
			// �ړ�����
			int		index = (int)g_Menu_Parts[i].move_time;
			float	time = g_Menu_Parts[i].move_time - index;
			int		size = g_Menu_Parts[i].tbl_size;

			float dt = 1.0f / g_Menu_Parts[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
			g_Menu_Parts[i].move_time += dt;					// �A�j���[�V�����̍��v���Ԃɑ���

			if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
			{
				g_Menu_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
			XMVECTOR p1 = XMLoadFloat3(&g_Menu_Parts[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
			XMVECTOR p0 = XMLoadFloat3(&g_Menu_Parts[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Menu_Parts[i].pos, p0 + vec * time);

			// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
			XMVECTOR r1 = XMLoadFloat3(&g_Menu_Parts[i].tbl_adr[index + 1].rot);	// ���̊p�x
			XMVECTOR r0 = XMLoadFloat3(&g_Menu_Parts[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Menu_Parts[i].rot, r0 + rot * time);

			// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
			XMVECTOR s1 = XMLoadFloat3(&g_Menu_Parts[i].tbl_adr[index + 1].scl);	// ����Scale
			XMVECTOR s0 = XMLoadFloat3(&g_Menu_Parts[i].tbl_adr[index + 0].scl);	// ���݂�Scale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Menu_Parts[i].scl, s0 + scl * time);

		}
	}







#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("Player:�� �� �� ���@Space\n");
	//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMenu(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Menu[0].scl.x, g_Menu[0].scl.y, g_Menu[0].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Menu[0].rot.x, g_Menu[0].rot.y + XM_PI, g_Menu[0].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Menu[0].pos.x, g_Menu[0].pos.y, g_Menu[0].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Menu[0].mtxWorld, mtxWorld);

	if (g_Menu[0].isDisplay == TRUE)
	{
		// ���f���`��
		DrawModel(&g_Menu[0].model);

		// �p�[�c�̊K�w�A�j���[�V����
		for (int i = 0; i < MENU_PARTS_MAX; i++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Menu_Parts[i].scl.x, g_Menu_Parts[i].scl.y, g_Menu_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Menu_Parts[i].rot.x, g_Menu_Parts[i].rot.y, g_Menu_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Menu_Parts[i].pos.x, g_Menu_Parts[i].pos.y, g_Menu_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Menu_Parts[i].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Menu_Parts[i].parent->mtxWorld));
				// ��
				// g_Player.mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Menu_Parts[i].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Menu_Parts[i].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// ���f���`��
			DrawModel(&g_Menu_Parts[i].model);

		}
	}

	



	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
MENU* GetMenu(void)
{
	return &g_Menu[0];
}

