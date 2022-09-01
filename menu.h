#pragma once

#define MAX_MENU				(1)					 

struct MENU
{
	XMFLOAT3			pos;		 
	XMFLOAT3			rot;		 
	XMFLOAT3			scl;		 

	XMFLOAT4X4			mtxWorld;	 

	BOOL				load;
	DX11_MODEL			model;		 

	float				spd;		 
	float				dir;		 
	BOOL				use;

	BOOL				isDisplay;	 

	INTERPOLATION_DATA* tbl_adr;	 
	int					tbl_size;	 
	float				move_time;	 

	MENU* parent;					 
};

HRESULT InitMenu(void);
void UninitMenu(void);
void UpdateMenu(void);
void DrawMenu(void);

MENU* GetMenu(void);
