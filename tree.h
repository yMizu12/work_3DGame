//=============================================================================
//
// �؏��� [tree.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct TREE
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	BOOL		use;			// �g�p���Ă��邩�ǂ���

} ;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);
HRESULT MakeVertexTree(void);

