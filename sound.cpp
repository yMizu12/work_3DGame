//=============================================================================
//
// サウンド処理 [sound.cpp]
// Author : HAL東京_水野裕介
// 
//=============================================================================
#include "sound.h"


//
IXAudio2 *pXAudio2 = NULL;									// XAudio2オブジェクトへのインターフェイス
IXAudio2MasteringVoice *pMasteringVoice = NULL;				// マスターボイス
IXAudio2SourceVoice *apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
BYTE *apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
DWORD aSizeAudio[SOUND_LABEL_MAX] = {};						// オーディオデータサイズ

// 各音素材のパラメータ
SOUNDPARAM aParam[SOUND_LABEL_MAX] =
{
	{ (char*)"data/BGM/sample000.wav", -1 },					// BGM0
	{ (char*)"data/BGM/bgm_maoudamashii_neorock73.wav", -1 },	// BGM1
	{ (char*)"data/BGM/sample001.wav", -1 },					// BGM2
	{ (char*)"data/BGM/screen_title.wav", -1 },					// タイトルBGM
	{ (char*)"data/BGM/screen_story.wav", -1 },					// あらすじBGM
	{ (char*)"data/BGM/screen_tutorial.wav", -1 },				// チュートリアルBGM
	{ (char*)"data/BGM/screen_game.wav", -1 },					// ゲームBGM
	{ (char*)"data/BGM/screen_result.wav", -1 },				// リザルトBGM
	{ (char*)"data/SE/bomb000.wav", 0 },						// 弾発射音
	{ (char*)"data/SE/defend000.wav", 0 },						// 弾発射音
	{ (char*)"data/SE/defend001.wav", 0 },						// 弾発射音
	{ (char*)"data/SE/hit000.wav", 0 },							// 弾発射音
	{ (char*)"data/SE/laser000.wav", 0 },						// 弾発射音
	{ (char*)"data/SE/lockon000.wav", 0 },						// 弾発射音
	{ (char*)"data/SE/shot000.wav", 0 },						// 弾発射音
	{ (char*)"data/SE/shot001.wav", 0 },						// ヒット音
	{ (char*)"data/SE/maou_se_system21.wav", 0 },				// SE_カーソル移動音
	{ (char*)"data/SE/maou_se_system49.wav", 0 },				// SE_決定音
	{ (char*)"data/SE/maou_se_system08.wav", 0 },				// SE_キャンセル音
	{ (char*)"data/SE/gunshot2.wav", 0 },						// SE_銃撃音
	{ (char*)"data/SE/enemyhit.wav", 0 },						// SE_エネミーヒット音
	{ (char*)"data/SE/enemy_attack2.wav", 0 },					// SE_エネミー到達音

};

//=============================================================================
// 初期化処理
//=============================================================================
BOOL InitSound(HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&pXAudio2, 0);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}
	
	// マスターボイスの生成
	hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if(pXAudio2)
		{
			// XAudio2オブジェクトの開放
			pXAudio2->Release();
			pXAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}

	// サウンドデータの初期化
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(aParam[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		if(dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &aSizeAudio[nCntSound], &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
		apDataAudio[nCntSound] = (BYTE*)malloc(aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, apDataAudio[nCntSound], aSizeAudio[nCntSound], dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
			return FALSE;
		}
	
		// ソースボイスの生成
		hr = pXAudio2->CreateSourceVoice(&apSourceVoice[nCntSound], &(wfx.Format));
		if(FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
			return FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = aSizeAudio[nCntSound];
		buffer.pAudioData = apDataAudio[nCntSound];
		buffer.Flags      = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount  = aParam[nCntSound].nCntLoop;

		// オーディオバッファの登録
		apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}

	return TRUE;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSound(void)
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(apSourceVoice[nCntSound])
		{
			// 一時停止
			apSourceVoice[nCntSound]->Stop(0);
	
			// ソースボイスの破棄
			apSourceVoice[nCntSound]->DestroyVoice();
			apSourceVoice[nCntSound] = NULL;
	
			// オーディオデータの開放
			free(apDataAudio[nCntSound]);
			apDataAudio[nCntSound] = NULL;
		}
	}
	
	// マスターボイスの破棄
	pMasteringVoice->DestroyVoice();
	pMasteringVoice = NULL;
	
	if(pXAudio2)
	{
		// XAudio2オブジェクトの開放
		pXAudio2->Release();
		pXAudio2 = NULL;
	}
	
	// COMライブラリの終了処理
	CoUninitialize();
}

//=============================================================================
// セグメント再生(再生中なら停止)
//=============================================================================
void PlaySound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = aSizeAudio[label];
	buffer.pAudioData = apDataAudio[label];
	buffer.Flags      = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount  = aParam[label].nCntLoop;

	// 状態取得
	apSourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	apSourceVoice[label]->Start(0);

}

//=============================================================================
// セグメント停止(ラベル指定)
//=============================================================================
void StopSound(int label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	apSourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		apSourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// セグメント停止(全て)
//=============================================================================
void StopSound(void)
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if(apSourceVoice[nCntSound])
		{
			// 一時停止
			apSourceVoice[nCntSound]->Stop(0);
		}
	}
}

//=============================================================================
// チャンクのチェック
//=============================================================================
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;
	
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	while(hr == S_OK)
	{
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch(dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if(dwChunkType == format)
		{
			*pChunkSize         = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}
	
	return S_OK;
}

//=============================================================================
// チャンクデータの読み込み
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	return S_OK;
}

