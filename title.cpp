//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)		// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)		// 
#define TEXTURE_MAX					(6)					// テクスチャの数
#define TEXTURE_WIDTH_LOGO			(960)				// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(160)				// 
#define TEXTURE_POS_X_LOGO			(TEXTURE_WIDTH/2)	//
#define TEXTURE_POS_Y_LOGO			(TEXTURE_HEIGHT/2 - 100.0f)//
#define TEXTURE_WIDTH_STORY			(250)				// あらすじサイズ
#define TEXTURE_HEIGHT_STORY		(60)				// 
#define TEXTURE_POS_X_STORY			(480)				// 
#define TEXTURE_POS_Y_STORY			(310)				// 
#define TEXTURE_WIDTH_TUTORIAL		(350)				// 操作説明サイズ
#define TEXTURE_HEIGHT_TUTORIAL		(60)				// 
#define TEXTURE_POS_X_TUTORIAL		(480)				// 
#define TEXTURE_POS_Y_TUTORIAL		(370)				// 
#define TEXTURE_WIDTH_START			(500)				// ゲーム開始サイズ
#define TEXTURE_HEIGHT_START		(60)				// 
#define TEXTURE_POS_X_START			(480)				// 
#define TEXTURE_POS_Y_START			(430)				// 

//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static BOOL						use;								// TRUE:使っている  FALSE:未使用
static float					width, height;						// 幅と高さ
static XMFLOAT3					pos;								// ポリゴンの座標
static int						texNo;								// テクスチャ番号
static int						curSelect;							// カーソル
float							alpha;
BOOL							flag_alpha;
static BOOL						load = FALSE;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/sky_night.jpg",
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/title_story.png",
	"data/TEXTURE/title_tutorial.png",
	"data/TEXTURE/title_start.png",
	"data/TEXTURE/gauge.png",

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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


	// 変数の初期化
	use		= TRUE;
	width	= TEXTURE_WIDTH;
	height	= TEXTURE_HEIGHT;
	pos		= XMFLOAT3(width/2, height/2, 0.0f);
	texNo	= 0;

	alpha = 1.0f;
	flag_alpha = TRUE;

	curSelect = SELECT_START;	// カーソルの初期位置設定

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_TITLE);

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		PlaySound(SOUND_LABEL_SE_DECISION);

		switch (curSelect)
		{
			case SELECT_STORY:
				SetFade(FADE_OUT, MODE_STORY);
				break;

			case SELECT_TUTORIAL:
				SetFade(FADE_OUT, MODE_TUTORIAL);
				break;

			case SELECT_START:
				SetFade(FADE_OUT, MODE_GAME);
				break;
		}
	
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		switch (curSelect)
		{
			case SELECT_STORY:
				SetFade(FADE_OUT, MODE_STORY);
				break;

			case SELECT_TUTORIAL:
				SetFade(FADE_OUT, MODE_TUTORIAL);
				break;

			case SELECT_START:
				SetFade(FADE_OUT, MODE_GAME);
				break;

		}
		
	}


	if (flag_alpha == TRUE)
	{
		alpha -= 0.02f;
		if (alpha <= 0.0f)
		{
			alpha = 0.0f;
			flag_alpha = FALSE;
		}
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			flag_alpha = TRUE;
		}
	}


	// 上キーで選択肢切替え
	if (GetKeyboardTrigger(DIK_UP))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + SELECT_MAX - 1) % SELECT_MAX;
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_UP) || IsButtonTriggered(0, STICK_UP_L))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + SELECT_MAX - 1) % SELECT_MAX;
	}


	// 下キーで選択肢切替え
	if (GetKeyboardTrigger(DIK_DOWN))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + 1) % SELECT_MAX;
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_DOWN) || IsButtonTriggered(0, STICK_DOWN_L))
	{
		PlaySound(SOUND_LABEL_SE_CURSOR);
		curSelect = (curSelect + 1) % SELECT_MAX;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(vertexBuffer, pos.x, pos.y, width, height, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// タイトルのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_LOGO, TEXTURE_POS_Y_LOGO, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// ハイライトを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		float r = 0.7f;
		float g = 0.7f;
		float b = 0.0f;
		float a = 0.5f;

		switch (curSelect)
		{
		case SELECT_STORY:
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer, TEXTURE_POS_X_STORY, TEXTURE_POS_Y_STORY, 
				TEXTURE_WIDTH_STORY, TEXTURE_HEIGHT_STORY, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(r, g, b, a));
			break;

		case SELECT_TUTORIAL:
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer, TEXTURE_POS_X_TUTORIAL, TEXTURE_POS_Y_TUTORIAL, 
				TEXTURE_WIDTH_TUTORIAL, TEXTURE_HEIGHT_TUTORIAL, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(r, g, b, a));
			break;

		case SELECT_START:
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(vertexBuffer, TEXTURE_POS_X_START, TEXTURE_POS_Y_START, 
				TEXTURE_WIDTH_START, TEXTURE_HEIGHT_START, 0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(r, g, b, a));
			break;
		}

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 色変換用
	float r;
	float g;
	float b;

	// 誘導メッセージを描画
	// STORY
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[2]);

		if (curSelect == SELECT_STORY)
		{
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
		}
		else
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_STORY, TEXTURE_POS_Y_STORY, 
			TEXTURE_WIDTH_STORY, TEXTURE_HEIGHT_STORY, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(r, g, b, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// TUTORIAL
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[3]);

		if (curSelect == SELECT_TUTORIAL)
		{
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
		}
		else
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_TUTORIAL, TEXTURE_POS_Y_TUTORIAL, 
			TEXTURE_WIDTH_TUTORIAL, TEXTURE_HEIGHT_TUTORIAL, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(r, g, b, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// GAME START
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[4]);

		if (curSelect == SELECT_START)
		{
			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
		}
		else
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer, TEXTURE_POS_X_START, TEXTURE_POS_Y_START, 
			TEXTURE_WIDTH_START, TEXTURE_HEIGHT_START, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(r, g, b, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}

