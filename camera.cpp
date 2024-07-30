//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CAMERA_POS_X				(0.0f)			// カメラの初期位置(X座標)
#define	CAMERA_POS_Y				(50.0f)			// カメラの初期位置(Y座標)
#define	CAMERA_POS_Z				(-140.0f)		// カメラの初期位置(Z座標)

#define CAMERA_AT_X					(0.0f)			// カメラの初期注視点(X座標)
#define CAMERA_AT_Y					(0.0f)			// カメラの初期注視点(Y座標)
#define CAMERA_AT_Z					(0.0f)			// カメラの初期注視点(Z座標)

#define CAMERA_UP_X					(0.0f)			// カメラの上方向の設定(X座標)
#define CAMERA_UP_Y					(1.0f)			// カメラの上方向の設定(Y座標)
#define CAMERA_UP_Z					(0.0f)			// カメラの上方向の設定(Z座標)

#define CAMERA_ROT_X				(XM_PI/2 +0.5f)	// カメラの初期回転角の設定(X座標)
#define CAMERA_ROT_Y				(0.0f)			// カメラの初期回転角の設定(Y座標)
#define CAMERA_ROT_Z				(0.0f)			// カメラの初期回転角の設定(Z座標)

#define	VIEW_ANGLE					(XMConvertToRadians(50.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT					((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z					(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z					(10000.0f)										// ビュー平面のFarZ値

#define	CAMERA_VALUE_MOVE						(2.0f)							// カメラの移動量
#define	CAMERA_VALUE_MOVE_2						(4.0f)							// カメラの移動量
#define	CAMERA_VALUE_ROTATE						(XM_PI * 0.015f)				// カメラの回転量
#define	CAMERA_VALUE_ROTATE_V					(CAMERA_VALUE_ROTATE * 0.75f)	// カメラの回転量
#define	CAMERA_VALUE_ROTATE_V_CORR				(0.008f)						// カメラの回転量補正値

#define CAMERA_VALUE_MOVE_LEN					(1.5f)
#define CAMERA_VALUE_ROTATE_LIMIT_X_TOP			(170.0f)
#define CAMERA_VALUE_ROTATE_LIMIT_X_DOWN		(10.0f)		// カメラが上向きになる限界角度
#define CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR	(80.0f)
#define CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_NEAR	(50.0f)
#define CAMERA_VALUE_ADD_HEIGHT_AT_FAR			(0.9f)
#define CAMERA_VALUE_ADD_HEIGHT_AT_NEAR			(0.5f)



//
static CAMERA			camera;		// カメラデータ

static float			cameraBaseLength = 0.0f;			// プレイヤーに寄ってない時のカメラ距離
static float			atHeight = 0.0f;					// プレイヤーに寄った時の注視点の高さの増減

static int				viewPortType = TYPE_FULL_SCREEN;

