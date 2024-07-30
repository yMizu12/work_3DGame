//=============================================================================
//
// �`���[�g���A����ʏ��� [tutorial.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "fade.h"
#include "sprite.h"
#include "tutorial.h"
#include "ui.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(960)		// �Q�[���{�҂̔w�i�T�C�Y
#define TEXTURE_HEIGHT				(640)

#define TEXTURE_TUT_WIDTH			(912)		// �`���[�g���A���̔w�i�T�C�Y
#define TEXTURE_TUT_HEIGHT			(608)

#define TEXTURE_MAX					(20)		// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)		// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)

#define TUTORIAL_MAX				(4)			// �e�N�X�`���̐�

#define TEXTURE_NUM_WIDTH			(16)		// �����̃e�N�X�`���T�C�Y�i���j
#define TEXTURE_NUM_HEIGHT			(32)		// �����̃e�N�X�`���T�C�Y�i�����j

#define TEXTURE_SLASH_WIDTH			(48)		// �y�[�W�X���b�V���T�C�Y
#define TEXTURE_SLASH_HEIGHT		(48)

#define PAGE_C_X					(440)
#define PAGE_C_Y					(520)

#define PAGE_S_X					(480)
#define PAGE_S_Y					(520)

#define PAGE_M_X					(520)
#define PAGE_M_Y					(520)

#define TEXTURE_KEY_WIDTH			(40)		// �L�[�T�C�Y�P�̃L�[
#define TEXTURE_KEY_HEIGHT			(40)			

#define TEXTURE_HEADER_PATTERN_DIVIDE_X	(1)		// �e�N�X�`�����������iX)
#define TEXTURE_HEADER_PATTERN_DIVIDE_Y	(4)		// �e�N�X�`�����������iY)

#define HEADER_X					(200)
#define HEADER_Y					(50)

#define TEXTURE_HEADER_WIDTH			(TEXTURE_HEADER_HEIGHT * 4)			// �w�b�_�[�T�C�Y�iW:H = 4:1�j
#define TEXTURE_HEADER_HEIGHT			(100)

#define TEXTURE_CONTROL_NAME_WIDTH		(TEXTURE_CONTROL_NAME_HEIGHT * 3)	// �w�b�_�[�T�C�Y�iW:H = 4:1�j
#define TEXTURE_CONTROL_NAME_HEIGHT		(25)

#define TEXTURE_CONTROL_DETAIL_WIDTH	(TEXTURE_CONTROL_DETAIL_HEIGHT * 4)	// �w�b�_�[�T�C�Y�iW:H = 4:1�j
#define TEXTURE_CONTROL_DETAIL_HEIGHT	(100)

#define TEXTURE_CONTROL_BUTTON_WIDTH	(100)
#define TEXTURE_CONTROL_BUTTON_HEIGHT	(100)

#define TEXTURE_CONTROL_KEY_WIDTH		(100)
#define TEXTURE_CONTROL_KEY_HEIGHT		(100)

#define TEXTURE_BACK_WIDTH				(150)
#define TEXTURE_BACK_HEIGHT				(75)


//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int						gameMode;							// ���݂̃Q�[�����[�h���L��
static int						tutorialNo;							// �`���[�g���A���̉����ڂ�
static BOOL						load = FALSE;						// ���������s�������̃t���O
static TUTORIAL					tutorial;


