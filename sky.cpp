//=============================================================================
//
// ��̏��� [player.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "sky.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	SKY_MODEL			"data/MODEL/sky.obj"			// �ǂݍ��ރ��f����
#define SKY_SCL				(5.0f)
#define	SKY_VALUE_ROTATE	(XM_PI * 0.0002f)				// ��]��

//
static SKY			sky;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(void)
{
	LoadModel(SKY_MODEL, &sky.model);
	sky.load = TRUE;

	sky.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sky.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sky.scl = XMFLOAT3(SKY_SCL, SKY_SCL, SKY_SCL);

	sky.use = TRUE;			// TRUE:�g�p��

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	// ���f���̉������
	if (sky.load == TRUE)
	{
		UnloadModel(&sky.model);
		sky.load = FALSE;
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{
	sky.rot.y += SKY_VALUE_ROTATE;

	if (sky.rot.y > -XM_PI)
	{
		sky.rot.y -= XM_PI * 2.0f;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(sky.scl.x, sky.scl.y, sky.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(sky.rot.x, sky.rot.y + XM_PI, sky.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(sky.pos.x, sky.pos.y, sky.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&sky.mtxWorld, mtxWorld);

	// ���f���`��
	DrawModel(&sky.model);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}



