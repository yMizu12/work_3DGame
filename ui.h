//=============================================================================
//
// UI�̏��� [ui.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_NUM_PATTERN_DIVIDE_X	(13)	// �e�N�X�`�����������iX)
#define TEXTURE_NUM_PATTERN_DIVIDE_Y	(1)		// �e�N�X�`�����������iY)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);

int GetVillHp(void);
void AddVillHp(int add);
