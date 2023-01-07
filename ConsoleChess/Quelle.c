#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Dont know what this is, but prevents warnings to stop compilation
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

#pragma region STRUCTS

typedef struct { int x; int y; } Point;

typedef struct
{

	int DBoxColorR, DBoxColorG, DBoxColorB;
	int LBoxColorR, LBoxColorG, LBoxColorB;

	int DPieceColorR, DPieceColorG, DPieceColorB;
	int LPieceColorR, LPieceColorG, LPieceColorB;

} Colors;

typedef struct
{

	char Board[64]; // 64-bit integer instead of Piece Array//Use char instead of int (one byte insted of 4)

	Point Cursor;

	Point Scope;

	int isWhiteTurn;

	Point markedPos[64];

	int MarkedCellsCounter;

	int markedPosCol[64];
} Game;
#pragma endregion

#pragma region ENGINE CLASS

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

#pragma endregion

#pragma region POINT CLASS

Point POINT(int x, int y)
{
	return (Point){x, y};
}

Point POINT_Add(Point point1, Point point2)
{
	return (Point){point1.x + point2.x, point1.y + point2.y};
}

int POINT_IsZero(Point point)
{
	return (point.x == 0 && point.y == 0);
}

int POINT_equals(Point point1, Point point2)
{
	return (point1.x == point2.x) && (point1.y == point2.y);
}

int POINT_getIndex(Point p)
{
	return (p.x - 1) + (p.y - 1) * 8;
}

int POINT_isPointInArray(Game *sys, Point p, int length, Point *arr)
{
	for (size_t i = 0; i < length; i++)
	{
		if (POINT_equals(p, arr[i]))
		{
			return 1;
		}
	}
	return 0;
}

#pragma endregion

#pragma region FUNCTION DEFINITIONS

void PIECE_getAllMovesAbsolute(Game* sys, Point* BufferSize50, Point piece, int antiRecursion, int isWhiteTurn);
void PIECE_getAllMovesRelative(char piece, Point* buffer);
int PIECE_getChessPieceValue(char piece);
int PIECE_isInBoard(Point x);
int PIECE_isThreat( char piece, int isWhiteTurn);

void BOARD_Print(Colors *colors, Game *sys);
void BOARD_getAllMovesForColor(Game *sys, int searchWhite, Point *arr);
int BOARD_ReadFEN(char* FEN, Game* sys);

void CELL_PrintPreview(Game *sys);
void CELL_ClearPreview(Game *sys);
void CELL_AddToPreview(Point coords, Game *sys, int color);

#pragma endregion

#pragma region PIECE DICTIONARIES

int PIECE_getChessPieceValue(char piece)
{
	switch (toupper(piece))
	{
	case 'P':
		return 1;
		break;
	case 'N':
		return 3;
		break;
	case 'B':
		return 3;
		break;
	case 'R':
		return 5;
		break;
	case 'Q':
		return 9;
		break;
	}
}

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

#pragma endregion

int PIECE_isThreat(char piece, int isWhiteTurn)
{
	if (isWhiteTurn)
		return (piece >= 'a' && piece <= 'z');
	else if (!isWhiteTurn)
		return (piece >= 'A' && piece <= 'Z');
}


int PIECE_isInBoard(Point x)
{
	return (x.x >= 1 && x.x <= 8) && (x.y >= 1 && x.y <= 8);
}

