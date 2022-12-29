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
	int value;
	int isSliding;
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

	int isWhiteTurn;

	Point markedPos[64];

	int MarkedCellsCounter;

	int markedPosCol[64];

	Point Scope;

	char* msg[64];
	int msgCounter;

} Game;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ENGINE_SetBackgroundColor(int R, int G, int B);
void ENGINE_SetForegroundColor(int R, int G, int B);
void ENGINE_SetCursorPos(Point coords);
void ENGINE_WriteCharToXy(Point old, Point target, char ch, int returnToOldPos);




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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Point POINT_Add(Point point1, Point point2);
int POINT_IsZero(Point point);
int POINT_equals(Point point1, Point point2);
int POINT_getIndex(Point p);


Point POINT_Add(Point point1, Point point2)
{

	return (Point) { point1.x + point2.x, point1.y + point2.y };
}

int POINT_IsZero(Point point)
{
	if (point.x == 0 && point.y == 0)
	{
		return 1;
	}
	return 0;
}

int POINT_equals(Point point1, Point point2)
{
	if (point1.x != point2.x)
		return 0;
	if (point1.y != point2.y)
		return 0;

	return 1;

}

int POINT_getIndex(Point p)
{
	return (p.x - 1) + (p.y - 1) * 8;
}

void log(char* msg, int severity, Game* sys)
{
	sys->msg[sys->msgCounter++] = msg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int BOARD_ReadFEN(char* FEN, Game* sys);

void PIECE_getChessPieceMoves(char piece, Point* buffer);
int PIECE_getSliding(char piece);
int PIECE_getChessPieceValue(char piece);


void BOARD_Print(Colors* colors, Game* sys);
void BOARD_MoveCursorLocal(Point d, Game* sys);

void CELL_PrintPreview(Game* sys);
void CELL_ClearPreview(Game* sys);
void CELL_AddToPreview(Point coords, Game* sys, int color);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int PointisInArray(Game* sys)
{
	for (size_t i = 0; i < sys->MarkedCellsCounter-1; i++)
	{
		if (POINT_equals(sys->Scope, sys->markedPos[i]))
		{
			return 1;
		}
	}
	return 0;
}


int isThreat(Game* sys, char piece)
{
	if (sys->isWhiteTurn)
	{
		if (piece >= 'a' && piece <= 'z')
			return 1;
		return 0;
	} else if (!sys->isWhiteTurn)
	{
		if (piece >= 'A' && piece <= 'Z')
			return 1;
		return 0;
	}
}


int BOARD_ReadFEN(char* FEN, Game* sys)
{
	//TODO: Process extended FEN string (The part after the long string with w-- etc)
	int index = 0;
	for (int i = 0; FEN[i] != '\0'; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			sys->Board[index] =
				(Piece)
			{
				.name = FEN[i],
				.value = PIECE_getChessPieceValue(FEN[i]),
				.isSliding = PIECE_getSliding(FEN[i]),
				.hasMoved = 0
			};
			index++;
		}

		else if ('0' <= FEN[i] && FEN[i] <= '9')
		{
			index += (FEN[i] - 48);
		}
	}
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void PIECE_getChessPieceMoves(char piece, Point* buffer)
{

	Point moves[8];
	for (size_t i = 0; i < 8; i++)
	{
		moves[i] = (Point){ 0, 0 };
	}

	piece = toupper(piece);

	switch (piece)
	{
	case 'R':
		moves[0] = (Point){ 0, -1 };
		moves[1] = (Point){ 1, 0 };
		moves[2] = (Point){ -1, 0 };
		moves[3] = (Point){ 0, 1 };
		break;
	case 'B':
		moves[0] = (Point){1, 1};
		moves[1] = (Point){-1, -1};
		moves[2] = (Point){-1, 1};
		moves[3] = (Point){1, -1};
		break;
	case 'Q':
		moves[0] = (Point){ 1, 1 };
		moves[1] = (Point){ -1, -1 };
		moves[2] = (Point){ -1, 1 };
		moves[3] = (Point){ 1, -1 };
		moves[4] = (Point){ 1, 0 };
		moves[5] = (Point){ -1, 0 };
		moves[6] = (Point){ 0, 1 };
		moves[7] = (Point){ 0, -1 };
		break;
	case 'K':
		moves[0] = (Point){ 0, 1 };
		moves[1] = (Point){ 1, 1 };
		moves[2] = (Point){ 1, 0 };
		moves[3] = (Point){ 1, -1 };
		moves[4] = (Point){ 0, -1 };
		moves[5] = (Point){ -1, -1 };
		moves[6] = (Point){ -1, 0 };
		moves[7] = (Point){ -1, 1 };
		break;
	case 'P':
		moves[0] = (Point){0, -1};
		moves[1] = (Point){0, -2};
		break;
	case 'N':
		moves[0] = (Point){ 2, -1};
		moves[1] = (Point){ 2, 1};
		moves[2] = (Point){ -2, 1};
		moves[3] = (Point){ -2, -1};
		moves[4] = (Point){ 1, 2};
		moves[5] = (Point){ -1, 2};
		moves[6] = (Point){ -1, -2};
		moves[7] = (Point){ 1, -2};

		break;
	default:
		break;
	}
	memcpy(buffer, moves, sizeof(Point) * 8);
}

int PIECE_getSliding(char piece)
{
	piece = toupper(piece);

	switch (piece)
	{
	case 'P':
		return 0;
		break;

	case 'N':
		return 0;
		break;

	case 'B':
		return 1;
		break;

	case 'R':
		return 1;
		break;


	case 'Q':
		return 1;
		break;

	case 'K':
		return 0;
		break;
	default:
		break;
	}
}




int PIECE_getChessPieceValue(char piece)
{
	piece = toupper(piece);

	switch (piece)
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
	default:
		break;
	}
}



