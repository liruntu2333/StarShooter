//=============================================================================
//
// �Q�[������UI���� [GameUI.cpp]
// Author : GP12B183 11 ����
//
//=============================================================================
#include "gameUI.h"
#include "sprite.h"
#include "player.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH_BOX					(100.0f)
#define TEXTURE_HEIGHT_BOX					(50.0f)

#define TEXTURE_WIDTH_HP					(40.0f)
#define TEXTURE_HEIGHT_HP					(40.0f)

#define TEXTURE_WIDTH_MP					(40.0f)
#define TEXTURE_HEIGHT_MP					(40.0f)

#define TEXTURE_PATTERN_DIVIDE_X			(1)														// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y			(1)														// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM					(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT							(1)														// �A�j���[�V�����̐؂�ւ��Wait�l
//
//#define PLAYER_HP_MAX						(5)
//#define PLAYER_MP_MAX						(5)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;					// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };		// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/GAME/hp_1.png",
	"data/TEXTURE/GAME/hp_0.png",
	"data/TEXTURE/GAME/mp_1.png",
	"data/TEXTURE/GAME/mp_0.png",
};

static BOOL	g_Load = FALSE;		// ���������s�������̃t���O

static GameUI_Box		g_UIBox[BOX_MAX];

static GameUI_HP		g_UIHP[PLAYER_HP_MAX];

static GameUI_MP		g_UIMP[PLAYER_MP_MAX];

// ����������
//=============================================================================
HRESULT InitGameUI(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����

	// �{�b�N�X
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_UIBox[i].w = TEXTURE_HEIGHT_BOX;
		g_UIBox[i].h = TEXTURE_WIDTH_BOX;
		g_UIBox[i].pos = XMFLOAT3(TEXTURE_WIDTH_BOX * 0.5f, SCREEN_HEIGHT - TEXTURE_HEIGHT_BOX * i, 0.0f);
		//g_UIBox[i].texNo = TEXTURE_UIBOX;
	}


	// HP
	for (int i = 0; i < PLAYER_HP_MAX; i++)
	{
		g_UIHP[i].w = TEXTURE_WIDTH_HP;
		g_UIHP[i].h = TEXTURE_HEIGHT_HP;
		g_UIHP[i].pos = XMFLOAT3(g_UIBox[BOX_HP].pos.x + i * TEXTURE_WIDTH_HP * 0.5f,
			g_UIBox[BOX_HP].pos.y - TEXTURE_HEIGHT_HP,
			0.0f);
		g_UIHP[i].texNo = TEXTURE_HP_1;
	}

	// MP
	for (int i = 0; i < PLAYER_MP_MAX; i++)
	{
		g_UIMP[i].w = TEXTURE_WIDTH_MP;
		g_UIMP[i].h = TEXTURE_HEIGHT_MP;
		g_UIMP[i].pos = XMFLOAT3(g_UIBox[BOX_MP].pos.x + i * TEXTURE_WIDTH_MP * 0.5f,
			g_UIBox[BOX_MP].pos.y - TEXTURE_HEIGHT_MP,
			0.0f);
		g_UIMP[i].texNo = TEXTURE_MP_1;
	}

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameUI(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGameUI(void)
{


	PLAYER* player = GetPlayer();
	// HP�̍X�V
	{

		for (int i = PLAYER_HP_MAX - 1; i > player->HP - 1; i--)
		{
			g_UIHP[i].texNo = TEXTURE_HP_0;
		}


		for (int i = 0; i < player->HP ; i++)
		{
			g_UIHP[i].texNo = TEXTURE_HP_1;
		}
	}


	// MP�̍X�V
	{
		for (int i = PLAYER_MP_MAX - 1; i > player->MP - 1; i--)

		{
			g_UIMP[i].texNo = TEXTURE_MP_0;
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameUI(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// GameUI��box��`��
	//{
	//	for (int i = 0; i < BOX_MAX; i++)
	//	{
	//		// �e�N�X�`���ݒ�
	//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIBox[i].texNo]);

	//		// UI�̈ʒu��e�N�X�`���[���W�𔽉f
	//		float px = g_UIBox[i].pos.x;
	//		float py = g_UIBox[i].pos.y;
	//		float pw = g_UIBox[i].w;
	//		float ph = g_UIBox[i].h;

	//		float tw = 1.0f;
	//		float th = 1.0f;
	//		float tx = 0.0f;
	//		float ty = 0.0f;

	//		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	//		SetSpriteLTColor(g_VertexBuffer,
	//			px, py, pw, ph,
	//			tx, ty, tw, th,
	//			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//		// �|���S���`��
	//		GetDeviceContext()->Draw(4, 0);
	//	}
	//}



	// HP��`��
	{
		for (int i = 0; i < PLAYER_HP_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIHP[i].texNo]);

			// UI�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_UIHP[i].pos.x;
			float py = g_UIHP[i].pos.y;
			float pw = g_UIHP[i].w;
			float ph = g_UIHP[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}


	// MP��`��
	{
		for (int i = 0; i < PLAYER_MP_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIMP[i].texNo]);

			// UI�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_UIMP[i].pos.x;
			float py = g_UIMP[i].pos.y;
			float pw = g_UIMP[i].w;
			float ph = g_UIMP[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}


}







