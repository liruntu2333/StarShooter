#pragma once

#include <windows.h>
#include "xaudio2.h"						 

enum
{
	SOUND_LABEL_BGM_sample000,	 
	SOUND_LABEL_BGM_sample001,	 
	SOUND_LABEL_BGM_sample002,	 
	SOUND_LABEL_SE_lockon000,	
	SOUND_LABEL_SE_shot000,		
	SOUND_LABEL_MAX,
};

BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
