//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "model.h"
#include "bullet.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	BULLET_WIDTH			(10.0f)			// 頂点サイズ
#define	BULLET_HEIGHT			(10.0f)			// 頂点サイズ
#define	BULLET_SPEED			(10.0f)			// 弾の移動スピード
#define	BULLET_LIFE				(150)			// 弾の寿命
#define BULLET_DIFFUSE_X		(1.0f)
#define BULLET_DIFFUSE_Y		(1.0f)
#define BULLET_DIFFUSE_Z		(1.0f)
#define BULLET_DIFFUSE_A		(1.0f)
#define BULLET_INIT_POS_X		(0.0f)
#define BULLET_INIT_POS_Y		(0.0f)
#define BULLET_INIT_POS_Z		(0.0f)
#define BULLET_INIT_ROT_X		(0.0f)
#define BULLET_INIT_ROT_Y		(XM_PI * 0.5f)
#define BULLET_INIT_ROT_Z		(0.0f)
#define BULLET_INIT_SCL_X		(0.5f)
#define BULLET_INIT_SCL_Y		(0.5f)
#define BULLET_INIT_SCL_Z		(0.5f)
#define BULLET_SHADOW_POS_Y		(0.1f)
#define BULLET_SHADOW_SIZE_X	(0.5f)
#define BULLET_SHADOW_SIZE_Z	(0.5f)
#define	BULLET_MODEL			"data/MODEL/bullet_silver.obj"	// 読み込むモデル名