static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/tutorial/pipo-battlebg012.jpg",
	"data/TEXTURE/tutorial/sky000.jpg",
	"data/TEXTURE/tutorial/sky001.jpg",
	"data/TEXTURE/tutorial/pipo-battlebg015.jpg",
	"data/TEXTURE/tutorial/tutrial_bg_0.png",
	"data/TEXTURE/tutorial/suuji16x32_07.png",
	"data/TEXTURE/tutorial/page_slash.png",
	"data/TEXTURE/tutorial/key_LEFT.png",
	"data/TEXTURE/tutorial/key_RIGHT.png",
	"data/TEXTURE/tutorial/key_ENTER.png",
	"data/TEXTURE/tutorial/key_SPACE.png",
	"data/TEXTURE/tutorial/tutorial_header2.png",
	"data/TEXTURE/tutorial/rule.png",
	"data/TEXTURE/tutorial/screen.png",
	"data/TEXTURE/tutorial/gamepad.png",
	"data/TEXTURE/tutorial/keyboard.png",
	"data/TEXTURE/tutorial/tutrial_finaly_02.png",
	"data/TEXTURE/tutorial/back_to_title.png",
	"data/TEXTURE/tutorial/start_game.png",
	"data/TEXTURE/tutorial/screen_detail.png",
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
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
	tutorial.w = TEXTURE_TUT_WIDTH;
	tutorial.h = TEXTURE_TUT_HEIGHT;
	tutorial.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	tutorial.texNo = 3;

	tutorialNo = 0;

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_TUTORIAL);


	load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
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
void UpdateTutorial(void)
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
	{// [Q]��������A�X�e�[�W��؂�ւ���
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_X))
	{
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}



	// �E�L�[�Ń`���[�g���A����ʐؑւ�
	if (GetKeyboardTrigger(DIK_RIGHT))
	{
		if (tutorialNo < TUTORIAL_MAX -1)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo++;
		}
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_RIGHT))
	{
		if (tutorialNo < TUTORIAL_MAX - 1)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo++;
		}
	}

	// ���L�[�Ń`���[�g���A����ʐؑւ�
	if (GetKeyboardTrigger(DIK_LEFT))
	{
		if (tutorialNo != 0)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo--;
		}
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_LEFT))
	{
		if (tutorialNo != 0)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo--;
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
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


	// �`���[�g���A���̔w�i��`��
	{
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[tutorial.texNo]);

	// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
	SetSpriteLTColor(vertexBuffer,
		tutorial.pos.x, tutorial.pos.y, tutorial.w, tutorial.h,
		0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
	}


	// �������
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer,
			SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXTURE_TUT_WIDTH, TEXTURE_TUT_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}




	// �y�[�W���Ƃɕ`��𕪂���
	switch (tutorialNo)
	{
		case 0:
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[12]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y, 800, 400,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			break;


		case 1:
			// ��ʍ\���̕`��
		{

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[13]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y, 900.0f, 450.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

			break;
		case 2:

		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[14]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y+30.0f, 900.0f, 450.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

			break;
		case 3:

		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[15]);

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y + 30.0f, 900.0f, 450.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}

			break;
	}

	// �w�b�_�[
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[11]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

		float tw = 1.0f / TEXTURE_HEADER_PATTERN_DIVIDE_X;		// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_HEADER_PATTERN_DIVIDE_Y;		// �e�N�X�`���̍���
		float tx = 0.0f;										// �e�N�X�`���̍���X���W
		float ty = (tutorialNo)*th;								// �e�N�X�`���̍���Y���W

		SetSpriteColor(vertexBuffer,
			HEADER_X, HEADER_Y, TEXTURE_HEADER_WIDTH, TEXTURE_HEADER_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �y�[�W
	{
		// ���q
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

		float tw = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_X;		// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_Y;		// �e�N�X�`���̍���
		float tx = (tutorialNo + 1) * tw;					// �e�N�X�`���̍���X���W
		float ty = 0.0f;									// �e�N�X�`���̍���Y���W

		SetSpriteColor(vertexBuffer,
			PAGE_C_X, PAGE_C_Y, TEXTURE_NUM_WIDTH, TEXTURE_NUM_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// �X���b�V��
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[6]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

		SetSpriteColor(vertexBuffer,
			PAGE_S_X, PAGE_S_Y, TEXTURE_SLASH_WIDTH, TEXTURE_SLASH_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// ����
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

		float tw = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_X;		// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_Y;		// �e�N�X�`���̍���
		float tx = (TUTORIAL_MAX) * tw;						// �e�N�X�`���̍���X���W
		float ty = 0.0f;									// �e�N�X�`���̍���Y���W

		SetSpriteColor(vertexBuffer,
			PAGE_M_X, PAGE_M_Y, TEXTURE_NUM_WIDTH, TEXTURE_NUM_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// ���L�[
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

		float alpha;

		if (tutorialNo != 0)
		{
			alpha = 1.0f;
		}
		else
		{
			alpha = 0.5f;
		}

		SetSpriteColor(vertexBuffer,
			PAGE_C_X - 70.0f, PAGE_C_Y, TEXTURE_KEY_WIDTH, TEXTURE_KEY_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}


	{
		// �E�L�[
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[8]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�

		float alpha;

		if (tutorialNo != TUTORIAL_MAX - 1)
		{
			alpha = 1.0f;
		}
		else
		{
			alpha = 0.5f;
		}

		SetSpriteColor(vertexBuffer,
			PAGE_M_X + 70.0f, PAGE_C_Y, TEXTURE_KEY_WIDTH, TEXTURE_KEY_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	{
		// �^�C�g���ɖ߂�
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[17]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer,
			200.0f, 500.0f, TEXTURE_BACK_WIDTH, TEXTURE_BACK_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// �Q�[���J�n
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[18]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer,
			760.0f, 500.0f, TEXTURE_BACK_WIDTH, TEXTURE_BACK_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


}





