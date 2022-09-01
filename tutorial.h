#pragma once

enum
{
	TEXTURE_TUTORIAL_1,
	TEXTURE_TUTORIAL_2,
	TEXTURE_TUTORIAL_3,
	TEXTURE_TUTORIAL_4,
	TEXTURE_TUTORIAL_TIPS_1,
	TEXTURE_TUTORIAL_TIPS_2,
	TEXTURE_TUTORIAL_MAX,
};

struct TUTORIAL
{
	XMFLOAT3	pos;		 
	float		width;		 
	float		height;		 
	int			texNo;		 
	BOOL		use;		 
	float		alpha;
};

HRESULT InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);
