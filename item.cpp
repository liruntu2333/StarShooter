//=============================================================================
//
// �A�C�e�����f������ [item.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "item.h"
#include "shadow.h"
#include "player.h"
#include "MathHelper.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ITEM_HP_HEAL					"data/MODEL/apple.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ITEM_HP_KILL					"data/MODEL/fishBones.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(1.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ITEM_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ITEM_OFFSET_Y		(0.0f)						// �A�C�e���̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ITEM				g_Item[MAX_ITEM];				

static BOOL				g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitItem(void)
{
	// hpHeal�̃��f���𓱓��i0,1,2�Ԃ́j
	for (int i = 0; i < MAX_ITEM_GOOD; i++)
	{
		LoadModel(MODEL_ITEM_HP_HEAL, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].type = hpHeal;
	}

	// hpKill�̃��f���𓱓��i3,4�Ԃ́j
	for (int i = MAX_ITEM_GOOD; i < MAX_ITEM; i++)
	{
		LoadModel(MODEL_ITEM_HP_KILL, &g_Item[i].model);
		g_Item[i].load = TRUE;

		g_Item[i].type = hpKill;
	}

	for (int i = 0; i < MAX_ITEM; i++)
	{
		
		g_Item[i].pos = XMFLOAT3(0.0f, ITEM_OFFSET_Y, 0.0f);
		g_Item[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Item[i].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

		g_Item[i].spd = VALUE_MOVE;			// �ړ��X�s�[�h�N���A
		g_Item[i].size = ITEM_SIZE;			// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Item[0].model, &g_Item[0].diffuse[0]);

		XMFLOAT3 pos = g_Item[i].pos;
		pos.y -= (ITEM_OFFSET_Y - 0.1f);
		g_Item[i].shadowIdx = CreateShadow(pos, ITEM_SHADOW_SIZE, ITEM_SHADOW_SIZE);

		g_Item[i].move_time = 0.0f;
		g_Item[i].tbl_adr = NULL;
		g_Item[i].tbl_size = 0;

		// �K�w�A�j���[�V�����p�̏���������
		g_Item[i].parent = NULL;

		g_Item[i].use = TRUE;

	}

	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitItem(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].load)
		{
			UnloadModel(&g_Item[i].model);
			g_Item[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateItem(void)
{
	const XMFLOAT3 playerPos = GetPlayer()->pos;
	const float playerProgress = GetPlayerFieldProgress();

	bool boarderFlag = IsPlayerOutOfBoarder();

	if (boarderFlag)
	{
		for (auto& item : g_Item)
		{
			//item.type = static_cast<itemBehaviorType>(rand() % 3);
			item.use = true;
			item.pos = GetRandomValidPositionAtConjuction();
			item.pos.y += ITEM_OFFSET_Y;


			/*if (item.type == Obstacle)
			{

			}*/

			
		}
		return;
	}

	for (auto& item : g_Item)
	{
		if (item.use == TRUE)
		{
			/*if (item.type == Obstacle)
			{
				continue;
			}*/

			XMFLOAT3 pos = item.pos;
			pos.y -= (ITEM_OFFSET_Y - 0.1f);
			SetPositionShadow(item.shadowIdx, pos);

		}
	}



}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawItem(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	//SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ITEM; i++)
	{
		if (g_Item[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Item[i].scl.x, g_Item[i].scl.y, g_Item[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Item[i].rot.x, g_Item[i].rot.y + XM_PI, g_Item[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Item[i].pos.x, g_Item[i].pos.y, g_Item[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Item[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Item[i].model);

	}




	// �J�����O�ݒ��߂�
	//SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ITEM* GetItem()
{
	return &g_Item[0];
}
