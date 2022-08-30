//=============================================================================
//
// 木処理 [Billboard.h]
// Author : 
//
//=============================================================================
#pragma once

#define	Billboard_WIDTH			(20.0f)
#define	Billboard_HEIGHT		(20.0f)

enum CommandCode : int
{
	Up = 0,
	Down = 1,
	Left = 2,
	Right = 3,
	None = -1,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBillboard(void);
void ShutdownBillboard(void);
void DrawBillboard(CommandCode code, XMFLOAT3 position, const XMFLOAT3 scale, bool isTriggered);

