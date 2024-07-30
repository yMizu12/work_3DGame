//=============================================================================
//
// �J�������� [camera.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	CAMERA_POS_X				(0.0f)			// �J�����̏����ʒu(X���W)
#define	CAMERA_POS_Y				(50.0f)			// �J�����̏����ʒu(Y���W)
#define	CAMERA_POS_Z				(-140.0f)		// �J�����̏����ʒu(Z���W)

#define CAMERA_AT_X					(0.0f)			// �J�����̏��������_(X���W)
#define CAMERA_AT_Y					(0.0f)			// �J�����̏��������_(Y���W)
#define CAMERA_AT_Z					(0.0f)			// �J�����̏��������_(Z���W)

#define CAMERA_UP_X					(0.0f)			// �J�����̏�����̐ݒ�(X���W)
#define CAMERA_UP_Y					(1.0f)			// �J�����̏�����̐ݒ�(Y���W)
#define CAMERA_UP_Z					(0.0f)			// �J�����̏�����̐ݒ�(Z���W)

#define CAMERA_ROT_X				(XM_PI/2 +0.5f)	// �J�����̏�����]�p�̐ݒ�(X���W)
#define CAMERA_ROT_Y				(0.0f)			// �J�����̏�����]�p�̐ݒ�(Y���W)
#define CAMERA_ROT_Z				(0.0f)			// �J�����̏�����]�p�̐ݒ�(Z���W)

#define	VIEW_ANGLE					(XMConvertToRadians(50.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT					((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z					(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z					(10000.0f)										// �r���[���ʂ�FarZ�l

#define	CAMERA_VALUE_MOVE						(2.0f)							// �J�����̈ړ���
#define	CAMERA_VALUE_MOVE_2						(4.0f)							// �J�����̈ړ���
#define	CAMERA_VALUE_ROTATE						(XM_PI * 0.015f)				// �J�����̉�]��
#define	CAMERA_VALUE_ROTATE_V					(CAMERA_VALUE_ROTATE * 0.75f)	// �J�����̉�]��
#define	CAMERA_VALUE_ROTATE_V_CORR				(0.008f)						// �J�����̉�]�ʕ␳�l

#define CAMERA_VALUE_MOVE_LEN					(1.5f)
#define CAMERA_VALUE_ROTATE_LIMIT_X_TOP			(170.0f)
#define CAMERA_VALUE_ROTATE_LIMIT_X_DOWN		(10.0f)		// �J������������ɂȂ���E�p�x
#define CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR	(80.0f)
#define CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_NEAR	(50.0f)
#define CAMERA_VALUE_ADD_HEIGHT_AT_FAR			(0.9f)
#define CAMERA_VALUE_ADD_HEIGHT_AT_NEAR			(0.5f)



//
static CAMERA			camera;		// �J�����f�[�^

static float			cameraBaseLength = 0.0f;			// �v���C���[�Ɋ���ĂȂ����̃J��������
static float			atHeight = 0.0f;					// �v���C���[�Ɋ�������̒����_�̍����̑���

