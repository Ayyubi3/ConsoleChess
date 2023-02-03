#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "Point.h"
#include "Game.h"

#include "Engine.h"


// Dont know what this is, but prevents warnings to stop compilation
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)


void PIECE_getAllMovesAbsolute(char* Board, Point* BufferSize50, Point piece, int antiRecursion, int isWhiteTurn);
void PIECE_getAllMovesRelative(char piece, Point* buffer);
int PIECE_isInBoard(Point x);
int PIECE_isThreat(char piece, int isWhiteTurn);
void PIECE_makeMove(char* Board, Point Piece, Point Target);

void BOARD_Print(char* Board);
void BOARD_getAllCellsDanger(char* Board, int searchWhite, Point* BufferSize128);
int BOARD_ReadFEN(char* FEN, Game* sys);

void CELL_PrintPreview(Game* sys);
void CELL_ClearPreview(Game* sys);
void CELL_AddToPreview(Point coords, Game* sys, int color);


int testBoard(char* Board, int wTurn, int depth);

int main()
{
	Game* sys = malloc(sizeof(Game));

	//Set Point index to 1,1
	sys->Cursor = POINT(1, 1);

	// clear everything before start, just because
	for (size_t i = 0; i < 64; i++)
	{
		sys->Board[i] = ' ';
		sys->markedPos[i] = POINT(0, 0);
	}
	CELL_ClearPreview(sys);
	sys->Scope = POINT(0, 0);
	printf("\033[H\033[J");


	BOARD_ReadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w", sys);
	// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

	//This part tests using Perft
	//printf("%i", testBoard(sys->Board, 1, 4));

	while (1)
	{
		BOARD_Print(sys->Board);

		ENGINE_SetCursorPos(sys->Cursor);

		CELL_PrintPreview(sys);



		char ctrl = getch();

		switch (ctrl)
		{
		case 'q':
			printf("\033[H\033[J");
			exit(0);
			break;
		case 'w':
			if (sys->Cursor.y > 1)
				sys->Cursor.y--;
			break;
		case 'a':
			if (sys->Cursor.x > 1)
				sys->Cursor.x--;
			break;
		case 's':
			if (sys->Cursor.y < 8)
				sys->Cursor.y++;
			break;
		case 'd':
			if (sys->Cursor.x < 8)
				sys->Cursor.x++;
			break;

		case '\r':
			
			// Cant click on opponents piece if scope is zero
			if (POINT_isZero(sys->Scope) &&
				sys->isWhiteTurn &&
				islower(sys->Board[POINT_getIndex(sys->Cursor)]))
				break;
			if (POINT_isZero(sys->Scope) &&
				!sys->isWhiteTurn &&
				isupper(sys->Board[POINT_getIndex(sys->Cursor)]))
				break;



			// if no piece is selected, select one
			if (POINT_isZero(sys->Scope) && sys->Board[POINT_getIndex(sys->Cursor)] != ' ')
			{
				sys->Scope = sys->Cursor;

				CELL_ClearPreview(sys);

				Point* mov = malloc(64 * sizeof(Point));

				//get all absolute positions possible for the currently playing
				PIECE_getAllMovesAbsolute(sys->Board, mov, sys->Scope, 1, sys->isWhiteTurn);

				CELL_AddToPreview(sys->Scope, sys, 1);

				for (size_t i = 0; i < 64; i++)
				{
					if (POINT_isZero(mov[i]))
						break;

					//Change Cell color according to the pieces owner
					PIECE_isThreat(sys->Board[POINT_getIndex(mov[i])], sys->isWhiteTurn) ? 
						CELL_AddToPreview(mov[i], sys, 3) : 
						CELL_AddToPreview(mov[i], sys, 2);
				}

				free(mov);
			}


			// if its the same piece selected prior, unselect it
			else if (POINT_equals(sys->Scope, sys->Cursor))
			{
				sys->Scope = POINT(0, 0);
				CELL_ClearPreview(sys);
			}


			// If Piece is selected, not same as Scope and in markedCells move and change turns
			else if (!POINT_isZero(sys->Scope) && !POINT_equals(sys->Scope, sys->Cursor) && POINT_isPointInArray(sys, sys->Cursor, sys->MarkedCellsCounter, sys->markedPos))
			{
				PIECE_makeMove(&sys->Board, sys->Scope, sys->Cursor);
				sys->Scope = POINT(0, 0);

				CELL_ClearPreview(sys);

				sys->isWhiteTurn = !sys->isWhiteTurn;
			}
			break;
		}
		ENGINE_SetCursorPos(POINT(0, 0));
	}

}






