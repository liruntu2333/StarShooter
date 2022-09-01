#pragma once

#include "model.h"

#define MAX_ITEM		(20)					 
#define MAX_ITEM_GOOD	(10)
#define MAX_ITEM_WORSE	(MAX_ITEM - MAX_ITEM_GOOD)

#define	ITEM_SIZE		(5.0f)				 

enum ItemType : int
{
	hpHeal = 0,
	hpKill = 1,
};

struct ITEM
{
	XMFLOAT4X4			mtxWorld;			 
	XMFLOAT3			pos;				 
	XMFLOAT3			rot;				 
	XMFLOAT3			scl;				 

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				 
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	 

	float				spd;				 

	float				size;				 

	INTERPOLATION_DATA* tbl_adr;			 
	int					tbl_size;			 
	float				move_time;			 

	ITEM* parent;				 

	ItemType			type;
};

HRESULT InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);

ITEM* GetItem(void);
