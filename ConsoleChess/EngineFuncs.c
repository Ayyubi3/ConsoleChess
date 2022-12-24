#include <stdio.h>
#include "EngineFuncs.h"


void ENGINE_SetBackgroundColor(int R, int G, int B)
{
	printf("\x1b[48;2;%i;%i;%im", R, G, B); 
}

void ENGINE_SetForegroundColor(int R, int G, int B)
{
	printf("\x1b[38;2;%i;%i;%im", R, G, B);
}

void ENGINE_SetCursorPos(int x, int y)
{
	printf("\033[%d;%dH", y, x);
}

void ENGINE_WriteCharToXy(int oldX, int oldY, int targetX, int targetY, char ch, int returnToOldPos)
{
	ENGINE_SetCursorPos(targetX, targetY);
	printf("%c", ch);
	ENGINE_SetCursorPos(targetX, targetY);

	returnToOldPos ? ENGINE_SetCursorPos(oldX, oldY) : ENGINE_SetCursorPos(targetX, targetY);
}