//Cell
void CELL_AddToPreview(Point coords, Game* sys, int color)
{
	sys->markedPos[sys->MarkedCellsCounter] = coords;
	sys->markedPosCol[sys->MarkedCellsCounter++] = color;
}

void CELL_ClearPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		sys->markedPos[i] = POINT(0, 0);
	}
	sys->MarkedCellsCounter = 0;
}

void CELL_PrintPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		if (POINT_isZero(sys->markedPos[i]))
			return;

		ENGINE_SetCursorPos(sys->markedPos[i]);

		switch (sys->markedPosCol[i])
		{
		case 0:
			ENGINE_SetBackgroundColor(44, 44, 233);
			break;
		case 1:
			ENGINE_SetBackgroundColor(0, 255, 0);
			break;
		case 2:
			ENGINE_SetBackgroundColor(0, 0, 255);
			break;
		}

		int index = (sys->Scope.y - 1) * 8 + (sys->Scope.x - 1);
		printf("%c", sys->Board[index]);
		ENGINE_SetBackgroundColor(0, 0, 0);

		ENGINE_SetCursorPos(sys->Cursor);
	}
}


/// <summary>
/// Copies the possible moves a piece can make relative to itself
/// (Rooks relative moves are { 1 , 0 }{ -1 , 0 }{ 0 , 1 }{ 0 , -1 })
/// </summary>
/// <param name="piece"></param>
/// <param name="bufferSize8"></param>
void PIECE_getAllMovesRelative(char piece, Point* bufferSize8)
{

	Point moves[8];
	for (size_t i = 0; i < 8; i++)
		moves[i] = POINT(0, 0);

	switch (toupper(piece))
	{
	case 'R':
		moves[0] = POINT(0, -1);
		moves[1] = POINT(1, 0);
		moves[2] = POINT(-1, 0);
		moves[3] = POINT(0, 1);
		break;
	case 'B':
		moves[0] = POINT(1, 1);
		moves[1] = POINT(-1, -1);
		moves[2] = POINT(-1, 1);
		moves[3] = POINT(1, -1);
		break;
	case 'Q':
		moves[0] = POINT(1, 1);
		moves[1] = POINT(-1, -1);
		moves[2] = POINT(-1, 1);
		moves[3] = POINT(1, -1);
		moves[4] = POINT(1, 0);
		moves[5] = POINT(-1, 0);
		moves[6] = POINT(0, 1);
		moves[7] = POINT(0, -1);
		break;
	case 'K':
		moves[0] = POINT(0, 1);
		moves[1] = POINT(1, 1);
		moves[2] = POINT(1, 0);
		moves[3] = POINT(1, -1);
		moves[4] = POINT(0, -1);
		moves[5] = POINT(-1, -1);
		moves[6] = POINT(-1, 0);
		moves[7] = POINT(-1, 1);
		break;
	case 'P':
		moves[0] = POINT(0, -1);
		moves[1] = POINT(0, -2);
		break;
	case 'N':
		moves[0] = POINT(2, -1);
		moves[1] = POINT(2, 1);
		moves[2] = POINT(-2, 1);
		moves[3] = POINT(-2, -1);
		moves[4] = POINT(1, 2);
		moves[5] = POINT(-1, 2);
		moves[6] = POINT(-1, -2);
		moves[7] = POINT(1, -2);
		break;
	}
	memcpy(bufferSize8, moves, sizeof(Point) * 8);
}


