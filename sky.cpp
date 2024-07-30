//=============================================================================
//
// 空の処理 [player.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "sky.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	SKY_MODEL			"data/MODEL/sky.obj"			// 読み込むモデル名
#define SKY_SCL				(5.0f)
#define	SKY_VALUE_ROTATE	(XM_PI * 0.0002f)				// 回転量

//
static SKY			sky;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	LoadModel(SKY_MODEL, &sky.model);
	sky.load = TRUE;

	sky.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sky.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	sky.scl = XMFLOAT3(SKY_SCL, SKY_SCL, SKY_SCL);

	sky.use = TRUE;			// TRUE:使用中

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	// モデルの解放処理
	if (sky.load == TRUE)
	{
		UnloadModel(&sky.model);
		sky.load = FALSE;
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{
	sky.rot.y += SKY_VALUE_ROTATE;

	if (sky.rot.y > -XM_PI)
	{
		sky.rot.y -= XM_PI * 2.0f;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(sky.scl.x, sky.scl.y, sky.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(sky.rot.x, sky.rot.y + XM_PI, sky.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(sky.pos.x, sky.pos.y, sky.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&sky.mtxWorld, mtxWorld);

	// モデル描画
	DrawModel(&sky.model);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}



