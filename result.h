//=============================================================================
//
// ���U���g��ʏ��� [result.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define GOOD_END		(100)	// �f���p�ݒ�
#define NORMAL_END		(10)	// �f���p�ݒ�

enum
{
	ENDING_GOOD,		// Good
	ENDING_NORMAL,		// Never
	ENDING_BAD,			// Cat
	ENDING_DEAD,		// Dead
	ENDING_MAX

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);

void SetEnding(int ending);