/// <summary>
/// returns true if the given piece is a threat to the currently playing player
/// (PIECE_isThreat('r', 1) == 1)
/// </summary>
/// <param name="piece"></param>
/// <param name="isWhiteTurn"></param>
/// <returns></returns>
int PIECE_isThreat(char piece, int isWhiteTurn)
{
	if (isWhiteTurn)
		return (piece >= 'a' && piece <= 'z');
	else if (!isWhiteTurn)
		return (piece >= 'A' && piece <= 'Z');
}



/// <summary>
/// Return 1 if given point is a valid position on board
/// (Board in Point represantation in index 1)
/// </summary>
/// <param name="x"></param>
/// <returns></returns>
int PIECE_isInBoard(Point x)
{
	return (x.x >= 1 && x.x <= 8) && (x.y >= 1 && x.y <= 8);
}

int cmpfunc(const void* a, const void* b) {

	Point s = *(Point*)a;
	Point ds = *(Point*)b;


	return POINT_getIndex(s) < POINT_getIndex(ds);
}

void PIECE_getAllMovesAbsolute(char* Board, Point *BufferSize50, Point piece, int antiRecursion, int isWhiteTurn)
{
#define possibleMovesArrayLength 50

	Point possibleMoves[possibleMovesArrayLength];
	int pMCounter = 0;

	int indexOfPiece = POINT_getIndex(piece);
	char inpt = toupper(Board[indexOfPiece]);

	for (size_t i = 0; i < possibleMovesArrayLength; i++)
		possibleMoves[i] = POINT(0, 0);


	switch (inpt)
	{
		case 'P':
		{
			Point Moves[8];
			PIECE_getAllMovesRelative(inpt, &Moves);

			for (size_t i = 0; i < 8; i++)
			{

				// Invert Move for Pawn(Other Pieces are independet of whos turn it is)
				isWhiteTurn ? (Moves[i].y = Moves[i].y) : (Moves[i].y = -Moves[i].y);

				// Pawn: If already moved dont add +2 movement
				if (!(
					((piece.y == 7 && isWhiteTurn) ||
						(piece.y == 2 && !isWhiteTurn))) &&
					i == 1)
					continue;

				// Calculate next cell
				int x = piece.x + Moves[i].x;
				int y = piece.y + Moves[i].y;

				if (!PIECE_isInBoard(POINT(x, y))) // if out of bound: kill iteration
					continue;

				if (Board[POINT_getIndex(POINT(x, y))] == ' ') // If cell is empty move should be safe todo. NOTE: might cause problems with king later
					possibleMoves[pMCounter++] = POINT(x, y);

				// Pawns movement
				int offsetMultiplier;
				isWhiteTurn ? (offsetMultiplier = -1) : (offsetMultiplier = 1);

				Point offset1 = POINT(1, offsetMultiplier);
				Point offset2 = POINT(-1, offsetMultiplier);

				Point p1Point = POINT_Add(piece, offset1);
				Point p2Point = POINT_Add(piece, offset2);

				char p1 = Board[POINT_getIndex(p1Point)];
				char p2 = Board[POINT_getIndex(p2Point)];

				if (p1 != ' ' && PIECE_isInBoard(p1Point) && PIECE_isThreat(Board[POINT_getIndex(p1Point)], isWhiteTurn))
					possibleMoves[pMCounter++] = p1Point;

				if (p2 != ' ' && PIECE_isInBoard(p2Point) && PIECE_isThreat(Board[POINT_getIndex(p2Point)], isWhiteTurn))
					possibleMoves[pMCounter++] = p2Point;
			}
		}
		break;
	case 'K':
	case 'N':
	{
		Point Moves[8];
		PIECE_getAllMovesRelative(inpt, &Moves);

		for (size_t i = 0; i < 8; i++)
		{

			// Calculate next cell
			int x = piece.x + Moves[i].x;
			int y = piece.y + Moves[i].y;

			if (!PIECE_isInBoard(POINT(x, y))) // if out of bound: kill iteration
				continue;

			if (Board[POINT_getIndex(POINT(x, y))] == ' ') // If cell is empty move should be safe todo. NOTE: might cause problems with king later
				possibleMoves[pMCounter++] = POINT(x, y);
			else if (PIECE_isThreat(Board[POINT_getIndex(POINT(x, y))], isWhiteTurn)) // If Cell is threat mark it. NOTE: maybe change that later so the dangered pieces are in a different array
				possibleMoves[pMCounter++] = POINT(x, y);
		}
	}
		break;
	case 'R':
	case 'Q':
	case 'B':
	{

		Point Moves[8];
		PIECE_getAllMovesRelative(inpt, &Moves);

		for (size_t i = 0; i < 8; i++)
		{

			int x = piece.x + Moves[i].x;
			int y = piece.y + Moves[i].y;
			if (POINT_isZero(Moves[i]))
				continue;

			while (Board[POINT_getIndex(POINT(x, y))] == ' ')
			{
				if (!PIECE_isInBoard(POINT(x, y)))
					break;

				possibleMoves[pMCounter++] = POINT(x, y);

				x = x + Moves[i].x;
				y = y + Moves[i].y;
			}

			// If next Cell is not out of Bounds and is Enemy Piece: Add to list
			if (PIECE_isInBoard(POINT(x, y)) && PIECE_isThreat(Board[POINT_getIndex(POINT(x, y))], isWhiteTurn))
				possibleMoves[pMCounter++] = POINT(x, y);
		}
	}
		break;
	}


	if (antiRecursion)
	{
			
			for (size_t i = 0; i < pMCounter; i++)
			{
				char test[64];
				memcpy(test, Board, sizeof(char)*64);

				PIECE_makeMove(test, piece, possibleMoves[i]);

				if (BOARD_isKingInCheck(test, isWhiteTurn))
				{
					possibleMoves[i] = POINT(0, 0);
				}

			}


	}

	qsort(possibleMoves, pMCounter, sizeof(Point), cmpfunc);
	memcpy(BufferSize50, possibleMoves, sizeof(Point) * possibleMovesArrayLength);


	#undef possibleMovesArrayLength
}






