//=============================================================================
//
// ���C������ [main.cpp]
// Author : HAL����_����T��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "sound.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "game.h"
#include "fade.h"
#include "title.h"
#include "story.h"
#include "result.h"
#include "tutorial.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"AppClass"								// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"Village Savior (HAL����_����T��)"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckHit(void);


//
long mouseX = 0;
long mouseY = 0;


#ifdef _DEBUG
int		countFPS;						// FPS�J�E���^
char	debugStr[2048] = WINDOW_NAME;	// �f�o�b�O�����\���p
#endif

int mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	// �E�B���h�E�N���X�����܂ލ\����
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),				// �\���̂̃T�C�Y
		CS_CLASSDC,						// �N���X�X�^�C��
		WndProc,						// �E�B���h�E�v���V�[�W���ւ̃|�C���^�[
		0,								// �E�B���h�E�N���X�\���̂̌�Ɋ��蓖�Ă�]���ȃo�C�g��
		0,								// �E�B���h�E�C���X�^���X�̌�Ɋ��蓖�Ă�]���ȃo�C�g��
		hInstance,						// �E�B���h�E�v���V�[�W�����܂ރC���X�^���X�ւ̃n���h��
		NULL,							// �A�C�R���ւ̃n���h���iNULL�̏ꍇ�A����̃A�C�R���j
		LoadCursor(NULL, IDC_ARROW),	// �J�[�\���ւ̃n���h��
		(HBRUSH)(COLOR_WINDOW + 1),		// �w�i�u���V�ւ̃n���h��
		NULL,							// ���j���[�̃��\�[�X�����w�肷�镶����ւ̃|�C���^�[�iNULL�̏ꍇ�A�E�B���h�E�Ɋ���̃��j���[�Ȃ��j
		CLASS_NAME,						// �E�B���h�E�̃N���X��
		NULL							// �A�C�R���ւ̃n���h��
	};

	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																			// �E�B���h�E�̍����W
		CW_USEDEFAULT,																			// �E�B���h�E�̏���W
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,										// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,
		NULL,
		hInstance,
		NULL);

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	BOOL mode = TRUE;

	int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// Yes�Ȃ�Window���[�h�ŋN��
		mode = TRUE;
		break;
	case IDNO:		// No�Ȃ�t���X�N���[�����[�h�ŋN��
		mode = FALSE;
		break;
	case IDCANCEL:	// CANCEL�Ȃ�I��
	default:
		return -1;
		break;
	}

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if (FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);								// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

	// �����_���̏���������
	srand((unsigned)time(NULL));

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				countFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;		// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(debugStr, WINDOW_NAME);
				wsprintf(&debugStr[strlen(debugStr)], " FPS:%d", countFPS);
#endif

				Update();		// �X�V����
				Draw();			// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&debugStr[strlen(debugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, debugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);		// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
