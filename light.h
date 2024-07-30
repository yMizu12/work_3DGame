//=============================================================================
//
// ���C�g���� [light.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
BOOL GetFogEnable(void);

LIGHT *GetLightData(int index);

