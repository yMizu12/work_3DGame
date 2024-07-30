//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "player.h"
#include "shadow.h"
#include "sound.h"
#include "collision.h"
#include "fieldobject.h"
#include "ui.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY				"data/MODEL/ghost.obj"	// �ǂݍ��ރ��f����

#define	VALUE_MOVE				(1.0f)			// �ړ���
#define	VALUE_ROTATE			(XM_PI * 0.02f)	// ��]��

#define ENEMY_SHADOW_SIZE		(0.6f)			// �e�̑傫��
#define ENEMY_OFFSET_Y			(25.0f)			// �G�l�~�[�̑��������킹��
#define ENEMY_SCALE				(2.0f)			// �G�l�~�[�̑傫��
#define ENEMY_ROT				(0.0f)			// �G�l�~�[��������
#define ENEMY_INTERVAL			(5.0f)			// �G�l�~�[�Ԃ̊Ԋu
#define ENEMY_GOAL_INTERVAL		(15.0f)			// �G�l�~�[�̖ڕW���B���̓����蔻��̑傫��

#define SPAWN_TIME				(120)			// �N�o�ɗv���鎞��
#define SPAWN_SPACE_INTERVAL	(50.0f)			// �N�o���̃G�l�~�[�Ԃ̊Ԋu

#define SPAWN_C_X_CENTER		(0)				// �����S�[����W�I�Ƃ���G�l�~�[�N�o�ɂ�����AX���̒������W 
#define SPAWN_C_X_WIDTH			(2000)			// �����S�[����W�I�Ƃ���G�l�~�[�N�o��X���̕�
#define SPAWN_C_Z_CENTER		(700)			// �����S�[����W�I�Ƃ���G�l�~�[�N�o�ɂ�����AZ���̒������W 
#define SPAWN_C_Z_WIDTH			(400)			// �����S�[����W�I�Ƃ���G�l�~�[�N�o��Z���̕�

#define SPAWN_L_X_CENTER		(-1200)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o�ɂ�����AX���̒������W 
#define SPAWN_L_X_WIDTH			(400)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o��X���̕�
#define SPAWN_L_Z_CENTER		(-200)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o�ɂ�����AZ���̒������W 
#define SPAWN_L_Z_WIDTH			(1340)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o��Z���̕�

#define SPAWN_R_X_CENTER		(1200)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o�ɂ�����AX���̒������W 
#define SPAWN_R_X_WIDTH			(400)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o��X���̕�
#define SPAWN_R_Z_CENTER		(-200)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o�ɂ�����AZ���̒������W 
#define SPAWN_R_Z_WIDTH			(1340)			// ���S�[����W�I�Ƃ���G�l�~�[�N�o��Z���̕�


