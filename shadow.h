//=============================================================================
//
// 影処理 [shadow.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct SHADOW
{
	XMFLOAT3	pos;		// 位置
	XMFLOAT3	rot;		// 回転
	XMFLOAT3	scl;		// スケール
	MATERIAL	material;	// マテリアル
	BOOL		use;		// 使用しているかどうか

};

//*****************************************************************************
// プロトタイプ宣言
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
