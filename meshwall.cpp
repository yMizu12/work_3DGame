//=============================================================================
//
// メッシュウォールの処理 [meshwall.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "meshwall.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)						// テクスチャの数
#define	MAX_MESH_WALL		(10)					// 壁の総数
#define	VALUE_MOVE_WALL		(5.0f)					// 移動速度
#define	VALUE_ROTATE_WALL	(D3DX_PI * 0.001f)		// 回転速度


//
static ID3D11ShaderResourceView		*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							texNo;		// テクスチャ番号

static MESH_WALL aMeshWall[MAX_MESH_WALL];		// メッシュ壁ワーク
static int nNumMeshField = 0;					// メッシュ壁の数

static char* textureName[] = {
	//"data/TEXTURE/wall000.jpg",
	"data/TEXTURE/gauge.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshWall(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT4 col,
						int nNumBlockX, int nNumBlockY, float fBlockSizeX, float fBlockSizeZ)
{
	MESH_WALL *pMesh;

	if(nNumMeshField >= MAX_MESH_WALL)
	{
		return E_FAIL;
	}

	// テクスチャ生成
	if (nNumMeshField == 0)
	{	// TEXの読み込みは初めの１回だけにする
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
	}

	texNo = 0;

	pMesh = &aMeshWall[nNumMeshField];

	nNumMeshField++;

	// マテリアル情報の初期化
	ZeroMemory(&pMesh->material, sizeof(pMesh->material));
	pMesh->material.Diffuse = col;

	// ポリゴン表示位置の中心座標を設定
	pMesh->pos = pos;

	pMesh->rot = rot;

	// ブロック数の設定
	pMesh->nNumBlockX = nNumBlockX;
	pMesh->nNumBlockY = nNumBlockY;

	// 頂点数の設定
	pMesh->nNumVertex = (nNumBlockX + 1) * (nNumBlockY + 1);

	// インデックス数の設定
	pMesh->nNumVertexIndex = (nNumBlockX + 1) * 2 * nNumBlockY + (nNumBlockY - 1) * 2;

	// ポリゴン数の設定
	pMesh->nNumPolygon = nNumBlockX * nNumBlockY * 2 + (nNumBlockY - 1) * 4;

	// ブロックサイズの設定
	pMesh->fBlockSizeX = fBlockSizeX;
	pMesh->fBlockSizeY = fBlockSizeZ;

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * pMesh->nNumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &pMesh->vertexBuffer);

	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * pMesh->nNumVertexIndex;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &pMesh->indexBuffer);

	{//頂点バッファの中身を埋める
#if 0
		const float texSizeX = 1.0f / nNumBlockX;
		const float texSizeZ = 1.0f / nNumBlockY;
#else
		const float texSizeX = 1.0f;
		const float texSizeZ = 1.0f;
#endif

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(pMesh->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		for(int nCntVtxY = 0; nCntVtxY < (pMesh->nNumBlockY + 1); nCntVtxY++)
		{
			for(int nCntVtxX = 0; nCntVtxX < (pMesh->nNumBlockX + 1); nCntVtxX++)
			{
				// 頂点座標の設定
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Position.x = -(pMesh->nNumBlockX / 2.0f) * pMesh->fBlockSizeX + nCntVtxX * pMesh->fBlockSizeX;
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Position.y = pMesh->nNumBlockY * pMesh->fBlockSizeY - nCntVtxY * pMesh->fBlockSizeY;
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Position.z = 0.0f;

				// 法線の設定
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

				// 拡散光の設定
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].TexCoord.x = texSizeX * nCntVtxX;
				pVtx[nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX].TexCoord.y = texSizeZ * nCntVtxY;
			}
		}

		GetDeviceContext()->Unmap(pMesh->vertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める
		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(pMesh->indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short *pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for(int nCntVtxY = 0; nCntVtxY < pMesh->nNumBlockY; nCntVtxY++)
		{
			if(nCntVtxY > 0)
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = (nCntVtxY + 1) * (pMesh->nNumBlockX + 1);
				nCntIdx++;
			}

			for(int nCntVtxX = 0; nCntVtxX < (pMesh->nNumBlockX + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxY + 1) * (pMesh->nNumBlockX + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxY * (pMesh->nNumBlockX + 1) + nCntVtxX;
				nCntIdx++;
			}

			if(nCntVtxY < (pMesh->nNumBlockY - 1))
			{// 縮退ポリゴンのためのダブりの設定
				pIdx[nCntIdx] = nCntVtxY * (pMesh->nNumBlockX + 1) + pMesh->nNumBlockX;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(pMesh->indexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshWall(void)
{
	MESH_WALL *pMesh;
	int nCntMeshField;

	for(nCntMeshField = 0; nCntMeshField < nNumMeshField; nCntMeshField++)
	{
		pMesh = &aMeshWall[nCntMeshField];

		if(pMesh->vertexBuffer)
		{// 頂点バッファの解放
			pMesh->vertexBuffer->Release();
			pMesh->vertexBuffer = NULL;
		}

		if(pMesh->indexBuffer)
		{// インデックスバッファの解放
			pMesh->indexBuffer->Release();
			pMesh->indexBuffer = NULL;
		}
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

	//読み込み数をリセットする
	nNumMeshField = 0;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshWall(void)
{
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshWall(void)
{
	MESH_WALL *pMesh;
	int nCntMeshField;
	
	for(nCntMeshField = 0; nCntMeshField < nNumMeshField; nCntMeshField++)
	{
		pMesh = &aMeshWall[nCntMeshField];

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &pMesh->vertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(pMesh->indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// マテリアル設定
		SetMaterial(pMesh->material);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);



		XMMATRIX mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(pMesh->rot.x, pMesh->rot.y, pMesh->rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(pMesh->pos.x, pMesh->pos.y, pMesh->pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// ポリゴンの描画
		GetDeviceContext()->DrawIndexed(pMesh->nNumVertexIndex, 0, 0);
	}

}

