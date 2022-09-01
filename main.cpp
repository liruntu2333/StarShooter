#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "light.h"
#include "meshfield.h"
#include "meshwall.h"
#include "collision.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"

#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "result.h"
#include "fade.h"

#define CLASS_NAME		"AppClass"			 
#define WINDOW_NAME		"スターシューター"		 

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

long g_MouseX = 0;
long g_MouseY = 0;

#ifdef _DEBUG
int		g_CountFPS;							 
char	g_DebugStr[2048] = WINDOW_NAME;		 

#endif

int	g_Mode = MODE_TITLE;					 

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	 
	UNREFERENCED_PARAMETER(lpCmdLine);		 

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	const WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};
	HWND		hWnd;
	MSG			msg;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		 
		CW_USEDEFAULT,																		 
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									 
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	 
		NULL,
		NULL,
		hInstance,
		NULL);

	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	timeBeginPeriod(1);	 
	dwExecLastTime = dwFPSLastTime = timeGetTime();	 
	dwCurrentTime = dwFrameCount = 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (1)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{ 
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	 
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				 
				dwFrameCount = 0;							 
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	 
			{
				dwExecLastTime = dwCurrentTime;	 

#ifdef _DEBUG	 
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			 
				Draw();				 

#ifdef _DEBUG	 
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				 

	UnregisterClass(CLASS_NAME, wcex.hInstance);

	Uninit();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);

	InitLight();

	InitCamera();

	InitInput(hInstance, hWnd);

	InitSound(hWnd);

	SetLightEnable(TRUE);

	SetCullingMode(CULL_MODE_BACK);

	InitFade();

	SetMode(g_Mode);	 

	return S_OK;
}

void Uninit(void)
{
	SetMode(MODE_MAX);

	UninitSound();

	UninitCamera();

	UninitInput();

	UninitRenderer();
}

void Update(void)
{
	UpdateInput();

	UpdateLight();

	UpdateCamera();

	switch (g_Mode)
	{
	case MODE_TITLE:		 

		UpdateTitle();

		break;

	case MODE_TUTORIAL:

		UpdateTutorial();

		break;

	case MODE_GAME:			 

		UpdateGame();

		break;

	case MODE_RESULT:		 

		UpdateResult();

		break;
	}

	UpdateFade();
}

void Draw(void)
{
	Clear();

	SetCamera();

	switch (g_Mode)
	{
	case MODE_TITLE:
		SetDepthEnable(FALSE);

		SetLightEnable(FALSE);

		DrawTitle();

		SetLightEnable(TRUE);

		SetDepthEnable(TRUE);

		break;

	case MODE_TUTORIAL:
		SetDepthEnable(FALSE);

		SetLightEnable(FALSE);

		DrawTutorial();

		SetLightEnable(TRUE);

		SetDepthEnable(TRUE);

		break;

	case MODE_GAME:
		DrawGame();

		break;

	case MODE_RESULT:
		SetDepthEnable(FALSE);

		SetLightEnable(FALSE);

		DrawResult();

		SetLightEnable(TRUE);

		SetDepthEnable(TRUE);

		break;
	}

	DrawFade();

#ifdef _DEBUG
	DrawDebugProc();
#endif

	Present();
}

long GetMousePosX(void)
{
	return g_MouseX;
}

long GetMousePosY(void)
{
	return g_MouseY;
}

#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

void SetMode(int mode)
{
	StopSound();

	UninitTitle();

	UninitTutorial();

	UninitGame();

	UninitResult();

	g_Mode = mode;	 

	switch (g_Mode)
	{
	case MODE_TITLE:
		InitTitle();

		PlaySound(SOUND_LABEL_BGM_sample000);

		break;

	case MODE_TUTORIAL:
		InitTutorial();

		PlaySound(SOUND_LABEL_BGM_sample000);

		break;

	case MODE_GAME:
		InitGame();

		PlaySound(SOUND_LABEL_BGM_sample001);

		break;

	case MODE_RESULT:
		InitResult();

		PlaySound(SOUND_LABEL_BGM_sample002);

		break;

	case MODE_MAX:

		StopSound();

		UninitEnemy();

		UninitPlayer();
		break;
	}
}

int GetMode(void)
{
	return g_Mode;
}