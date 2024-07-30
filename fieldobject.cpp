//=============================================================================
//
// �t�B�[���h�I�u�W�F�N�g���f������ [fieldobject.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "collision.h"
#include "input.h"
#include "debugproc.h"
#include "fieldobject.h"
#include "shadow.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_FLOOR				"data/MODEL/test_floor000.obj"		// �ǂݍ��ރ��f����
#define	MODEL_SLOPE				"data/MODEL/test_slope000.obj"		// �ǂݍ��ރ��f����
#define	MODEL_PILLAR			"data/MODEL/pillar.obj"				// �ǂݍ��ރ��f����
#define	MODEL_HOME				"data/MODEL/home.obj"				// �ǂݍ��ރ��f����
#define	MODEL_ROOF				"data/MODEL/roof.obj"				// �ǂݍ��ރ��f����
#define	MODEL_GOAL				"data/MODEL/goal.obj"				// �ǂݍ��ރ��f����
#define	MODEL_AREA_NOT_ENTRY_P	"data/MODEL/area_not_entry_p.obj"	// �ǂݍ��ރ��f����


//
static FOBJECT	fieldObject[MAX_FIELD_OBJECT];	// �t�B�[���h�I�u�W�F�N�g
static GOAL		goal[MAX_GOAL];
static AREA		area[MAX_AREA];


