#pragma once

#define SCORE_MAX			(99999)		 
#define SCORE_DIGIT			(5)			 

HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
int GetScore(void);
