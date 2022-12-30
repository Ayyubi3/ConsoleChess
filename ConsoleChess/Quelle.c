#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//Dont know what this is, but prevents warnings to stop compilation
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	char name;
	int hasMoved;//Pawn can only move if it is on the starting cell and has to free cells in front of it
} Piece;

typedef struct {

	int DBoxColorR, DBoxColorG, DBoxColorB;
	int LBoxColorR, LBoxColorG, LBoxColorB;

	int DPieceColorR, DPieceColorG, DPieceColorB;
	int LPieceColorR, LPieceColorG, LPieceColorB;

} Colors;

typedef struct {

	Piece Board[64];//64-bit integer instead of Piece Array//Use char instead of int (one byte insted of 4)

	Point Cursor;

	Point Scope;

	int isWhiteTurn;

	Point markedPos[64];

	int MarkedCellsCounter;

	int markedPosCol[64];
} Game;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ENGINE_SetBackgroundColor(int R, int G, int B)
{ printf("\x1b[48;2;%i;%i;%im", R, G, B); }

void ENGINE_SetForegroundColor(int R, int G, int B)
{ printf("\x1b[38;2;%i;%i;%im", R, G, B); }

void ENGINE_SetCursorPos(Point coords)
{ printf("\033[%d;%dH", coords.y, coords.x); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Point POINT(int x, int y)
{ return (Point) { x, y }; }

Point POINT_Add(Point point1, Point point2)
{ return (Point) { point1.x + point2.x, point1.y + point2.y }; }

int POINT_IsZero(Point point)
{ return (point.x == 0 && point.y == 0); }

int POINT_equals(Point point1, Point point2)
{ return (point1.x == point2.x) && (point1.y == point2.y); }

int POINT_getIndex(Point p)
{ return (p.x - 1) + (p.y - 1) * 8; }

int POINT_isPointInArray(Game* sys, Point p, int length, Point* arr)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BOARD_ReadFEN(char* FEN, Game* sys);

void PIECE_getChessPieceMoves(char piece, Point* buffer);
int PIECE_getChessPieceValue(char piece);

void BOARD_Print(Colors* colors, Game* sys);
void BOARD_MoveCursorLocal(Point d, Game* sys);

void CELL_PrintPreview(Game* sys);
void CELL_ClearPreview(Game* sys);
void CELL_AddToPreview(Point coords, Game* sys, int color);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int isThreat(Game* sys, char piece)
{
	if (sys->isWhiteTurn)
		return (piece >= 'a' && piece <= 'z'); 
	else if (!sys->isWhiteTurn)
		return (piece >= 'A' && piece <= 'Z');
}

int BOARD_ReadFEN(char* FEN, Game* sys)
{
	//TODO: Process extended FEN string (The part after the long string with w-- etc)
	int index = 0, i;
	for (i = 0; FEN[i] != '\0' && FEN[i] != ' '; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			sys->Board[index] =
				(Piece)
			{
				.name = FEN[i],
				.hasMoved = 0
			};
			index++;
		}
		else if ('0' <= FEN[i] && FEN[i] <= '9')
		{
			index += (FEN[i] - 48);
		}
	}

	sys->isWhiteTurn = FEN[++i] == 'w';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PIECE_getChessPieceMoves(char piece, Point* buffer)
{

	Point moves[8];
	for (size_t i = 0; i < 8; i++)
		moves[i] = POINT( 0, 0);

	switch (toupper(piece))
	{
		case 'R':
			moves[0] = POINT( 0, -1 );
			moves[1] = POINT( 1, 0 );
			moves[2] = POINT( -1, 0 );
			moves[3] = POINT( 0, 1 );
			break;
		case 'B':
			moves[0] = POINT(1, 1);
			moves[1] = POINT(-1, -1);
			moves[2] = POINT(-1, 1);
			moves[3] = POINT(1, -1);
			break;
		case 'Q':
			moves[0] = POINT( 1, 1 );
			moves[1] = POINT( -1, -1 );
			moves[2] = POINT( -1, 1 );
			moves[3] = POINT( 1, -1 );
			moves[4] = POINT( 1, 0 );
			moves[5] = POINT( -1, 0 );
			moves[6] = POINT( 0, 1 );
			moves[7] = POINT( 0, -1 );
			break;
		case 'K':
			moves[0] = POINT( 0, 1 );
			moves[1] = POINT( 1, 1 );
			moves[2] = POINT( 1, 0 );
			moves[3] = POINT( 1, -1 );
			moves[4] = POINT( 0, -1 );
			moves[5] = POINT( -1, -1 );
			moves[6] = POINT( -1, 0 );
			moves[7] = POINT( -1, 1 );
			break;
		case 'P':
			moves[0] = POINT(0, -1);
			moves[1] = POINT(0, -2);
			break;
		case 'N':
			moves[0] = POINT( 2, -1);
			moves[1] = POINT( 2, 1);
			moves[2] = POINT( -2, 1);
			moves[3] = POINT( -2, -1);
			moves[4] = POINT( 1, 2);
			moves[5] = POINT( -1, 2);
			moves[6] = POINT( -1, -2);
			moves[7] = POINT( 1, -2);
			break;
	}
	memcpy(buffer, moves, sizeof(Point) * 8);
}

int PIECE_getChessPieceValue(char piece)
{
	switch (toupper(piece))
	{
		case 'P': return 1; break;
		case 'N': return 3; break;
		case 'B': return 3; break;
		case 'R': return 5; break;
		case 'Q': return 9; break;
	}
}

void PIECE_getAllLegalMoves(Game* sys, Point* Buffer, Point piece)
{
	Point possibleMoves[50];
	int pMCounter = 0;

	int indexOfPiece = POINT_getIndex(piece);
	char inpt = toupper(sys->Board[indexOfPiece].name);

	for (size_t i = 0; i < 50; i++) 
		possibleMoves[i] = POINT( 0, 0 ); 

	if (inpt == 'N' || inpt == 'K' || inpt == 'P')
	{

		Point Moves[8];
		PIECE_getChessPieceMoves(inpt, &Moves);
		
		for (size_t i = 0; i < 8; i++)
		{


			if (inpt == 'P')//Pawn has some special rules
			{
				//Invert Move for Pawn(Other Pieces are independet of whos turn it is)
				sys->isWhiteTurn ? (Moves[i].y = Moves[i].y) : (Moves[i].y = -Moves[i].y);

				//Pawn: If already moved dont add +2 movement
				if (sys->Board[indexOfPiece].hasMoved && i == 1)
					continue;
			}

			//Calculate next cell
			int x = piece.x + Moves[i].x;
			int y = piece.y + Moves[i].y;

			if (!(x >= 1 && x <= 8) || !(y >= 1 && y <= 8))//if out of bound: kill iteration
				continue;
			
			if (sys->Board[POINT_getIndex(POINT( x, y ))].name == ' ') //If cell is empty move should be safe todo. NOTE: might cause problems with king later
				possibleMoves[pMCounter++] = POINT( x, y ); 
			else if (isThreat(sys, sys->Board[POINT_getIndex(POINT(x, y ))].name) && inpt != 'P')//If Cell is threat mark it. NOTE: maybe change that later so the dangered pieces are in a different array
				possibleMoves[pMCounter++] = POINT( x, y ); 

			if (inpt != 'P') continue;

			//Pawns movement
			int offsetMultiplier;
			sys->isWhiteTurn ? (offsetMultiplier = -1) : (offsetMultiplier = 1);

			Point offset1 = POINT( 1, offsetMultiplier );
			Point offset2 = POINT( -1, offsetMultiplier );

			Point p1Point = POINT_Add(piece, offset1);
			Point p2Point = POINT_Add(piece, offset2);

			Piece p1 = sys->Board[POINT_getIndex(p1Point)];
			Piece p2 = sys->Board[POINT_getIndex(p2Point)];

			if (p1.name != ' ' && ((p1Point.x >= 1 && p1Point.x <= 8) && (p1Point.y >= 1 && p1Point.y <= 8)))
				possibleMoves[pMCounter++] = p1Point;

			if (p2.name != ' ' && ((p2Point.x >= 1 && p2Point.x <= 8) && (p2Point.y >= 1 && p2Point.y <= 8)))
				possibleMoves[pMCounter++] = p2Point;
		}
	} 
	else 
	if (inpt == 'R' || inpt == 'Q' || inpt == 'B')
		{

			Point Moves[8];
			PIECE_getChessPieceMoves(inpt, &Moves);

			for (size_t i = 0; i < 8; i++)
			{

				int x = piece.x + Moves[i].x;
				int y = piece.y + Moves[i].y;

				while (sys->Board[POINT_getIndex(POINT(x, y ))].name == ' ')
				{
					if (!(x >= 1 && x <= 8) || !(y >= 1 && y <= 8))
						break;

					possibleMoves[pMCounter++] = POINT(x, y );

					x = x + Moves[i].x;
					y = y + Moves[i].y;

				}

				//If next Cell is not out of Bounds and is Enemy Piece: Add to list
				if ((x >= 1 && x <= 8) && (y >= 1 && y <= 8) && isThreat(sys, sys->Board[POINT_getIndex(POINT(x, y ))].name))
					possibleMoves[pMCounter++] = POINT(x, y );
			}
		}
	



	memcpy(Buffer, possibleMoves, sizeof(Point) * 50);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BOARD_Print(Colors* colors, Game* sys)
{

	ENGINE_SetBackgroundColor(0, 0, 0); 

	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{

			int index = 8 * i + j;
			//Draw Chess Pattern
			//Modulo-part offsets every row by one
			((8 * i + j) + (i % 2)) % 2 == 0 ?
				ENGINE_SetBackgroundColor(colors->DBoxColorR, colors->DBoxColorB, colors->DBoxColorB) :
				ENGINE_SetBackgroundColor(colors->LBoxColorR, colors->LBoxColorB, colors->LBoxColorB);

			//Color Pieces
			(islower(sys->Board[index].name)) ?
				ENGINE_SetForegroundColor(colors->DPieceColorR, colors->DPieceColorB, colors->DPieceColorB) :
				ENGINE_SetForegroundColor(colors->LPieceColorR, colors->LPieceColorB, colors->LPieceColorB);

			printf("%c", sys->Board[index].name);

			//Reset Colors
			ENGINE_SetForegroundColor(255, 255, 255);
			ENGINE_SetBackgroundColor(0, 0, 0);

		}

		printf("\n");

	}





}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CELL_AddToPreview(Point coords, Game* sys, int color)
{
	sys->markedPos[sys->MarkedCellsCounter] = coords;
	sys->markedPosCol[sys->MarkedCellsCounter++] = color;
}

void CELL_ClearPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		sys->markedPos[i] = POINT(0, 0 );
	}
	sys->MarkedCellsCounter = 0;
}

