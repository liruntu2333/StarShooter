#pragma once

#define	MAX_BULLET				(256)	 

#define	BULLET_ATTACK_SIZE		(5.0f)	 

#include <memory>

#include "MathHelper.h"

class BezierCurve;
struct ENEMY;

struct BULLET
{
	XMFLOAT3			pos{};			 
	XMFLOAT3			rot{};			 
	XMFLOAT3			scl{};			 

	XMFLOAT4X4			mtxWorld{};		 

	BOOL				load{};
	DX11_MODEL			model{};			 

	INTERPOLATION_DATA* tbl_adr{};		 
	int					tbl_size{};		 
	float				move_time{};		 

	BULLET* parent{};			 

	float				spd{};			 
	float				attackSize{};		 
	BOOL				use{};			 

	std::unique_ptr<BezierCurveQuadratic> curve = nullptr;
	float				flyingTime{};
	float				hitTime{};
	ENEMY* target{};
};

HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);
int SetBullet(const std::array<XMFLOAT3, 3>& controlPoints, float tHit, ENEMY* target);

BULLET* GetBullet(void);