void PIECE_getAllLegalMoves(Game* sys, Point* Buffer)
{
	Point ps[50];
	int psCounter = 0;

	char inpt = toupper(sys->Board[POINT_getIndex(sys->Scope)].name);

	for (size_t i = 0; i < 50; i++)
	{
		ps[i] = (Point){ 0, 0 };
	}

	if (inpt == 'N')
	{

		Point Moves[8];
		PIECE_getChessPieceMoves('N', &Moves);

		for (size_t i = 0; i < 8; i++)
		{
			int x = sys->Scope.x + Moves[i].x;
			int y = sys->Scope.y + Moves[i].y;
			if (x >= 1 && x <= 8)
				if (y >= 1 && y <= 8)
				{
					ps[psCounter++] = (Point){ x, y };

				}
		}
	}
	else 
	if (inpt ==  'P')
	{
		Point Moves[8];
		PIECE_getChessPieceMoves('P', &Moves);

		for (size_t i = 0; i < 8; i++)
		{
			//if piece has moved delete second element
			if (sys->Board[POINT_getIndex(sys->Scope)].hasMoved && i == 1) continue;

			sys->isWhiteTurn ? (Moves[i].y = Moves[i].y) : (Moves[i].y = -Moves[i].y);

			int x = sys->Scope.x + Moves[i].x;
			int y = sys->Scope.y + Moves[i].y;
			if (x >= 1 && x <= 8)
				if (y >= 1 && y <= 8)
				{
					ps[psCounter++] = (Point){ x, y };

				}
		}
	}
	else
	if (inpt == 'K')
	{

		Point Moves[8];
		PIECE_getChessPieceMoves('K', &Moves);

		for (size_t i = 0; i < 8; i++)
		{
			int x = sys->Scope.x + Moves[i].x;
			int y = sys->Scope.y + Moves[i].y;

			if ((x >= 1 && x <= 8) && (y >= 1 && y <= 8))
			{
				ps[psCounter++] = (Point){ x, y };

			}
		}
	}
	else
		if (inpt == 'R' || inpt == 'Q' || inpt == 'B')
		{

			Point movesa[8];
			PIECE_getChessPieceMoves(inpt, &movesa);

			for (size_t i = 0; i < 8; i++)
			{

				int x = sys->Scope.x + movesa[i].x;
				int y = sys->Scope.y + movesa[i].y;

				while (sys->Board[POINT_getIndex((Point) { x, y })].name == ' ')
				{
					if (!(x >= 1 && x <= 8) || !(y >= 1 && y <= 8))
						break;


					ps[psCounter++] = (Point){ x, y };

					x = x + movesa[i].x;
					y = y + movesa[i].y;

				}
				if (sys->Board[POINT_getIndex((Point) { x, y })].name != ' ' &&
					(x >= 1 && x <= 8) && (y >= 1 && y <= 8) &&
					isThreat(sys, sys->Board[POINT_getIndex((Point) { x, y })].name)
					)
				{
					ps[psCounter++] = (Point){ x, y };
				}
			}
		}
	



	memcpy(Buffer, ps, sizeof(Point) * 50);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void BOARD_MoveCursorLocal(Point d, Game* sys)
{
	if (d.x == -1 && sys->Cursor.x > 1)//left
	{ sys->Cursor.x--; }
	else if (d.x == 1 && sys->Cursor.x < 8)//right
	{ sys->Cursor.x++; }
	else if (d.y == 1 && sys->Cursor.y > 1)//up
	{ sys->Cursor.y--; }
	else if (d.y == -1 && sys->Cursor.y < 8)//down
	{ sys->Cursor.y++; }
}

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
			('a' <= sys->Board[index].name && sys->Board[index].name <= 'z') ?
				ENGINE_SetForegroundColor(colors->DPieceColorR, colors->DPieceColorB, colors->DPieceColorB) :
				ENGINE_SetForegroundColor(colors->LPieceColorR, colors->LPieceColorB, colors->LPieceColorB);



			printf("%c", sys->Board[index].name);

			//Reset Colors
			ENGINE_SetForegroundColor(255, 255, 255);
			ENGINE_SetBackgroundColor(0, 0, 0);

		}

		printf("\n");

	}

	for (size_t i = 0; i < sys->msgCounter; i++)
	{
		printf("%s\n", sys->msg[i]);
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
		sys->markedPos[i] = (Point){ .x = -1, .y = -1 };
	}
	sys->MarkedCellsCounter = 0;
}

