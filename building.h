//=============================================================================
//
// �������f������ [building.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_BUILDING		(25)					// �����̐�

#define	ENEMY_SIZE			(5.0f)					// �����蔻��̑傫��


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum BuildingType : int;

struct BUILDING
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	XMFLOAT3			pos_center;

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	BUILDING*			parent;				// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	BuildingType		type;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBuilding(void);
void UninitBuilding(void);
void UpdateBuilding(void);
void DrawBuilding(void);

BUILDING* GetBuilding(void);

