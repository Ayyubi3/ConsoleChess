#include "Point.h"
#include <stdio.h>


//Engine
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