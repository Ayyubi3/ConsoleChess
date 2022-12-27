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
	int hasMoved;//Pawn can only move if it is on the starting cell and has to free cells in front of it
} Piece;

typedef struct {

	int DBoxColorR, DBoxColorG, DBoxColorB;
	int LBoxColorR, LBoxColorG, LBoxColorB;

	int DPieceColorR, DPieceColorG, DPieceColorB;
	int LPieceColorR, LPieceColorG, LPieceColorB;


}Colors;


typedef struct {


	Piece Board[64];//64-bit integer instead of Piece Array//Use char instead of int (one byte insted of 4)

	Point Cursor;

	int LocalIndex;

	int isWhiteTurn;

	Point markedPos[64];

	int MarkedCellsCounter;

	Point Scope;



} Game;

Point POINT_Add(Point point1, Point point2);
int POINT_IsZero(Point point);
int POINT_equals(Point point1, Point point2);