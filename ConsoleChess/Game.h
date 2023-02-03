#pragma once

#include "Point.h"


typedef struct
{

	char Board[64];

	Point Cursor;

	Point Scope;

	int isWhiteTurn;

	Point markedPos[64];

	int MarkedCellsCounter;

	int markedPosCol[64];

} Game;