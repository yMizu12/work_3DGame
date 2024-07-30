//=============================================================================
//
// メッシュウォールの処理 [meshwall.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once

#include "renderer.h"

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct MESH_WALL
{
	ID3D11Buffer* vertexBuffer;					// 頂点バッファ
	ID3D11Buffer* indexBuffer;					// インデックスバッファ

	XMFLOAT3		pos;						// ポリゴン表示位置の中心座標
	XMFLOAT3		rot;						// ポリゴンの回転角
	MATERIAL		material;					// マテリアル
	int				nNumBlockX, nNumBlockY;		// ブロック数
	int				nNumVertex;					// 総頂点数	
	int				nNumVertexIndex;			// 総インデックス数
	int				nNumPolygon;				// 総ポリゴン数
	float			fBlockSizeX, fBlockSizeY;	// ブロックサイズ
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshWall(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
						int nNumBlockX, int nNumBlockY, float fSizeBlockX, float fSizeBlockY);
void UninitMeshWall(void);
void UpdateMeshWall(void);
void DrawMeshWall(void);

