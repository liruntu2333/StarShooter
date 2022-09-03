#pragma once
#include "model.h"

#define MAX_PLAYER		(1)					 

#define	PLAYER_SIZE		(5.0f)				 

#define PLAYER_HP_MAX	(5)

#define PLAYER_MP_MAX	(5)

struct ENEMY;
struct PLAYER
{
	XMFLOAT3			pos;		 
	XMFLOAT3			rot;		 
	XMFLOAT3			scl;		 

	XMFLOAT4X4			mtxWorld;	 

	BOOL				load;
	DX11_MODEL			model;		 

	float				spd;		 
	float				dir;		 
	float				size;		 
	BOOL				use;

	INTERPOLATION_DATA* tbl_adr;	 
	int					tbl_size;	 
	float				move_time;	 

	PLAYER* parent;	 

	XMFLOAT4			quaternion;	 
	XMFLOAT3			upVector;	 

	int					HP;
	int					MP;
};

HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void DrawPlayerToDepthTex(void);

PLAYER* GetPlayer(void);

/**
 * \brief Every object moving correspondingly to player should check this flag to decide
 * whether wrap their position in field or not.
 *
 * \return An int type flag, last four bits used for representing a player's position's state
 * to field's boarder.
 * e.g. 0b0000 represents isn't out of any boarder.
 *      0b0001 represents out of Z plus boarder.
 */
int IsPlayerOutOfBoarder();
float GetPlayerFieldProgress();

ENEMY* GetPlayerLockedTarget();
bool IsPlayerRampage();
