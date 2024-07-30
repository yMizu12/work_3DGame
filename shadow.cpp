//=============================================================================
//
// �e���� [shadow.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "light.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�
#define	SHADOW_SIZE_X		(50.0f)		// ���_�T�C�Y
#define	SHADOW_SIZE_Z		(50.0f)		// ���_�T�C�Y
#define	MAX_SHADOW			(1024)		// �e�ő吔


//
static ID3D11Buffer					*vertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView		*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static SHADOW						aShadow[MAX_SHADOW];				// �e���[�N
static int							texNo;								// �e�N�X�`���ԍ�

static char* textureName[] = {
	"data/TEXTURE/shadow000.jpg",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitShadow(void)
{
	// ���_�o�b�t�@�̍쐬
	MakeVertexShadow();

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

	// �e���[�N������
	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		ZeroMemory(&aShadow[nCntShadow].material, sizeof(aShadow[nCntShadow].material));
		aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		aShadow[nCntShadow].pos = XMFLOAT3(0.0f, 0.1f, 0.0f);
		aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		aShadow[nCntShadow].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		aShadow[nCntShadow].use = FALSE;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitShadow(void)
{
	// ���_�o�b�t�@�̉��
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (texture[i])
		{
			texture[i]->Release();
			texture[i] = NULL;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateShadow(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawShadow(void)
{
	// ���Z����
	SetBlendState(BLEND_MODE_SUBTRACT);

	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// �t�H�O����
	SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for(int i = 0; i < MAX_SHADOW; i++)
	{
		if(aShadow[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(aShadow[i].scl.x, aShadow[i].scl.y, aShadow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(aShadow[i].rot.x, aShadow[i].rot.y, aShadow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(aShadow[i].pos.x, aShadow[i].pos.y, aShadow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���̐ݒ�
			SetMaterial(aShadow[i].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// �ʏ�u�����h
	SetBlendState(BLEND_MODE_ALPHABLEND);
	
	// Z��r����
	SetDepthEnable(TRUE);

	// �t�H�O�����ɖ߂�
	SetFogEnable(GetFogEnable());
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexShadow()
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[1].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[2].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);
		vertex[3].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		// �g�U���̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[1].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[2].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[3].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(vertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �e�̍쐬
//=============================================================================
int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ)
{
	int nIdxShadow = -1;

	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		// ���g�p�iFALSE�j�������炻����g��
		if(!aShadow[nCntShadow].use)
		{
			aShadow[nCntShadow].pos = pos;
			aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			aShadow[nCntShadow].scl = XMFLOAT3(fSizeX, 1.0f, fSizeZ);
			aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			aShadow[nCntShadow].use = TRUE;

			nIdxShadow = nCntShadow;	// Index�ԍ�
			break;
		}
	}

	return nIdxShadow;		// Index�ԍ���Ԃ�
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorShadow(int nIdxShadow, XMFLOAT4 col)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		aShadow[nIdxShadow].material.Diffuse = col;
	}
}

//=============================================================================
// �e�̔j��
//=============================================================================
void ReleaseShadow(int nIdxShadow)
{
	if(nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		aShadow[nIdxShadow].use = FALSE;
	}
}

//=============================================================================
// �ʒu�̐ݒ�
//=============================================================================
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		aShadow[nIdxShadow].pos = pos;
	}
}