void CELL_PrintPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		if (POINT_IsZero(sys->markedPos[i]))
			return;

		ENGINE_SetCursorPos(sys->markedPos[i]);

		switch (sys->markedPosCol[i])
		{
			case 0: ENGINE_SetBackgroundColor(44, 44, 233); break;		
			case 1: ENGINE_SetBackgroundColor(0, 255, 0); break;		
			case 2: ENGINE_SetBackgroundColor(0, 0, 255); break;
		}

		int index = (sys->Scope.y-1) * 8 + (sys->Scope.x - 1);
		printf("%c", sys->Board[index].name);
		ENGINE_SetBackgroundColor(0, 0, 0);

		ENGINE_SetCursorPos(sys->Cursor);

	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



int main()
{
	Game* sys = malloc(sizeof(Game));
	Colors* SysCol = malloc(sizeof(Colors));

	//Configure Game Colors
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

	//Configure Game Settings
	sys->Cursor = POINT( 1, 1 );

	//clear everything before start, just because
	for (size_t i = 0; i < 64; i++)
	{
		sys->Board[i] = (Piece){ ' '};
		sys->markedPos[i] = POINT( 0, 0 ); 
	}

	CELL_ClearPreview(sys);
	sys->Scope = POINT( 0, 0 );

	BOARD_ReadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w", sys);
	//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

	while (1)
	{
		int index = (sys->Cursor.x - 1) + (sys->Cursor.y - 1) * 8;

		BOARD_Print(SysCol, sys);

		ENGINE_SetCursorPos(sys->Cursor);

		CELL_PrintPreview(sys);

		//Point ss[122];
		//int co = 0;
		//for (size_t i = 1; i < 8; i++)
		//	for (size_t j = 1; j < 8; j++)
		//	{
		//		if (!isThreat(sys, sys->Board[POINT_getIndex(POINT( i, j ))].name)
		//				&& sys->Board[POINT_getIndex(POINT( i, j ))].name != ' ')
		//			{
		//				

		//			Point arr[50];
		//			PIECE_getAllLegalMoves(sys, &arr, POINT( i, j ));

		//			for (size_t d = 0; d < 50; d++)
		//			{
		//				if (POINT_IsZero(arr[d]))
		//				{
		//					break;
		//				}
		//				ss[co++] = arr[d];
		//			}
		//		} 
		//	}

		char ctrl = getch();

		switch (ctrl)			//outside the wwire
		{
			//Note: Cursor pos is "enforced" in the next iteration
			case 'w':
				if (sys->Cursor.y > 1) sys->Cursor.y--;
				break;
			case 'a':
				if (sys->Cursor.x > 1) sys->Cursor.x--;
				break;
			case 's':
				if (sys->Cursor.y < 8) sys->Cursor.y++;
				break;
			case 'd':
				if (sys->Cursor.x < 8) sys->Cursor.x++;
				break;
			case '\r':

				//Cant click on opponents piece if scope is zero
				if (POINT_IsZero(sys->Scope) &&
					sys->isWhiteTurn &&
					islower(sys->Board[POINT_getIndex(sys->Cursor)].name))
					break;
				if (POINT_IsZero(sys->Scope) &&
					!sys->isWhiteTurn &&
					isupper(sys->Board[POINT_getIndex(sys->Cursor)].name))
					break;


				//if no piece is selected, select one
				if (POINT_IsZero(sys->Scope) && sys->Board[index].name != ' ')
				{
					sys->Scope = sys->Cursor;

					CELL_ClearPreview(sys);
									
					Point* mov = malloc(50 * sizeof(Point));

					PIECE_getAllLegalMoves(sys, mov, sys->Scope);

					CELL_AddToPreview(sys->Scope, sys, 1);

					for (size_t i = 0; i < 50; i++)
					{
						if (POINT_IsZero(mov[i])) break;

						if (isThreat(sys, sys->Board[POINT_getIndex(mov[i])].name))
							CELL_AddToPreview(mov[i], sys, 3);
						else
							CELL_AddToPreview(mov[i], sys, 2); 
					}

					free(mov);

				}
				//if its the same piece selected prior, unselect it
				else if (POINT_equals(sys->Scope, sys->Cursor))
				{
					sys->Scope = POINT(0, 0 );
					CELL_ClearPreview(sys);
				} 
				//If Piece is selected, not same as Scope and in markedCells move and change turns
				else if(!POINT_IsZero(sys->Scope) && !POINT_equals(sys->Scope, sys->Cursor) && POINT_isPointInArray(sys, sys->Scope,sys->MarkedCellsCounter, sys->markedPos))
				{
					int ScopeIndex = POINT_getIndex(sys->Scope);
					sys->Board[ScopeIndex].hasMoved = 1;
					sys->Board[index] = sys->Board[ScopeIndex];
					sys->Board[ScopeIndex] = (Piece){ ' ' };

					sys->Scope = POINT( 0, 0 );
					CELL_ClearPreview(sys);

					sys->isWhiteTurn = !sys->isWhiteTurn;
				}			
				break;
		}
		//printf("\033[H\033[J");//vllt optimieren anstatt console clearen einfach array überschreiben
		ENGINE_SetCursorPos(POINT(0, 0));
	}
	//return 0;
}