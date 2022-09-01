#pragma once

#define MAX_WEAPON				(1)					 

struct WEAPON
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

	INTERPOLATION_DATA* tbl_adr;	 
	int					tbl_size;	 
	float				move_time;	 

	WEAPON* parent;					 
};

HRESULT InitWeapon(void);
void UninitWeapon(void);
void UpdateWeapon(void);
void DrawWeapon(void);

WEAPON* GetWeapon(void);
