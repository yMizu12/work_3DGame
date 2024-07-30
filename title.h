//=============================================================================
//
// タイトル画面処理 [title.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once


enum
{
	SELECT_STORY,
	SELECT_TUTORIAL,
	SELECT_START,

	SELECT_MAX,
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);