//
static BULLET			bullet[MAX_BULLET];	// 弾ワーク
static int				texNo;				// テクスチャ番号


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT BulletManager::InitBullet(void)
{
	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		bullet[nCntBullet].setLoad(TRUE);
		LoadModel(BULLET_MODEL, &bullet[nCntBullet].model);
		ZeroMemory(&bullet[nCntBullet].getMaterial(), sizeof(bullet[nCntBullet].getMaterial()));
		bullet[nCntBullet].setMaterialDiffuse(BULLET_DIFFUSE_X, BULLET_DIFFUSE_Y, BULLET_DIFFUSE_Z, BULLET_DIFFUSE_A);
		bullet[nCntBullet].setPos(BULLET_INIT_POS_X, BULLET_INIT_POS_Y, BULLET_INIT_POS_Z);
		bullet[nCntBullet].setRot(BULLET_INIT_ROT_X, BULLET_INIT_ROT_Y, BULLET_INIT_ROT_Z);
		bullet[nCntBullet].setScl(BULLET_INIT_SCL_X, BULLET_INIT_SCL_Y, BULLET_INIT_SCL_Z);
		bullet[nCntBullet].setSpd(BULLET_SPEED);
		bullet[nCntBullet].setfWidth(BULLET_WIDTH);
		bullet[nCntBullet].setfHeight(BULLET_HEIGHT);
		bullet[nCntBullet].setLife(BULLET_LIFE);
		bullet[nCntBullet].setUse(FALSE);
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void BulletManager::UninitBullet(void)
{
	for (int i = 0; i < MAX_BULLET; i++)
	{
		// モデルの解放処理
		if (bullet[i].getLoad() == TRUE)
		{
			UnloadModel(&bullet[i].getModel());
			bullet[i].setLoad(FALSE);
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void BulletManager::UpdateBullet(void)
{

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (bullet[i].getUse())
		{
			// 弾の移動処理
			{
				XMFLOAT3 pos = bullet[i].getPos();
				XMFLOAT3 rot = bullet[i].getRot();
				float	 spd = bullet[i].getSpd();
				pos.x -= sinf(rot.y) * spd;
				pos.z -= cosf(rot.y) * spd;
				bullet[i].setPos(pos);
			}

			// 影の位置設定
			{
				int idx = bullet[i].getShadowIdx();
				XMFLOAT3 pos = bullet[i].getPos();
				SetPositionShadow(idx, XMFLOAT3(pos.x, BULLET_SHADOW_POS_Y, pos.z));
			}

			// 弾の寿命を減らす
			{
				int life = bullet[i].getLife();
				bullet[i].setLife(life - 1);

				if (bullet[i].getLife() < 0)
				{
					bullet[i].setUse(FALSE);
					ReleaseShadow(bullet[i].getShadowIdx());
				}
			}

		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void BulletManager::DrawBullet(void)
{
	// ライティングを無効
	SetLightEnable(FALSE);

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (bullet[i].getUse())
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			XMFLOAT3 scl = bullet[i].getScl();
			mtxScl = XMMatrixScaling(scl.x, scl.y, scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			XMFLOAT3 rot = bullet[i].getRot();
			mtxRot = XMMatrixRotationRollPitchYaw(rot.x, rot.y + XM_PI, rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			XMFLOAT3 pos = bullet[i].getPos();
			mtxTranslate = XMMatrixTranslation(pos.x, pos.y, pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&bullet[i].getMtxWorld(), mtxWorld);

			// モデル描画
			DrawModel(&bullet[i].getModel());

		}
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	// ライティングを有効に
	SetLightEnable(TRUE);

}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int BulletManager::SetBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!bullet[nCntBullet].getUse())
		{
			bullet[nCntBullet].setPos(pos);

			XMFLOAT3 bulletRot = rot;
			bulletRot.x = XM_PI * 0.5f;

			bullet[nCntBullet].setRot(bulletRot);
			bullet[nCntBullet].setUse(TRUE);

			// 影の設定
			bullet[nCntBullet].setShadowIdx(CreateShadow(pos, BULLET_SHADOW_SIZE_X, BULLET_SHADOW_SIZE_Z));

			nIdxBullet = nCntBullet;

			// 弾の寿命の設定
			bullet[nCntBullet].setLife(BULLET_LIFE);

			break;
		}
	}

	PlaySound(SOUND_LABEL_SE_GUNSHOT);

	return nIdxBullet;
}


//=============================================================================
// 弾の取得
//=============================================================================
BULLET* BulletManager::GetBullet(void)
{
	return &(bullet[0]);
}



XMFLOAT4X4 BULLET::getMtxWorld(void)
{
	return this->mtxWorld;
}
void BULLET::setMtxWorld(XMFLOAT4X4 mtx)
{
	this->mtxWorld = mtx;
}

XMFLOAT3 BULLET::getPos(void)
{
	return this->pos;
}
void BULLET::setPos(XMFLOAT3 pos)
{
	this->pos = pos;
}
void BULLET::setPos(float x, float y, float z)
{
	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
}
void BULLET::setPosXYZ(char xyz, float value)
{
	switch (xyz)
	{
	case 'x':
		this->pos.x = value;
		break;

	case 'y':
		this->pos.y = value;
		break;

	case 'z':
		this->pos.z = value;
		break;

	default:
		this->pos.x = value;
		break;
	}

}


XMFLOAT3 BULLET::getRot(void)
{
	return this->rot;
}
void BULLET::setRot(XMFLOAT3 rot)
{
	this->rot = rot;
}
void BULLET::setRot(float x, float y, float z)
{
	this->rot.x = x;
	this->rot.y = y;
	this->rot.z = z;
}

XMFLOAT3 BULLET::getScl(void)
{
	return this->scl;
}
void BULLET::setScl(XMFLOAT3 scl)
{
	this->scl = scl;
}
void BULLET::setScl(float x, float y, float z)
{
	this->scl.x = x;
	this->scl.y = y;
	this->scl.z = z;
}

MATERIAL BULLET::getMaterial(void)
{
	return this->material;
}
void BULLET::setMaterial(MATERIAL material)
{
	this->material = material;
}
void BULLET::setMaterialDiffuse(float r, float g, float b, float a)
{
	this->material.Diffuse.x = r;
	this->material.Diffuse.y = g;
	this->material.Diffuse.z = b;
	this->material.Diffuse.w = a;
}


float BULLET::getSpd(void)
{
	return this->spd;
}
void BULLET::setSpd(float spd)
{
	this->spd = spd;
}

float BULLET::getfWidth(void)
{
	return this->fWidth;
}
void BULLET::setfWidth(float w)
{
	this->fWidth = w;
}

float BULLET::getfHeight(void)
{
	return this->fHeight;
}
void BULLET::setfHeight(float h)
{
	this->fHeight = h;
}

int BULLET::getLife(void)
{
	return this->life;
}
void BULLET::setLife(int life)
{
	this->life = life;
}

int BULLET::getShadowIdx(void)
{
	return this->shadowIdx;
}
void BULLET::setShadowIdx(int idx)
{
	this->shadowIdx = idx;
}

BOOL BULLET::getUse(void)
{
	return this->use;
}
void BULLET::setUse(BOOL use)
{
	this->use = use;
}

BOOL BULLET::getLoad(void)
{
	return this->load;
}
void BULLET::setLoad(BOOL load)
{
	this->load = load;
}

DX11_MODEL BULLET::getModel(void)
{
	return this->model;
}
void BULLET::setModel(DX11_MODEL model)
{
	this->model = model;
}

