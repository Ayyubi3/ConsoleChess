#include <stdio.h>
#include <conio.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)





enum Pieces { KING, ROOK, BISHOP, QUEEN, KNIGHT, PAWN };
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
	int FrameColorR, FrameColorG, FrameColorB;

}Colors;


typedef struct {


	Piece Board[64];

	int CursorX, CursorY;
	int BoardX, BoardY;

	int isWhiteTurn;
	int ScopeX, ScopeY;

	int markedPosX[64];
	int markedPosY[64];
	int MarkedCellsCounter;



}Game;



void oneDimToTwoDim(int index, int* x, int* y)
{
	*x = (int)(index % 8);
	*y = (int)(index / 8);

}




void getChessPieceMoves(char piece, int* buffer)
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
		moves[0] = 1;
		moves[1] = -1;
		moves[2] = 8;
		moves[3] = -8;
		break;
	case 'K':
		moves[0] = 7;
		moves[1] = -7;
		moves[2] = 9;
		moves[3] = -9;
		moves[0] = 1;
		moves[1] = -1;
		moves[2] = 8;
		moves[3] = -8;
		break;
	case 'P':
		moves[0] = 16;
		moves[1] = 8;
		break;
	case 'N':

		moves[0] = -15;
		moves[1] = -17;
		moves[2] = -10;
		moves[3] = 6;
		moves[0] = 15;
		moves[1] = 17;
		moves[2] = -6;
		moves[3] = 10;

		break;
	default:
		break;
	}
	memcpy(buffer, moves, sizeof(int) * 50);
}

int getRecursiveness(char piece)
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


int getChessPieceValue(char piece)
{
	(('a' <= piece) && (piece <= 'z')) ? piece -= ('a' - 'A') : (piece = piece);

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

void MoveInBoard(int dx, int dy, Game* sys)
{

	if (dx == -1 && sys->BoardX > 0)//left
	{
		sys->CursorX--;
		sys->BoardX--;
	}
	else if (dx == 1 && sys->BoardX < 7)//right
	{
		sys->CursorX++;
		sys->BoardX++;
	}
	else if (dy == 1 && sys->BoardY > 0)//up
	{
		sys->CursorY--;
		sys->BoardY--;
	}
	else if (dy == -1 && sys->BoardY < 7)//down
	{
		sys->CursorY++;
		sys->BoardY++;
	}

}





void printboard(Colors colors, Game* sys)
{
	//TODO: Erweiterte Fenstreing einlesen (wer dran ist und so)
	printf("\x1b[48;2;%i;%i;%im", colors.FrameColorR, colors.FrameColorG, colors.FrameColorB);

	printf("XABCDEFGH\n");

	printf("\x1b[48;2;%i;%i;%im", 0, 0, 0);


	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 9; j++)
		{
			if (j == 0)
			{
				printf("\x1b[48;2;%i;%i;%im", colors.FrameColorR, colors.FrameColorG, colors.FrameColorB);

				printf("%c", i + '1');//Die Zahl links
				printf("\x1b[48;2;%i;%i;%im", 0, 0, 0);


			}
			else {

				int index = 8 * i + (j - 1);
				//Raster einfärben
				//Die erste Klammer rechnet den Index aus(die minus 1 bei j isi weil die Schleife bis 9 geht(wegen der zahl jede Reihe))
				//Der + Moduo teil ist verscheibt das Raster um eins jede Reihe;
				((8 * i + (j - 1)) + (i % 2)) % 2 == 0 ?
					printf("\x1b[48;2;%i;%i;%im", colors.DBoxColorR, colors.DBoxColorB, colors.DBoxColorB) :
					printf("\x1b[48;2;%i;%i;%im", colors.LBoxColorR, colors.LBoxColorB, colors.LBoxColorB);

				//Spieler einfärben
				('a' <= sys->Board[index].name && sys->Board[index].name <= 'z') ?
					printf("\x1b[38;2;%i;%i;%im", colors.DPieceColorR, colors.DPieceColorB, colors.DPieceColorB) :
					printf("\x1b[38;2;%i;%i;%im", colors.LPieceColorR, colors.LPieceColorB, colors.LPieceColorB);



				printf("%c", sys->Board[index].name);

				printf("\x1b[48;2;0;0;0m");
				printf("\x1b[38;2;255;255;255m");


			}
		}

		printf("\n");

	}




}

