//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : HAL����_����T��
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"	// �T�E���h�����ŕK�v

//*****************************************************************************
// �p�����[�^�\���̒�`
//*****************************************************************************
struct SOUNDPARAM
{
	char* pFilename;	// �t�@�C����
	int nCntLoop;		// ���[�v�J�E���g
} ;


//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_sample000,	// BGM0
	SOUND_LABEL_BGM_sample001,	// BGM1
	SOUND_LABEL_BGM_sample002,	// BGM2
	SOUND_LABEL_BGM_TITLE,		// �^�C�g��BGM
	SOUND_LABEL_BGM_STORY,		// ���炷��BGM
	SOUND_LABEL_BGM_TUTORIAL,	// �`���[�g���A��BGM
	SOUND_LABEL_BGM_GAME,		// �Q�[��BGM
	SOUND_LABEL_BGM_RESULT,		// ���U���gBGM
	SOUND_LABEL_SE_bomb000,		// ������
	SOUND_LABEL_SE_defend000,	// 
	SOUND_LABEL_SE_defend001,	// 
	SOUND_LABEL_SE_hit000,		// 
	SOUND_LABEL_SE_laser000,	// 
	SOUND_LABEL_SE_lockon000,	// 
	SOUND_LABEL_SE_shot000,		// 
	SOUND_LABEL_SE_shot001,		// 
	SOUND_LABEL_SE_CURSOR,		// SE_�J�[�\���ړ���
	SOUND_LABEL_SE_DECISION,	// SE_���艹
	SOUND_LABEL_SE_CANCEL,		// SE_�L�����Z����
	SOUND_LABEL_SE_GUNSHOT,		// SE_�e��
	SOUND_LABEL_SE_ENEMYHIT,	// SE_�G�l�~�[�q�b�g��
	SOUND_LABEL_SE_ENEMYATTACK,	// SE_�G�l�~�[���B��
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
