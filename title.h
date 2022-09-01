#pragma once

enum
{
	TEXTURE_TITLE_BG,
	TEXTURE_TITLE_MOON,
	TEXTURE_TITLE_BOX,
	TEXTURE_TITLE_TEXT,
	TEXTRUE_TITLE_STAR,
	TEXTURE_TITLE_MAX,
};

struct TITLE
{
	XMFLOAT3		pos;			 
	float			w, h;			 
	int				texNo;			 
};

struct MOON
{
	XMFLOAT3		pos;			 
	XMFLOAT3		rot;			 
	float			w, h;			 
	float			countAnim;		 
	int				patternAnim;	 
	int				texNo;			 
};

struct CONFIRM
{
	XMFLOAT3		pos;			 
	float			w, h;			 
	int				texNo;			 
	int				choose;			 
};

HRESULT InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);
