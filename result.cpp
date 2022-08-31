//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX						(4)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_BG				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT_BG				(SCREEN_HEIGHT)	// 

#define TEXTURE_WIDTH_TEXT				(300)
#define TEXTURE_HEIGHT_TEXT				(300)
#define TEXT_DISTANCE					(110)

#define BG_MAX							(3)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/RESULT/resultbg_0.png",
	"data/TEXTURE/RESULT/resultbg_1.png",
	"data/TEXTURE/RESULT/resultbg_2.png",
	"data/TEXTURE/TITLE/text_English.png",
};


static RESULT		g_ResultBG[BG_MAX];

static RESULT		g_ResultText;

static int			g_ResultScore = 0;

static BOOL			g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	ID3D11Device *pDevice = GetDevice();

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

	for (int i = 0; i < BG_MAX; i++)
	{
		g_ResultBG[i].w = TEXTURE_WIDTH_BG;
		g_ResultBG[i].h = TEXTURE_HEIGHT_BG;
		g_ResultBG[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_ResultBG[i].texNo = i;
	}


	// �����̕`��̏�����
	g_ResultText.w = TEXTURE_WIDTH_TEXT;
	g_ResultText.h = TEXTURE_HEIGHT_TEXT;
	g_ResultText.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_ResultText.texNo = 3;

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_sample002);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_GAME);
	}
	


#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
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

	// ���U���g�̔w�i��`��
	{
		for (int i = 0; i < BG_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultBG[i].texNo]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor_LeftTop(g_VertexBuffer,
				g_ResultBG[i].pos.x, g_ResultBG[i].pos.y, g_ResultBG[i].w, g_ResultBG[i].h,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}

	}

	


	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultText.texNo]);

		// UI�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_ResultText.pos.x;
		float py = g_ResultText.pos.y;
		float pw = g_ResultText.w;
		float ph = g_ResultText.h;

		float tw = 1.0f / 17;
		float th = 1.0f / 12;

		//C
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 6, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//O
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 8, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//N
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//T
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 1, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//I
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 11, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//N
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, py, pw, ph,
			tw * 4, th * 8, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//U
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, py, pw, ph,
			tw * 3, th * 9, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//E
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, py, pw, ph,
			tw * 3, th * 7, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//?
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor_LeftTop(g_VertexBuffer,
			px + TEXT_DISTANCE * 8, py, pw, ph,
			tw * 1, th * 10, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �ŏI�X�R�A�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_ResultText.texNo]);

		// ��������������
		int number = g_ResultScore;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// ���̌���
			number /= 10;

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = 220.0f - i*TEXT_DISTANCE*0.4f;	// �X�R�A�̕\���ʒuX
			float py = 220.0f;						// �X�R�A�̕\���ʒuY
			float pw = TEXTURE_WIDTH_TEXT*0.4f;					// �X�R�A�̕\����
			float ph = TEXTURE_WIDTH_TEXT*0.4f;					// �X�R�A�̕\������

			float tw = 1.0f / 17;				// �e�N�X�`���̕�
			float th = 1.0f / 12;				// �e�N�X�`���̍���
			float tx = 0;
			if (x == 0)
			{
				tx = 10 * tw;
			}
			else
			{
				tx = x * tw;					// �e�N�X�`���̍���X���W
			}
			float ty = 6 * th;					// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor_LeftTop(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

	}



}




