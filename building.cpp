//=============================================================================
//
// �������f������ [building.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "building.h"

#include "light.h"
#include "shadow.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BUILDING_LIGHTPOSTSINGLE			"data/MODEL/lightpostSingle.obj"			// �ǂݍ��ރ��f����

#define	VALUE_ROTATE			(XM_PI * 0.02f)				// ��]��

#define BUILDING_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define BUILDING_OFFSET_Y		(0.0f)						// BUILDING�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
enum BuildingType : int
{
	
};

void GetYOffset(XMFLOAT3& pos)
{
	pos.y = GetFieldHeight(pos.x, pos.z);
}

void SetStreetLight(const BUILDING& building, int lightIdx)
{
	LIGHT* light = GetLightData(lightIdx);
	XMFLOAT3 lightPos{ building.pos };
	lightPos.y += 20.0f;

	light->Position    = lightPos;
	light->Diffuse     = { 0.7f, 0.7f, 0.7f, 1.0f };
	light->Ambient     = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light->Attenuation = 500.0f;
	light->Type        = LIGHT_TYPE_POINT;
	light->Enable      = TRUE;
	SetLightData(lightIdx, light);
}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BUILDING			g_Building[MAX_BUILDING];		

static BOOL				g_Load = FALSE;

static char* g_BuildingName[MAX_BUILDING] = {
	"data/MODEL/lightpostSingle.obj",
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBuilding(void)
{

	for (int i = 0; i < MAX_BUILDING; i++)
	{
		g_Building[i].load = FALSE;

		g_Building[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Building[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Building[i].scl = XMFLOAT3(5.0f, 5.0f, 5.0f);


		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Building[0].model, &g_Building[0].diffuse[0]);

		XMFLOAT3 pos = g_Building[i].pos;
		pos.y -= (BUILDING_OFFSET_Y - 0.1f);
		g_Building[i].shadowIdx = CreateShadow(pos, BUILDING_SHADOW_SIZE, BUILDING_SHADOW_SIZE);

		g_Building[i].move_time = 0.0f;

		g_Building[i].tbl_adr = NULL;
		g_Building[i].tbl_size = 0;

		g_Building[i].use = TRUE;

	}

	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[0].model);
	g_Building[0].load = TRUE;
	g_Building[0].pos = XMFLOAT3(40.0f, 0.0f, 100.0f);
	GetYOffset(g_Building[0].pos);
	g_Building[0].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	SetStreetLight(g_Building[0], 2);

	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[1].model);
	g_Building[1].load = TRUE;
	g_Building[1].pos = XMFLOAT3(40.0f, 0.0f, 300.0f);
	GetYOffset(g_Building[1].pos);
	g_Building[1].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	SetStreetLight(g_Building[1], 3);

	LoadModel(MODEL_BUILDING_LIGHTPOSTSINGLE, &g_Building[2].model);
	g_Building[2].load = TRUE;
	g_Building[2].pos = XMFLOAT3(40.0f, 0.0f, 500.0f);
	GetYOffset(g_Building[2].pos);
	g_Building[2].rot = XMFLOAT3(0.0f, XM_PIDIV2, 0.0f);
	SetStreetLight(g_Building[2], 4);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBuilding(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_BUILDING; i++)
	{
		if (g_Building[i].load)
		{
			UnloadModel(&g_Building[i].model);
			g_Building[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBuilding(void)
{

	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBuilding(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	//SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_BUILDING; i++)
	{
		if (g_Building[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Building[i].scl.x, g_Building[i].scl.y, g_Building[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Building[i].rot.x, g_Building[i].rot.y + XM_PI, g_Building[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Building[i].pos.x, g_Building[i].pos.y, g_Building[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Building[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Building[i].model);


	}




	// �J�����O�ݒ��߂�
	//SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
BUILDING* GetBuilding()
{
	return &g_Building[0];
}