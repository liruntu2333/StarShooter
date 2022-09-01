#pragma once

#include "model.h"

#define MAX_BUILDING		(25)					 

#define	ENEMY_SIZE			(5.0f)					 

enum BuildingType : int;

struct BUILDING
{
	XMFLOAT4X4			mtxWorld;			 
	XMFLOAT3			pos;				 
	XMFLOAT3			rot;				 
	XMFLOAT3			scl;				 

	XMFLOAT3			pos_center;

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				 
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	 

	INTERPOLATION_DATA* tbl_adr;			 
	int					tbl_size;			 
	float				move_time;			 

	BUILDING* parent;				 

	BuildingType		type;
};

HRESULT InitBuilding(void);
void UninitBuilding(void);
void UpdateBuilding(void);
void DrawBuilding(void);

BUILDING* GetBuilding(void);
