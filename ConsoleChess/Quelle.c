#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "EngineFuncs.h"


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)



typedef struct {
	char name;
	int possibleMoves[50];
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

	int CursorX, CursorY;


	int LocalIndex;

	int isWhiteTurn;
	int ScopeX, ScopeY;

	int markedPosX[64];
	int markedPosY[64];
	int MarkedCellsCounter;



}Game;



int BOARD_ReadFEN(char* FEN, Game* sys);

void PIECE_getChessPieceMoves(char piece, int* buffer);
int PIECE_getRecursivness(char piece);
int PIECE_getChessPieceValue(char piece);


void BOARD_Print(Colors colors, Game* sys);
void BOARD_MoveCursorLocal(int dx, int dy, Game* sys);










void oneDimToTwoDim(int index, int* x, int* y)
{
	*x = (int)(index % 8) + 1;
	*y = (int)(index / 8) + 1;

}

int BOARD_ReadFEN(char* FEN, Game* sys)
{




	int index = 0;
	for (int i = 0; FEN[i] != '\0'; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			sys->Board[index] = (Piece){
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

void PIECE_getChessPieceMoves(char piece, int* buffer)
{

	int moves[50];
	for (size_t i = 0; i < 50; i++)
	{
		moves[i] = 0;
	}

	(('a' <= piece) && (piece <= 'z')) ? piece -= ('a' - 'A') : (piece = piece);

	switch (piece)
	{
	case 'R':
		moves[0] = 1;
		moves[1] = -1;
		moves[2] = 8;
		moves[3] = -8;
		break;
	case 'B':
		moves[0] = 7;
		moves[1] = -7;
		moves[2] = 9;
		moves[3] = -9;
		break;
	case 'Q':
		moves[0] = 7;
		moves[1] = -7;
		moves[2] = 9;
		moves[3] = -9;
		moves[4] = 1;
		moves[5] = -1;
		moves[6] = 8;
		moves[7] = -8;
		break;
	case 'K':
		moves[0] = 7;
		moves[1] = -7;
		moves[2] = 9;
		moves[3] = -9;
		moves[4] = 1;
		moves[5] = -1;
		moves[6] = 8;
		moves[7] = -8;
		break;
	case 'P':
		moves[0] = -16;
		moves[1] = -8;
		break;
	case 'N':

		moves[0] = -15;
		moves[1] = -17;
		moves[2] = -10;
		moves[3] = 6;
		moves[4] = 15;
		moves[5] = 17;
		moves[6] = -6;
		moves[7] = 10;

		break;
	default:
		break;
	}
	memcpy(buffer, moves, sizeof(int) * 50);
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

void BOARD_MoveCursorLocal(int dx, int dy, Game* sys)
{

	if (dx == -1 && sys->CursorX > 1)//left
	{ sys->CursorX--; }
	else if (dx == 1 && sys->CursorX < 8)//right
	{ sys->CursorX++; }
	else if (dy == 1 && sys->CursorY > 1)//up
	{ sys->CursorY--; }
	else if (dy == -1 && sys->CursorY < 8)//down
	{ sys->CursorY++; }

}





void BOARD_Print(Colors colors, Game* sys)
{
	//TODO: Erweiterte Fenstreing einlesen (wer dran ist und so)


	ENGINE_SetBackgroundColor(0, 0, 0); 


	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{

			int index = 8 * i + j;
			//Draw Chess Pattern
			//Modulo-part offsets every row by one
			((8 * i + j) + (i % 2)) % 2 == 0 ?
				ENGINE_SetBackgroundColor(colors.DBoxColorR, colors.DBoxColorB, colors.DBoxColorB) :
				ENGINE_SetBackgroundColor(colors.LBoxColorR, colors.LBoxColorB, colors.LBoxColorB);

			//Color Pieces
			('a' <= sys->Board[index].name && sys->Board[index].name <= 'z') ?
				ENGINE_SetForegroundColor(colors.DPieceColorR, colors.DPieceColorB, colors.DPieceColorB) :
				ENGINE_SetForegroundColor(colors.LPieceColorR, colors.LPieceColorB, colors.LPieceColorB);



			printf("%c", sys->Board[index].name);

			ENGINE_SetForegroundColor(255, 255, 255);
			ENGINE_SetBackgroundColor(0, 0, 0);

		}

		printf("\n");

	}




}


void CELL_AddToPreview(int x, int y, Game* sys)
{
	sys->markedPosX[sys->MarkedCellsCounter] = x;

	sys->markedPosY[sys->MarkedCellsCounter++] = y;

}


void CELL_ClearPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		sys->markedPosX[i] = -1;
		sys->markedPosY[i] = -1;
	}
	sys->MarkedCellsCounter = 0;
}

void CELL_PrintPreview(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		if (sys->markedPosX[i] == -1 || sys->markedPosY[i] == -1)
			return;
		


		ENGINE_SetCursorPos(sys->markedPosX[i], sys->markedPosY[i]);

		ENGINE_SetBackgroundColor(233, 233, 233);
		printf("%c", sys->Board[sys->CursorY * 8 + sys->CursorX].name);
		ENGINE_SetBackgroundColor(0, 0, 0);

		ENGINE_SetCursorPos(sys->CursorX, sys->CursorY);

	}
}





int main()
{
	//Configure Game Colors
	Colors SysCol = {
		.DBoxColorR = 136, .DBoxColorG = 119, .DBoxColorB = 183,
		.LBoxColorR = 233, .LBoxColorG = 4, .LBoxColorB = 33,

		.DPieceColorR = 0, .DPieceColorG = 0, .DPieceColorB = 0,
		.LPieceColorR = 255, .LPieceColorG = 255, .LPieceColorB = 255,
	};

	//Configure Game Settings
	Game sys = {

		
		.CursorX = 1,
		.CursorY = 1,


		.isWhiteTurn = 1,

		.MarkedCellsCounter = 0

	};



	for (size_t i = 0; i < 64; i++)
	{
		sys.Board[i] = (Piece){ .name = ' ', .possibleMoves = 0, .value = 0 };
		sys.markedPosX[i] = 0;
		sys.markedPosY[i] = 0;
	}


	BOARD_ReadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", &sys);

	CELL_ClearPreview(&sys);



	while (1)
	{
		int index = (sys.CursorX - 1) + (sys.CursorY - 1) * 8;

		BOARD_Print(SysCol, &sys);


		ENGINE_SetCursorPos(sys.CursorX, sys.CursorY);


		CELL_PrintPreview(&sys);

		char ctrl = getch();

		switch (ctrl)
		{
			//Note: Cursor pos is "enforced" in the next iteration
			case 'w':
				BOARD_MoveCursorLocal(0, 1, &sys);
				break;
			case 'a':
				BOARD_MoveCursorLocal(-1, 0, &sys);
				break;
			case 's':
				BOARD_MoveCursorLocal(0, -1, &sys);
				break;
			case 'd':
				BOARD_MoveCursorLocal(1, 0, &sys);
				break;
			case '\r':

				CELL_ClearPreview(&sys);

				if (sys.Board[index].name == ' ')
					break;
				
				for (size_t i = 0; i < 50; i++)
				{

					if (sys.Board[index].isRecursive == 0)
					{
						if (sys.Board[index].possibleMoves[i] == 0)
							break;

						int x, y;
						int markedPos = sys.Board[index].possibleMoves[i] + index;

						if (markedPos < 0 || markedPos > 63)
							continue;

						oneDimToTwoDim(markedPos, &x, &y);

						CELL_AddToPreview(x, y, &sys);
					}
					else if (sys.Board[index].isRecursive == 1)
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

