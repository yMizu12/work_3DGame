//=============================================================================
//
// ���炷����ʏ��� [story.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "fade.h"
#include "story.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(960)		// �Q�[���{�҂̔w�i�T�C�Y
#define TEXTURE_HEIGHT				(640)		//
#define TEXTURE_MAX					(11)		// �e�N�X�`���̐�
#define TEXTURE_WIDTH_LOGO			(480)		// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)		// 
#define SRORY_DESC_POS_X			(480.0f)	// DESK�FDescription
#define SRORY_DESC_POS_Y			(270.0f)	//
#define SRORY_DESC_WIDTH			(900.0f)	//
#define SRORY_DESC_HEIGHT			(500.0f)	//
#define SRORY_DESC_COLOR_A			(0.9f)		// ���������̃��l
#define SRORY_GHOST_POS_X			(640.0f)	// �S�[�X�g
#define SRORY_GHOST_POS_Y			(220.0f)	//
#define SRORY_GHOST_WIDTH			(120.0f)	//
#define SRORY_GHOST_HEIGHT			(120.0f)	//
#define SRORY_SPARK_POS_X			(750.0f)	// �Ή�
#define SRORY_SPARK_POS_Y			(220.0f)	//
#define SRORY_SPARK_WIDTH			(70.0f)		//
#define SRORY_SPARK_HEIGHT			(70.0f)		//
#define SRORY_SAVIOR_POS_X			(840.0f)	// ����
#define SRORY_SAVIOR_POS_Y			(200.0f)	//
#define SRORY_SAVIOR_WIDTH			(180.0f)	//
#define SRORY_SAVIOR_HEIGHT			(180.0f)	//

//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static BOOL						load = FALSE;						// ���������s�������̃t���O
static STORY					story;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/story/pipo-battlebg015.jpg",
	"data/TEXTURE/story/pipo-battlebg015.jpg",
	"data/TEXTURE/story/pipo-battlebg015.jpg",
	"data/TEXTURE/story/pipo-battlebg012.jpg",
	"data/TEXTURE/story/tut002.png",
	"data/TEXTURE/story/logo_title02.png",
	"data/TEXTURE/story/pipo-battlebg012.jpg",
	"data/TEXTURE/story/ghost-japanese.png",
	"data/TEXTURE/story/shock.png",
	"data/TEXTURE/story/hunter.png",
	"data/TEXTURE/story/boy03_cat01_1.png",

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitStory(void)
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
	story.w = TEXTURE_WIDTH;
	story.h = TEXTURE_HEIGHT;
	story.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	story.texNo = 0;

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_STORY);

	load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitStory(void)
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
void UpdateStory(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_TITLE);
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStory(void)
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


	// �w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[story.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(vertexBuffer,
			story.pos.x, story.pos.y, story.w, story.h,
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
			SRORY_DESC_POS_X, SRORY_DESC_POS_Y, SRORY_DESC_WIDTH, SRORY_DESC_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, SRORY_DESC_COLOR_A));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	

	// �S�[�X�g
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer,
			SRORY_GHOST_POS_X, SRORY_GHOST_POS_Y, SRORY_GHOST_WIDTH, SRORY_GHOST_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �o�`�o�`
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[8]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer,
			SRORY_SPARK_POS_X, SRORY_SPARK_POS_Y, SRORY_SPARK_WIDTH, SRORY_SPARK_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// ����
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[9]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer,
			SRORY_SAVIOR_POS_X, SRORY_SAVIOR_POS_Y, SRORY_SAVIOR_WIDTH, SRORY_SAVIOR_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}




