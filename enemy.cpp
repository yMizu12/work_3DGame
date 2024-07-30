//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "player.h"
#include "shadow.h"
#include "sound.h"
#include "collision.h"
#include "fieldobject.h"
#include "ui.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY				"data/MODEL/ghost.obj"	// 読み込むモデル名

#define	VALUE_MOVE				(1.0f)			// 移動量
#define	VALUE_ROTATE			(XM_PI * 0.02f)	// 回転量

#define ENEMY_SHADOW_SIZE		(0.6f)			// 影の大きさ
#define ENEMY_OFFSET_Y			(25.0f)			// エネミーの足元をあわせる
#define ENEMY_SCALE				(2.0f)			// エネミーの大きさ
#define ENEMY_ROT				(0.0f)			// エネミー初期向き
#define ENEMY_INTERVAL			(5.0f)			// エネミー間の間隔
#define ENEMY_GOAL_INTERVAL		(15.0f)			// エネミーの目標到達時の当たり判定の大きさ

#define SPAWN_TIME				(120)			// 湧出に要する時間
#define SPAWN_SPACE_INTERVAL	(50.0f)			// 湧出時のエネミー間の間隔

#define SPAWN_C_X_CENTER		(0)				// 中央ゴールを標的とするエネミー湧出における、X軸の中央座標 
#define SPAWN_C_X_WIDTH			(2000)			// 中央ゴールを標的とするエネミー湧出のX軸の幅
#define SPAWN_C_Z_CENTER		(700)			// 中央ゴールを標的とするエネミー湧出における、Z軸の中央座標 
#define SPAWN_C_Z_WIDTH			(400)			// 中央ゴールを標的とするエネミー湧出のZ軸の幅

#define SPAWN_L_X_CENTER		(-1200)			// 左ゴールを標的とするエネミー湧出における、X軸の中央座標 
#define SPAWN_L_X_WIDTH			(400)			// 左ゴールを標的とするエネミー湧出のX軸の幅
#define SPAWN_L_Z_CENTER		(-200)			// 左ゴールを標的とするエネミー湧出における、Z軸の中央座標 
#define SPAWN_L_Z_WIDTH			(1340)			// 左ゴールを標的とするエネミー湧出のZ軸の幅

#define SPAWN_R_X_CENTER		(1200)			// 左ゴールを標的とするエネミー湧出における、X軸の中央座標 
#define SPAWN_R_X_WIDTH			(400)			// 左ゴールを標的とするエネミー湧出のX軸の幅
#define SPAWN_R_Z_CENTER		(-200)			// 左ゴールを標的とするエネミー湧出における、Z軸の中央座標 
#define SPAWN_R_Z_WIDTH			(1340)			// 左ゴールを標的とするエネミー湧出のZ軸の幅


