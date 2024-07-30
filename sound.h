//=============================================================================
//
// サウンド処理 [sound.h]
// Author : HAL東京_水野裕介
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"	// サウンド処理で必要

//*****************************************************************************
// パラメータ構造体定義
//*****************************************************************************
struct SOUNDPARAM
{
	char* pFilename;	// ファイル名
	int nCntLoop;		// ループカウント
} ;


//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_sample000,	// BGM0
	SOUND_LABEL_BGM_sample001,	// BGM1
	SOUND_LABEL_BGM_sample002,	// BGM2
	SOUND_LABEL_BGM_TITLE,		// タイトルBGM
	SOUND_LABEL_BGM_STORY,		// あらすじBGM
	SOUND_LABEL_BGM_TUTORIAL,	// チュートリアルBGM
	SOUND_LABEL_BGM_GAME,		// ゲームBGM
	SOUND_LABEL_BGM_RESULT,		// リザルトBGM
	SOUND_LABEL_SE_bomb000,		// 爆発音
	SOUND_LABEL_SE_defend000,	// 
	SOUND_LABEL_SE_defend001,	// 
	SOUND_LABEL_SE_hit000,		// 
	SOUND_LABEL_SE_laser000,	// 
	SOUND_LABEL_SE_lockon000,	// 
	SOUND_LABEL_SE_shot000,		// 
	SOUND_LABEL_SE_shot001,		// 
	SOUND_LABEL_SE_CURSOR,		// SE_カーソル移動音
	SOUND_LABEL_SE_DECISION,	// SE_決定音
	SOUND_LABEL_SE_CANCEL,		// SE_キャンセル音
	SOUND_LABEL_SE_GUNSHOT,		// SE_銃撃
	SOUND_LABEL_SE_ENEMYHIT,	// SE_エネミーヒット音
	SOUND_LABEL_SE_ENEMYATTACK,	// SE_エネミー到達音
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