//
static ENEMY enemy[MAX_ENEMY];		// �G�l�~�[
int enemy_load = 0;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &enemy[i].model);
		enemy[i].load = TRUE;

		//=========================================================================
		// �f�B�]����
		//=========================================================================
		enemy[i].dissolve.Enable = TRUE;	// �f�B�]���u��L���ɂ���
		enemy[i].dissolve.threshold = 1.0f;	// �f�B�]���u�̂������l�����߂�


		enemy[i].state = ENEMY_STATE_SPAWN;	//	�ŏ��̓X�|�[����Ԃɐݒ�
		enemy[i].spawnTimeCnt = SPAWN_TIME;
		enemy[i].targetGoalNo = rand() % MAX_GOAL;

		GOAL* goal = GetGoal();
		float posX = goal[enemy[i].targetGoalNo].pos.x;

		PLAYER* player = GetPlayer();	// �v���C���[�̃|�C���^�[��������


		int spawnCenterX = 0;	// �G�l�~�[�N�o�ɂ�����AX���̒������W 
		int spawnWidthX  = 0;	// �N�o��X���̕�
		int spawnCalcX   = 0;	// �����_���֐���X���̒������W�����߂�ׂɎg�p
		
		int spawnCenterZ = 0;	// �G�l�~�[�N�o�ɂ�����AZ���̒������W 
		int spawnWidthZ  = 0;	// �N�o��Z���̕�
		int spawnCalcZ   = 0;	// �����_���֐���Z���̒������W�����߂�ׂɎg�p


		if (posX == 0)	// �����̃S�[���̏ꍇ
		{
			spawnCenterX = SPAWN_C_X_CENTER;
			spawnWidthX = SPAWN_C_X_WIDTH;
			spawnCalcX = spawnCenterX - spawnWidthX / 2;

			spawnCenterZ = SPAWN_C_Z_CENTER;
			spawnWidthZ = SPAWN_C_Z_WIDTH;
			spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

		}
		else if (posX < 0) // �����猩�č����̃S�[���̏ꍇ
		{
			spawnCenterX = SPAWN_L_X_CENTER;
			spawnWidthX = SPAWN_L_X_WIDTH;
			spawnCalcX = spawnCenterX - spawnWidthX / 2;

			spawnCenterZ = SPAWN_L_Z_CENTER;
			spawnWidthZ = SPAWN_L_Z_WIDTH;
			spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;


		}
		else // �E���̃S�[���̏ꍇ
		{
			spawnCenterX = SPAWN_R_X_CENTER;
			spawnWidthX = SPAWN_R_X_WIDTH;
			spawnCalcX = spawnCenterX - spawnWidthX / 2;

			spawnCenterZ = SPAWN_R_Z_CENTER;
			spawnWidthZ = SPAWN_R_Z_WIDTH;
			spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

		}


		// �N�o���ɃG�l�~�[���d�Ȃ�Ȃ��悤�ɂ���
		while (1)
		{
			enemy[i].pos = XMFLOAT3((float)(rand() % spawnWidthX + spawnCalcX), ENEMY_OFFSET_Y, (float)(rand() % spawnWidthZ + spawnCalcZ));

			if (!CollisionBC(player->pos, enemy[i].pos, SPAWN_SPACE_INTERVAL, SPAWN_SPACE_INTERVAL)) break;
		}


		enemy[i].rot = XMFLOAT3(ENEMY_ROT, ENEMY_ROT, ENEMY_ROT);
		enemy[i].scl = XMFLOAT3(ENEMY_SCALE, ENEMY_SCALE, ENEMY_SCALE);

		enemy[i].spd  = 0.0f;		// �ړ��X�s�[�h�N���A
		enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&enemy[i].model, &enemy[i].diffuse[0]);

		XMFLOAT3 pos = enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		enemy[i].use = TRUE;		// TRUE:�����Ă�

	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].load)
		{
			UnloadModel(&enemy[i].model);
			enemy[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes

			switch (enemy[i].state)
			{
				case ENEMY_STATE_SPAWN:

					enemy[i].spawnTimeCnt -= 1;

					if (enemy[i].spawnTimeCnt < 0)
					{
						enemy[i].spawnTimeCnt = SPAWN_TIME;
						enemy[i].dissolve.Enable = FALSE;
						enemy[i].state = ENEMY_STATE_ACTIVE;
						break;
					}

					enemy[i].dissolve.threshold -= 0.0085f;	// 

					break;


				case ENEMY_STATE_ACTIVE:
					GOAL* goal = GetGoal();	// �S�[���̃|�C���^�[��������

					int goalNo = enemy[i].targetGoalNo;

					float dx = goal[goalNo].pos.x - enemy[i].pos.x;
					float dz = goal[goalNo].pos.z - enemy[i].pos.z;

					float angle = atan2f(dz, dx);

					float tx = enemy[i].pos.x;
					float tz = enemy[i].pos.z;

					enemy[i].pos.x += cosf(angle) * VALUE_MOVE;
					enemy[i].pos.z += sinf(angle) * VALUE_MOVE;

					for (int j = 0; j < i; j++)
					{
						if (enemy[j].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
						{
							if (CollisionBC(enemy[i].pos, enemy[j].pos, ENEMY_INTERVAL, ENEMY_INTERVAL))	// �G�l�~�[���d�Ȃ�Ȃ��悤�ɂ���
							{
								enemy[i].pos.x = tx;
								enemy[i].pos.z = tz;
								break;
							}
						}
					}

					enemy[i].rot.y = -angle - XM_PI * 0.5f;

					if (CollisionBC(enemy[i].pos, goal[goalNo].pos, ENEMY_GOAL_INTERVAL, ENEMY_GOAL_INTERVAL))
					{
						enemy[i].use = FALSE;
						ReleaseShadow(enemy[i].shadowIdx);
						AddVillHp(-1);

					}


					break;


			}

			//=========================================================================
			// �f�B�]���u�i����m�F�j
			//=========================================================================
			if (GetKeyboardPress(DIK_I))   enemy[i].dissolve.threshold += 0.01f;
			if (GetKeyboardPress(DIK_K)) enemy[i].dissolve.threshold -= 0.01f;


			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(enemy[i].shadowIdx, pos);


		}
		else
		{
			// �G�l�~�[���s�g�p(FALSE)�̂Ƃ��A�ēx����������

			enemy[i].state = ENEMY_STATE_SPAWN;			// �X�|�[����Ԃɐݒ�
			enemy[i].dissolve.Enable = TRUE;
			enemy[i].dissolve.threshold = 1.0f;

			enemy[i].targetGoalNo = rand() % MAX_GOAL;	// �W�I�̃S�[����ݒ�

			GOAL* goal = GetGoal();
			float posX = goal[enemy[i].targetGoalNo].pos.x;

			PLAYER* player = GetPlayer();	// �v���C���[�̃|�C���^�[��������


			int spawnCenterX = 0;	// �G�l�~�[�N�o�ɂ�����AX���̒������W 
			int spawnWidthX  = 0;	// �N�o��X���̕�
			int spawnCalcX   = 0;	// �����_���֐���X���̒������W�����߂�ׂɎg�p

			int spawnCenterZ = 0;	// �G�l�~�[�N�o�ɂ�����AZ���̒������W 
			int spawnWidthZ  = 0;	// �N�o��Z���̕�
			int spawnCalcZ   = 0;	// �����_���֐���Z���̒������W�����߂�ׂɎg�p


			if (posX == 0)	// �����̃S�[���̏ꍇ
			{
				spawnCenterX = SPAWN_C_X_CENTER;
				spawnWidthX = SPAWN_C_X_WIDTH;
				spawnCalcX = spawnCenterX - spawnWidthX / 2;

				spawnCenterZ = SPAWN_C_Z_CENTER;
				spawnWidthZ = SPAWN_C_Z_WIDTH;
				spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

			}
			else if (posX < 0) // �����猩�č����̃S�[���̏ꍇ
			{
				spawnCenterX = SPAWN_L_X_CENTER;
				spawnWidthX = SPAWN_L_X_WIDTH;
				spawnCalcX = spawnCenterX - spawnWidthX / 2;

				spawnCenterZ = SPAWN_L_Z_CENTER;
				spawnWidthZ = SPAWN_L_Z_WIDTH;
				spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

			}
			else // �E���̃S�[���̏ꍇ
			{
				spawnCenterX = SPAWN_R_X_CENTER;
				spawnWidthX = SPAWN_R_X_WIDTH;
				spawnCalcX = spawnCenterX - spawnWidthX / 2;

				spawnCenterZ = SPAWN_R_Z_CENTER;
				spawnWidthZ = SPAWN_R_Z_WIDTH;
				spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

			}

			while (1)
			{
				enemy[i].pos = XMFLOAT3((float)(rand() % spawnWidthX + spawnCalcX), ENEMY_OFFSET_Y, (float)(rand() % spawnWidthZ + spawnCalcZ));

				if (!CollisionBC(player->pos, enemy[i].pos, SPAWN_SPACE_INTERVAL, SPAWN_SPACE_INTERVAL)) break;
			}

			enemy[i].rot = XMFLOAT3(ENEMY_ROT, ENEMY_ROT, ENEMY_ROT);	// �����z�u���̌����𐳖ʂɒ���


			XMFLOAT3 pos = enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

			enemy[i].use = TRUE;			// TRUE:�����Ă�


		}

	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;


		//=========================================================================
		// �f�B�]���u
		//=========================================================================
		SetDissolve(&enemy[i].dissolve);
		SetAlphaTestEnable(enemy[i].dissolve.Enable);		// ���e�X�g��L����


		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(enemy[i].scl.x, enemy[i].scl.y, enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(enemy[i].rot.x, enemy[i].rot.y + XM_PI, enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(enemy[i].pos.x, enemy[i].pos.y, enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&enemy[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&enemy[i].model);

	}

		//=========================================================================
		// �f�B�]���u
		//=========================================================================
		SetDissolveEnable(FALSE);
		SetAlphaTestEnable(FALSE);		// ���e�X�g�𖳌���


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy(void)
{
	return &enemy[0];
}