void addCellToColor(int x, int y, Game* sys)
{
	sys->markedPosX[sys->MarkedCellsCounter] = x;

	sys->markedPosY[sys->MarkedCellsCounter++] = y;

}


void clearMarkedCells(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		sys->markedPosX[i] = 0;
		sys->markedPosY[i] = 0;
	}
}

void displayMarkedCells(Game* sys)
{
	for (size_t i = 0; i < 64; i++)
	{
		if (sys->markedPosX[i] == 0|| sys->markedPosY[i] == 0)
			return;
		


		printf("\033[%d;%dH", sys->markedPosY[i] +2, sys->markedPosX[i] + 2);

		printf("\x1b[48;2;%i;%i;%im", 233, 233, 233);
		printf("%c", sys->Board[sys->BoardY * 8 + sys->BoardX].name);

		printf("\x1b[48;2;0;0;0m");

		printf("\033[%d;%dH", sys->CursorY, sys->CursorX);

	}
}


int ReadFenStringToBoard(char* FEN, Game* sys)
{




	int index = 0;
	for (int i = 0; FEN[i] != '\0'; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			sys->Board[index] = (Piece){
				.name = FEN[i],
				.value = getChessPieceValue(FEN[i]),
				.isRecursive = getRecursiveness(FEN[i]),
				.hasMoved = 0 
			};

			getChessPieceMoves(FEN[i], &sys->Board[index].possibleMoves);
			index++;
		}

		else if ('0' <= FEN[i] && FEN[i] <= '9')
		{
			index += (FEN[i] - 48);
		}
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

		.FrameColorR = 136, .FrameColorG = 119, .FrameColorB = 183
	};

	//Configure Game Settings
	Game sys = {

		//Pivot(most Top Left Cell)
		.CursorX = 2, .CursorY = 2,

		.BoardX = 0, .BoardY = 0,

		.isWhiteTurn = 1,

		.MarkedCellsCounter = 0

	};



	for (size_t i = 0; i < 64; i++)
	{
		sys.Board[i] = (Piece){ .name = ' ', .possibleMoves = 0, .value = 0 };
		sys.markedPosX[i] = 0;
		sys.markedPosY[i] = 0;
	}

	ReadFenStringToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", &sys);




	while (1)
	{

		printboard(SysCol, &sys);
		displayMarkedCells(&sys);

		printf("\033[%d;%dH", sys.CursorY, sys.CursorX);




		char ctrl = getch();

		switch (ctrl)
		{
		case 'w':
			MoveInBoard(0, 1, &sys);
			break;
		case 'a':
			MoveInBoard(-1, 0, &sys);
			break;
		case 's':
			MoveInBoard(0, -1, &sys);
			break;
		case 'd':
			MoveInBoard(1, 0, &sys);
			break;
		case '\r':
			clearMarkedCells(&sys);
			if (sys.Board[sys.BoardY * 8 + sys.BoardX].name != ' ' && sys.Board[sys.BoardY * 8 + sys.BoardX].name != '\0')
			{

				sys.ScopeX = sys.BoardX;
				sys.ScopeY = sys.BoardY;

				if (sys.Board[sys.BoardY * 8 + sys.BoardX].isRecursive == 0)
				{
					for (size_t i = 0; i < 50; i++)
					{

						int x, y;
					
						int currentIndex = (sys.BoardY * 8 + sys.BoardX);
						int move = sys.Board[sys.BoardY * 8 + sys.BoardX].possibleMoves[i];

						if (move == 0) continue;

					oneDimToTwoDim(currentIndex - move, &x, &y);
					addCellToColor(x, y, &sys);

					}
				}

			}
			break;
		default:
			break;
		}


		printf("\033[H\033[J");


	}

	return 0;
}

