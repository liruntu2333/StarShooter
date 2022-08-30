//=============================================================================
//
// �A�C�e�����f������ [item.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ITEM		(10)					// item�̐�
#define MAX_ITEM_GOOD	(6)
#define MAX_ITEM_WORSE	(MAX_ITEM - MAX_ITEM_GOOD)	

#define	ITEM_SIZE		(5.0f)				// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum ItemType : int;

struct ITEM
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)
	XMFLOAT3			velocity;

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float				spd;				// �ړ��X�s�[�h

	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	ITEM* parent;				// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	ItemType			type;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

ITEM* GetItem(void);

