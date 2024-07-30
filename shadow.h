//=============================================================================
//
// �e���� [shadow.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct SHADOW
{
	XMFLOAT3	pos;		// �ʒu
	XMFLOAT3	rot;		// ��]
	XMFLOAT3	scl;		// �X�P�[��
	MATERIAL	material;	// �}�e���A��
	BOOL		use;		// �g�p���Ă��邩�ǂ���

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitShadow(void);
void UninitShadow(void);
void UpdateShadow(void);
void DrawShadow(void);

int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ);
void ReleaseShadow(int nIdxShadow);
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos);
void SetColorShadow(int nIdxShadow, XMFLOAT4 col);

HRESULT MakeVertexShadow(void);
