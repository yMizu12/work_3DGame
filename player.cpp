//=============================================================================
//
// プレイヤーの処理 [player.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "bullet.h"
#include "debugproc.h"
#include "meshfield.h"
#include "fieldobject.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player2.obj"	// 読み込むモデル名
#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)			// 回転量

#define PLAYER_INIT_POS_X	(-50.0f)
#define PLAYER_INIT_POS_Z	(-50.0f)
#define PLAYER_SHADOW_SIZE	(0.8f)						// 影の大きさ
#define PLAYER_OFFSET_Y		(24.0f)						// プレイヤーの足元をあわせる


//
static PLAYER	player;
static float	rotY = 0.0f;
static LIGHT	light;
static BOOL		lightTest = TRUE;	// 検証用
BulletManager	bulletMng;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	player.load = TRUE;
	LoadModel(MODEL_PLAYER, &player.model);

	player.pos = XMFLOAT3(PLAYER_INIT_POS_X, PLAYER_OFFSET_Y, PLAYER_INIT_POS_Z);
	player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	player.spd = 0.0f;			// 移動スピードクリア

	player.use = TRUE;			// TRUE:生きてる
	player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	// プレイヤー用の影を作成
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	// キーを押した時のプレイヤーの向き
	rotY = 0.0f;

	// クォータニオンの初期化
	XMStoreFloat4(&player.Quaternion, XMQuaternionIdentity());

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (player.load == TRUE)
	{
		UnloadModel(&player.model);
		player.load = FALSE;
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	player.spd *= 0.7f;
	if (player.spd < 0.1f) player.spd = 0.0f;

	// 移動処理
	if (GetKeyboardPress(DIK_LEFT))
	{
		player.spd = VALUE_MOVE;
		rotY = XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		player.spd = VALUE_MOVE;
		rotY = -XM_PI / 2;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		player.spd = VALUE_MOVE;
		rotY = XM_PI;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{
		player.spd = VALUE_MOVE;
		rotY = 0.0f;
	}

	DIJOYSTATE2 pad = GetPad();

	// 左スティックにいずれかの方向に入力のあった場合
	if (IsButtonPressed(0, STICK_LEFT_L) || IsButtonPressed(0, STICK_RIGHT_L)
		|| IsButtonPressed(0, STICK_UP_L) || IsButtonPressed(0, STICK_DOWN_L) )
	{
		player.spd = VALUE_MOVE;
		rotY = atan2f(float(pad.lX), float(pad.lY)) * -1;
	}


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		player.pos.z = player.pos.x = 0.0f;
		player.spd = 0.0f;
		rotY = 0.0f;
	}
#endif

	XMFLOAT3 oldPos = player.pos;
	XMFLOAT3 nowPos = player.pos;


	// キー入力があった時（移動スピードが0より大きい場合）
	if(player.spd > 0.0f)
	{	// 押した方向にプレイヤーを移動させる
		// 押した方向にプレイヤーを向かせている所
		player.rot.y = rotY + cam->rot.y;	// 

		//  y ←   2次元座標は、xが手前向きに正、yが左向きに正の座標系
		//      ↓ 0度、180度の時に、左手系3次元座標の z軸の＋－方向を向くようにするため、z軸の値をcosで計算、
		//      x  90度、270度の時に、左手系3次元座標の x軸の＋－方向を向くようにするため、x軸の値をsinで計算
		nowPos.x -= sinf(player.rot.y) * player.spd;
		nowPos.z -= cosf(player.rot.y) * player.spd;

		nowPos.y = player.pos.y;

		AREA* area = GetArea();
		int usedAreaNum = GetUsedArea();

		for (int i = 0; i < usedAreaNum; i++)
		{
			if (nowPos.x > area[i].min.x &&
				nowPos.x < area[i].max.x &&
				nowPos.z > area[i].min.z &&
				nowPos.z < area[i].max.z)
			{
				player.pos = oldPos;
				break;
			}
			else
			{
				player.pos = nowPos;
			}
		}

		// マップ外に出れないようにする
		if (nowPos.x < -1500 || nowPos.x > 1500 || nowPos.z < -1000 || nowPos.z > 1000)
		{
			player.pos = oldPos;
		}

	}


	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	BOOL ans = RayHitField(player.pos, &HitPosition, &Normal);
	if (ans)
	{
		player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


	// 弾発射処理
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_B))
	{
		XMFLOAT3 pos = player.pos;
		pos.y += 5.0f;

		bulletMng.SetBullet(pos, player.rot);
	}


	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(player.shadowIdx, pos);


	// ポイントライト
	{
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = player.pos;
		pos.y += 70.0f;		// 頭の上くらいの位置

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		light->Attenuation = 250.0f;
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;

#ifdef _DEBUG
		// ライトの色確認
		if (GetKeyboardTrigger(DIK_J))
		{
			lightTest = lightTest ? FALSE : TRUE;
		}

		if (lightTest == TRUE)
		{
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

		}
		else
		{
			light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		}

#endif


		SetLightData(1, light);
	}



	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;


	player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&player.Quaternion, quat);

#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Player X:%f Y:%f Z:%f N:%f\n", player.pos.x, player.pos.y, player.pos.z, Normal.y);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(player.scl.x, player.scl.y, player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(player.rot.x, player.rot.y + XM_PI, player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// クォータニオンを反映
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(player.pos.x, player.pos.y, player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&player.model);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &player;
}

