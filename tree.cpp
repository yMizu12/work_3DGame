//=============================================================================
//
// �؂̏��� [tree.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "tree.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�
#define	TREE_WIDTH			(60.0f)			// �؂̃T�C�Y
#define	TREE_HEIGHT			(90.0f)			// 
#define	MAX_TREE			(256)			// �؍ő吔
#define TREE_SHADOW_SIZE	(0.5f)			// �؂̉e�̃T�C�Y
#define TREE_FRONT_ROW		(3)				// ���ʂ̖؂̍s��
#define TREE_FRONT_COL		(15)			// ���ʂ̖؂̗�
#define TREE_LEFT_ROW		(6)				// �����̖؂̍s��
#define TREE_LEFT_COL		(3)				// �����̖؂̗�
#define TREE_RIGHT_ROW		(6)				// �E���̖؂̍s��
#define TREE_RIGHT_COL		(3)				// �E���̖؂̗�


//
static ID3D11Buffer				*vertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static TREE						tree[MAX_TREE];						// �؃��[�N
static BOOL						alpaTest;							// �A���t�@�e�X�gON/OFF
static int						texNo;								// �e�N�X�`���ԍ�
static int						texAnim;							// �e�N�X�`���A�j���p

static char *textureName[] =
{
	"data/TEXTURE/tree_99.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTree(void)
{
	MakeVertexTree();

	// �e�N�X�`������
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

	texNo = texAnim = 0;

	// �؃��[�N�̏�����
	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		ZeroMemory(&tree[nCntTree].material, sizeof(tree[nCntTree].material));
		tree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		tree[nCntTree].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		tree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		tree[nCntTree].fWidth = TREE_WIDTH;
		tree[nCntTree].fHeight = TREE_HEIGHT;
		tree[nCntTree].use = FALSE;
	}

	alpaTest = TRUE;
	//g_nAlpha = 0x0;

	// ���ʂ̖؂̃Z�b�g
	for (int z = 0; z < TREE_FRONT_ROW; z++)
	{
		for (int x = 0; x < TREE_FRONT_COL; x++)
		{
			SetTree(XMFLOAT3(-1400.0f + 200.0f * x, 0.0f, 800.0f - 200.0f * z), TREE_WIDTH, TREE_HEIGHT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	// ���葤�̖؂̃Z�b�g
	for (int z = 0; z < TREE_LEFT_ROW; z++)
	{
		for (int x = 0; x < TREE_LEFT_COL; x++)
		{
			SetTree(XMFLOAT3(-1400.0f + 200.0f * x, 0.0f, 200.0f - 200.0f * z), TREE_WIDTH, TREE_HEIGHT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	// �E�葤�̖؂̃Z�b�g
	for (int z = 0; z < TREE_RIGHT_ROW; z++)
	{
		for (int x = 0; x < TREE_RIGHT_COL; x++)
		{
			SetTree(XMFLOAT3(1000.0f + 200.0f * x, 0.0f, 200.0f - 200.0f * z), TREE_WIDTH, TREE_HEIGHT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTree(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			texture[nCntTex]->Release();
			texture[nCntTex] = NULL;
		}
	}

	if(vertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTree(void)
{

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(tree[nCntTree].use)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(tree[nCntTree].nIdxShadow, XMFLOAT3(tree[nCntTree].pos.x, 0.1f, tree[nCntTree].pos.z));
		}
	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		alpaTest = alpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTree(void)
{
	// ���e�X�g�ݒ�
	if (alpaTest == TRUE)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_TREE; i++)
	{
		if(tree[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �֐��g������
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// �����s��i�����s��j��]�u�s�񂳂��ċt�s�������Ă��(����)
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(tree[i].scl.x, tree[i].scl.y, tree[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(tree[i].pos.x, tree[i].pos.y, tree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(tree[i].material);

			// �e�N�X�`���ݒ�
			int texNo = i % TEXTURE_MAX;
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexTree(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(vertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �؂̃p�����[�^���Z�b�g
//=============================================================================
int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxTree = -1;

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(!tree[nCntTree].use)
		{
			tree[nCntTree].pos = pos;
			tree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			tree[nCntTree].fWidth = fWidth;
			tree[nCntTree].fHeight = fHeight;
			tree[nCntTree].use = TRUE;

			// �e�̐ݒ�
			tree[nCntTree].nIdxShadow = CreateShadow(tree[nCntTree].pos, TREE_SHADOW_SIZE, TREE_SHADOW_SIZE);

			nIdxTree = nCntTree;

			break;
		}
	}

	return nIdxTree;
}
