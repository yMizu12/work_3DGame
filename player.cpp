//=============================================================================
//
// �v���C���[�̏��� [player.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "fieldobject.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player2.obj"	// �ǂݍ��ރ��f����
#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)			// ��]��

#define PLAYER_INIT_POS_X	(-50.0f)
#define PLAYER_INIT_POS_Z	(-50.0f)
#define PLAYER_SHADOW_SIZE	(0.8f)						// �e�̑傫��
#define PLAYER_OFFSET_Y		(24.0f)						// �v���C���[�̑��������킹��


//
static PLAYER	player;
static float	rotY = 0.0f;
static LIGHT	light;
static BOOL		lightTest = TRUE;	// ���ؗp
BulletManager	bulletMng;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	player.load = TRUE;
	LoadModel(MODEL_PLAYER, &player.model);

	player.pos = XMFLOAT3(PLAYER_INIT_POS_X, PLAYER_OFFSET_Y, PLAYER_INIT_POS_Z);
	player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	player.spd = 0.0f;			// �ړ��X�s�[�h�N���A

	player.use = TRUE;			// TRUE:�����Ă�
	player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	// �v���C���[�p�̉e���쐬
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	// �L�[�����������̃v���C���[�̌���
	rotY = 0.0f;

	// �N�H�[�^�j�I���̏�����
	XMStoreFloat4(&player.Quaternion, XMQuaternionIdentity());

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (player.load == TRUE)
	{
		UnloadModel(&player.model);
		player.load = FALSE;
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	player.spd *= 0.7f;
	if (player.spd < 0.1f) player.spd = 0.0f;

	// �ړ�����
	if (GetKeyboardPress(DIK_LEFT))
	{
		player.spd = VALUE_MOVE;
		rotY = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		player.spd = VALUE_MOVE;
		rotY = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		player.spd = VALUE_MOVE;
		rotY = XM_PI;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{
		player.spd = VALUE_MOVE;
		rotY = 0.0f;
	}

	DIJOYSTATE2 pad = GetPad();

	// ���X�e�B�b�N�ɂ����ꂩ�̕����ɓ��͂̂������ꍇ
	if (IsButtonPressed(0, STICK_LEFT_L) || IsButtonPressed(0, STICK_RIGHT_L)
		|| IsButtonPressed(0, STICK_UP_L) || IsButtonPressed(0, STICK_DOWN_L) )
	{
		player.spd = VALUE_MOVE;
		rotY = atan2f(float(pad.lX), float(pad.lY)) * -1;
	}


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		player.pos.z = player.pos.x = 0.0f;
		player.spd = 0.0f;
		rotY = 0.0f;
	}
#endif

	XMFLOAT3 oldPos = player.pos;
	XMFLOAT3 nowPos = player.pos;


	// �L�[���͂����������i�ړ��X�s�[�h��0���傫���ꍇ�j
	if(player.spd > 0.0f)
	{	// �����������Ƀv���C���[���ړ�������
		// �����������Ƀv���C���[���������Ă��鏊
		player.rot.y = rotY + cam->rot.y;	// 

		//  y ��   2�������W�́Ax����O�����ɐ��Ay���������ɐ��̍��W�n
		//      �� 0�x�A180�x�̎��ɁA����n3�������W�� z���́{�|�����������悤�ɂ��邽�߁Az���̒l��cos�Ōv�Z�A
		//      x  90�x�A270�x�̎��ɁA����n3�������W�� x���́{�|�����������悤�ɂ��邽�߁Ax���̒l��sin�Ōv�Z
		nowPos.x -= sinf(player.rot.y) * player.spd;
		nowPos.z -= cosf(player.rot.y) * player.spd;

		nowPos.y = player.pos.y;

		AREA* area = GetArea();
		int usedAreaNum = GetUsedArea();

		for (int i = 0; i < usedAreaNum; i++)
		{
			if (nowPos.x > area[i].min.x &&
				nowPos.x < area[i].max.x &&
				nowPos.z > area[i].min.z &&
				nowPos.z < area[i].max.z)
			{
				player.pos = oldPos;
				break;
			}
			else
			{
				player.pos = nowPos;
			}
		}

		// �}�b�v�O�ɏo��Ȃ��悤�ɂ���
		if (nowPos.x < -1500 || nowPos.x > 1500 || nowPos.z < -1000 || nowPos.z > 1000)
		{
			player.pos = oldPos;
		}

	}


	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 HitPosition;		// ��_
	XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
	BOOL ans = RayHitField(player.pos, &HitPosition, &Normal);
	if (ans)
	{
		player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


	// �e���ˏ���
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
	{
		XMFLOAT3 pos = player.pos;
		pos.y += 5.0f;

		bulletMng.SetBullet(pos, player.rot);
	}


	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(player.shadowIdx, pos);


	// �|�C���g���C�g
	{
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = player.pos;
		pos.y += 70.0f;		// ���̏キ�炢�̈ʒu

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		light->Attenuation = 250.0f;
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;

#ifdef _DEBUG
		// ���C�g�̐F�m�F
		if (GetKeyboardTrigger(DIK_J))
		{
			lightTest = lightTest ? FALSE : TRUE;
		}

		if (lightTest == TRUE)
		{
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

		}
		else
		{
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		}

#endif


		SetLightData(1, light);
	}



	//////////////////////////////////////////////////////////////////////
	// �p������
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;


	player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&player.Quaternion, quat);

#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("Player X:%f Y:%f Z:%f N:%f\n", player.pos.x, player.pos.y, player.pos.z, Normal.y);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y + XM_PI, player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �N�H�[�^�j�I���𔽉f
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&player.mtxWorld, mtxWorld);


	// ���f���`��
	DrawModel(&player.model);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &player;
}