void CELL_PrintPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		if (sys->markedPos[i].x == -1 || sys->markedPos[i].y == -1)
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
		default:
			break;
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
	sys->Cursor = (Point){ .x = 1, .y = 1 };
	sys->isWhiteTurn = 1; // Read that later of FEN string
	sys->MarkedCellsCounter = 0;

	sys->msgCounter = 0;

	//clear everything before start, just because
	for (size_t i = 0; i < 64; i++)
	{
		sys->Board[i] = (Piece){ ' ', .value = 0 };
		sys->markedPos[i] = (Point){ .x = 0, .y = 0 };
		sys->msg[i] = 0;
	}
	CELL_ClearPreview(sys);
	sys->Scope = (Point){ 0, 0 };

	BOARD_ReadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", sys);
	//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

	while (1)
	{
		int index = (sys->Cursor.x - 1) + (sys->Cursor.y - 1) * 8;

		BOARD_Print(SysCol, sys);

		ENGINE_SetCursorPos(sys->Cursor);

		for (size_t i = 0; i < 64; i++)
		{
			
		}

		CELL_PrintPreview(sys);

		char ctrl = getch();

		switch (ctrl)
		{
			//Note: Cursor pos is "enforced" in the next iteration
			case 'w':
				BOARD_MoveCursorLocal((Point){ 0, 1 }, sys);
				break;
			case 'a':
				BOARD_MoveCursorLocal((Point) { -1, 0 }, sys);
				break;
			case 's':
				BOARD_MoveCursorLocal((Point) { 0, -1 }, sys);
				break;
			case 'd':
				BOARD_MoveCursorLocal((Point) { 1, 0 }, sys);
				break;
			case '\r':
				////Cant click on opponents piece if scope is zero
				//if (POINT_IsZero(sys->Scope) &&
				//	sys->isWhiteTurn &&
				//	islower(sys->Board[POINT_getIndex(sys->Cursor)].name))
				//{
				//	log("White cant move Black Pieces! White is in turn", 1, sys);  break;
				//}
				//if (POINT_IsZero(sys->Scope) &&
				//	!sys->isWhiteTurn &&
				//	isupper(sys->Board[POINT_getIndex(sys->Cursor)].name))
				//{
				//	log("Black cant move White Pieces! Black is in turn", 1, sys);  break;
				//}
				//if no piece is selected, select one
				if (POINT_IsZero(sys->Scope) && sys->Board[index].name != ' ')
				{
					
					sys->Scope = sys->Cursor;

					CELL_ClearPreview(sys);
					
					
					Point* mov = malloc(50 * sizeof(Point));

					PIECE_getAllLegalMoves(sys, mov);

					CELL_AddToPreview(sys->Scope, sys, 1);

					for (size_t i = 0; i < 50; i++)
					{
						if (POINT_IsZero(mov[i])) break;

						if (isThreat(sys, sys->Board[POINT_getIndex(mov[i])].name))
						{
							CELL_AddToPreview(mov[i], sys, 3);

						}else
							CELL_AddToPreview(mov[i], sys, 2); 
					}
					free(mov);

				}
				//if its the same piece selected prior, unselect it
				else if (POINT_equals(sys->Scope, sys->Cursor))
				{
					sys->Scope = (Point){ 0, 0 };
					CELL_ClearPreview(sys);
				} 
				else if(!POINT_IsZero(sys->Scope) && !POINT_equals(sys->Scope, sys->Cursor)/* && PointisInArray(sys)*/)//If Point is selected and not the same prior move piece
				{



					sys->Board[(sys->Scope.x - 1) + (sys->Scope.y - 1) * 8].hasMoved = 1;
					sys->Board[index] = sys->Board[(sys->Scope.x - 1) + (sys->Scope.y - 1) * 8];
					sys->Board[(sys->Scope.x - 1) + (sys->Scope.y - 1) * 8] = (Piece){ ' ', .value = 0 };

					sys->Scope = (Point){ 0, 0 };
					CELL_ClearPreview(sys);

					sys->isWhiteTurn = !sys->isWhiteTurn;
				}

				
				break;
			default:
				break;
		}

		printf("\033[H\033[J");//vllt optimieren anstatt console clearen einfach array überschreiben


	}

	return 0;
}

