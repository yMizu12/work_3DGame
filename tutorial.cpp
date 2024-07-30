//=============================================================================
//
// チュートリアル画面処理 [tutorial.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "fade.h"
#include "sprite.h"
#include "tutorial.h"
#include "ui.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(960)		// ゲーム本編の背景サイズ
#define TEXTURE_HEIGHT				(640)

#define TEXTURE_TUT_WIDTH			(912)		// チュートリアルの背景サイズ
#define TEXTURE_TUT_HEIGHT			(608)

#define TEXTURE_MAX					(20)		// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)		// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)

#define TUTORIAL_MAX				(4)			// テクスチャの数

#define TEXTURE_NUM_WIDTH			(16)		// 数字のテクスチャサイズ（幅）
#define TEXTURE_NUM_HEIGHT			(32)		// 数字のテクスチャサイズ（高さ）

#define TEXTURE_SLASH_WIDTH			(48)		// ページスラッシュサイズ
#define TEXTURE_SLASH_HEIGHT		(48)

#define PAGE_C_X					(440)
#define PAGE_C_Y					(520)

#define PAGE_S_X					(480)
#define PAGE_S_Y					(520)

#define PAGE_M_X					(520)
#define PAGE_M_Y					(520)

#define TEXTURE_KEY_WIDTH			(40)		// キーサイズ１のキー
#define TEXTURE_KEY_HEIGHT			(40)			

#define TEXTURE_HEADER_PATTERN_DIVIDE_X	(1)		// テクスチャ内分割数（X)
#define TEXTURE_HEADER_PATTERN_DIVIDE_Y	(4)		// テクスチャ内分割数（Y)

#define HEADER_X					(200)
#define HEADER_Y					(50)

#define TEXTURE_HEADER_WIDTH			(TEXTURE_HEADER_HEIGHT * 4)			// ヘッダーサイズ（W:H = 4:1）
#define TEXTURE_HEADER_HEIGHT			(100)

#define TEXTURE_CONTROL_NAME_WIDTH		(TEXTURE_CONTROL_NAME_HEIGHT * 3)	// ヘッダーサイズ（W:H = 4:1）
#define TEXTURE_CONTROL_NAME_HEIGHT		(25)

#define TEXTURE_CONTROL_DETAIL_WIDTH	(TEXTURE_CONTROL_DETAIL_HEIGHT * 4)	// ヘッダーサイズ（W:H = 4:1）
#define TEXTURE_CONTROL_DETAIL_HEIGHT	(100)

#define TEXTURE_CONTROL_BUTTON_WIDTH	(100)
#define TEXTURE_CONTROL_BUTTON_HEIGHT	(100)

#define TEXTURE_CONTROL_KEY_WIDTH		(100)
#define TEXTURE_CONTROL_KEY_HEIGHT		(100)

#define TEXTURE_BACK_WIDTH				(150)
#define TEXTURE_BACK_HEIGHT				(75)


//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int						gameMode;							// 現在のゲームモードを記憶
static int						tutorialNo;							// チュートリアルの何枚目か
static BOOL						load = FALSE;						// 初期化を行ったかのフラグ
static TUTORIAL					tutorial;


