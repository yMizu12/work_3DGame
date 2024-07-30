//=============================================================================
//
// あらすじ画面処理 [story.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "fade.h"
#include "story.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(960)		// ゲーム本編の背景サイズ
#define TEXTURE_HEIGHT				(640)		//
#define TEXTURE_MAX					(11)		// テクスチャの数
#define TEXTURE_WIDTH_LOGO			(480)		// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)		// 
#define SRORY_DESC_POS_X			(480.0f)	// DESK：Description
#define SRORY_DESC_POS_Y			(270.0f)	//
#define SRORY_DESC_WIDTH			(900.0f)	//
#define SRORY_DESC_HEIGHT			(500.0f)	//
#define SRORY_DESC_COLOR_A			(0.9f)		// 説明書きのα値
#define SRORY_GHOST_POS_X			(640.0f)	// ゴースト
#define SRORY_GHOST_POS_Y			(220.0f)	//
#define SRORY_GHOST_WIDTH			(120.0f)	//
#define SRORY_GHOST_HEIGHT			(120.0f)	//
#define SRORY_SPARK_POS_X			(750.0f)	// 火花
#define SRORY_SPARK_POS_Y			(220.0f)	//
#define SRORY_SPARK_WIDTH			(70.0f)		//
#define SRORY_SPARK_HEIGHT			(70.0f)		//
#define SRORY_SAVIOR_POS_X			(840.0f)	// 守り手
#define SRORY_SAVIOR_POS_Y			(200.0f)	//
#define SRORY_SAVIOR_WIDTH			(180.0f)	//
#define SRORY_SAVIOR_HEIGHT			(180.0f)	//

//
static ID3D11Buffer				*vertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static BOOL						load = FALSE;						// 初期化を行ったかのフラグ
static STORY					story;

static char *textureName[TEXTURE_MAX] = {
	"data/TEXTURE/story/pipo-battlebg015.jpg",
	"data/TEXTURE/story/pipo-battlebg015.jpg",
	"data/TEXTURE/story/pipo-battlebg015.jpg",
	"data/TEXTURE/story/pipo-battlebg012.jpg",
	"data/TEXTURE/story/tut002.png",
	"data/TEXTURE/story/logo_title02.png",
	"data/TEXTURE/story/pipo-battlebg012.jpg",
	"data/TEXTURE/story/ghost-japanese.png",
	"data/TEXTURE/story/shock.png",
	"data/TEXTURE/story/hunter.png",
	"data/TEXTURE/story/boy03_cat01_1.png",

};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStory(void)
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
	story.w = TEXTURE_WIDTH;
	story.h = TEXTURE_HEIGHT;
	story.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	story.texNo = 0;

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_STORY);

	load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStory(void)
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
void UpdateStory(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		PlaySound(SOUND_LABEL_SE_DECISION);
		SetFade(FADE_OUT, MODE_TITLE);
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStory(void)
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


	// 背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[story.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(vertexBuffer,
			story.pos.x, story.pos.y, story.w, story.h,
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
			SRORY_DESC_POS_X, SRORY_DESC_POS_Y, SRORY_DESC_WIDTH, SRORY_DESC_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, SRORY_DESC_COLOR_A));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	

	// ゴースト
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[7]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer,
			SRORY_GHOST_POS_X, SRORY_GHOST_POS_Y, SRORY_GHOST_WIDTH, SRORY_GHOST_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// バチバチ
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[8]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer,
			SRORY_SPARK_POS_X, SRORY_SPARK_POS_Y, SRORY_SPARK_WIDTH, SRORY_SPARK_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// 守り手
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &texture[9]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(vertexBuffer,
			SRORY_SAVIOR_POS_X, SRORY_SAVIOR_POS_Y, SRORY_SAVIOR_WIDTH, SRORY_SAVIOR_HEIGHT,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}




