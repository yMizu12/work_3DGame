//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH	(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT	(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX		(1)				// テクスチャの数
#define	FADE_RATE		(0.02f)			// フェード係数


//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
};

static BOOL			use;				// TRUE:使っている  FALSE:未使用
static float		weight, height;		// 幅と高さ
static int			texNo;				// テクスチャ番号
FADE				fade = FADE_IN;		// フェードの状態
static int			modeNext;			// 次のモード
XMFLOAT4			color;				// フェードのカラー（α値）
static BOOL			load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
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

	use		= TRUE;
	weight	= TEXTURE_WIDTH;
	height	= TEXTURE_HEIGHT;
	texNo	= 0;

	fade  = FADE_IN;
	color = { 1.0, 1.0, 1.0, 1.0 };

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
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
void UpdateFade(void)
{

	if (fade != FADE_NONE)
	{// フェード処理中
		if (fade == FADE_OUT)
		{// フェードアウト処理
			color.w += FADE_RATE;		// α値を加算して画面を消していく
			if (color.w >= 1.0f)
			{
				// 鳴っている曲を全部止める
				StopSound();

				// フェードイン処理に切り替え
				color.w = 1.0f;
				SetFade(FADE_IN, modeNext);

				// モードを設定
				SetMode(modeNext);
			}

		}
		else if (fade == FADE_IN)
		{// フェードイン処理
			color.w -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
			if (color.w <= 0.0f)
			{
				// フェード処理終了
				color.w = 0.0f;
				SetFade(FADE_NONE, modeNext);
			}

		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	// PrintDebugProc("\n");

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (fade == FADE_NONE) return;	// フェードしないのなら描画しない

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


	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, SCREEN_WIDTH/2, TEXTURE_WIDTH/2, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f,
			color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


}


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(FADE f, int next)
{
	fade = f;
	modeNext = next;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
FADE GetFade(void)
{
	return fade;
}