//=============================================================================
// ����������
//=============================================================================
HRESULT InitFieldObject(void)
{
	for (int i = 0; i < MAX_FIELD_OBJECT; i++)
	{
		fieldObject[i].load = FALSE;	//�����ł͂܂����f�������[�h���Ȃ�
		
		fieldObject[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		fieldObject[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		fieldObject[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		
		fieldObject[i].use = FALSE;		// TRUE:�g�p��

	}

	// �S�[���̏�����
	for (int i = 0; i < MAX_GOAL; i++)
	{
		goal[i].use = FALSE;
	}

	// �G���A�̏�����
	for (int i = 0; i < MAX_AREA; i++)
	{
		area[i].max = XMFLOAT3(-100000.0f, 0.0f, -100000.0f);	// �G���A�̗̈�v�Z���ɁA�ŏ��̔�r�Ł�
		area[i].min = XMFLOAT3(100000.0f, 0.0f, 100000.0f);		// �K���ő�l����эŏ��l�ɂȂ�悤�ɏ[���ɏ������E�傫���l��ݒ�
		area[i].use = FALSE;
	}

	char path[256] = ".\\data\\MAYA_DATA\\fieldObjectData.txt";

	FILE* file;
	file = fopen(path, "rt");
	if (file == NULL)
	{
		printf("�G���[:LoadModel %s \n", path);
		return S_OK;
	}

	int num = 0;
	float param[9];
	fscanf(file, "%d", &num);	// �I�u�W�F�N�g�̑�����ǂݍ���

	for (int i = 0; i < num; i++)
	{

		//�p�����[�^9��(pos:xyz, rot:xyz, scl:xyz)������
		for (int j = 0; j < 9; j++)
		{
			fscanf(file, "%f", &param[j]);
		}

		int objType;
		fscanf(file, "%d", &objType);	// �I�u�W�F�N�g�^�C�v�̓ǂݍ���

		int totalVtx = 0;

		switch (objType)
		{
			case F_OBJ_AREA_NOT_ENTRY_P:
				fscanf(file, "%d", &totalVtx);		//	���_���̓ǂݍ��݁i����͂S�Œ肾���A�g�������l�������`�j
				XMFLOAT3 vtxPos[MAX_AREA_VERTEX];

				for (int k = 0; k < totalVtx; k++)	// ���_���Ԃ�J��Ԃ�
				{					
					// xyz��ǂݍ���

					float x, y, z;

					fscanf(file, "%f", &x);
					fscanf(file, "%f", &y);
					fscanf(file, "%f", &z);

					vtxPos[k].x = x * SCALE_MODEL;
					vtxPos[k].y = y * SCALE_MODEL;
					vtxPos[k].z = z * SCALE_MODEL;

				}

				for (int k = 0; k < MAX_AREA; k++)
				{
					if (area[k].use == TRUE) continue;

					area[k].vtxPos[0] = vtxPos[0];
					area[k].vtxPos[1] = vtxPos[1];
					area[k].vtxPos[2] = vtxPos[2];
					area[k].vtxPos[3] = vtxPos[3];
					area[k].areaType = AREA_NOT_ENTRY_P;
					area[k].use = TRUE;

					calcArea(&area[k]);

					break;
				}

				SetFieldObject(objType, XMFLOAT3(param[0] * SCALE_MODEL, param[1] * SCALE_MODEL, param[2] * SCALE_MODEL), XMFLOAT3(param[3], param[4], param[5]), XMFLOAT3(param[6], param[7], param[8]));

				break;

			default:
				SetFieldObject(objType, XMFLOAT3(param[0] * SCALE_MODEL, param[1] * SCALE_MODEL, param[2] * SCALE_MODEL), XMFLOAT3(param[3], param[4], param[5]), XMFLOAT3(param[6], param[7], param[8]));
				break;
		}
		
	}

	fclose(file);	//�d�v�I�I�F�J�����t�@�C�������

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFieldObject(void)
{
	for (int i = 0; i < MAX_FIELD_OBJECT; i++)
	{
		if (fieldObject[i].load)
		{
			UnloadModel(&fieldObject[i].model);
			fieldObject[i].load = FALSE;
		}

	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateFieldObject(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFieldObject(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_FIELD_OBJECT; i++)
	{
		if (fieldObject[i].use == FALSE) continue;

		int objType = fieldObject[i].objType;
		switch (objType)
		{
			case F_OBJ_AREA_NOT_ENTRY_P:
			case F_OBJ_GOAL:
				SetLightEnable(FALSE);	// ���C�e�B���O�𖳌���
				break;
		}


		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(fieldObject[i].scl.x, fieldObject[i].scl.y, fieldObject[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(fieldObject[i].rot.x, fieldObject[i].rot.y + XM_PI, fieldObject[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(fieldObject[i].pos.x, fieldObject[i].pos.y, fieldObject[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&fieldObject[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&fieldObject[i].model);

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

	}
}


//=============================================================================
// �t�B�[���h�I�u�W�F�N�g�̎擾
//=============================================================================
FOBJECT *GetFieldObject(void)
{
	return &fieldObject[0];
}


//=============================================================================
// �t�B�[���h�I�u�W�F�N�g�̃Z�b�g
//=============================================================================

void SetFieldObject(int objType, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scl)
{
	for (int i = 0; i < MAX_FIELD_OBJECT; i++)
	{

		if (fieldObject[i].use == TRUE)
		{
			// �g�p����������X�L�b�v
			continue;
		}

		fieldObject[i].objType = objType;
		fieldObject[i].pos = pos;
		fieldObject[i].rot = rot;
		fieldObject[i].scl = scl;

		XMFLOAT3 pos = fieldObject[i].pos;

		switch (objType)
		{
			case F_OBJ_FLOOR:
				LoadModel(MODEL_FLOOR, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				break;

			case F_OBJ_SLOPE:
				LoadModel(MODEL_SLOPE, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				break;

			case F_OBJ_HOME:
				LoadModel(MODEL_HOME, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				break;

			case F_OBJ_ROOF:
				LoadModel(MODEL_ROOF, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				break;

			case F_OBJ_PILLAR:
				LoadModel(MODEL_PILLAR, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				break;

			case F_OBJ_AREA_NOT_ENTRY_P:
				LoadModel(MODEL_AREA_NOT_ENTRY_P, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				// ���f���̐F��ύX
				for (int j = 0; j < fieldObject[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&fieldObject[i].model, j, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f));
				}

				break;

			case F_OBJ_GOAL:
				LoadModel(MODEL_GOAL, &fieldObject[i].model);
				fieldObject[i].load = TRUE;
				GetModelDiffuse(&fieldObject[i].model, &fieldObject[i].diffuse[0]);

				// ���f���̐F��ύX
				for (int j = 0; j < fieldObject[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&fieldObject[i].model, j, XMFLOAT4(1.0f, 1.0f, 0.0f, 0.3f));
				}

				for (int j = 0; j < MAX_GOAL; j++)
				{
					if (goal[j].use == TRUE) continue;

					goal[j].pos = fieldObject[i].pos;
					goal[j].use = TRUE;
					break;
				}
				
				fieldObject[i].shadowIdx = CreateShadow(pos, 0.0f, 0.0f);
				break;

		}
		
		fieldObject[i].use = TRUE;	// �g�p���ɕύX
		break;
	}
}

//=============================================================================
// �S�[���I�u�W�F�N�g�̎擾
//=============================================================================
GOAL* GetGoal(void)
{
	return &goal[0];
}


//=============================================================================
// �G���A�I�u�W�F�N�g�̎擾
//=============================================================================
AREA* GetArea(void)
{
	return &area[0];
}


//=============================================================================
// �N���s�G���A�̌v�Z
//=============================================================================
void calcArea(AREA *area)
{
	for (int i = 0; i < MAX_AREA_VERTEX; i++)
	{
		XMFLOAT3 pos = area->vtxPos[i];
		XMFLOAT3 max = area->max;
		XMFLOAT3 min = area->min;

		if (pos.x < min.x) min.x = pos.x;
		if (pos.x > max.x) max.x = pos.x;
		if (pos.z < min.z) min.z = pos.z;
		if (pos.z > min.z) max.z = pos.z;

		area->max = max;
		area->min = min;
		
	}
}

//=============================================================================
// �g�p���G���A�̌��擾
//=============================================================================
int GetUsedArea(void)
{
	int ans = 0;

	for (int i = 0; i < MAX_AREA; i++)
	{
		if (area[i].use)
		{
			ans++;
		}
	}

	return ans;
}
