//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "fieldobject.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "ui.h"
#include "particle.h"
#include "collision.h"
#include "sky.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define WALL_BLOCK_NUM_X	(50)
#define WALL_BLOCK_SIZE		(50.0f)
#define WALL_COL_BLUE		(0.5f)


//
static int	viewPortType_Game = TYPE_FULL_SCREEN;
static BOOL	pause = TRUE;		// �|�[�YON/OFF
BulletManager bulletManager;
ParticleManager particleManager;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	viewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 300, 300, 20.0f, 20.0f);

	// �e�̏���������
	InitShadow();

	// �t�B�[���h�I�u�W�F�N�g�̔z�u
	InitFieldObject();

	InitSky();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	// �ǂ̏�����
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);

	// ��(�����p�̔�����)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);

	// �؂𐶂₷
	InitTree();

	// �e�̏�����
	bulletManager.InitBullet();

	// �X�R�A�̏�����
	InitScore();

	// UI�̏�����
	InitUI();

	// �p�[�e�B�N���̏�����
	particleManager.InitParticle();

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_GAME);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �p�[�e�B�N���̏I������
	particleManager.UninitParticle();

	// UI�̏I������
	UninitUI();

	// �X�R�A�̏I������
	UninitScore();

	// �e�̏I������
	bulletManager.UninitBullet();

	// �؂̏I������
	UninitTree();

	// �ǂ̏I������
	UninitMeshWall();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	UninitSky();

	// �t�B�[���h�I�u�W�F�N�g�̏I������
	UninitFieldObject();

	// �e�̏I������
	UninitShadow();

	// �n�ʂ̏I������
	UninitMeshField();


}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		viewPortType_Game = (viewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(viewPortType_Game);
	}

#endif

	// �ꎞ��~
	if (GetKeyboardTrigger(DIK_P))
	{
		pause = pause ? FALSE : TRUE;
	}

	if(pause == FALSE)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �t�B�[���h�I�u�W�F�N�g�̍X�V
	UpdateFieldObject();

	UpdateSky();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	// �Ǐ����̍X�V
	UpdateMeshWall();

	// �؂̍X�V����
	UpdateTree();

	// �e�̍X�V����
	bulletManager.UpdateBullet();

	// �p�[�e�B�N���̍X�V����
	particleManager.UpdateParticle();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateScore();

	// UI�̍X�V����
	UpdateUI();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	DrawShadow();

	DrawSky();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	// �e�̕`�揈��
	bulletManager.DrawBullet();

	// �t�B�[���h�I�u�W�F�N�g�̕`�揈��
	DrawFieldObject();

	// �ǂ̕`�揈��
	DrawMeshWall();

	// �؂̕`�揈��
	DrawTree();

	// �p�[�e�B�N���̕`�揈��
	particleManager.DrawParticle();


	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// �X�R�A�̕`�揈��
	DrawScore();

	// UI�̕`�揈��
	DrawUI();


	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", viewPortType_Game);

	CAMERA* cam = GetCamera();
	PrintDebugProc("CameraRot   X:%f  Y:%f  Z:%f\n", XMConvertToDegrees(cam->rot.x), XMConvertToDegrees(cam->rot.y), XMConvertToDegrees(cam->rot.z));


#endif

	// �v���C���[���_
	pos = GetPlayer()->pos;
	pos.y = PLAYER_HEIGHT;			// �J����������h�����߂ɃN���A���Ă���
	SetCameraAT(pos);
	SetCamera();

	switch(viewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();					// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();				// �v���C���[�̃|�C���^�[��������
	BULLET *bullet = bulletManager.GetBullet();	// �e�̃|�C���^�[��������

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			enemy[i].use = FALSE;
			ReleaseShadow(enemy[i].shadowIdx);

			// �X�R�A�𑫂�
			AddScore(-500);
		}
	}


	// �v���C���[�̒e�ƓG
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (bullet[i].getUse() == FALSE)
			continue;

		// �G�Ɠ������Ă邩���ׂ�
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//�G�̗L���t���O���`�F�b�N����
			if (enemy[j].use == FALSE)
				continue;

			XMFLOAT3 move;
			float fWidthBase = 3.0f;	// ��̕�
			float fHeightBase = 4.0f;	// ��̍���
			float fAngle, fLength;


			//BC�̓����蔻��
			if (CollisionBC(bullet[i].getPos(), enemy[j].pos, bullet[i].getfWidth(), enemy[j].size))
			{

				for (int k = 0; k < 25; k++)
				{
					// �p�[�e�B�N������
					fAngle = (float)(rand() % 628 - 314) / 100.0f;						// �p�x
					fLength = rand() % (int)(fWidthBase * 200) / 100.0f - fWidthBase;	// ����
					move.x = sinf(fAngle) * fLength;
					move.y = rand() % 30 / 100.0f + fHeightBase;
					move.z = cosf(fAngle) * fLength;

					particleManager.SetParticle(enemy[j].pos, move, XMFLOAT4(0.4f, 0.0f, 1.0f, 0.85f), 1.0f, 1.0f, 100);
				}

				PlaySound(SOUND_LABEL_SE_ENEMYHIT);

				// �G�L�����N�^�[�͓|�����
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// �X�R�A�𑫂�
				AddScore(250);
			}
		}

	}



}


