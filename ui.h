//=============================================================================
//
// UIの処理 [ui.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_NUM_PATTERN_DIVIDE_X	(13)	// テクスチャ内分割数（X)
#define TEXTURE_NUM_PATTERN_DIVIDE_Y	(1)		// テクスチャ内分割数（Y)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);

int GetVillHp(void);
void AddVillHp(int add);

