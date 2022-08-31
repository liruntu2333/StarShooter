//=============================================================================
//
// �J�������� [camera.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "MathHelper.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	POS_X_CAM_PLAYER			(0.0f)			// �J�����̏����ʒu(X���W)	// �v���[���[�̏ꍇ
#define	POS_Y_CAM_PLAYER			(50.0f)			// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM_PLAYER			(-100.0f)		// �J�����̏����ʒu(Z���W)

#define POS_X_CAM_MENU				(0.0f)			// �J�����̈ʒu(X���W)		// ���j���[�̏ꍇ
#define POS_Y_CAM_MENU				(30.0f)			// �J�����̈ʒu(Y���W)
#define POS_Z_CAM_MENU				(-100.0f)		// �J�����̈ʒu(Z���W)


#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(10000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE_CAMERA	(2.0f)										// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// �J�����̉�]��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera;		// �J�����f�[�^

static float g_ViewAngle = XMConvertToRadians(45.0f);
static float g_ViewAspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

void ApplyThetaPhi();

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = { POS_X_CAM_PLAYER, POS_Y_CAM_PLAYER, POS_Z_CAM_PLAYER };
	g_Camera.at  = { 0.0f, 0.0f, 0.0f };
	g_Camera.up  = { 0.0f, 1.0f, 0.0f };
	g_Camera.theta = 0.0f;
	g_Camera.phi = 0.0f;

	// ���_�ƒ����_�̋������v�Z
	// �v���[���[�̏ꍇ
	float vx_player, vz_player;
	vx_player = POS_X_CAM_PLAYER - g_Camera.at.x;
	vz_player = POS_Z_CAM_PLAYER - g_Camera.at.z;
	g_Camera.lenPlayer = sqrtf(vx_player * vx_player + vz_player * vz_player);
	// ���j���[�̏ꍇ
	float vx_menu, vz_menu;
	vx_menu = POS_X_CAM_MENU - g_Camera.at.x;
	vz_menu = POS_Z_CAM_MENU - g_Camera.at.z;
	g_Camera.lenMenu = sqrtf(vx_menu * vx_menu + vz_menu * vz_menu);

}


//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
	static long prevX = 0;
	static long prevY = 0;

	const long currX = GetMouseX();
	const long currY = GetMouseY();

	if (IsMouseRightPressed())
	{

		const float dx = XMConvertToRadians(0.25f * static_cast<float>(currX - prevX));
		const float dy = XMConvertToRadians(0.25f * static_cast<float>(currY - prevY));

		g_Camera.theta += dx;
		g_Camera.phi += dy;

		g_Camera.phi = MathHelper::Clamp(g_Camera.phi, 0.1f, XM_PI - 0.1f);
	}

	prevX = currX;
	prevY = currY;

#ifdef _DEBUG



#endif



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(void) 
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(g_ViewAngle, g_ViewAspect, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

void LerpCameraPosition(XMFLOAT3 pos, float dir, float tPos)
{
	const XMVECTOR pPos = XMLoadFloat3(&pos);
	const XMVECTOR vDir = XMVector3Normalize({ sinf(dir), 0.0f, cosf(dir), 0.0f });
	const XMVECTOR target = pPos - vDir * g_Camera.lenPlayer;
	const XMVECTOR result = MathHelper::Lerp(XMLoadFloat3(&g_Camera.pos), target, tPos);
	XMStoreFloat3(&g_Camera.pos, result);

	g_Camera.at = pos;

	//ApplyThetaPhi();
}

void LerpCameraPositionAt(XMFLOAT3 playerPos, XMFLOAT3 enemyPos, float dir, float tPos, float tAt)
{
	const XMVECTOR pPos = XMLoadFloat3(&playerPos);
	const XMVECTOR ePos = XMLoadFloat3(&enemyPos);
	const XMVECTOR vDir = XMVector3Normalize(ePos - pPos);
	const XMVECTOR target = pPos - vDir * g_Camera.lenPlayer;
	const XMVECTOR pLerp = MathHelper::Lerp(XMLoadFloat3(&g_Camera.pos), target, tPos);
	XMStoreFloat3(&g_Camera.pos, pLerp);

	const XMVECTOR aLerp = MathHelper::Lerp(XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&enemyPos), tAt);
	XMStoreFloat3(&g_Camera.at, aLerp);
}

void LerpCameraViewAngle(float angle, float t = 1.0f)
{
	g_ViewAngle = MathHelper::Lerp(g_ViewAngle, angle, t);
}

void ApplyThetaPhi()
{
	auto& camera = g_Camera;
	camera.pos.x += camera.at.x + camera.lenPlayer * sinf(camera.phi) * cosf(camera.theta);
	camera.pos.z += camera.at.z + camera.lenPlayer * sinf(camera.phi) * sinf(camera.theta);
	camera.pos.y += camera.at.y + camera.lenPlayer * cosf(camera.phi);
}
