#pragma once

#include "renderer.h"

struct CAMERA
{
	XMFLOAT4X4			mtxView;		 
	XMFLOAT4X4			mtxInvView;		 
	XMFLOAT4X4			mtxProjection;	 

	XMFLOAT3			pos;			 
	XMFLOAT3			at;				 
	XMFLOAT3			up;				 
	float				theta;
	float				phi;

	float				lenPlayer;			 
	float				lenMenu;			 
};

enum {
	TYPE_FULL_SCREEN,
	TYPE_LEFT_HALF_SCREEN,
	TYPE_RIGHT_HALF_SCREEN,
	TYPE_UP_HALF_SCREEN,
	TYPE_DOWN_HALF_SCREEN,
	TYPE_NONE,
};

void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA* GetCamera(void);

void LerpCameraPosition(XMFLOAT3 pos, float dir, float tPos = 1.0f);
void LerpCameraPositionAt(XMFLOAT3 playerPos, XMFLOAT3 enemyPos, float dir, float tPos, float tAt);
void LerpCameraViewAngle(float angle, float t);
