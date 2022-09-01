#pragma once

#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			  
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

using namespace DirectX;

#define DIRECTINPUT_VERSION 0x0800		 
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#define SCREEN_WIDTH	(1280)			 
#define SCREEN_HEIGHT	(720)			 
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	 
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	 

#define	MAP_W			(1300.0f)
#define	MAP_H			(1300.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		 
	XMFLOAT3	rot;		 
	XMFLOAT3	scl;		 
	float		frame;		      
};

enum
{
	MODE_TITLE = 0,			 
	MODE_TUTORIAL,			 
	MODE_GAME,				 
	MODE_RESULT,			 
	MODE_MAX
};

long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(int mode);
int GetMode(void);
