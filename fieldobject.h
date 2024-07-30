//=============================================================================
//
// �t�B�[���h�I�u�W�F�N�g�̃��f������ [fieldobject.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_FIELD_OBJECT	(200)	// �t�B�[���h�I�u�W�F�N�g�̍ő吔
#define MAX_GOAL			(3)		// �S�[���̍ő吔
#define MAX_AREA			(6)		// �G���A�̍ő吔
#define MAX_AREA_VERTEX		(4)		// �G���A�̍ő咸�_���i����͎l�p�`�Ȃ̂łS�j

enum
{
	F_OBJ_FLOOR,
	F_OBJ_SLOPE,
	F_OBJ_PILLAR,
	F_OBJ_HOME,
	F_OBJ_ROOF,
	F_OBJ_AREA_NOT_ENTRY_P = 98,
	F_OBJ_GOAL = 99,
};

enum
{
	AREA_NOT_ENTRY_P,	// �v���C���[�N���s�G���A
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct FOBJECT
{
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;							// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	int					objType;
};

struct GOAL
{
	XMFLOAT3			pos;				// ���f���̈ʒu
	BOOL				use;
};

struct AREA
{
	XMFLOAT3			vtxPos[MAX_AREA_VERTEX];	// ���f���̒��_���W
	XMFLOAT3			max;
	XMFLOAT3			min;
	BOOL				use;
	int					areaType;
	
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFieldObject(void);
void UninitFieldObject(void);
void UpdateFieldObject(void);
void DrawFieldObject(void);

FOBJECT *GetFieldObject(void);

void SetFieldObject(int objType, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scl);

GOAL* GetGoal(void);

AREA* GetArea(void);
void calcArea(AREA* area);
int GetUsedArea(void);