void PIECE_getAllMovesAbsolute(Game *sys, Point *BufferSize50, Point piece, int antiRecursion, int isWhiteTurn)
{
#define possibleMovesArrayLength 50

	Point possibleMoves[possibleMovesArrayLength];
	int pMCounter = 0;

	int indexOfPiece = POINT_getIndex(piece);
	char inpt = toupper(sys->Board[indexOfPiece]);

	for (size_t i = 0; i < possibleMovesArrayLength; i++)
		possibleMoves[i] = POINT(0, 0);

	//if (antiRecursion)
	//{
	//	if (BOARD_isKingInCheck(sys, isWhiteTurn))
	//	{
	//		memcpy(BufferSize50, possibleMoves, sizeof(Point) * possibleMovesArrayLength);
	//		return;
	//	}
	//}

	if (inpt == 'P')
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

			if (sys->Board[POINT_getIndex(POINT(x, y))] == ' ') // If cell is empty move should be safe todo. NOTE: might cause problems with king later
				possibleMoves[pMCounter++] = POINT(x, y);

			// Pawns movement
			int offsetMultiplier;
			isWhiteTurn ? (offsetMultiplier = -1) : (offsetMultiplier = 1);

			Point offset1 = POINT(1, offsetMultiplier);
			Point offset2 = POINT(-1, offsetMultiplier);

			Point p1Point = POINT_Add(piece, offset1);
			Point p2Point = POINT_Add(piece, offset2);

			char p1 = sys->Board[POINT_getIndex(p1Point)];
			char p2 = sys->Board[POINT_getIndex(p2Point)];

			if (p1 != ' ' && PIECE_isInBoard(p1Point) && PIECE_isThreat(sys->Board[POINT_getIndex(p1Point)], isWhiteTurn))
				possibleMoves[pMCounter++] = p1Point;

			if (p2 != ' ' && PIECE_isInBoard(p2Point) && PIECE_isThreat(sys->Board[POINT_getIndex(p1Point)], isWhiteTurn))
				possibleMoves[pMCounter++] = p2Point;
		}
	} ////////////////////////////////////////////////////////////////////////////////////////////
	else if (inpt == 'K') 
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

			if (sys->Board[POINT_getIndex(POINT(x, y))] == ' ') // If cell is empty move should be safe todo. NOTE: might cause problems with king later
				possibleMoves[pMCounter++] = POINT(x, y);
			else if (PIECE_isThreat( sys->Board[POINT_getIndex(POINT(x, y))], isWhiteTurn)) // If Cell is threat mark it. NOTE: maybe change that later so the dangered pieces are in a different array
				possibleMoves[pMCounter++] = POINT(x, y);
		}

		// Here possibleMoves Array has all position Kings can go to without considering the enmies pieces
		if (antiRecursion)
		{
			Point EnemyTargetCells[64];
			Point newOutput[30];
			int newOutputCounter = 0;
			for (size_t i = 0; i < 30; i++)
			{
				EnemyTargetCells[i] = POINT(0, 0);
				newOutput[i] = POINT(0, 0);
			}

			BOARD_getAllMovesForColor(sys, !isWhiteTurn, &EnemyTargetCells);

			// Go through all possibleMoves and sort out any that are threatend and put them in newOutput

			for (size_t i = 0; i < 8; i++)
			{
				int check = 0;
				for (size_t j = 0; j < 30; j++)
				{
					if (POINT_equals(possibleMoves[i], EnemyTargetCells[j]))
					{
						check = 1;
					}
				}
				if (!check)
					newOutput[newOutputCounter++] = possibleMoves[i];
			}

			for (size_t i = 0; i < possibleMovesArrayLength; i++)
			{
				possibleMoves[i] = POINT(0, 0);
			}
			for (size_t i = 0; i < 30; i++)
			{
				possibleMoves[i] = newOutput[i];
			}
		}

	} ////////////////////////////////////////////////////////////////////////////////////////////
	else if (inpt == 'N')
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

			if (sys->Board[POINT_getIndex(POINT(x, y))] == ' ') // If cell is empty move should be safe todo. NOTE: might cause problems with king later
				possibleMoves[pMCounter++] = POINT(x, y);
			else if (PIECE_isThreat(sys->Board[POINT_getIndex(POINT(x, y))], isWhiteTurn)) // If Cell is threat mark it. NOTE: maybe change that later so the dangered pieces are in a different array
				possibleMoves[pMCounter++] = POINT(x, y);
		}
	} ////////////////////////////////////////////////////////////////////////////////////////////
	else if (inpt == 'R' || inpt == 'Q' || inpt == 'B')
	{

		Point Moves[8];
		PIECE_getAllMovesRelative(inpt, &Moves);

		for (size_t i = 0; i < 8; i++)
		{

			int x = piece.x + Moves[i].x;
			int y = piece.y + Moves[i].y;
			if (POINT_IsZero(Moves[i]))
			{
				continue;
			}
			while (sys->Board[POINT_getIndex(POINT(x, y))] == ' ')
			{
				if (!PIECE_isInBoard(POINT(x, y)))
					break;

				possibleMoves[pMCounter++] = POINT(x, y);

				x = x + Moves[i].x;
				y = y + Moves[i].y;
			}

			// If next Cell is not out of Bounds and is Enemy Piece: Add to list
			if (PIECE_isInBoard(POINT(x, y)) && PIECE_isThreat(sys->Board[POINT_getIndex(POINT(x, y))], isWhiteTurn))
				possibleMoves[pMCounter++] = POINT(x, y);
		}
	}

	memcpy(BufferSize50, possibleMoves, sizeof(Point) * possibleMovesArrayLength);


	#undef possibleMovesArrayLength
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BOARD_Print(Colors *colors, Game *sys)
{

	ENGINE_SetBackgroundColor(0, 0, 0);

	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{

			int index = 8 * i + j;
			// Draw Chess Pattern
			// Modulo-part offsets every row by one
			((8 * i + j) + (i % 2)) % 2 == 0 ? ENGINE_SetBackgroundColor(colors->DBoxColorR, colors->DBoxColorB, colors->DBoxColorB) : ENGINE_SetBackgroundColor(colors->LBoxColorR, colors->LBoxColorB, colors->LBoxColorB);

			// Color Pieces
			(islower(sys->Board[index])) ? ENGINE_SetForegroundColor(colors->DPieceColorR, colors->DPieceColorB, colors->DPieceColorB) : ENGINE_SetForegroundColor(colors->LPieceColorR, colors->LPieceColorB, colors->LPieceColorB);

			printf("%c", sys->Board[index]);

			// Reset Colors
			ENGINE_SetForegroundColor(255, 255, 255);
			ENGINE_SetBackgroundColor(0, 0, 0);
		}

		printf("\n");
	}
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

