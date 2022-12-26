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

void ENGINE_SetCursorPos(Point coords)
{
	printf("\033[%d;%dH", coords.y, coords.x);
}

void ENGINE_WriteCharToXy(Point old, Point target, char ch, int returnToOldPos)
{
	ENGINE_SetCursorPos(target);
	printf("%c", ch);
	ENGINE_SetCursorPos(target);

	returnToOldPos ? ENGINE_SetCursorPos(old) : ENGINE_SetCursorPos(target);
}


