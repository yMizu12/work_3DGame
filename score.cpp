//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_WIDTH			(16)	// �T�C�Y
#define SCORE_HEIGHT		(32)	// 
#define SCORE_POS_X			(500.0f)// X���W
#define SCORE_POS_Y			(20.0f)	// Y���W
#define SCORE_TEX_WIDTH		(1.0f / 13.0f)
#define SCORE_TEX_HEIGHT	(1.0f)
#define TEXTURE_MAX			(1)		// �e�N�X�`���̐�


//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static BOOL						use;								// TRUE:�g���Ă���  FALSE:���g�p
static float					weight, height;						// ���ƍ���
static int						texNo;								// �e�N�X�`���ԍ�
static int						score;								// �X�R�A
static BOOL						load = FALSE;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/suuji16x32_02.png",
};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
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


	// ������
	use		= TRUE;
	weight	= SCORE_WIDTH;
	height	= SCORE_HEIGHT;
	texNo	= 0;

	score = 0;	// �X�R�A�̏�����

	load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

	// ��������������
	int number = score;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = SCORE_POS_X - weight*i;	// �X�R�A�̕\���ʒuX
		float py = SCORE_POS_Y;				// �X�R�A�̕\���ʒuY
		float pw = weight;					// �X�R�A�̕\����
		float ph = height;					// �X�R�A�̕\������

		float tw = SCORE_TEX_WIDTH;			// �e�N�X�`���̕�
		float th = SCORE_TEX_HEIGHT;		// �e�N�X�`���̍���
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


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddScore(int add)
{
	score += add;
	if (score > SCORE_MAX)
	{
		score = SCORE_MAX;
	}
	else if (score < SCORE_MIN)
	{
		score = SCORE_MIN;
	}

}


int GetScore(void)
{
	return score;
}

