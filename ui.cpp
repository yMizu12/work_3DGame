//=============================================================================
//
// UIの処理 [ui.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "result.h"
#include "score.h"
#include "sprite.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_WIDTH_VILL_HP		(32)			// 村HPサイズ
#define TEXTURE_HEIGHT_VILL_HP		(64)			// 
#define TEXTURE_MAX					(6)				// テクスチャの数
#define VILL_HP_MAX					(70)			// 村HPの最大値
#define VILL_HP_MIN					(0)				// 村HPの最小値
#define VILL_HP_DIGIT				(2)				// 村HPの表示桁数
#define TIME_LIMIT					(60)			// 制限時間
#define TIME_DIGIT					(2)				// 制限時間の表示桁数

//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static BOOL						use;								// TRUE:使っている  FALSE:未使用
static float					weight, height;						// 幅と高さ
static XMFLOAT3					pos;								// ポリゴンの座標
static int						texNo;								// テクスチャ番号
static int						score;								// スコア
static int						villHp = 0;
static int timecnt;
static int timestart;
static BOOL						load = FALSE;


static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/test_line.png",
	"data/TEXTURE/suuji16x32_05.png",
	"data/TEXTURE/suuji16x32_03.png",
	"data/TEXTURE/clock.png",
	"data/TEXTURE/icon_village.png",
	"data/TEXTURE/text_last_02.png",
};



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
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


	// UIの初期化
	use   = TRUE;
	weight     = TEXTURE_WIDTH_VILL_HP;
	height     = TEXTURE_HEIGHT_VILL_HP;
	pos   = { 735.0f, 20.0f, 0.0f };	// 村HPの表示基準位置
	texNo = 0;

	score = 0;	// スコアの初期化

	villHp = VILL_HP_MAX;			// 村HPの初期化

	timecnt = TIME_LIMIT;			// 制限時間カウンタの初期化
	timestart = (int)time(NULL);	// 現在時間を保存

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	// 制限時間更新処理
	timecnt -= (int)time(NULL) - timestart;		// 制限時間カウンタから、前回時刻と現在時刻の差分を引く
	timestart = (int)time(NULL);				// 現在時刻を保存する

	int score = GetScore();

	if (timecnt < 0)
	{
		timecnt = 0;
		
		SetEnding(ENDING_GOOD);
		SetFade(FADE_OUT, MODE_RESULT);

	}

	if (villHp <= 0)
	{
		SetEnding(ENDING_DEAD);
		SetFade(FADE_OUT, MODE_RESULT);
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
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

	// 村アイコン表示処理
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[4]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, 550.0f, 50.0f, 65.0f, 65.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 文字「Last」表示処理
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, 630.0f, 50.0f, 100.0f, 100.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}



	// 村HP表示処理
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[1]);
	
		// 桁数分処理する
		int number = villHp;
		for (int i = 0; i < VILL_HP_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);
	
			// スコアの位置やテクスチャー座標を反映
			float px = pos.x - weight * i;	// スコアの表示位置X
			float py = pos.y + 30.0f;		// スコアの表示位置Y
			float pw = 32.0f;				// スコアの表示幅
			float ph = 64.0f;				// スコアの表示高さ
	
			float tw = 1.0f / 13;			// テクスチャの幅
			float th = 1.0f / 1;			// テクスチャの高さ
			float tx = x * tw;				// テクスチャの左上X座標
			float ty = 0.0f;				// テクスチャの左上Y座標
	
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
	
			// 次の桁へ
			number /= 10;
		}

	}


	// 制限時間表示処理
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[2]);

		int number = timecnt;

		for (int i = 0; i < TIME_DIGIT; i++)
		{
			float x = (float)(number % 10);

			float px = 900.0f - i * 40;	// スコアの表示位置X
			float py = 50.0f;			// スコアの表示位置Y
			float pw = 32;				// スコアの表示幅
			float ph = 64;				// スコアの表示高さ

			float tw = 1.0f / 13;		// テクスチャの幅
			float th = 1.0f / 1;		// テクスチャの高さ
			float tx = x * tw;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			number /= 10;
		}
	}

	// 時計アイコン表示処理
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[3]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, 800.0f, 50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	
	}


}


int GetVillHp(void)
{
	return villHp;
}

void AddVillHp(int add)
{
	villHp += add;
	if (villHp > VILL_HP_MAX)
	{
		villHp = VILL_HP_MAX;
	}
	else if (villHp < VILL_HP_MIN)
	{
		villHp = VILL_HP_MIN;
	}

}