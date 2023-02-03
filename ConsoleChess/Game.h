#pragma once

#include "Point.h"

typedef struct
{

	char Board[64];

	struct Point Cursor;

	struct Point Scope;

	int isWhiteTurn;

	struct Point markedPos[64];

	int MarkedCellsCounter;

	int markedPosCol[64];

} Game;