static int				viewPortType = TYPE_FULL_SCREEN;

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	camera.pos = { CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z };
	camera.at  = { CAMERA_AT_X, CAMERA_AT_Y, CAMERA_AT_Z };
	camera.up  = { CAMERA_UP_X, CAMERA_UP_Y, CAMERA_UP_Z };
	camera.rot = { CAMERA_ROT_X, CAMERA_ROT_Y, CAMERA_ROT_Z };

	// ���_�ƒ����_�̋������v�Z
	float vx;	// X���W
	float vz;	// Z���W
	float vxz;	// X���W�EZ���W�̕�����
	float vy;	// Y���W

	vx = camera.pos.x - camera.at.x;
	vz = camera.pos.z - camera.at.z;
	vxz = sqrtf(vx * vx + vz * vz);

	vy = camera.pos.y - camera.at.y;

	cameraBaseLength = sqrtf(vxz * vxz + vy * vy);
	camera.len = cameraBaseLength;
	
	// �r���[�|�[�g�^�C�v�̏�����
	SetViewPort(viewPortType);
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

	if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, STICK_RIGHT_R))
	{// ���_����u���v
		camera.rot.y += CAMERA_VALUE_ROTATE;
		if (camera.rot.y > XM_PI)
		{
			camera.rot.y -= XM_PI * 2.0f;
		}
	}

	if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, STICK_LEFT_R))
	{// ���_����u�E�v
		camera.rot.y -= CAMERA_VALUE_ROTATE;
		if (camera.rot.y < -XM_PI)
		{
			camera.rot.y += XM_PI * 2.0f;
		}
	}


	// �X�e�B�b�N����ɓ|�������ɁA�J�������������
	if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, STICK_UP_R))
	{// �J������]�u��v

		if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			camera.rot.x -= CAMERA_VALUE_ROTATE_V + CAMERA_VALUE_ROTATE_V_CORR;
		}
		else
		{
			camera.rot.x -= CAMERA_VALUE_ROTATE_V;
		}


		// ������̌��E�p�x�𒴂��ăJ���������Ȃ��悤�ɂ���
		if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN))
		{
			camera.rot.x = XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN);
		}


		if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			if (camera.rot.x > XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN))
			{
				atHeight += CAMERA_VALUE_ADD_HEIGHT_AT_FAR;

			}
		}

	}

	// �X�e�B�b�N�����ɓ|�������ɁA�J��������������
	if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, STICK_DOWN_R))
	{// �J������]�u���v

		if (camera.rot.x <= XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			camera.rot.x += CAMERA_VALUE_ROTATE_V + CAMERA_VALUE_ROTATE_V_CORR;
		}
		else
		{
			camera.rot.x += CAMERA_VALUE_ROTATE_V;
		}

		// �������̌��E�p�x�𒴂��ăJ���������Ȃ��悤�ɂ���
		if (camera.rot.x > XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_TOP))
		{
			camera.rot.x = XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_TOP);
		}


		if (camera.rot.x <= XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			atHeight -= CAMERA_VALUE_ADD_HEIGHT_AT_FAR;

		}
		else if (camera.rot.x > XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			atHeight = 0.0f;
		}

	}


	if (GetKeyboardPress(DIK_T))
	{// �����_�ړ��u��v
		camera.at.y += CAMERA_VALUE_MOVE;
	}

	if (GetKeyboardPress(DIK_B))
	{// �����_�ړ��u���v
		camera.at.y -= CAMERA_VALUE_MOVE;
	}

	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		//camera.len -= CAMERA_VALUE_MOVE;
		camera.len -= CAMERA_VALUE_MOVE_2;
		camera.pos.x = camera.at.x - sinf(camera.rot.y) * camera.len;
		camera.pos.z = camera.at.z - cosf(camera.rot.y) * camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		//camera.len += CAMERA_VALUE_MOVE;
		camera.len += CAMERA_VALUE_MOVE_2;
		camera.pos.x = camera.at.x - sinf(camera.rot.y) * camera.len;
		camera.pos.z = camera.at.z - cosf(camera.rot.y) * camera.len;
	}

	// �J�����������ɖ߂�
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}


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
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&camera.pos), XMLoadFloat3(&camera.at), XMLoadFloat3(&camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&camera.mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&camera.mtxProjection, mtxProjection);

	SetShaderCamera(camera.pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &camera;
}

//=============================================================================
// �r���[�|�[�g�̐ݒ�
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	viewPortType = type;

	// �r���[�|�[�g�ݒ�
	switch (viewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return viewPortType;
}



// �J�����̎��_�ƒ����_���Z�b�g
void SetCameraAT(XMFLOAT3 pos)
{
	// �J�����̒����_�������̍��W��
	pos.y += atHeight;
	camera.at = pos;

	camera.pos.x = camera.at.x - sinf(camera.rot.x) * sinf(camera.rot.y) * camera.len;
	camera.pos.z = camera.at.z - sinf(camera.rot.x) * cosf(camera.rot.y) * camera.len;


	if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
	{
		camera.pos.y = camera.at.y - cosf(XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR)) * camera.len;
	}
	else
	{
		camera.pos.y = camera.at.y - cosf(camera.rot.x) * camera.len;
	}

}