static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/tutorial/pipo-battlebg012.jpg",
	"data/TEXTURE/tutorial/sky000.jpg",
	"data/TEXTURE/tutorial/sky001.jpg",
	"data/TEXTURE/tutorial/pipo-battlebg015.jpg",
	"data/TEXTURE/tutorial/tutrial_bg_0.png",
	"data/TEXTURE/tutorial/suuji16x32_07.png",
	"data/TEXTURE/tutorial/page_slash.png",
	"data/TEXTURE/tutorial/key_LEFT.png",
	"data/TEXTURE/tutorial/key_RIGHT.png",
	"data/TEXTURE/tutorial/key_ENTER.png",
	"data/TEXTURE/tutorial/key_SPACE.png",
	"data/TEXTURE/tutorial/tutorial_header2.png",
	"data/TEXTURE/tutorial/rule.png",
	"data/TEXTURE/tutorial/screen.png",
	"data/TEXTURE/tutorial/gamepad.png",
	"data/TEXTURE/tutorial/keyboard.png",
	"data/TEXTURE/tutorial/tutrial_finaly_02.png",
	"data/TEXTURE/tutorial/back_to_title.png",
	"data/TEXTURE/tutorial/start_game.png",
	"data/TEXTURE/tutorial/screen_detail.png",
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
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
	tutorial.w = TEXTURE_TUT_WIDTH;
	tutorial.h = TEXTURE_TUT_HEIGHT;
	tutorial.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	tutorial.texNo = 3;

	tutorialNo = 0;

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_TUTORIAL);


	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
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
void UpdateTutorial(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_GAME);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_GAME);
	}


	if (GetKeyboardTrigger(DIK_Q))
	{// [Q]押したら、ステージを切り替える
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_X))
	{
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}



	// 右キーでチュートリアル画面切替え
	if (GetKeyboardTrigger(DIK_RIGHT))
	{
		if (tutorialNo < TUTORIAL_MAX -1)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo++;
		}
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_RIGHT))
	{
		if (tutorialNo < TUTORIAL_MAX - 1)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo++;
		}
	}

	// 左キーでチュートリアル画面切替え
	if (GetKeyboardTrigger(DIK_LEFT))
	{
		if (tutorialNo != 0)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo--;
		}
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_LEFT))
	{
		if (tutorialNo != 0)
		{
			PlaySound(SOUND_LABEL_SE_CURSOR);
			tutorialNo--;
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
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


	// チュートリアルの背景を描画
	{
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &texture[tutorial.texNo]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(vertexBuffer,
		tutorial.pos.x, tutorial.pos.y, tutorial.w, tutorial.h,
		0.0f, 0.0f, 1.0f, 1.0f,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
	}


	// 説明画面
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[4]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer,
			SCREEN_CENTER_X, SCREEN_CENTER_Y, TEXTURE_TUT_WIDTH, TEXTURE_TUT_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}




	// ページごとに描画を分ける
	switch (tutorialNo)
	{
		case 0:
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[12]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y, 800, 400,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			break;


		case 1:
			// 画面構成の描画
		{

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[13]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y, 900.0f, 450.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

			break;
		case 2:

		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[14]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y+30.0f, 900.0f, 450.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

			break;
		case 3:

		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &texture[15]);

			// １枚のポリゴンの頂点とテクスチャ座標を設定

			SetSpriteColor(vertexBuffer,
				SCREEN_CENTER_X, SCREEN_CENTER_Y + 30.0f, 900.0f, 450.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}

			break;
	}

	// ヘッダー
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[11]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定

		float tw = 1.0f / TEXTURE_HEADER_PATTERN_DIVIDE_X;		// テクスチャの幅
		float th = 1.0f / TEXTURE_HEADER_PATTERN_DIVIDE_Y;		// テクスチャの高さ
		float tx = 0.0f;										// テクスチャの左上X座標
		float ty = (tutorialNo)*th;								// テクスチャの左上Y座標

		SetSpriteColor(vertexBuffer,
			HEADER_X, HEADER_Y, TEXTURE_HEADER_WIDTH, TEXTURE_HEADER_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// ページ
	{
		// 分子
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定

		float tw = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_X;		// テクスチャの幅
		float th = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_Y;		// テクスチャの高さ
		float tx = (tutorialNo + 1) * tw;					// テクスチャの左上X座標
		float ty = 0.0f;									// テクスチャの左上Y座標

		SetSpriteColor(vertexBuffer,
			PAGE_C_X, PAGE_C_Y, TEXTURE_NUM_WIDTH, TEXTURE_NUM_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// スラッシュ
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[6]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定

		SetSpriteColor(vertexBuffer,
			PAGE_S_X, PAGE_S_Y, TEXTURE_SLASH_WIDTH, TEXTURE_SLASH_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// 分母
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定

		float tw = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_X;		// テクスチャの幅
		float th = 1.0f / TEXTURE_NUM_PATTERN_DIVIDE_Y;		// テクスチャの高さ
		float tx = (TUTORIAL_MAX) * tw;						// テクスチャの左上X座標
		float ty = 0.0f;									// テクスチャの左上Y座標

		SetSpriteColor(vertexBuffer,
			PAGE_M_X, PAGE_M_Y, TEXTURE_NUM_WIDTH, TEXTURE_NUM_HEIGHT,
			tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// 左キー
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定

		float alpha;

		if (tutorialNo != 0)
		{
			alpha = 1.0f;
		}
		else
		{
			alpha = 0.5f;
		}

		SetSpriteColor(vertexBuffer,
			PAGE_C_X - 70.0f, PAGE_C_Y, TEXTURE_KEY_WIDTH, TEXTURE_KEY_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}


	{
		// 右キー
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[8]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定

		float alpha;

		if (tutorialNo != TUTORIAL_MAX - 1)
		{
			alpha = 1.0f;
		}
		else
		{
			alpha = 0.5f;
		}

		SetSpriteColor(vertexBuffer,
			PAGE_M_X + 70.0f, PAGE_C_Y, TEXTURE_KEY_WIDTH, TEXTURE_KEY_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	{
		// タイトルに戻る
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[17]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer,
			200.0f, 500.0f, TEXTURE_BACK_WIDTH, TEXTURE_BACK_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		// ゲーム開始
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[18]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer,
			760.0f, 500.0f, TEXTURE_BACK_WIDTH, TEXTURE_BACK_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


}





