//=============================================================================
//
// ライト処理 [light.cpp]
// Author : HAL東京_水野裕介
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"


#define LIGHT_DIRECTIONAL_COLOR_R	(0.5f)
#define LIGHT_DIRECTIONAL_COLOR_G	(0.3f)
#define LIGHT_DIRECTIONAL_COLOR_B	(0.7f)
#define LIGHT_DIRECTIONAL_COLOR_A	(1.0f)

#define FOG_START					(200.0f)
#define FOG_END						(550.0f)
#define FOG_COLOR_R					(0.0f)
#define FOG_COLOR_G					(0.0f)
#define FOG_COLOR_B					(0.0f)
#define FOG_COLOR_A					(1.0f)



//
static LIGHT	light[LIGHT_MAX];
static FOG		fog;
static BOOL		fogEnable = FALSE;
static BOOL		lightChange = TRUE;


//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		light[i].Attenuation = 100.0f;		// 減衰距離
		light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	light[0].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );		// 光の向き
	light[0].Diffuse   = XMFLOAT4(LIGHT_DIRECTIONAL_COLOR_R, LIGHT_DIRECTIONAL_COLOR_G, 
									LIGHT_DIRECTIONAL_COLOR_B, LIGHT_DIRECTIONAL_COLOR_A);	// 光の色
	light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	light[0].Enable = TRUE;									// このライトをON
	SetLight(0, &light[0]);									// これで設定している
	

	// フォグの初期化（霧の効果）
	fog.FogStart = FOG_START;								// 視点からこの距離離れるとフォグがかかり始める
	fog.FogEnd   = FOG_END;									// ここまで離れるとフォグの色で見えなくなる
	fog.FogColor = XMFLOAT4(FOG_COLOR_R, FOG_COLOR_G, 
							FOG_COLOR_B, FOG_COLOR_A);		// フォグの色
	SetFog(&fog);
	SetFogEnable(fogEnable);		// 他の場所もチェックする shadow

}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{

	// ポイントライトのテスト
	{

#ifdef _DEBUG
		// ライトの色確認
		if (GetKeyboardTrigger(DIK_F2))
		{
			lightChange = lightChange ? FALSE : TRUE;
		}

		if (lightChange == TRUE)
		{
			light[0].Diffuse = XMFLOAT4(LIGHT_DIRECTIONAL_COLOR_R, LIGHT_DIRECTIONAL_COLOR_G, 
										LIGHT_DIRECTIONAL_COLOR_B, LIGHT_DIRECTIONAL_COLOR_A);
		}
		else
		{
			light[0].Diffuse = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
		}

#endif


		SetLightData(0, &light[0]);
	}

}


//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL GetFogEnable(void)
{
	return(fogEnable);
}


