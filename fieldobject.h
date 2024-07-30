//=============================================================================
//
// フィールドオブジェクトのモデル処理 [fieldobject.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_FIELD_OBJECT	(200)	// フィールドオブジェクトの最大数
#define MAX_GOAL			(3)		// ゴールの最大数
#define MAX_AREA			(6)		// エリアの最大数
#define MAX_AREA_VERTEX		(4)		// エリアの最大頂点数（今回は四角形なので４）

enum
{
	F_OBJ_FLOOR,
	F_OBJ_SLOPE,
	F_OBJ_PILLAR,
	F_OBJ_HOME,
	F_OBJ_ROOF,
	F_OBJ_AREA_NOT_ENTRY_P = 98,
	F_OBJ_GOAL = 99,
};

enum
{
	AREA_NOT_ENTRY_P,	// プレイヤー侵入不可エリア
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct FOBJECT
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;							// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	int					shadowIdx;			// 影のインデックス番号
	int					objType;
};

struct GOAL
{
	XMFLOAT3			pos;				// モデルの位置
	BOOL				use;
};

struct AREA
{
	XMFLOAT3			vtxPos[MAX_AREA_VERTEX];	// モデルの頂点座標
	XMFLOAT3			max;
	XMFLOAT3			min;
	BOOL				use;
	int					areaType;
	
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFieldObject(void);
void UninitFieldObject(void);
void UpdateFieldObject(void);
void DrawFieldObject(void);

FOBJECT *GetFieldObject(void);

void SetFieldObject(int objType, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scl);

GOAL* GetGoal(void);

AREA* GetArea(void);
void calcArea(AREA* area);
int GetUsedArea(void);
