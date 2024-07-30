//=============================================================================
//
// ���b�V���E�H�[���̏��� [meshwall.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once

#include "renderer.h"

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct MESH_WALL
{
	ID3D11Buffer* vertexBuffer;					// ���_�o�b�t�@
	ID3D11Buffer* indexBuffer;					// �C���f�b�N�X�o�b�t�@

	XMFLOAT3		pos;						// �|���S���\���ʒu�̒��S���W
	XMFLOAT3		rot;						// �|���S���̉�]�p
	MATERIAL		material;					// �}�e���A��
	int				nNumBlockX, nNumBlockY;		// �u���b�N��
	int				nNumVertex;					// �����_��	
	int				nNumVertexIndex;			// ���C���f�b�N�X��
	int				nNumPolygon;				// ���|���S����
	float			fBlockSizeX, fBlockSizeY;	// �u���b�N�T�C�Y
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshWall(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
						int nNumBlockX, int nNumBlockY, float fSizeBlockX, float fSizeBlockY);
void UninitMeshWall(void);
void UpdateMeshWall(void);
void DrawMeshWall(void);

