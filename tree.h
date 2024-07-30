//=============================================================================
//
// 木処理 [tree.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct TREE
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	BOOL		use;			// 使用しているかどうか

} ;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);
HRESULT MakeVertexTree(void);

