#pragma once

#define	Billboard_WIDTH			(15.0f)
#define	Billboard_HEIGHT		(15.0f)

enum CommandCode : int
{
	Up = 0,
	Down = 1,
	Left = 2,
	Right = 3,
	None = -1,
};

HRESULT InitBillboard(void);
void ShutdownBillboard(void);
void DrawBillboard(CommandCode code, XMFLOAT3 position, const XMFLOAT3 scale, bool isTriggered);