//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	camera.pos = { CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z };
	camera.at  = { CAMERA_AT_X, CAMERA_AT_Y, CAMERA_AT_Z };
	camera.up  = { CAMERA_UP_X, CAMERA_UP_Y, CAMERA_UP_Z };
	camera.rot = { CAMERA_ROT_X, CAMERA_ROT_Y, CAMERA_ROT_Z };

	// 視点と注視点の距離を計算
	float vx;	// X座標
	float vz;	// Z座標
	float vxz;	// X座標・Z座標の平方根
	float vy;	// Y座標

	vx = camera.pos.x - camera.at.x;
	vz = camera.pos.z - camera.at.z;
	vxz = sqrtf(vx * vx + vz * vz);

	vy = camera.pos.y - camera.at.y;

	cameraBaseLength = sqrtf(vxz * vxz + vy * vy);
	camera.len = cameraBaseLength;
	
	// ビューポートタイプの初期化
	SetViewPort(viewPortType);
}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{

	if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, STICK_RIGHT_R))
	{// 視点旋回「左」
		camera.rot.y += CAMERA_VALUE_ROTATE;
		if (camera.rot.y > XM_PI)
		{
			camera.rot.y -= XM_PI * 2.0f;
		}
	}

	if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, STICK_LEFT_R))
	{// 視点旋回「右」
		camera.rot.y -= CAMERA_VALUE_ROTATE;
		if (camera.rot.y < -XM_PI)
		{
			camera.rot.y += XM_PI * 2.0f;
		}
	}


	// スティックを上に倒した時に、カメラも上を向く
	if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, STICK_UP_R))
	{// カメラ回転「上」

		if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			camera.rot.x -= CAMERA_VALUE_ROTATE_V + CAMERA_VALUE_ROTATE_V_CORR;
		}
		else
		{
			camera.rot.x -= CAMERA_VALUE_ROTATE_V;
		}


		// 上向きの限界角度を超えてカメラが回らないようにする
		if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN))
		{
			camera.rot.x = XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN);
		}


		if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			if (camera.rot.x > XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN))
			{
				atHeight += CAMERA_VALUE_ADD_HEIGHT_AT_FAR;

			}
		}

	}

	// スティックを下に倒した時に、カメラも下を向く
	if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, STICK_DOWN_R))
	{// カメラ回転「下」

		if (camera.rot.x <= XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			camera.rot.x += CAMERA_VALUE_ROTATE_V + CAMERA_VALUE_ROTATE_V_CORR;
		}
		else
		{
			camera.rot.x += CAMERA_VALUE_ROTATE_V;
		}

		// 下向きの限界角度を超えてカメラが回らないようにする
		if (camera.rot.x > XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_TOP))
		{
			camera.rot.x = XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_TOP);
		}


		if (camera.rot.x <= XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			atHeight -= CAMERA_VALUE_ADD_HEIGHT_AT_FAR;

		}
		else if (camera.rot.x > XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
		{
			atHeight = 0.0f;
		}

	}


	if (GetKeyboardPress(DIK_T))
	{// 注視点移動「上」
		camera.at.y += CAMERA_VALUE_MOVE;
	}

	if (GetKeyboardPress(DIK_B))
	{// 注視点移動「下」
		camera.at.y -= CAMERA_VALUE_MOVE;
	}

	if (GetKeyboardPress(DIK_U))
	{// 近づく
		//camera.len -= CAMERA_VALUE_MOVE;
		camera.len -= CAMERA_VALUE_MOVE_2;
		camera.pos.x = camera.at.x - sinf(camera.rot.y) * camera.len;
		camera.pos.z = camera.at.z - cosf(camera.rot.y) * camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		//camera.len += CAMERA_VALUE_MOVE;
		camera.len += CAMERA_VALUE_MOVE_2;
		camera.pos.x = camera.at.x - sinf(camera.rot.y) * camera.len;
		camera.pos.z = camera.at.z - cosf(camera.rot.y) * camera.len;
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void) 
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&camera.pos), XMLoadFloat3(&camera.at), XMLoadFloat3(&camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&camera.mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&camera.mtxProjection, mtxProjection);

	SetShaderCamera(camera.pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &camera;
}

//=============================================================================
// ビューポートの設定
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	viewPortType = type;

	// ビューポート設定
	switch (viewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}


int GetViewPortType(void)
{
	return viewPortType;
}



// カメラの視点と注視点をセット
void SetCameraAT(XMFLOAT3 pos)
{
	// カメラの注視点を引数の座標に
	pos.y += atHeight;
	camera.at = pos;

	camera.pos.x = camera.at.x - sinf(camera.rot.x) * sinf(camera.rot.y) * camera.len;
	camera.pos.z = camera.at.z - sinf(camera.rot.x) * cosf(camera.rot.y) * camera.len;


	if (camera.rot.x < XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR))
	{
		camera.pos.y = camera.at.y - cosf(XMConvertToRadians(CAMERA_VALUE_ROTATE_LIMIT_X_DOWN_FAR)) * camera.len;
	}
	else
	{
		camera.pos.y = camera.at.y - cosf(camera.rot.x) * camera.len;
	}

}