//
static ENEMY enemy[MAX_ENEMY];		// エネミー
int enemy_load = 0;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &enemy[i].model);
		enemy[i].load = TRUE;

		//=========================================================================
		// ディゾルヴ
		//=========================================================================
		enemy[i].dissolve.Enable = TRUE;	// ディゾルブを有効にする
		enemy[i].dissolve.threshold = 1.0f;	// ディゾルブのしきい値を決める


		enemy[i].state = ENEMY_STATE_SPAWN;	//	最初はスポーン状態に設定
		enemy[i].spawnTimeCnt = SPAWN_TIME;
		enemy[i].targetGoalNo = rand() % MAX_GOAL;

		GOAL* goal = GetGoal();
		float posX = goal[enemy[i].targetGoalNo].pos.x;

		PLAYER* player = GetPlayer();	// プレイヤーのポインターを初期化


		int spawnCenterX = 0;	// エネミー湧出における、X軸の中央座標 
		int spawnWidthX  = 0;	// 湧出のX軸の幅
		int spawnCalcX   = 0;	// ランダム関数でX軸の中央座標を決める為に使用
		
		int spawnCenterZ = 0;	// エネミー湧出における、Z軸の中央座標 
		int spawnWidthZ  = 0;	// 湧出のZ軸の幅
		int spawnCalcZ   = 0;	// ランダム関数でZ軸の中央座標を決める為に使用


		if (posX == 0)	// 中央のゴールの場合
		{
			spawnCenterX = SPAWN_C_X_CENTER;
			spawnWidthX = SPAWN_C_X_WIDTH;
			spawnCalcX = spawnCenterX - spawnWidthX / 2;

			spawnCenterZ = SPAWN_C_Z_CENTER;
			spawnWidthZ = SPAWN_C_Z_WIDTH;
			spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

		}
		else if (posX < 0) // 村から見て左側のゴールの場合
		{
			spawnCenterX = SPAWN_L_X_CENTER;
			spawnWidthX = SPAWN_L_X_WIDTH;
			spawnCalcX = spawnCenterX - spawnWidthX / 2;

			spawnCenterZ = SPAWN_L_Z_CENTER;
			spawnWidthZ = SPAWN_L_Z_WIDTH;
			spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;


		}
		else // 右側のゴールの場合
		{
			spawnCenterX = SPAWN_R_X_CENTER;
			spawnWidthX = SPAWN_R_X_WIDTH;
			spawnCalcX = spawnCenterX - spawnWidthX / 2;

			spawnCenterZ = SPAWN_R_Z_CENTER;
			spawnWidthZ = SPAWN_R_Z_WIDTH;
			spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

		}


		// 湧出時にエネミーが重ならないようにする
		while (1)
		{
			enemy[i].pos = XMFLOAT3((float)(rand() % spawnWidthX + spawnCalcX), ENEMY_OFFSET_Y, (float)(rand() % spawnWidthZ + spawnCalcZ));

			if (!CollisionBC(player->pos, enemy[i].pos, SPAWN_SPACE_INTERVAL, SPAWN_SPACE_INTERVAL)) break;
		}


		enemy[i].rot = XMFLOAT3(ENEMY_ROT, ENEMY_ROT, ENEMY_ROT);
		enemy[i].scl = XMFLOAT3(ENEMY_SCALE, ENEMY_SCALE, ENEMY_SCALE);

		enemy[i].spd  = 0.0f;		// 移動スピードクリア
		enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&enemy[i].model, &enemy[i].diffuse[0]);

		XMFLOAT3 pos = enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		enemy[i].use = TRUE;		// TRUE:生きてる

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].load)
		{
			UnloadModel(&enemy[i].model);
			enemy[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes

			switch (enemy[i].state)
			{
				case ENEMY_STATE_SPAWN:

					enemy[i].spawnTimeCnt -= 1;

					if (enemy[i].spawnTimeCnt < 0)
					{
						enemy[i].spawnTimeCnt = SPAWN_TIME;
						enemy[i].dissolve.Enable = FALSE;
						enemy[i].state = ENEMY_STATE_ACTIVE;
						break;
					}

					enemy[i].dissolve.threshold -= 0.0085f;	// 

					break;


				case ENEMY_STATE_ACTIVE:
					GOAL* goal = GetGoal();	// ゴールのポインターを初期化

					int goalNo = enemy[i].targetGoalNo;

					float dx = goal[goalNo].pos.x - enemy[i].pos.x;
					float dz = goal[goalNo].pos.z - enemy[i].pos.z;

					float angle = atan2f(dz, dx);

					float tx = enemy[i].pos.x;
					float tz = enemy[i].pos.z;

					enemy[i].pos.x += cosf(angle) * VALUE_MOVE;
					enemy[i].pos.z += sinf(angle) * VALUE_MOVE;

					for (int j = 0; j < i; j++)
					{
						if (enemy[j].use == TRUE)		// このエネミーが使われている？
						{
							if (CollisionBC(enemy[i].pos, enemy[j].pos, ENEMY_INTERVAL, ENEMY_INTERVAL))	// エネミーが重ならないようにする
							{
								enemy[i].pos.x = tx;
								enemy[i].pos.z = tz;
								break;
							}
						}
					}

					enemy[i].rot.y = -angle - XM_PI * 0.5f;

					if (CollisionBC(enemy[i].pos, goal[goalNo].pos, ENEMY_GOAL_INTERVAL, ENEMY_GOAL_INTERVAL))
					{
						enemy[i].use = FALSE;
						ReleaseShadow(enemy[i].shadowIdx);
						AddVillHp(-1);

					}


					break;


			}

			//=========================================================================
			// ディゾルブ（動作確認）
			//=========================================================================
			if (GetKeyboardPress(DIK_I))   enemy[i].dissolve.threshold += 0.01f;
			if (GetKeyboardPress(DIK_K)) enemy[i].dissolve.threshold -= 0.01f;


			// 影をエネミーの位置に合わせる
			XMFLOAT3 pos = enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(enemy[i].shadowIdx, pos);


		}
		else
		{
			// エネミーが不使用(FALSE)のとき、再度復活させる

			enemy[i].state = ENEMY_STATE_SPAWN;			// スポーン状態に設定
			enemy[i].dissolve.Enable = TRUE;
			enemy[i].dissolve.threshold = 1.0f;

			enemy[i].targetGoalNo = rand() % MAX_GOAL;	// 標的のゴールを設定

			GOAL* goal = GetGoal();
			float posX = goal[enemy[i].targetGoalNo].pos.x;

			PLAYER* player = GetPlayer();	// プレイヤーのポインターを初期化


			int spawnCenterX = 0;	// エネミー湧出における、X軸の中央座標 
			int spawnWidthX  = 0;	// 湧出のX軸の幅
			int spawnCalcX   = 0;	// ランダム関数でX軸の中央座標を決める為に使用

			int spawnCenterZ = 0;	// エネミー湧出における、Z軸の中央座標 
			int spawnWidthZ  = 0;	// 湧出のZ軸の幅
			int spawnCalcZ   = 0;	// ランダム関数でZ軸の中央座標を決める為に使用


			if (posX == 0)	// 中央のゴールの場合
			{
				spawnCenterX = SPAWN_C_X_CENTER;
				spawnWidthX = SPAWN_C_X_WIDTH;
				spawnCalcX = spawnCenterX - spawnWidthX / 2;

				spawnCenterZ = SPAWN_C_Z_CENTER;
				spawnWidthZ = SPAWN_C_Z_WIDTH;
				spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

			}
			else if (posX < 0) // 村から見て左側のゴールの場合
			{
				spawnCenterX = SPAWN_L_X_CENTER;
				spawnWidthX = SPAWN_L_X_WIDTH;
				spawnCalcX = spawnCenterX - spawnWidthX / 2;

				spawnCenterZ = SPAWN_L_Z_CENTER;
				spawnWidthZ = SPAWN_L_Z_WIDTH;
				spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

			}
			else // 右側のゴールの場合
			{
				spawnCenterX = SPAWN_R_X_CENTER;
				spawnWidthX = SPAWN_R_X_WIDTH;
				spawnCalcX = spawnCenterX - spawnWidthX / 2;

				spawnCenterZ = SPAWN_R_Z_CENTER;
				spawnWidthZ = SPAWN_R_Z_WIDTH;
				spawnCalcZ = spawnCenterZ - spawnWidthZ / 2;

			}

			while (1)
			{
				enemy[i].pos = XMFLOAT3((float)(rand() % spawnWidthX + spawnCalcX), ENEMY_OFFSET_Y, (float)(rand() % spawnWidthZ + spawnCalcZ));

				if (!CollisionBC(player->pos, enemy[i].pos, SPAWN_SPACE_INTERVAL, SPAWN_SPACE_INTERVAL)) break;
			}

			enemy[i].rot = XMFLOAT3(ENEMY_ROT, ENEMY_ROT, ENEMY_ROT);	// 初期配置時の向きを正面に直す


			XMFLOAT3 pos = enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

			enemy[i].use = TRUE;			// TRUE:生きてる


		}

	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;


		//=========================================================================
		// ディゾルブ
		//=========================================================================
		SetDissolve(&enemy[i].dissolve);
		SetAlphaTestEnable(enemy[i].dissolve.Enable);		// αテストを有効に


		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(enemy[i].scl.x, enemy[i].scl.y, enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(enemy[i].rot.x, enemy[i].rot.y + XM_PI, enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(enemy[i].pos.x, enemy[i].pos.y, enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&enemy[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&enemy[i].model);

	}

		//=========================================================================
		// ディゾルブ
		//=========================================================================
		SetDissolveEnable(FALSE);
		SetAlphaTestEnable(FALSE);		// αテストを無効に


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy(void)
{
	return &enemy[0];
}
