//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET				(256)	// �e�ő吔

#define	BULLET_ATTACK_SIZE		(5.0f)	// �����蔻��̑傫��

#include <memory>

#include "MathHelper.h"

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class BezierCurve;
struct ENEMY;

struct BULLET
{
	XMFLOAT3			pos;			// �|���S���̈ʒu
	XMFLOAT3			rot;			// �|���S���̌���(��])
	XMFLOAT3			scl;			// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4			mtxWorld;		// ���[���h�}�g���b�N�X

	BOOL				load;
	DX11_MODEL			model;			// ���f�����
	
	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA* tbl_adr;		// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;		// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;		// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	BULLET*				parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	float				spd;			// �ړ���
	int					shadowIdx;		// �eID
	float				attackSize;		// �����蔻��̑傫��
	BOOL				use;			// �g�p���Ă��邩�ǂ���

	std::unique_ptr<BezierCurveQuadratic> curve = nullptr;
	float				flyingTime;
	float				hitTime;
	ENEMY*				target;
} ;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);
int SetBullet(const std::array<XMFLOAT3, 3>& controlPoints, float tHit, ENEMY* target);

BULLET *GetBullet(void);

