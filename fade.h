#pragma once

typedef enum
{
	FADE_NONE = 0,		 
	FADE_IN,			 
	FADE_OUT,			 
	FADE_MAX
} FADE;

HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, int modeNext);
FADE GetFade(void);
