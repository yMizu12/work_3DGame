//=============================================================================
//
// 影処理 [shadow.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数
#define	SHADOW_SIZE_X		(50.0f)		// 頂点サイズ
#define	SHADOW_SIZE_Z		(50.0f)		// 頂点サイズ
#define	MAX_SHADOW			(1024)		// 影最大数


//
static ID3D11Buffer					*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView		*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static SHADOW						aShadow[MAX_SHADOW];				// 影ワーク
static int							texNo;								// テクスチャ番号

static char* textureName[] = {
	"data/TEXTURE/shadow000.jpg",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitShadow(void)
{
	// 頂点バッファの作成
	MakeVertexShadow();

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

	// 影ワーク初期化
	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		ZeroMemory(&aShadow[nCntShadow].material, sizeof(aShadow[nCntShadow].material));
		aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		aShadow[nCntShadow].pos = XMFLOAT3(0.0f, 0.1f, 0.0f);
		aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		aShadow[nCntShadow].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		aShadow[nCntShadow].use = FALSE;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitShadow(void)
{
	// 頂点バッファの解放
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (texture[i])
		{
			texture[i]->Release();
			texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateShadow(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawShadow(void)
{
	// 減算合成
	SetBlendState(BLEND_MODE_SUBTRACT);

	// Z比較なし
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

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for(int i = 0; i < MAX_SHADOW; i++)
	{
		if(aShadow[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(aShadow[i].scl.x, aShadow[i].scl.y, aShadow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(aShadow[i].rot.x, aShadow[i].rot.y, aShadow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(aShadow[i].pos.x, aShadow[i].pos.y, aShadow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアルの設定
			SetMaterial(aShadow[i].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// 通常ブレンド
	SetBlendState(BLEND_MODE_ALPHABLEND);
	
	// Z比較あり
	SetDepthEnable(TRUE);

	// フォグを元に戻す
	SetFogEnable(GetFogEnable());
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexShadow()
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
		vertex[0].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[1].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[2].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);
		vertex[3].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		// 拡散光の設定
		vertex[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[1].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[2].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[3].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(vertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// 影の作成
//=============================================================================
int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ)
{
	int nIdxShadow = -1;

	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		// 未使用（FALSE）だったらそれを使う
		if(!aShadow[nCntShadow].use)
		{
			aShadow[nCntShadow].pos = pos;
			aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			aShadow[nCntShadow].scl = XMFLOAT3(fSizeX, 1.0f, fSizeZ);
			aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			aShadow[nCntShadow].use = TRUE;

			nIdxShadow = nCntShadow;	// Index番号
			break;
		}
	}

	return nIdxShadow;		// Index番号を返す
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorShadow(int nIdxShadow, XMFLOAT4 col)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		aShadow[nIdxShadow].material.Diffuse = col;
	}
}

//=============================================================================
// 影の破棄
//=============================================================================
void ReleaseShadow(int nIdxShadow)
{
	if(nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		aShadow[nIdxShadow].use = FALSE;
	}
}

//=============================================================================
// 位置の設定
//=============================================================================
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		aShadow[nIdxShadow].pos = pos;
	}
}

