//=============================================================================
//
// スコア処理 [score.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_WIDTH			(16)	// サイズ
#define SCORE_HEIGHT		(32)	// 
#define SCORE_POS_X			(500.0f)// X座標
#define SCORE_POS_Y			(20.0f)	// Y座標
#define SCORE_TEX_WIDTH		(1.0f / 13.0f)
#define SCORE_TEX_HEIGHT	(1.0f)
#define TEXTURE_MAX			(1)		// テクスチャの数


//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static BOOL						use;								// TRUE:使っている  FALSE:未使用
static float					weight, height;						// 幅と高さ
static int						texNo;								// テクスチャ番号
static int						score;								// スコア
static BOOL						load = FALSE;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/suuji16x32_02.png",
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &vertexBuffer);


	// 初期化
	use		= TRUE;
	weight	= SCORE_WIDTH;
	height	= SCORE_HEIGHT;
	texNo	= 0;

	score = 0;	// スコアの初期化

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	if (load == FALSE) return;

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (texture[i])
		{
			texture[i]->Release();
			texture[i] = NULL;
		}
	}

	load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

	// 桁数分処理する
	int number = score;
	for (int i = 0; i < SCORE_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = SCORE_POS_X - weight*i;	// スコアの表示位置X
		float py = SCORE_POS_Y;				// スコアの表示位置Y
		float pw = weight;					// スコアの表示幅
		float ph = height;					// スコアの表示高さ

		float tw = SCORE_TEX_WIDTH;			// テクスチャの幅
		float th = SCORE_TEX_HEIGHT;		// テクスチャの高さ
		float tx = x * tw;					// テクスチャの左上X座標
		float ty = 0.0f;					// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));	// 色はそのまま

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}


//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddScore(int add)
{
	score += add;
	if (score > SCORE_MAX)
	{
		score = SCORE_MAX;
	}
	else if (score < SCORE_MIN)
	{
		score = SCORE_MIN;
	}

}


int GetScore(void)
{
	return score;
}

