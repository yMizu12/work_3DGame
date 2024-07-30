//=============================================================================
//
// 木の処理 [tree.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "tree.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)				// テクスチャの数
#define	TREE_WIDTH			(60.0f)			// 木のサイズ
#define	TREE_HEIGHT			(90.0f)			// 
#define	MAX_TREE			(256)			// 木最大数
#define TREE_SHADOW_SIZE	(0.5f)			// 木の影のサイズ
#define TREE_FRONT_ROW		(3)				// 正面の木の行数
#define TREE_FRONT_COL		(15)			// 正面の木の列数
#define TREE_LEFT_ROW		(6)				// 左側の木の行数
#define TREE_LEFT_COL		(3)				// 左側の木の列数
#define TREE_RIGHT_ROW		(6)				// 右側の木の行数
#define TREE_RIGHT_COL		(3)				// 右側の木の列数


//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点バッファ
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static TREE						tree[MAX_TREE];						// 木ワーク
static BOOL						alpaTest;							// アルファテストON/OFF
static int						texNo;								// テクスチャ番号
static int						texAnim;							// テクスチャアニメ用

static char *textureName[] =
{
	"data/TEXTURE/tree_99.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTree(void)
{
	MakeVertexTree();

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

	texNo = texAnim = 0;

	// 木ワークの初期化
	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		ZeroMemory(&tree[nCntTree].material, sizeof(tree[nCntTree].material));
		tree[nCntTree].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		tree[nCntTree].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		tree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		tree[nCntTree].fWidth = TREE_WIDTH;
		tree[nCntTree].fHeight = TREE_HEIGHT;
		tree[nCntTree].use = FALSE;
	}

	alpaTest = TRUE;
	//g_nAlpha = 0x0;

	// 正面の木のセット
	for (int z = 0; z < TREE_FRONT_ROW; z++)
	{
		for (int x = 0; x < TREE_FRONT_COL; x++)
		{
			SetTree(XMFLOAT3(-1400.0f + 200.0f * x, 0.0f, 800.0f - 200.0f * z), TREE_WIDTH, TREE_HEIGHT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	// 左手側の木のセット
	for (int z = 0; z < TREE_LEFT_ROW; z++)
	{
		for (int x = 0; x < TREE_LEFT_COL; x++)
		{
			SetTree(XMFLOAT3(-1400.0f + 200.0f * x, 0.0f, 200.0f - 200.0f * z), TREE_WIDTH, TREE_HEIGHT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	// 右手側の木のセット
	for (int z = 0; z < TREE_RIGHT_ROW; z++)
	{
		for (int x = 0; x < TREE_RIGHT_COL; x++)
		{
			SetTree(XMFLOAT3(1000.0f + 200.0f * x, 0.0f, 200.0f - 200.0f * z), TREE_WIDTH, TREE_HEIGHT, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTree(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(texture[nCntTex] != NULL)
		{// テクスチャの解放
			texture[nCntTex]->Release();
			texture[nCntTex] = NULL;
		}
	}

	if(vertexBuffer != NULL)
	{// 頂点バッファの解放
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTree(void)
{

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(tree[nCntTree].use)
		{
			// 影の位置設定
			SetPositionShadow(tree[nCntTree].nIdxShadow, XMFLOAT3(tree[nCntTree].pos.x, 0.1f, tree[nCntTree].pos.z));
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		alpaTest = alpaTest ? FALSE: TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTree(void)
{
	// αテスト設定
	if (alpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_TREE; i++)
	{
		if(tree[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// 関数使った版
			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;


			// 正方行列（直交行列）を転置行列させて逆行列を作ってる版(速い)
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
			mtxScl = XMMatrixScaling(tree[i].scl.x, tree[i].scl.y, tree[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(tree[i].pos.x, tree[i].pos.y, tree[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(tree[i].material);

			// テクスチャ設定
			int texNo = i % TEXTURE_MAX;
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexTree(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(vertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 木のパラメータをセット
//=============================================================================
int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxTree = -1;

	for(int nCntTree = 0; nCntTree < MAX_TREE; nCntTree++)
	{
		if(!tree[nCntTree].use)
		{
			tree[nCntTree].pos = pos;
			tree[nCntTree].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			tree[nCntTree].fWidth = fWidth;
			tree[nCntTree].fHeight = fHeight;
			tree[nCntTree].use = TRUE;

			// 影の設定
			tree[nCntTree].nIdxShadow = CreateShadow(tree[nCntTree].pos, TREE_SHADOW_SIZE, TREE_SHADOW_SIZE);

			nIdxTree = nCntTree;

			break;
		}
	}

	return nIdxTree;
}
