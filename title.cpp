//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)		// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)		// 
#define TEXTURE_MAX					(6)					// �e�N�X�`���̐�
#define TEXTURE_WIDTH_LOGO			(960)				// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(160)				// 
#define TEXTURE_POS_X_LOGO			(TEXTURE_WIDTH/2)	//
#define TEXTURE_POS_Y_LOGO			(TEXTURE_HEIGHT/2 - 100.0f)//
#define TEXTURE_WIDTH_STORY			(250)				// ���炷���T�C�Y
#define TEXTURE_HEIGHT_STORY		(60)				// 
#define TEXTURE_POS_X_STORY			(480)				// 
#define TEXTURE_POS_Y_STORY			(310)				// 
#define TEXTURE_WIDTH_TUTORIAL		(350)				// ��������T�C�Y
#define TEXTURE_HEIGHT_TUTORIAL		(60)				// 
#define TEXTURE_POS_X_TUTORIAL		(480)				// 
#define TEXTURE_POS_Y_TUTORIAL		(370)				// 
#define TEXTURE_WIDTH_START			(500)				// �Q�[���J�n�T�C�Y
#define TEXTURE_HEIGHT_START		(60)				// 
#define TEXTURE_POS_X_START			(480)				// 
#define TEXTURE_POS_Y_START			(430)				// 

//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static BOOL						use;								// TRUE:�g���Ă���  FALSE:���g�p
static float					width, height;						// ���ƍ���
static XMFLOAT3					pos;								// �|���S���̍��W
static int						texNo;								// �e�N�X�`���ԍ�
static int						curSelect;							// �J�[�\��
float							alpha;
BOOL							flag_alpha;
static BOOL						load = FALSE;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/sky_night.jpg",
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/title_story.png",
	"data/TEXTURE/title_tutorial.png",
	"data/TEXTURE/title_start.png",
	"data/TEXTURE/gauge.png",

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	width	= TEXTURE_WIDTH;
	height	= TEXTURE_HEIGHT;
	pos		= XMFLOAT3(width/2, height/2, 0.0f);
	texNo	= 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	curSelect = SELECT_START;	// �J�[�\���̏����ʒu�ݒ�

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_TITLE);

	load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		PlaySound(SOUND_LABEL_SE_DECISION);

		switch (curSelect)
		{
			case SELECT_STORY:
				SetFade(FADE_OUT, MODE_STORY);
				break;

			case SELECT_TUTORIAL:
				SetFade(FADE_OUT, MODE_TUTORIAL);
				break;

			case SELECT_START:
				SetFade(FADE_OUT, MODE_GAME);
				break;
		}
	
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		switch (curSelect)
		{
			case SELECT_STORY:
				SetFade(FADE_OUT, MODE_STORY);
				break;

			case SELECT_TUTORIAL:
				SetFade(FADE_OUT, MODE_TUTORIAL);
				break;

			case SELECT_START:
				SetFade(FADE_OUT, MODE_GAME);
				break;

		}
		
	}


	if (flag_alpha == TRUE)
	{
		alpha -= 0.02f;
		if (alpha <= 0.0f)
		{
			alpha = 0.0f;
			flag_alpha = FALSE;
		}
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			flag_alpha = TRUE;
		}
	}


	// ��L�[�őI�����ؑւ�
	if (GetKeyboardTrigger(DIK_UP))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + SELECT_MAX - 1) % SELECT_MAX;
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_UP) || IsButtonTriggered(0, STICK_UP_L))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + SELECT_MAX - 1) % SELECT_MAX;
	}


	// ���L�[�őI�����ؑւ�
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + 1) % SELECT_MAX;
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_DOWN) || IsButtonTriggered(0, STICK_DOWN_L))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + 1) % SELECT_MAX;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(vertexBuffer, pos.x, pos.y, width, height, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̃��S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_LOGO, TEXTURE_POS_Y_LOGO, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �n�C���C�g��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		float r = 0.7f;
		float g = 0.7f;
		float b = 0.0f;
		float a = 0.5f;

		switch (curSelect)
		{
		case SELECT_STORY:
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, TEXTURE_POS_X_STORY, TEXTURE_POS_Y_STORY, 
				TEXTURE_WIDTH_STORY, TEXTURE_HEIGHT_STORY, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(r, g, b, a));
			break;

		case SELECT_TUTORIAL:
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, TEXTURE_POS_X_TUTORIAL, TEXTURE_POS_Y_TUTORIAL, 
				TEXTURE_WIDTH_TUTORIAL, TEXTURE_HEIGHT_TUTORIAL, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(r, g, b, a));
			break;

		case SELECT_START:
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, TEXTURE_POS_X_START, TEXTURE_POS_Y_START, 
				TEXTURE_WIDTH_START, TEXTURE_HEIGHT_START, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(r, g, b, a));
			break;
		}

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �F�ϊ��p
	float r;
	float g;
	float b;

	// �U�����b�Z�[�W��`��
	// STORY
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[2]);

		if (curSelect == SELECT_STORY)
		{
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
		}
		else
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_STORY, TEXTURE_POS_Y_STORY, 
			TEXTURE_WIDTH_STORY, TEXTURE_HEIGHT_STORY, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(r, g, b, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// TUTORIAL
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[3]);

		if (curSelect == SELECT_TUTORIAL)
		{
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
		}
		else
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_TUTORIAL, TEXTURE_POS_Y_TUTORIAL, 
			TEXTURE_WIDTH_TUTORIAL, TEXTURE_HEIGHT_TUTORIAL, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(r, g, b, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// GAME START
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[4]);

		if (curSelect == SELECT_START)
		{
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
		}
		else
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_START, TEXTURE_POS_Y_START, 
			TEXTURE_WIDTH_START, TEXTURE_HEIGHT_START, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(r, g, b, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}

