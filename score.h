//=============================================================================
//
// �X�R�A���� [score.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_MAX			(999999)	// �X�R�A�̍ő�l
#define SCORE_MIN			(0)			// �X�R�A�̍ő�l
#define SCORE_DIGIT			(6)			// ����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);

