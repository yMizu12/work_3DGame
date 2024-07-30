//=============================================================================
//
// モデルの処理 [model.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE_MODEL	(0.50f)					// 移動速度
#define	RATE_MOVE_MODEL		(0.20f)					// 移動慣性係数
#define	VALUE_ROTATE_MODEL	(XM_PI * 0.05f)			// 回転速度
#define	RATE_ROTATE_MODEL	(0.20f)					// 回転慣性係数

#define	SCALE_MODEL						(10.0f)		// モデルの大きさの倍率
#define	SCALE_MODEL_FIELDOBJECT			(1.0f)		// モデルの大きさの倍率

#define MODEL_MAX_MATERIAL		(16)				// １モデルのMaxマテリアル数


//*********************************************************
// 構造体定義
//*********************************************************
// マテリアル構造体
struct MODEL_MATERIAL
{
	char		Name[256];
	MATERIAL	Material;
	char		TextureName[256];
};

// 描画サブセット構造体
struct SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	MODEL_MATERIAL	Material;
};

// モデル構造体
struct MODEL
{
	VERTEX_3D* VertexArray;
	unsigned short	VertexNum;
	unsigned short* IndexArray;
	unsigned short	IndexNum;
	SUBSET* SubsetArray;
	unsigned short	SubsetNum;
};


// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL					Material;
	ID3D11ShaderResourceView	*Texture;
};

// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct DX11_MODEL
{
	ID3D11Buffer*	VertexBuffer;
	ID3D11Buffer*	IndexBuffer;

	DX11_SUBSET		*SubsetArray;
	unsigned short	SubsetNum;
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadModel( char *FileName, DX11_MODEL *Model );
void UnloadModel( DX11_MODEL *Model );
void DrawModel( DX11_MODEL *Model );

// モデルのマテリアルのディフューズを取得する。
void GetModelDiffuse(DX11_MODEL *Model, XMFLOAT4 *diffuse);

// モデルの指定マテリアルのディフューズをセットする。
void SetModelDiffuse(DX11_MODEL *Model, int mno, XMFLOAT4 diffuse);

void LoadObj(char* FileName, MODEL* Model);
void LoadMaterial(char* FileName, MODEL_MATERIAL** MaterialArray, unsigned short* MaterialNum);