// �� Windows���瑗���Ă������b�Z�[�W����������R�[���o�b�N�֐�
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:				// ESC�L�[������Window��j��
					DestroyWindow(hWnd);
					break;
			}
			break;

		case WM_MOUSEMOVE:
			mouseX = LOWORD(lParam);
			mouseY = HIWORD(lParam);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	SetViewPort(TYPE_FULL_SCREEN);

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�̏�����
	InitSound(hWnd);

	// ���C�g��L����
	SetLightEnable(TRUE);

	// �w�ʃ|���S�����J�����O
	SetCullingMode(CULL_MODE_BACK);

	// �t�F�[�h�̏�����
	InitFade();

	// �ŏ��̃��[�h���Z�b�g
	SetMode(mode);


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �I���̃��[�h���Z�b�g
	SetMode(MODE_MAX);

	// �t�F�[�h�̏I������
	UninitFade();

	// �T�E���h�I������
	UninitSound();

	// �J�����̏I������
	UninitCamera();

	//���͂̏I������
	UninitInput();

	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// ���C�g�̍X�V����
	UpdateLight();

	// �J�����X�V
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (mode)
	{
		case MODE_TITLE:		// �^�C�g����ʂ̍X�V
			UpdateTitle();
			break;

		case MODE_STORY:		// ���炷����ʂ̍X�V
			UpdateStory();
			break;

		case MODE_TUTORIAL:		// �`���[�g���A����ʂ̍X�V
			UpdateTutorial();
			break;

		case MODE_GAME:			// �Q�[����ʂ̍X�V
			UpdateGame();
			break;

		case MODE_RESULT:		// ���U���g��ʂ̍X�V
			UpdateResult();
			break;
	}

	// �t�F�[�h�����̍X�V
	UpdateFade();


}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	SetCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (mode)
	{
		case MODE_TITLE:		// �^�C�g����ʂ̕`��
			SetViewPort(TYPE_FULL_SCREEN);

			// 2D�̕���`�悷�鏈��
			// Z��r�Ȃ�
			SetDepthEnable(FALSE);

			// ���C�e�B���O�𖳌�
			SetLightEnable(FALSE);

			DrawTitle();

			// ���C�e�B���O��L����
			SetLightEnable(TRUE);

			// Z��r����
			SetDepthEnable(TRUE);
			break;

		case MODE_STORY:		// ���炷����ʂ̕`��
			SetViewPort(TYPE_FULL_SCREEN);

			// 2D�̕���`�悷�鏈��
			// Z��r�Ȃ�
			SetDepthEnable(FALSE);

			// ���C�e�B���O�𖳌�
			SetLightEnable(FALSE);

			DrawStory();

			// ���C�e�B���O��L����
			SetLightEnable(TRUE);

			// Z��r����
			SetDepthEnable(TRUE);
			break;

		case MODE_TUTORIAL:		// �`���[�g���A����ʂ̕`��
			SetViewPort(TYPE_FULL_SCREEN);

			// 2D�̕���`�悷�鏈��
			// Z��r�Ȃ�
			SetDepthEnable(FALSE);

			// ���C�e�B���O�𖳌�
			SetLightEnable(FALSE);

			DrawTutorial();

			// ���C�e�B���O��L����
			SetLightEnable(TRUE);

			// Z��r����
			SetDepthEnable(TRUE);
			break;

		case MODE_GAME:			// �Q�[����ʂ̕`��
			DrawGame();
			break;

		case MODE_RESULT:		// ���U���g��ʂ̕`��
			SetViewPort(TYPE_FULL_SCREEN);

			// 2D�̕���`�悷�鏈��
			// Z��r�Ȃ�
			SetDepthEnable(FALSE);

			// ���C�e�B���O�𖳌�
			SetLightEnable(FALSE);

			DrawResult();

			// ���C�e�B���O��L����
			SetLightEnable(TRUE);

			// Z��r����
			SetDepthEnable(TRUE);
			break;
	}



	{	// �t�F�[�h����
		SetViewPort(TYPE_FULL_SCREEN);

		// 2D�̕���`�悷�鏈��
		// Z��r�Ȃ�
		SetDepthEnable(FALSE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		// �t�F�[�h�`��
		DrawFade();

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// Z��r����
		SetDepthEnable(TRUE);
	}


#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();
}


long GetMousePosX(void)
{
	return mouseX;
}


long GetMousePosY(void)
{
	return mouseY;
}


#ifdef _DEBUG
char* GetDebugStr(void)
{
	return debugStr;
}
#endif


//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int nextMode)
{
	// ���[�h��ς���O�ɑS�����������������

	// �^�C�g����ʂ̏I������
	UninitTitle();

	// ���炷����ʂ̏I������
	UninitStory();

	// �`���[�g���A����ʂ̏I������
	UninitTutorial();

	// �Q�[����ʂ̏I������
	UninitGame();

	// ���U���g��ʂ̏I������
	UninitResult();


	mode = nextMode;	// ���̃��[�h���Z�b�g���Ă���

	switch (mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		break;

	case MODE_STORY:
		// ���炷����ʂ̏�����
		InitStory();
		break;

	case MODE_TUTORIAL:
		// �`���[�g���A����ʂ̏�����
		InitTutorial();
		break;

	case MODE_GAME:
		// �J�����������ŏ��������Ă���
		UninitCamera();
		InitCamera();
		// �Q�[����ʂ̏�����
		InitGame();
		break;

	case MODE_RESULT:
		// ���U���g��ʂ̏�����
		InitResult();
		break;

		// �Q�[���I�����̏���
	case MODE_MAX:
		// �G�l�~�[�̏I������
		UninitEnemy();
		
		// �v���C���[�̏I������
		UninitPlayer();
		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return mode;
}