void PIECE_makeMove(char* Board, Point Piece, Point Target)
{
	int ScopeIndex = POINT_getIndex(Piece);
	Board[POINT_getIndex(Target)] = Board[ScopeIndex];
	Board[ScopeIndex] = ' ';
}


int BOARD_ReadFEN(char* FEN, Game* sys)
{
	// TODO: Process extended FEN string (The part after the long string with w-- etc)
	int index = 0, i;
	for (i = 0; FEN[i] != '\0' && FEN[i] != ' '; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			sys->Board[index] = FEN[i];
			index++;
		}
		else if ('0' <= FEN[i] && FEN[i] <= '9')
		{
			index += (FEN[i] - 48);
		}
		else if (FEN[i] == '/')
		{
			for (size_t j = 0; index % 8 == 1; j++)
			{
				index++;
			}
		}
	}

	sys->isWhiteTurn = FEN[++i] == 'w';
}



void BOARD_Print(char* Board)
{

	ENGINE_SetBackgroundColor(0, 0, 0);

	int DBoxColorR = 136;
	int DBoxColorG = 119;
	int DBoxColorB = 183;

	int LBoxColorR = 233;
	int LBoxColorG = 4;
	int LBoxColorB = 33;

	int DPieceColorR = 0;
	int DPieceColorG = 0;
	int DPieceColorB = 0;

	int LPieceColorR = 255;
	int LPieceColorG = 255;
	int LPieceColorB = 255;

	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{

			int index = 8 * i + j;
			// Draw Chess Pattern
			// Modulo-part offsets every row by one
			((8 * i + j) + (i % 2)) % 2 == 0 ? ENGINE_SetBackgroundColor(DBoxColorR, DBoxColorB, DBoxColorB) : ENGINE_SetBackgroundColor(LBoxColorR, LBoxColorB, LBoxColorB);

			// Color Pieces
			(islower(Board[index])) ? ENGINE_SetForegroundColor(DPieceColorR, DPieceColorB, DPieceColorB) : ENGINE_SetForegroundColor(LPieceColorR, LPieceColorB, LPieceColorB);

			printf("%c", Board[index]);

			// Reset Colors
			ENGINE_SetForegroundColor(255, 255, 255);
			ENGINE_SetBackgroundColor(0, 0, 0);
		}

		printf("\n");
	}
}




