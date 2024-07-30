//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "fieldobject.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "ui.h"
#include "particle.h"
#include "collision.h"
#include "sky.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define WALL_BLOCK_NUM_X	(50)
#define WALL_BLOCK_SIZE		(50.0f)
#define WALL_COL_BLUE		(0.5f)


//
static int	viewPortType_Game = TYPE_FULL_SCREEN;
static BOOL	pause = TRUE;		// ポーズON/OFF
BulletManager bulletManager;
ParticleManager particleManager;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	viewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 300, 300, 20.0f, 20.0f);

	// 影の初期化処理
	InitShadow();

	// フィールドオブジェクトの配置
	InitFieldObject();

	InitSky();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(0.0f, 1.0f, WALL_COL_BLUE, 1.0f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X -10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, WALL_COL_BLUE, 0.25f), WALL_BLOCK_NUM_X +10, 1, WALL_BLOCK_SIZE, WALL_BLOCK_SIZE);

	// 木を生やす
	InitTree();

	// 弾の初期化
	bulletManager.InitBullet();

	// スコアの初期化
	InitScore();

	// UIの初期化
	InitUI();

	// パーティクルの初期化
	particleManager.InitParticle();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_GAME);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	particleManager.UninitParticle();

	// UIの終了処理
	UninitUI();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	bulletManager.UninitBullet();

	// 木の終了処理
	UninitTree();

	// 壁の終了処理
	UninitMeshWall();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	UninitSky();

	// フィールドオブジェクトの終了処理
	UninitFieldObject();

	// 影の終了処理
	UninitShadow();

	// 地面の終了処理
	UninitMeshField();


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		viewPortType_Game = (viewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(viewPortType_Game);
	}

#endif

	// 一時停止
	if (GetKeyboardTrigger(DIK_P))
	{
		pause = pause ? FALSE : TRUE;
	}

	if(pause == FALSE)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// フィールドオブジェクトの更新
	UpdateFieldObject();

	UpdateSky();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// 壁処理の更新
	UpdateMeshWall();

	// 木の更新処理
	UpdateTree();

	// 弾の更新処理
	bulletManager.UpdateBullet();

	// パーティクルの更新処理
	particleManager.UpdateParticle();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();

	// UIの更新処理
	UpdateUI();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	DrawShadow();

	DrawSky();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// 弾の描画処理
	bulletManager.DrawBullet();

	// フィールドオブジェクトの描画処理
	DrawFieldObject();

	// 壁の描画処理
	DrawMeshWall();

	// 木の描画処理
	DrawTree();

	// パーティクルの描画処理
	particleManager.DrawParticle();


	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawScore();

	// UIの描画処理
	DrawUI();


	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", viewPortType_Game);

	CAMERA* cam = GetCamera();
	PrintDebugProc("CameraRot   X:%f  Y:%f  Z:%f\n", XMConvertToDegrees(cam->rot.x), XMConvertToDegrees(cam->rot.y), XMConvertToDegrees(cam->rot.z));


#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = PLAYER_HEIGHT;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(viewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();					// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();				// プレイヤーのポインターを初期化
	BULLET *bullet = bulletManager.GetBullet();	// 弾のポインターを初期化

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// 敵キャラクターは倒される
			enemy[i].use = FALSE;
			ReleaseShadow(enemy[i].shadowIdx);

			// スコアを足す
			AddScore(-500);
		}
	}


	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].getUse() == FALSE)
			continue;

		// 敵と当たってるか調べる
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			//敵の有効フラグをチェックする
			if (enemy[j].use == FALSE)
				continue;

			XMFLOAT3 move;
			float fWidthBase = 3.0f;	// 基準の幅
			float fHeightBase = 4.0f;	// 基準の高さ
			float fAngle, fLength;


			//BCの当たり判定
			if (CollisionBC(bullet[i].getPos(), enemy[j].pos, bullet[i].getfWidth(), enemy[j].size))
			{

				for (int k = 0; k < 25; k++)
				{
					// パーティクル発生
					fAngle = (float)(rand() % 628 - 314) / 100.0f;						// 角度
					fLength = rand() % (int)(fWidthBase * 200) / 100.0f - fWidthBase;	// 距離
					move.x = sinf(fAngle) * fLength;
					move.y = rand() % 30 / 100.0f + fHeightBase;
					move.z = cosf(fAngle) * fLength;

					particleManager.SetParticle(enemy[j].pos, move, XMFLOAT4(0.4f, 0.0f, 1.0f, 0.85f), 1.0f, 1.0f, 100);
				}

				PlaySound(SOUND_LABEL_SE_ENEMYHIT);

				// 敵キャラクターは倒される
				enemy[j].use = FALSE;
				ReleaseShadow(enemy[j].shadowIdx);

				// スコアを足す
				AddScore(250);
			}
		}

	}



}


