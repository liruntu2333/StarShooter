#pragma once

void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT* light);
void SetFogData(FOG* fog);
LIGHT* GetLightData(int index);
