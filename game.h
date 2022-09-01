#pragma once

enum
{
	GAMEMODE_START,
	GAMEMODE_ATTACK,
	GAMEMODE_SELECT,
	GAMEMODE_MAX,
};

enum FocusMode
{
	FOCUS_PLAYER,
	FOCUS_ENEMY,
};

HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);
int GetFocusMode();
void SetFocusMode(int mode);
