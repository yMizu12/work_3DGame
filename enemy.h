//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_ENEMY		(30)		// �G�l�~�[�̐�
#define	ENEMY_SIZE		(15.0f)		// �����蔻��̑傫��

enum
{
	ENEMY_STATE_SPAWN,		// �N�o��
	ENEMY_STATE_ACTIVE,		// �A�N�e�B�u��ԁi�N�o���o�I����j
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;							// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	XMFLOAT3			P0;
	XMFLOAT3			P1;
	XMFLOAT3			P2;
	float				s;

	int					state;				// ���
	int					spawnTimeCnt;		// �N�o���̓����^�C�}�[

	float				spd;				// �ړ��X�s�[�h
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	int					targetGoalNo;		// �W�I�̃S�[���ԍ�

	DISSOLVE			dissolve;			// �f�B�]���u

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

