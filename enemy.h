#pragma once
#include <vector>

#include "model.h"

#define MAX_ENEMY		(8)					 

#define	ENEMY_SIZE		(5.0f)				 

enum EnemyBehaviorType : int;

struct ENEMY
{
	XMFLOAT4X4			mtxWorld{};			 
	XMFLOAT3			pos{};				 
	XMFLOAT3			rot{};				 
	XMFLOAT3			scl{};				 
	XMFLOAT3			velocity{};

	BOOL				use{};
	BOOL				load{};
	DX11_MODEL			model{};				 
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL]{};	 

	float				spd{};				 

	float				size{};				 

	INTERPOLATION_DATA* tbl_adr{};			 
	int					tbl_size{};			 
	float				move_time{};			 

	ENEMY* parent{};				 

	std::vector<int>	codes{};
	int					compare_index{};

	EnemyBehaviorType			type;
};

HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);
void DrawEnemyToDepthTex();

ENEMY* GetEnemy(void);
