//=============================================================================
//
// UI�̏��� [ui.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "result.h"
#include "score.h"
#include "sprite.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_WIDTH_VILL_HP		(32)			// ��HP�T�C�Y
#define TEXTURE_HEIGHT_VILL_HP		(64)			// 
#define TEXTURE_MAX					(6)				// �e�N�X�`���̐�
#define VILL_HP_MAX					(70)			// ��HP�̍ő�l
#define VILL_HP_MIN					(0)				// ��HP�̍ŏ��l
#define VILL_HP_DIGIT				(2)				// ��HP�̕\������
#define TIME_LIMIT					(60)			// ��������
#define TIME_DIGIT					(2)				// �������Ԃ̕\������

//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static BOOL						use;								// TRUE:�g���Ă���  FALSE:���g�p
static float					weight, height;						// ���ƍ���
static XMFLOAT3					pos;								// �|���S���̍��W
static int						texNo;								// �e�N�X�`���ԍ�
static int						score;								// �X�R�A
static int						villHp = 0;
static int timecnt;
static int timestart;
static BOOL						load = FALSE;


static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/test_line.png",
	"data/TEXTURE/suuji16x32_05.png",
	"data/TEXTURE/suuji16x32_03.png",
	"data/TEXTURE/clock.png",
	"data/TEXTURE/icon_village.png",
	"data/TEXTURE/text_last_02.png",
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
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


	// UI�̏�����
	use   = TRUE;
	weight     = TEXTURE_WIDTH_VILL_HP;
	height     = TEXTURE_HEIGHT_VILL_HP;
	pos   = { 735.0f, 20.0f, 0.0f };	// ��HP�̕\����ʒu
	texNo = 0;

	score = 0;	// �X�R�A�̏�����

	villHp = VILL_HP_MAX;			// ��HP�̏�����

	timecnt = TIME_LIMIT;			// �������ԃJ�E���^�̏�����
	timestart = (int)time(NULL);	// ���ݎ��Ԃ�ۑ�

	load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	// �������ԍX�V����
	timecnt -= (int)time(NULL) - timestart;		// �������ԃJ�E���^����A�O�񎞍��ƌ��ݎ����̍���������
	timestart = (int)time(NULL);				// ���ݎ�����ۑ�����

	int score = GetScore();

	if (timecnt < 0)
	{
		timecnt = 0;
		
		SetEnding(ENDING_GOOD);
		SetFade(FADE_OUT, MODE_RESULT);

	}

	if (villHp <= 0)
	{
		SetEnding(ENDING_DEAD);
		SetFade(FADE_OUT, MODE_RESULT);
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void)
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

	// ���A�C�R���\������
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[4]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, 550.0f, 50.0f, 65.0f, 65.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �����uLast�v�\������
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, 630.0f, 50.0f, 100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}



	// ��HP�\������
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[1]);
	
		// ��������������
		int number = villHp;
		for (int i = 0; i < VILL_HP_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);
	
			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = pos.x - weight * i;	// �X�R�A�̕\���ʒuX
			float py = pos.y + 30.0f;		// �X�R�A�̕\���ʒuY
			float pw = 32.0f;				// �X�R�A�̕\����
			float ph = 64.0f;				// �X�R�A�̕\������
	
			float tw = 1.0f / 13;			// �e�N�X�`���̕�
			float th = 1.0f / 1;			// �e�N�X�`���̍���
			float tx = x * tw;				// �e�N�X�`���̍���X���W
			float ty = 0.0f;				// �e�N�X�`���̍���Y���W
	
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
	
			// ���̌���
			number /= 10;
		}

	}


	// �������ԕ\������
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[2]);

		int number = timecnt;

		for (int i = 0; i < TIME_DIGIT; i++)
		{
			float x = (float)(number % 10);

			float px = 900.0f - i * 40;	// �X�R�A�̕\���ʒuX
			float py = 50.0f;			// �X�R�A�̕\���ʒuY
			float pw = 32;				// �X�R�A�̕\����
			float ph = 64;				// �X�R�A�̕\������

			float tw = 1.0f / 13;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			number /= 10;
		}
	}

	// ���v�A�C�R���\������
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[3]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(vertexBuffer, 800.0f, 50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	
	}


}


int GetVillHp(void)
{
	return villHp;
}

void AddVillHp(int add)
{
	villHp += add;
	if (villHp > VILL_HP_MAX)
	{
		villHp = VILL_HP_MAX;
	}
	else if (villHp < VILL_HP_MIN)
	{
		villHp = VILL_HP_MIN;
	}

}