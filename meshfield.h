#pragma once

enum
{
	EndOfNone = 0b0000,
	EndOfZPlus = 0b0001,
	EndOfZMinus = 0b0010,
	EndOfXPlus = 0b0100,
	EndOfXMinus = 0b1000,
};

HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);
void DrawMeshFieldToDepthTex();

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3* HitPosition, XMFLOAT3* Normal);

bool IsPositionValid(float x, float z);

XMFLOAT3 XM_CALLCONV GetWrapPosition(XMFLOAT3 pos, int endOfBoarderFlag);

float GetFieldHeight(float x, float z);
int IsOutOfBoarder(float x, float z);
float GetFieldProgress(float x, float z, float dir);
int IsAtConjunction(float x, float z, float dir);

XMFLOAT3 GetRandomValidPosition();
// in a range of [-0.8, 0.8] ^ 2
XMFLOAT3 GetRandomPosition();