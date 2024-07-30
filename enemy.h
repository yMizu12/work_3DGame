//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY		(30)		// エネミーの数
#define	ENEMY_SIZE		(15.0f)		// 当たり判定の大きさ

enum
{
	ENEMY_STATE_SPAWN,		// 湧出中
	ENEMY_STATE_ACTIVE,		// アクティブ状態（湧出演出終了後）
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;							// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	XMFLOAT3			P0;
	XMFLOAT3			P1;
	XMFLOAT3			P2;
	float				s;

	int					state;				// 状態
	int					spawnTimeCnt;		// 湧出時の内部タイマー

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号

	int					targetGoalNo;		// 標的のゴール番号

	DISSOLVE			dissolve;			// ディゾルブ

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

