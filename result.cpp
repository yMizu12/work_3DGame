//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_WIDTH_RESULT_MSG	(600)			// リザルトメッセージサイズ
#define TEXTURE_HEIGHT_RESULT_MSG	(90)			// 
#define RESULT_MSG_X				(480)			// X座標
#define RESULT_MSG_Y				(100)			// Y座標
#define SCORE_HEIGHT				(32*2)			// スコアの表示高さ
#define SCORE_WIDTH					(16*2)			// スコアの表示幅
#define SCORE_POS_X					(600.0f)		// スコアの表示座標X
#define SCORE_POS_Y					(300.0f)		// スコアの表示座標Y
#define SCORE_TEXTURE_HEIGHT		(1.0f)			// スコアテクスチャの高さ
#define SCORE_TEXTURE_WIDTH			(1.0f / 13.0f)	// スコアテクスチャの幅
#define BACK_TITLE_POS_X			(300.0f)
#define BACK_TITLE_POS_Y			(470.0f)
#define BACK_TITLE_TEXTURE_HEIGHT	(100.0f)
#define BACK_TITLE_TEXTURE_WIDTH	(200.0f)
#define BACK_GAME_POS_X				(660.0f)
#define BACK_GAME_POS_Y				(470.0f)
#define BACK_GAME_TEXTURE_HEIGHT	(100.0f)
#define BACK_GAME_TEXTURE_WIDTH		(200.0f)
#define TEXTURE_MAX					(10)			// テクスチャの数


//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static BOOL						use;								// TRUE:使っている  FALSE:未使用
static float					weight, height;						// 幅と高さ
static int						texNo;								// テクスチャ番号
static int						ending;								// ゲームの結果によるエンディング
static BOOL						load = FALSE;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/bg001.jpg",
	"data/TEXTURE/result_logo.png",
	"data/TEXTURE/suuji16x32_02.png",
	"data/TEXTURE/yoake.jpg",
	"data/TEXTURE/resultB.png",
	"data/TEXTURE/fire.jpg",
	"data/TEXTURE/back_to_title.png",
	"data/TEXTURE/return_game.png",
	"data/TEXTURE/game_clear.png",
	"data/TEXTURE/game_over.png",

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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
	weight	= TEXTURE_WIDTH;
	height	= TEXTURE_HEIGHT;
	texNo	= 0;

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_RESULT);

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
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
	{// Enter押したら、ステージを切り替える
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_X))
	{
		PlaySound(SOUND_LABEL_SE_CANCEL);
		SetFade(FADE_OUT, MODE_TITLE);
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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

	// エンディングにより、描画を分岐
	{
		switch (ending)
		{
		case ENDING_GOOD:
		
		{
			//背景
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[3]);	// 背景テクスチャ

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLeftTop(vertexBuffer, 0.0f, 0.0f, weight, height, 0.0f, 0.0f, 1.0f, 1.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			//リザルトメッセージ
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[8]);	// ゲームクリアテクスチャ

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSprite(vertexBuffer, RESULT_MSG_X, RESULT_MSG_Y, TEXTURE_WIDTH_RESULT_MSG, TEXTURE_HEIGHT_RESULT_MSG, 0.0f, 0.0f, 1.0f, 1.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}



		}
		break;


		case ENDING_DEAD:
		
		{
			//背景
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[5]);	// 背景テクスチャ

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLeftTop(vertexBuffer, 0.0f, 0.0f, weight, height, 0.0f, 0.0f, 1.0f, 1.0f);	

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			//リザルトメッセージ
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &texture[9]);	// ゲームオーバーテクスチャ

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSprite(vertexBuffer, RESULT_MSG_X, RESULT_MSG_Y, TEXTURE_WIDTH_RESULT_MSG, TEXTURE_HEIGHT_RESULT_MSG, 0.0f, 0.0f, 1.0f, 1.0f);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

		break;

		}
	}



	// スコア表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[2]);

		// 桁数分処理する
		int number = GetScore();
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// スコアの位置やテクスチャー座標を反映
			float pw = SCORE_WIDTH;				// スコアの表示幅
			float ph = SCORE_HEIGHT;			// スコアの表示高さ
			float px = SCORE_POS_X - i*pw;		// スコアの表示位置X
			float py = SCORE_POS_Y;				// スコアの表示位置Y

			float tw = SCORE_TEXTURE_WIDTH;		// テクスチャの幅
			float th = SCORE_TEXTURE_HEIGHT;	// テクスチャの高さ
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

	// タイトルに戻る表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[6]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(vertexBuffer, BACK_TITLE_POS_X, BACK_TITLE_POS_Y, BACK_TITLE_TEXTURE_WIDTH, BACK_TITLE_TEXTURE_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}


	// ゲーム再挑戦表示
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(vertexBuffer, BACK_GAME_POS_X, BACK_GAME_POS_Y, BACK_GAME_TEXTURE_WIDTH, BACK_GAME_TEXTURE_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}


//=============================================================================
// エンディングをセットする
//=============================================================================
void SetEnding(int ending)
{
	ending = ending;
}
