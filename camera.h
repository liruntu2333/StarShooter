//=============================================================================
//
// �J�������� [camera.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct CAMERA
{
	XMFLOAT4X4			mtxView;		// �r���[�}�g���b�N�X
	XMFLOAT4X4			mtxInvView;		// �r���[�}�g���b�N�X
	XMFLOAT4X4			mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
	
	XMFLOAT3			pos;			// �J�����̎��_(�ʒu)
	XMFLOAT3			at;				// �J�����̒����_
	XMFLOAT3			up;				// �J�����̏�����x�N�g��
	float				theta;
	float				phi;
	
	float				lenPlayer;			// �J�����̎��_�ƒ����_�̋���(�v���[���[�̏ꍇ)
	float				lenMenu;			// �J�����̎��_�ƒ����_�̋���(���j���[�̏ꍇ)
};


enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_NONE,

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA *GetCamera(void);

void LerpCameraPosition(XMFLOAT3 pos, float dir, float tPos = 1.0f);
//void SetCameraAtMenu(XMFLOAT3 pos, float dir, float t = 1.0f);
void LerpCameraPositionAt(XMFLOAT3 playerPos, XMFLOAT3 enemyPos, float dir, float tPos, float tAt);
void LerpCameraViewAngle(float angle, float t);