void BOARD_getAllMovesForColor(Game* sys, int searchWhite, Point* BufferSize128)
{
	Point possibleMove[128];

	for (size_t i = 0; i < 128; i++)
		possibleMove[i] = POINT(0, 0);

	int counter = 0;

	char test = searchWhite ? 'P' : 'p';

	for (size_t i = 1; i < 9; i++)
		for (size_t j = 1; j < 9; j++)
		{
			// Just calculate all possible moves for all white pieces
			if (1)
			{
				if (sys->Board[POINT_getIndex(POINT(i, j))] != ' ' && PIECE_isThreat( sys->Board[POINT_getIndex(POINT(i, j))], !searchWhite))
				{

					Point arr[50];
					PIECE_getAllMovesAbsolute(sys, &arr, POINT(i, j), 0, searchWhite);

					if (sys->Board[POINT_getIndex(POINT(i, j))] != test)
					{

						for (size_t d = 0; d < 50; d++)
						{
							if (POINT_IsZero(arr[d]))
							{
								break;
							}
							possibleMove[counter++] = arr[d];
						}
					}

					else if (sys->Board[POINT_getIndex(POINT(i, j))] == test)
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

	memcpy(BufferSize128, possibleMove, sizeof(Point) * 64);
}

int BOARD_isKingInCheck(Game* sys, int isWhiteTurn)
{
	Point buff[64];
	BOARD_getAllMovesForColor(sys, !isWhiteTurn, &buff);

	char target = isWhiteTurn ? 'K' : 'k';
	int targetIndex = 0;

	//find the your king
	for (size_t i = 0; i < 64; i++)
	{
		if (sys->Board[i] == target)
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

void CELL_AddToPreview(Point coords, Game *sys, int color)
{
	sys->markedPos[sys->MarkedCellsCounter] = coords;
	sys->markedPosCol[sys->MarkedCellsCounter++] = color;
}

void CELL_ClearPreview(Game *sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		sys->markedPos[i] = POINT(0, 0);
	}
	sys->MarkedCellsCounter = 0;
}

void CELL_PrintPreview(Game *sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		if (POINT_IsZero(sys->markedPos[i]))
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	Game *sys = malloc(sizeof(Game));
	Colors *SysCol = malloc(sizeof(Colors));

	// Configure Game Colors
	SysCol->DBoxColorR = 136;
	SysCol->DBoxColorG = 119;
	SysCol->DBoxColorB = 183;

	SysCol->LBoxColorR = 233;
	SysCol->LBoxColorG = 4;
	SysCol->LBoxColorB = 33;

	SysCol->DPieceColorR = 0;
	SysCol->DPieceColorG = 0;
	SysCol->DPieceColorB = 0;

	SysCol->LPieceColorR = 255;
	SysCol->LPieceColorG = 255;
	SysCol->LPieceColorB = 255;

	// Configure Game Settings
	sys->Cursor = POINT(1, 1);

	// clear everything before start, just because
	for (size_t i = 0; i < 64; i++)
	{
		sys->Board[i] = ' ';
		sys->markedPos[i] = POINT(0, 0);
	}

	CELL_ClearPreview(sys);
	sys->Scope = POINT(0, 0);

	BOARD_ReadFEN("8/4k3/8/8/2R1K3/8/8/8 w", sys);
	// rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

	while (1)
	{
		int index = (sys->Cursor.x - 1) + (sys->Cursor.y - 1) * 8;

		BOARD_Print(SysCol, sys);

		ENGINE_SetCursorPos(sys->Cursor);

		CELL_PrintPreview(sys);



		char ctrl = getch();

		switch (ctrl)
		{
		// Note: Cursor pos is "enforced" in the next iteration
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
			if (POINT_IsZero(sys->Scope) &&
				sys->isWhiteTurn &&
				islower(sys->Board[POINT_getIndex(sys->Cursor)]))
				break;
			if (POINT_IsZero(sys->Scope) &&
				!sys->isWhiteTurn &&
				isupper(sys->Board[POINT_getIndex(sys->Cursor)]))
				break;

			// if no piece is selected, select one
			if (POINT_IsZero(sys->Scope) && sys->Board[index] != ' ')
			{
				sys->Scope = sys->Cursor;

				CELL_ClearPreview(sys);

				Point *mov = malloc(64 * sizeof(Point));

				//get all absolute positions possible for the currently playing
				PIECE_getAllMovesAbsolute(sys, mov, sys->Scope, 1, sys->isWhiteTurn);

				CELL_AddToPreview(sys->Scope, sys, 1);

				for (size_t i = 0; i < 64; i++)
				{
					if (POINT_IsZero(mov[i]))
						break;

					PIECE_isThreat(sys->Board[POINT_getIndex(mov[i])], sys->isWhiteTurn) ? CELL_AddToPreview(mov[i], sys, 3) : CELL_AddToPreview(mov[i], sys, 2);
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
			else if (!POINT_IsZero(sys->Scope) && !POINT_equals(sys->Scope, sys->Cursor) && POINT_isPointInArray(sys, sys->Cursor, sys->MarkedCellsCounter, sys->markedPos))
			{
				int ScopeIndex = POINT_getIndex(sys->Scope);
				sys->Board[index] = sys->Board[ScopeIndex];
				sys->Board[ScopeIndex] = ' ';

				sys->Scope = POINT(0, 0);
				CELL_ClearPreview(sys);

				sys->isWhiteTurn = !sys->isWhiteTurn;
			}
			break;
		}
		// printf("\033[H\033[J");//vllt optimieren anstatt console clearen einfach array �berschreiben
		ENGINE_SetCursorPos(POINT(0, 0));
	}
	// return 0;
}