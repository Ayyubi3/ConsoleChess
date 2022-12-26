#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>

#include "structs.h"

#include "EngineFuncs.h"


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)





int BOARD_ReadFEN(char* FEN, Game* sys);

void PIECE_getChessPieceMoves(char piece, Point* buffer);
int PIECE_getRecursivness(char piece);
int PIECE_getChessPieceValue(char piece);


void BOARD_Print(Colors colors, Game* sys);
void BOARD_MoveCursorLocal(Point d, Game* sys);

void CELL_PrintPreview(Game* sys);
void CELL_ClearPreview(Game* sys);
void CELL_AddToPreview(Point coords, Game* sys);



int BOARD_ReadFEN(char* FEN, Game* sys)
{
	//TODO: Process extended FEN string (The part after the long string with w-- etc)
	int index = 0;
	for (int i = 0; FEN[i] != '\0'; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			//ERROR: Do i need use malloc here again, if yes how? 
			sys->Board[index] =
				(Piece)
			{
				.name = FEN[i],
				.value = PIECE_getRecursivness(FEN[i]),
				.isRecursive = PIECE_getRecursivness(FEN[i]),
				.hasMoved = 0
			};

			PIECE_getChessPieceMoves(FEN[i], &sys->Board[index].possibleMoves);
			index++;
		}

		else if ('0' <= FEN[i] && FEN[i] <= '9')
		{
			index += (FEN[i] - 48);
		}
	}
}




//************************************************************************************************
//PIECE

void PIECE_getChessPieceMoves(char piece, Point* buffer)
{

	Point moves[50];
	for (size_t i = 0; i < 50; i++)
	{
		moves[i] = (Point){ 0, 0 };
	}

	(('a' <= piece) && (piece <= 'z')) ? piece -= ('a' - 'A') : (piece = piece);

	switch (piece)
	{
	//case 'R':
	//	moves[0] = 1;
	//	moves[1] = -1;
	//	moves[2] = 8;
	//	moves[3] = -8;
	//	break;
	//case 'B':
	//	moves[0] = 7;
	//	moves[1] = -7;
	//	moves[2] = 9;
	//	moves[3] = -9;
	//	break;
	//case 'Q':
	//	moves[0] = 7;
	//	moves[1] = -7;
	//	moves[2] = 9;
	//	moves[3] = -9;
	//	moves[4] = 1;
	//	moves[5] = -1;
	//	moves[6] = 8;
	//	moves[7] = -8;
	//	break;
	//case 'K':
	//	moves[0] = 7;
	//	moves[1] = -7;
	//	moves[2] = 9;
	//	moves[3] = -9;
	//	moves[4] = 1;
	//	moves[5] = -1;
	//	moves[6] = 8;
	//	moves[7] = -8;
	//	break;
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
	memcpy(buffer, moves, sizeof(Point) * 50);
}

int PIECE_getRecursivness(char piece)
{
	(('a' <= piece) && (piece <= 'z')) ? piece -= ('a' - 'A') : (piece = piece);

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
	(('a' <= piece) && (piece <= 'z')) ? piece -= ('a' - 'A') : (piece = piece);//toupper

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
//*******************************************************************************************
//BOARD

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




}


void CELL_AddToPreview(Point coords, Game* sys)
{
	int index = (coords.y - 1) * 8 + (coords.x - 1);
	if(sys->Board[index].name == ' ')
		sys->markedPos[sys->MarkedCellsCounter++] = coords;

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

		ENGINE_SetBackgroundColor(233, 233, 233);

		int index = (sys->Cursor.y-1) * 8 + (sys->Cursor.x - 1);
		printf("%c", sys->Board[index].name);
		ENGINE_SetBackgroundColor(0, 0, 0);

		ENGINE_SetCursorPos(sys->Cursor);

	}
}





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





	//clear everything before start, just because
	for (size_t i = 0; i < 64; i++)
	{
		sys->Board[i] = (Piece){ ' ', .possibleMoves = 0, .value = 0 };
		sys->markedPos[i] = (Point){ .x = 0, .y = 0 };
	}
	CELL_ClearPreview(sys);



	BOARD_ReadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", sys);



	while (1)
	{
		int index = (sys->Cursor.x - 1) + (sys->Cursor.y - 1) * 8;

		BOARD_Print(SysCol, sys);

		ENGINE_SetCursorPos(sys->Cursor);

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
			case '\r'://if enter just show every possible(Except knight and pawn everything is missing)

				CELL_ClearPreview(sys);

				if (sys->Board[index].name == ' ')
					break;
				
				for (size_t i = 0; i < 50; i++) 
				{

					if (sys->Board[index].possibleMoves[i].x == 0 && sys->Board[index].possibleMoves[i].y == 0)
						break;


					if (sys->Board[index].isRecursive == 0)
					{

						Point markedPos = {0, 0};

						//If black turn negate possible moveS
						int multi = sys->isWhiteTurn ? 1 : -1;
							markedPos = POINT_Add((Point) { 
								multi * (sys->Board[index].possibleMoves[i].x), 
									multi * sys->Board[index].possibleMoves[i].y }, 
								sys->Cursor);


						if (markedPos.x < 1 || markedPos.x > 8 || markedPos.y < 1 || markedPos.y > 8)
							continue;

						CELL_AddToPreview(markedPos, sys);

					}
					else if (sys->Board[index].isRecursive == 1)
					{

					}

				}
				
				break;
			default:
				break;
		}

		printf("\033[H\033[J");//vllt optimieren anstatt console clearen einfach array überschreiben


	}

	return 0;
}

