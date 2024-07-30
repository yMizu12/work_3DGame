//=============================================================================
//
// メッシュフィールドの処理 [meshfield.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshfield.h"
#include "renderer.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(1)		// テクスチャの数

//
static ID3D11Buffer					*vertexBuffer = NULL;				// 頂点バッファ
static ID3D11Buffer					*indexBuffer = NULL;				// インデックスバッファ

static ID3D11ShaderResourceView		*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							texNo;								// テクスチャ番号

static XMFLOAT3		posField;				// ポリゴン表示位置の中心座標
static XMFLOAT3		rotField;				// ポリゴンの回転角

static int			nNumBlockXField;		// ブロック数X
static int			nNumBlockZField;		// ブロック数Z
static int			nNumVertexField;		// 総頂点数	
static int			nNumVertexIndexField;	// 総インデックス数
static int			nNumPolygonField;		// 総ポリゴン数
static float		fBlockSizeXField;		// ブロックサイズX
static float		fBlockSizeZField;		// ブロックサイズZ

static char* textureName[TEXTURE_MAX] = {
	//"data/TEXTURE/test_plane.png",
	"data/TEXTURE/glass.jpg",
};

// 波の処理
static VERTEX_3D	*Vertex = NULL;

// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
static XMFLOAT3		Center;						// 波の発生場所
static float		Time = 0.0f;				// 波の経過時間
static float		wave_frequency  = 2.0f;		// 波の周波数
static float		wave_correction = 0.02f;	// 波の距離補正
static float		wave_amplitude  = 20.0f;	// 波の振幅
static BOOL			Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// ポリゴン表示位置の中心座標を設定
	posField = pos;

	rotField = rot;

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

	// ブロック数の設定
	nNumBlockXField = nNumBlockX;
	nNumBlockZField = nNumBlockZ;

	// 頂点数の設定
	nNumVertexField = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	nNumVertexIndexField = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	nNumPolygonField = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	fBlockSizeXField = fBlockSizeX;
	fBlockSizeZField = fBlockSizeZ;

	// 頂点情報をメモリに作っておく
	Vertex = new VERTEX_3D[nNumVertexField];

	// 地形生成のための頂点情報作成処理
	for (int z = 0; z < (nNumBlockZField + 1); z++)
	{
		for (int x = 0; x < (nNumBlockXField + 1); x++)
		{
			Vertex[z * (nNumBlockXField + 1) + x].Position.x = -(nNumBlockXField / 2.0f) * fBlockSizeXField + x * fBlockSizeXField;
			Vertex[z * (nNumBlockXField + 1) + x].Position.y = 0.0f;
			Vertex[z * (nNumBlockXField + 1) + x].Position.z = (nNumBlockZField / 2.0f) * fBlockSizeZField - z * fBlockSizeZField;

			// 法線の設定
			Vertex[z * (nNumBlockXField + 1) + x].Normal = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			Vertex[z * (nNumBlockXField + 1) + x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			float texSizeX = 1.0f;
			float texSizeZ = 1.0f;
			Vertex[z * (nNumBlockXField + 1) + x].TexCoord.x = texSizeX * x;
			Vertex[z * (nNumBlockXField + 1) + x].TexCoord.y = texSizeZ * z;
		}

	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * nNumVertexField;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * nNumVertexIndexField;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &indexBuffer);


	{//頂点バッファの中身を埋める

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, Vertex, sizeof(VERTEX_3D)*nNumVertexField);

		GetDeviceContext()->Unmap(vertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める

		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxZ = 0; nCntVtxZ < nNumBlockZField; nCntVtxZ++)
		{
			if(nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockXField + 1);
				nCntIdx++;
			}

			for(int nCntVtxX = 0; nCntVtxX < (nNumBlockXField + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (nNumBlockXField + 1) + nCntVtxX;
				nCntIdx++;
			}

			if(nCntVtxZ < (nNumBlockZField - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxZ * (nNumBlockXField + 1) + nNumBlockXField;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(indexBuffer, 0);
	}

	Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	if (Load == FALSE) return;

	// インデックスバッファの解放
	if (indexBuffer) {
		indexBuffer->Release();
		indexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (vertexBuffer) {
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

	if (Vertex != NULL)
	{
		delete[] Vertex;
		Vertex = NULL;
	}

	Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);


	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(rotField.x, rotField.y, rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(posField.x, posField.y, posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);


	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(nNumVertexIndexField, 0, 0);
}



BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;

	if (start.x == 0.0f)
	{
		start.x += 0.5f;
		end.x += 0.5f;
	}
	if (start.z == 0.0f)
	{
		start.z -= 0.5f;
		end.z -= 0.5f;
	}

	// 少し上から、ズドーンと下へレイを飛ばす
	start.y += 100.0f;
	end.y -= 1000.0f;

	// 処理を高速化する為に全検索ではなくて、座標からポリゴンを割り出す
	float fz = (nNumBlockZField / 2.0f) * fBlockSizeZField;
	float fx = (nNumBlockXField / 2.0f) * fBlockSizeXField;
	int sz = (int)((-start.z+fz) / fBlockSizeZField);
	int sx = (int)(( start.x+fx) / fBlockSizeXField);
	int ez = sz + 1;
	int ex = sx + 1;


	if ((sz < 0) || (sz > nNumBlockZField-1) ||
		(sx < 0) || (sx > nNumBlockXField-1))
	{
		*Normal = {0.0f, 1.0f, 0.0f};
		return FALSE;
	}


	// 必要数分検索を繰り返す
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = Vertex[z * (nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p1 = Vertex[z * (nNumBlockXField + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = Vertex[(z + 1) * (nNumBlockXField + 1) + x].Position;
			XMFLOAT3 p3 = Vertex[(z + 1) * (nNumBlockXField + 1) + (x + 1)].Position;

			// 三角ポリゴンだから２枚分の当たり判定
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}


	return FALSE;
}
