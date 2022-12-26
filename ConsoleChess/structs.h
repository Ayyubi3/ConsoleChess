#pragma once

#include "structs.h"

typedef struct {
	int x;
	int y;
} Point;



typedef struct {
	char name;
	Point possibleMoves[50];
	int value;
	int isRecursive;
	int hasMoved;
} Piece;

typedef struct {

	int DBoxColorR, DBoxColorG, DBoxColorB;
	int LBoxColorR, LBoxColorG, LBoxColorB;

	int DPieceColorR, DPieceColorG, DPieceColorB;
	int LPieceColorR, LPieceColorG, LPieceColorB;


}Colors;


typedef struct {


	Piece Board[64];

	Point Cursor;

	int LocalIndex;

	int isWhiteTurn;

	Point markedPos[64];

	int MarkedCellsCounter;



} Game;

Point POINT_Add(Point point1, Point point2);