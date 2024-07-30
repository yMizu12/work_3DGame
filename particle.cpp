//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "particle.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数
#define PARTICLE_SIZE_RATE	(4.0f)			// サイズ割合
#define	VALUE_MOVE_PARTICLE	(5.0f * 10)		// 移動速度
#define	MAX_PARTICLE		(512)			// パーティクル最大数
#define	DISP_SHADOW							// 影の表示
#define	PARTICLE_SIZE_X		(40.0f / PARTICLE_SIZE_RATE)	// 頂点サイズ
#define	PARTICLE_SIZE_Y		(40.0f / PARTICLE_SIZE_RATE)	// 頂点サイズ


//
static ID3D11Buffer					*vertexBuffer = NULL;				// 頂点バッファ
static ID3D11ShaderResourceView		*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							texNo;								// テクスチャ番号

static PARTICLE		aParticle[MAX_PARTICLE];	// パーティクルワーク
static XMFLOAT3		posBase;					// ビルボード発生位置
static float		fWidthBase = 5.0f;			// 基準の幅
static float		fHeightBase = 10.0f;		// 基準の高さ
static float		roty = 0.0f;				// 移動方向
static float		spd = 0.0f;					// 移動スピード
static BOOL			load = FALSE;

static char *textureName[] =
{
	"data/TEXTURE/effect000.jpg",
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT ParticleManager::InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			textureName[i],
			NULL,
			NULL,
			&texture[i],
			NULL);
	}

	texNo = 0;

	// パーティクルワークの初期化
	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		aParticle[nCntParticle].setPos(0.0f, 0.0f, 0.0f);
		aParticle[nCntParticle].setRot(0.0f, 0.0f, 0.0f);
		aParticle[nCntParticle].setScale(1.0f, 1.0f, 1.0f);
		aParticle[nCntParticle].setMove(1.0f, 1.0f, 1.0f);

		ZeroMemory(&aParticle[nCntParticle].getMaterial(), sizeof(aParticle[nCntParticle].getMaterial()));
		aParticle[nCntParticle].setMaterialDiff(1.0f, 1.0f, 1.0f, 1.0f);

		aParticle[nCntParticle].setfSizeX(PARTICLE_SIZE_X);
		aParticle[nCntParticle].setfSizeY(PARTICLE_SIZE_Y);
		aParticle[nCntParticle].setnIdxShadow(-1);
		aParticle[nCntParticle].setnLife(0);
		aParticle[nCntParticle].setUse(FALSE);
	}

	posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	roty = 0.0f;
	spd = 0.0f;

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void ParticleManager::UninitParticle(void)
{
	if (load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (texture[nCntTex] != NULL)
		{
			texture[nCntTex]->Release();
			texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void ParticleManager::UpdateParticle(void)
{
	{
		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			if(aParticle[nCntParticle].getUse())
			{// 使用中
				float px,pz,py,my,mx,mz;

				px = aParticle[nCntParticle].getPos().x;
				mx = aParticle[nCntParticle].getMove().x;
				aParticle[nCntParticle].setPosXYZ('x', px + mx);
				
				pz = aParticle[nCntParticle].getPos().z;
				mz = aParticle[nCntParticle].getMove().z;
				aParticle[nCntParticle].setPosXYZ('z', pz + mz);

				py = aParticle[nCntParticle].getPos().y;
				my = aParticle[nCntParticle].getMove().y;
				aParticle[nCntParticle].setPosXYZ('y', py + my);
				if(aParticle[nCntParticle].getPos().y <= aParticle[nCntParticle].getfSizeY() / 4)
				{// 着地した
					aParticle[nCntParticle].setPosXYZ('y', aParticle[nCntParticle].getfSizeY() / 4);
					aParticle[nCntParticle].setMoveXYZ('y', -aParticle[nCntParticle].getMove().y * 0.75f);
				}


				aParticle[nCntParticle].setMoveXYZ('x', aParticle[nCntParticle].getMove().x + (0.0f - aParticle[nCntParticle].getMove().x) * 0.015f);
				aParticle[nCntParticle].setMoveXYZ('y', aParticle[nCntParticle].getMove().y - 0.25f);
				aParticle[nCntParticle].setMoveXYZ('z', aParticle[nCntParticle].getMove().z + (0.0f - aParticle[nCntParticle].getMove().z) * 0.015f);

#ifdef DISP_SHADOW
				if(aParticle[nCntParticle].getnIdxShadow() != -1)
				{// 影使用中
					float colA;

					// 影の位置設定
					SetPositionShadow(aParticle[nCntParticle].getnIdxShadow(), XMFLOAT3(aParticle[nCntParticle].getPos().x, 0.1f, aParticle[nCntParticle].getPos().z));

					// 影の色の設定
					colA = aParticle[nCntParticle].getMaterial().Diffuse.w;
					SetColorShadow(aParticle[nCntParticle].getnIdxShadow(), XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
				}
#endif

				int life = aParticle[nCntParticle].getnLife();
				aParticle[nCntParticle].setnLife(life-1);
				if (aParticle[nCntParticle].getnLife() <= 0)
				{
					aParticle[nCntParticle].setUse(FALSE);
					ReleaseShadow(aParticle[nCntParticle].getnIdxShadow());
					aParticle[nCntParticle].setnIdxShadow(-1);
				}
				else
				{
					if(aParticle[nCntParticle].getnLife() <= 80)
					{
						aParticle[nCntParticle].setMaterialDiffXYZW('x', 0.8f - (float)(80 - aParticle[nCntParticle].getnLife()) / 80 * 0.8f);
						aParticle[nCntParticle].setMaterialDiffXYZW('y', 0.7f - (float)(80 - aParticle[nCntParticle].getnLife()) / 80 * 0.7f);
						aParticle[nCntParticle].setMaterialDiffXYZW('z', 0.2f - (float)(80 - aParticle[nCntParticle].getnLife()) / 80 * 0.2f);
					}

					if(aParticle[nCntParticle].getnLife() <= 20)
					{
						// α値設定
						float w = aParticle[nCntParticle].getMaterial().Diffuse.w;
						aParticle[nCntParticle].setMaterialDiffXYZW('w', w - 0.05f);
						if(aParticle[nCntParticle].getMaterial().Diffuse.w < 0.0f)
						{
							aParticle[nCntParticle].setMaterialDiffXYZW('w', 0.0f);
						}
					}
				}
			}
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void ParticleManager::DrawParticle(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(aParticle[nCntParticle].getUse())
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// 処理が速い
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(aParticle[nCntParticle].getScale().x, aParticle[nCntParticle].getScale().y, aParticle[nCntParticle].getScale().z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(aParticle[nCntParticle].getPos().x, aParticle[nCntParticle].getPos().y, aParticle[nCntParticle].getPos().z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(aParticle[nCntParticle].getMaterial());

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	SetFogEnable( GetFogEnable() );

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT ParticleManager::MakeVertexParticle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(vertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void ParticleManager::SetColorParticle(int nIdxParticle, XMFLOAT4 col)
{
	aParticle[nIdxParticle].setMaterialDiff(col);
}

//=============================================================================
// パーティクルの発生処理
//=============================================================================
int ParticleManager::SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!aParticle[nCntParticle].getUse())
		{
			aParticle[nCntParticle].setPos(pos);
			aParticle[nCntParticle].setRot(0.0f, 0.0f, 0.0f);
			aParticle[nCntParticle].setScale(1.0f, 1.0f, 1.0f);
			aParticle[nCntParticle].setMove(move);
			aParticle[nCntParticle].setMaterialDiff(col);
			aParticle[nCntParticle].setfSizeX(fSizeX);
			aParticle[nCntParticle].setfSizeY(fSizeY);
			aParticle[nCntParticle].setnLife(nLife);
			aParticle[nCntParticle].setUse(TRUE);

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// 影の設定
			aParticle[nCntParticle].setnIdxShadow(CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f));		// 影の設定
			if(aParticle[nCntParticle].getnIdxShadow() != -1)
			{
				SetColorShadow(aParticle[nCntParticle].getnIdxShadow(), XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}



XMFLOAT3 PARTICLE::getPos(void)
{
	return this->pos;
}
void PARTICLE::setPos(float x, float y, float z)
{
	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
}

void PARTICLE::setPos(XMFLOAT3 pos)
{
	this->pos = pos;
}

void PARTICLE::setPosXYZ(char xyz, float value)
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


XMFLOAT3 PARTICLE::getRot(void)
{
	return this->rot;
}
void PARTICLE::setRot(float x, float y, float z)
{
	this->rot.x = x;
	this->rot.y = y;
	this->rot.z = z;
}
void PARTICLE::setRotXYZ(char xyz, float value)
{
	switch (xyz)
	{
	case 'x':
		this->rot.x = value;
		break;

	case 'y':
		this->rot.y = value;
		break;

	case 'z':
		this->rot.z = value;
		break;

	default:
		this->rot.x = value;
		break;
	}
}


XMFLOAT3 PARTICLE::getScale(void)
{
	return this->scale;
}
void PARTICLE::setScale(float x, float y, float z)
{
	this->scale.x = x;
	this->scale.y = y;
	this->scale.z = z;
}

XMFLOAT3 PARTICLE::getMove(void)
{
	return this->move;
}
void PARTICLE::setMove(float x, float y, float z)
{
	this->move.x = x;
	this->move.y = y;
	this->move.z = z;
}
void PARTICLE::setMove(XMFLOAT3 move)
{
	this->move = move;
}

void PARTICLE::setMoveXYZ(char xyz, float value)
{
	switch (xyz)
	{
	case 'x':
		this->move.x = value;
		break;

	case 'y':
		this->move.y = value;
		break;

	case 'z':
		this->move.z = value;
		break;

	default:
		this->move.x = value;
		break;
	}
}


MATERIAL PARTICLE::getMaterial(void)
{
	return this->material;
}
void PARTICLE::setMaterialDiff(float x, float y, float z, float w)
{
	this->material.Diffuse.x = x;
	this->material.Diffuse.y = y;
	this->material.Diffuse.z = z;
	this->material.Diffuse.w = w;

}

void PARTICLE::setMaterialDiff(XMFLOAT4 col)
{
	this->material.Diffuse = col;
}

void PARTICLE::setMaterialDiffXYZW(char xyzw, float value)
{
	switch (xyzw)
	{
	case 'x':
		this->material.Diffuse.x = value;
		break;

	case 'y':
		this->material.Diffuse.y = value;
		break;

	case 'z':
		this->material.Diffuse.z = value;
		break;

	case 'w':
		this->material.Diffuse.w = value;
		break;

	default:
		this->material.Diffuse.x = value;
		break;
	}


}

float PARTICLE::getfSizeX(void)
{
	return this->fSizeX;
}
void PARTICLE::setfSizeX(float x)
{
	this->fSizeX = x;
}

float PARTICLE::getfSizeY(void)
{
	return this->fSizeY;
}
void PARTICLE::setfSizeY(float y)
{
	this->fSizeX = y;
}

int PARTICLE::getnIdxShadow(void)
{
	return this->nIdxShadow;
}
void PARTICLE::setnIdxShadow(int idx)
{
	this->nIdxShadow = idx;
}

int PARTICLE::getnLife(void)
{
	return this->nLife;
}
void PARTICLE::setnLife(int life)
{
	this->nLife = life;
}

BOOL PARTICLE::getUse(void)
{
	return this->use;
}
void PARTICLE::setUse(int use)
{
	this->use = use;
}
