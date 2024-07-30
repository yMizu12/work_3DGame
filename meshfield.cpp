//=============================================================================
//
// ���b�V���t�B�[���h�̏��� [meshfield.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX		(1)		// �e�N�X�`���̐�

//
static ID3D11Buffer					*vertexBuffer = NULL;				// ���_�o�b�t�@
static ID3D11Buffer					*indexBuffer = NULL;				// �C���f�b�N�X�o�b�t�@

static ID3D11ShaderResourceView		*texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							texNo;								// �e�N�X�`���ԍ�

static XMFLOAT3		posField;				// �|���S���\���ʒu�̒��S���W
static XMFLOAT3		rotField;				// �|���S���̉�]�p

static int			nNumBlockXField;		// �u���b�N��X
static int			nNumBlockZField;		// �u���b�N��Z
static int			nNumVertexField;		// �����_��	
static int			nNumVertexIndexField;	// ���C���f�b�N�X��
static int			nNumPolygonField;		// ���|���S����
static float		fBlockSizeXField;		// �u���b�N�T�C�YX
static float		fBlockSizeZField;		// �u���b�N�T�C�YZ

static char* textureName[TEXTURE_MAX] = {
	//"data/TEXTURE/test_plane.png",
	"data/TEXTURE/glass.jpg",
};

// �g�̏���
static VERTEX_3D	*Vertex = NULL;

// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
static XMFLOAT3		Center;						// �g�̔����ꏊ
static float		Time = 0.0f;				// �g�̌o�ߎ���
static float		wave_frequency  = 2.0f;		// �g�̎��g��
static float		wave_correction = 0.02f;	// �g�̋����␳
static float		wave_amplitude  = 20.0f;	// �g�̐U��
static BOOL			Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// �|���S���\���ʒu�̒��S���W��ݒ�
	posField = pos;

	rotField = rot;

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

	texNo = 0;

	// �u���b�N���̐ݒ�
	nNumBlockXField = nNumBlockX;
	nNumBlockZField = nNumBlockZ;

	// ���_���̐ݒ�
	nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// �|���S�����̐ݒ�
	nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// �u���b�N�T�C�Y�̐ݒ�
	fBlockSizeXField = fBlockSizeX;
	fBlockSizeZField = fBlockSizeZ;

	// ���_�����������ɍ���Ă���
	Vertex = new VERTEX_3D[nNumVertexField];

	// �n�`�����̂��߂̒��_���쐬����
	for (int z = 0; z < (nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (nNumBlockXField + 1); x++)
		{
			Vertex[z * (nNumBlockXField + 1) + x].Position.x = -(nNumBlockXField / 2.0f) * fBlockSizeXField + x * fBlockSizeXField;
			Vertex[z * (nNumBlockXField + 1) + x].Position.y = 0.0f;
			Vertex[z * (nNumBlockXField + 1) + x].Position.z = (nNumBlockZField / 2.0f) * fBlockSizeZField - z * fBlockSizeZField;

			// �@���̐ݒ�
			Vertex[z * (nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// ���ˌ��̐ݒ�
			Vertex[z * (nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			Vertex[z * (nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			Vertex[z * (nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}

	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// �C���f�b�N�X�o�b�t�@����
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &indexBuffer);


	{//���_�o�b�t�@�̒��g�𖄂߂�

		// ���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, Vertex, sizeof(VERTEX_3D)*nNumVertexField);

		GetDeviceContext()->Unmap(vertexBuffer, 0);
	}

	{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�

		// �C���f�b�N�X�o�b�t�@�̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxZ = 0; nCntVtxZ < nNumBlockZField; nCntVtxZ++)
		{
			if(nCntVtxZ > 0)
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockXField + 1);
				nCntIdx++;
			}

			for(int nCntVtxX = 0; nCntVtxX < (nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if(nCntVtxZ < (nNumBlockZField - 1))
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = nCntVtxZ * (nNumBlockXField + 1) + nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(indexBuffer, 0);
	}

	Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMeshField(void)
{
	if (Load == FALSE) return;

	// �C���f�b�N�X�o�b�t�@�̉��
	if (indexBuffer) {
		indexBuffer->Release();
		indexBuffer = NULL;
	}

	// ���_�o�b�t�@�̉��
	if (vertexBuffer) {
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

	if (Vertex != NULL)
	{
		delete[] Vertex;
		Vertex = NULL;
	}

	Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMeshField(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeshField(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);


	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(rotField.x, rotField.y, rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(posField.x, posField.y, posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);


	// �|���S���̕`��
	GetDeviceContext()->DrawIndexed(nNumVertexIndexField, 0, 0);
}



BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;

	if (start.x == 0.0f)
	{
		start.x += 0.5f;
		end.x += 0.5f;
	}
	if (start.z == 0.0f)
	{
		start.z -= 0.5f;
		end.z -= 0.5f;
	}

	// �����ォ��A�Y�h�[���Ɖ��փ��C���΂�
	start.y += 100.0f;
	end.y -= 1000.0f;

	// ����������������ׂɑS�����ł͂Ȃ��āA���W����|���S��������o��
	float fz = (nNumBlockZField / 2.0f) * fBlockSizeZField;
	float fx = (nNumBlockXField / 2.0f) * fBlockSizeXField;
	int sz = (int)((-start.z+fz) / fBlockSizeZField);
	int sx = (int)(( start.x+fx) / fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;


	if ((sz < 0) || (sz > nNumBlockZField-1) ||
		(sx < 0) || (sx > nNumBlockXField-1))
	{
		*Normal = {0.0f, 1.0f, 0.0f};
		return FALSE;
	}


	// �K�v�����������J��Ԃ�
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = Vertex[z * (nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = Vertex[z * (nNumBlockXField + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = Vertex[(z + 1) * (nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = Vertex[(z + 1) * (nNumBlockXField + 1) + (x + 1)].Position;

			// �O�p�|���S��������Q�����̓����蔻��
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}


	return FALSE;
}
