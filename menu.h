//=============================================================================
//
// ���j���[�̃��f������ [menu.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_MENU				(1)					// ���j���[�̃A�C�e����


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct MENU
{
	XMFLOAT3			pos;		// �|���S���̈ʒu
	XMFLOAT3			rot;		// �|���S���̌���(��])
	XMFLOAT3			scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4			mtxWorld;	// ���[���h�}�g���b�N�X

	BOOL				load;
	DX11_MODEL			model;		// ���f�����

	float				spd;		// �ړ��X�s�[�h
	float				dir;		// ����
	BOOL				use;

	BOOL				isDisplay;	// �\�����ǂ���

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	INTERPOLATION_DATA* tbl_adr;	// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;	// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;	// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	MENU* parent;					// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMenu(void);
void UninitMenu(void);
void UpdateMenu(void);
void DrawMenu(void);

MENU* GetMenu(void);
