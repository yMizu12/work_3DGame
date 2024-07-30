//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : HAL����_����T��
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
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_WIDTH_RESULT_MSG	(600)			// ���U���g���b�Z�[�W�T�C�Y
#define TEXTURE_HEIGHT_RESULT_MSG	(90)			// 
#define RESULT_MSG_X				(480)			// X���W
#define RESULT_MSG_Y				(100)			// Y���W
#define SCORE_HEIGHT				(32*2)			// �X�R�A�̕\������
#define SCORE_WIDTH					(16*2)			// �X�R�A�̕\����
#define SCORE_POS_X					(600.0f)		// �X�R�A�̕\�����WX
#define SCORE_POS_Y					(300.0f)		// �X�R�A�̕\�����WY
#define SCORE_TEXTURE_HEIGHT		(1.0f)			// �X�R�A�e�N�X�`���̍���
#define SCORE_TEXTURE_WIDTH			(1.0f / 13.0f)	// �X�R�A�e�N�X�`���̕�
#define BACK_TITLE_POS_X			(300.0f)
#define BACK_TITLE_POS_Y			(470.0f)
#define BACK_TITLE_TEXTURE_HEIGHT	(100.0f)
#define BACK_TITLE_TEXTURE_WIDTH	(200.0f)
#define BACK_GAME_POS_X				(660.0f)
#define BACK_GAME_POS_Y				(470.0f)
#define BACK_GAME_TEXTURE_HEIGHT	(100.0f)
#define BACK_GAME_TEXTURE_WIDTH		(200.0f)
#define TEXTURE_MAX					(10)			// �e�N�X�`���̐�


//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static BOOL						use;								// TRUE:�g���Ă���  FALSE:���g�p
static float					weight, height;						// ���ƍ���
static int						texNo;								// �e�N�X�`���ԍ�
static int						ending;								// �Q�[���̌��ʂɂ��G���f�B���O
static BOOL						load = FALSE;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/bg001.jpg",
	"data/TEXTURE/result_logo.png",
	"data/TEXTURE/suuji16x32_02.png",
	"data/TEXTURE/yoake.jpg",
	"data/TEXTURE/resultB.png",
	"data/TEXTURE/fire.jpg",
	"data/TEXTURE/back_to_title.png",
	"data/TEXTURE/return_game.png",
	"data/TEXTURE/game_clear.png",
	"data/TEXTURE/game_over.png",

};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			textureName[i],
			NULL,
			NULL,
			&texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �ϐ��̏�����
	use		= TRUE;
	weight	= TEXTURE_WIDTH;
	height	= TEXTURE_HEIGHT;
	texNo	= 0;

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_RESULT);

	load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
{
	if (load == FALSE) return;

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (texture[i])
		{
			texture[i]->Release();
			texture[i] = NULL;
		}
	}

	load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateResult(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_GAME);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_GAME);
	}


	if (GetKeyboardTrigger(DIK_Q))
	{// Enter��������A�X�e�[�W��؂�ւ���
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_X))
	{
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �G���f�B���O�ɂ��A�`��𕪊�
	{
		switch (ending)
		{
		case ENDING_GOOD:
		
		{
			//�w�i
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[3]);	// �w�i�e�N�X�`��

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLeftTop(vertexBuffer, 0.0f, 0.0f, weight, height, 0.0f, 0.0f, 1.0f, 1.0f);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			//���U���g���b�Z�[�W
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[8]);	// �Q�[���N���A�e�N�X�`��

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSprite(vertexBuffer, RESULT_MSG_X, RESULT_MSG_Y, TEXTURE_WIDTH_RESULT_MSG, TEXTURE_HEIGHT_RESULT_MSG, 0.0f, 0.0f, 1.0f, 1.0f);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}



		}
		break;


		case ENDING_DEAD:
		
		{
			//�w�i
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);	// �w�i�e�N�X�`��

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteLeftTop(vertexBuffer, 0.0f, 0.0f, weight, height, 0.0f, 0.0f, 1.0f, 1.0f);	

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			//���U���g���b�Z�[�W
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[9]);	// �Q�[���I�[�o�[�e�N�X�`��

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSprite(vertexBuffer, RESULT_MSG_X, RESULT_MSG_Y, TEXTURE_WIDTH_RESULT_MSG, TEXTURE_HEIGHT_RESULT_MSG, 0.0f, 0.0f, 1.0f, 1.0f);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

		break;

		}
	}



	// �X�R�A�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[2]);

		// ��������������
		int number = GetScore();
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float pw = SCORE_WIDTH;				// �X�R�A�̕\����
			float ph = SCORE_HEIGHT;			// �X�R�A�̕\������
			float px = SCORE_POS_X - i*pw;		// �X�R�A�̕\���ʒuX
			float py = SCORE_POS_Y;				// �X�R�A�̕\���ʒuY

			float tw = SCORE_TEXTURE_WIDTH;		// �e�N�X�`���̕�
			float th = SCORE_TEXTURE_HEIGHT;	// �e�N�X�`���̍���
			float tx = x * tw;					// �e�N�X�`���̍���X���W
			float ty = 0.0f;					// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));	// �F�͂��̂܂�

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			// ���̌���
			number /= 10;
		}

	}

	// �^�C�g���ɖ߂�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[6]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(vertexBuffer, BACK_TITLE_POS_X, BACK_TITLE_POS_Y, BACK_TITLE_TEXTURE_WIDTH, BACK_TITLE_TEXTURE_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}


	// �Q�[���Ē���\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(vertexBuffer, BACK_GAME_POS_X, BACK_GAME_POS_Y, BACK_GAME_TEXTURE_WIDTH, BACK_GAME_TEXTURE_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}


//=============================================================================
// �G���f�B���O���Z�b�g����
//=============================================================================
void SetEnding(int ending)
{
	ending = ending;
}
