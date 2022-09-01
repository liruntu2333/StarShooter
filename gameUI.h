#pragma once

#include "main.h"
#include "renderer.h"

enum
{
	TEXTURE_HP_1,
	TEXTURE_HP_0,
	TEXTURE_MP_1,
	TEXTURE_MP_0,
	TEXTURE_BUTTON_LEFT,
	TEXTURE_BUTTON_RIGHT,
	TEXTURE_BUTTON_SHIFT,
	TEXTURE_MAX,
};

enum
{
	BOX_HP,
	BOX_MP,
	BOX_MAX,
};

struct GameUI_Box
{
	XMFLOAT3		pos;			 
	float			w, h;			 
	int				texNo;			 
};

struct GameUI_HP
{
	XMFLOAT3		pos;			 
	float			w, h;			 
	int				texNo;			 
};

struct GameUI_MP
{
	XMFLOAT3		pos;			 
	float			w, h;			 
	int				texNo;			 
};

HRESULT InitGameUI(void);
void UninitGameUI(void);
void UpdateGameUI(void);
void DrawGameUI(void);