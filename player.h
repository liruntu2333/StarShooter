//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(1)					// �v���C���[�̐�

#define	PLAYER_SIZE		(5.0f)				// �����蔻��̑傫��

#define PLAYER_HP_MAX	(5)

#define PLAYER_MP_MAX	(5)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct PLAYER
{
	XMFLOAT3			pos;		// �|���S���̈ʒu
	XMFLOAT3			rot;		// �|���S���̌���(��])
	XMFLOAT3			scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4			mtxWorld;	// ���[���h�}�g���b�N�X

	BOOL				load;
	DX11_MODEL			model;		// ���f�����

	float				spd;		// �ړ��X�s�[�h
	float				dir;		// ����
	float				size;		// �����蔻��̑傫��
	int					shadowIdx;	// �e��Index
	BOOL				use;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA	*tbl_adr;	// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;	// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;	// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;	// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			quaternion;	// �N�H�[�^�j�I��
	XMFLOAT3			upVector;	// �����������Ă��鏊

	// ����
	int					HP;
	int					MP;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

/**
 * \brief Every object moving correspondingly to player should check this flag to decide
 * whether wrap their position in field or not.
 *
 * \return An int type flag, last four bits used for representing a player's position's state
 * to field's boarder.
 * e.g. 0b0000 represents isn't out of any boarder.
 *      0b0001 represents out of Z plus boarder.
 */
int IsPlayerEndOfBoarder();
