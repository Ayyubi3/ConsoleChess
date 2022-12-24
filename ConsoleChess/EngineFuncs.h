#pragma once
#include "EngineFuncs.h"


void ENGINE_SetBackgroundColor(int R, int G, int B);

void ENGINE_SetForegroundColor(int R, int G, int B);
void ENGINE_SetCursorPos(int x, int y);

void ENGINE_WriteCharToXy(int oldX, int oldY, int targetX, int targetY, char ch, int returnToOldPos);