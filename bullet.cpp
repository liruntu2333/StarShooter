//=============================================================================
//
// �e���ˏ��� [bullet.cpp]
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
// �}�N����`
//*****************************************************************************
#define	MODEL_BULLET				"data/MODEL/bullet_star.obj"			// �ǂݍ��ރ��f����

#define	BULLET_SPEED				(5.0f)									// �e�̈ړ��X�s�[�h

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static BULLET						g_Bullet[MAX_BULLET];	

static BOOL							g_Load = FALSE;


//=============================================================================
// ����������
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
// �I������
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	// ���f���̉������
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
// �X�V����
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


			// �e�̈ʒu�ݒ�
			SetPositionShadow(bullet.shadowIdx, XMFLOAT3(bullet.pos.x, 0.1f, bullet.pos.z));


			// �t�B�[���h�̊O�ɏo����e����������
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
// �`�揈��
//=============================================================================
void DrawBullet(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y, g_Bullet[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);

			// ���f���`��
			DrawModel(&g_Bullet[i].model);
		}
	}

	

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);



	//SetCullingMode(CULL_MODE_NONE);

	//XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	//// ���_�o�b�t�@�ݒ�
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//// �v���~�e�B�u�g�|���W�ݒ�
	//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//for (int i = 0; i < MAX_BULLET; i++)
	//{
	//	if (g_Bullet[i].use)
	//	{
	//		// ���[���h�}�g���b�N�X�̏�����
	//		mtxWorld = XMMatrixIdentity();

	//		// �X�P�[���𔽉f
	//		mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
	//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	//		// ��]�𔽉f
	//		mtxRot = XMMatrixRotationRollPitchYaw(g_Bullet[i].rot.x, g_Bullet[i].rot.y + XM_PI, g_Bullet[i].rot.z);
	//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//		// �ړ��𔽉f
	//		mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
	//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//		// ���[���h�}�g���b�N�X�̐ݒ�
	//		SetWorldMatrix(&mtxWorld);

	//		XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


	//		// �}�e���A���ݒ�
	//		SetMaterial(g_Bullet[i].material);

	//		// �e�N�X�`���ݒ�
	//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	//		// �|���S���̕`��
	//		GetDeviceContext()->Draw(4, 0);
	//	}
	//}

	//// ���C�e�B���O��L����
	//SetLightEnable(TRUE);
	//SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �e�̃p�����[�^���Z�b�g
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
//			// �e�̐ݒ�
//			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);
//
//			nIdxBullet = nCntBullet;
//
//			// ���ˉ�
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
// �e�̎擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