/// <summary>
/// Returns all the endangered cells
/// (The Cell infront of a pawn is not endangered)
/// </summary>
/// <param name="Board"></param>
/// <param name="searchWhite"></param>
/// <param name="BufferSize64"></param>
void BOARD_getAllCellsDanger(char* Board, int searchWhite, Point* BufferSize64)
{
	Point possibleMove[128];

	for (size_t i = 0; i < 128; i++)
		possibleMove[i] = POINT(0, 0);

	int counter = 0;

	//If searching for cells that white pieces endanger, P
	char test = searchWhite ? 'P' : 'p';

	for (size_t i = 1; i < 9; i++)
		for (size_t j = 1; j < 9; j++)
		{

			if (Board[POINT_getIndex(POINT(i, j))] != ' ' && PIECE_isThreat( Board[POINT_getIndex(POINT(i, j))], !searchWhite))
			{

				Point arr[50];
				PIECE_getAllMovesAbsolute(Board, &arr, POINT(i, j), 0, searchWhite);

				if (Board[POINT_getIndex(POINT(i, j))] != test)
				{

					for (size_t d = 0; d < 50; d++)
					{
						if (POINT_isZero(arr[d]))
						{
							break;
						}
						possibleMove[counter++] = arr[d];
					}
				}

				else if (Board[POINT_getIndex(POINT(i, j))] == test)
				{
					int x1 = i + 1, y1 = j + (searchWhite ? -1 : 1);
					if (PIECE_isInBoard(POINT(x1, y1)))
					{
						possibleMove[counter++] = POINT(x1, y1);
					}
					int x2 = i - 1, y2 = j + (searchWhite ? -1 : 1);
					if (PIECE_isInBoard(POINT(x2, y2)))
					{
						possibleMove[counter++] = POINT(x2, y2);
					}
				}
			}
			

		}

	int i, j, k, number = 128;
	for (i = 0; i < number; i++)
	{
		for (j = i + 1; j < number; j++)
		{
			if (POINT_equals(possibleMove[i], possibleMove[j]))
			{
				for (k = j; k < number; k++)
				{
					possibleMove[k] = possibleMove[k + 1];
				}
				j--;
				number--;
			}
		}
	}

	for (size_t i = 0; i < 64; i++)
	{
		if (!PIECE_isInBoard(possibleMove[i]))
			possibleMove[i] = POINT(0, 0);
	}

	memcpy(BufferSize64, possibleMove, sizeof(Point) * 64);
}


int BOARD_isKingInCheck(char* Board, int isWhiteTurn)
{
	Point buff[64];
	BOARD_getAllCellsDanger(Board, !isWhiteTurn, &buff);

	char target = isWhiteTurn ? 'K' : 'k';
	int targetIndex = 0;

	//find the your king
	for (size_t i = 0; i < 64; i++)
	{
		if (Board[i] == target)
		{
			targetIndex = i;
			break;
		}
	}

	Point trag = POINT(targetIndex % 8 + 1, targetIndex / 8 + 1);
	for (size_t i = 0; i < 64; i++)
	{
		if (POINT_equals(buff[i], trag))
		{
			return 1;
		}
	}

	return 0;
}




int testBoard(char* Board, int wTurn, int depth)
{

	//Code to Test board
	if (depth == 0)
		return 1;

	int counter = 0;

	for (int i = 1; i < 9; i++)
		for (int j = 1; j < 9; j++)
		{
			if (!PIECE_isThreat(Board[POINT_getIndex(POINT(i, j))], wTurn) && Board[POINT_getIndex(POINT(i, j))] != ' ')
			{
				Point Buf[50];
				PIECE_getAllMovesAbsolute(Board, Buf, POINT(i, j), 1, wTurn);

				for (size_t i = 0; i < 50; i++)
				{
					if (POINT_isZero(Buf[i]))
						break;

					char x[64];
					memcpy(x, Board, 64);

					PIECE_makeMove(Board, POINT(i, j), Buf[i]);
					counter += testBoard(Board, !wTurn, depth - 1);

					memcpy(Board, x, 64);

				}
			}
		}

	return counter;
}