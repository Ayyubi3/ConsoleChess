#include <stdio.h>
#include <conio.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)


int DBoxColorR = 136, DBoxColorG = 119, DBoxColorB = 183;
int LBoxColorR = 233, LBoxColorG = 4, LBoxColorB = 33;

int DPieceColorR = 0, DPieceColorG = 0, DPieceColorB = 0;
int LPieceColorR = 255, LPieceColorG = 255, LPieceColorB = 255;
int FrameColorR = 136, FrameColorG = 119, FrameColorB = 183;

enum Pieces { KING, ROOK, BISHOP, QUEEN, KNIGHT, PAWN };

char Board[64];

int GlobalX = 2, GlobalY = 9;
int BoardX = 1, BoardY = 1;

int isWhiteTurn = 1;


void MoveInBoard(int dx, int dy)
{
	//left
	if (dx == -1 && BoardX > 1)
	{
		GlobalX--;
		BoardX--;
	}
	else
		//right
		if (dx == 1 && BoardX < 8)
		{
			GlobalX++;
			BoardX++;
		}
		else
			//up	
			if (dy == 1 && BoardY < 8)
			{
				GlobalY--;
				BoardY++;
			}
			else
				//down
				if (dy == -1 && BoardY > 1)
				{
					GlobalY++;
					BoardY--;
				}

}





void printboard()
{
	//TODO: Erweiterte Fenstreing einlesen (wer dran ist und so)
	printf("\x1b[48;2;%i;%i;%im", FrameColorR, FrameColorG, FrameColorB);

	printf("XABCDEFGH\n");

	printf("\x1b[48;2;%i;%i;%im", 0, 0, 0);


	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 9; j++)
		{
			if (j == 0)
			{
				printf("\x1b[48;2;%i;%i;%im", FrameColorR, FrameColorG, FrameColorB);

				printf("%c", i + '1');//Die Zahl links
				printf("\x1b[48;2;%i;%i;%im", 0, 0, 0);


			}
			else {

				int index = 8 * i + (j - 1);
				//Raster einfärben
				//Die erste Klammer rechnet den Index aus(die minus 1 bei j isi weil die Schleife bis 9 geht(wegen der zahl jede Reihe))
				//Der + Moduo teil ist verscheibt das Raster um eins jede Reihe;
				((8 * i + (j - 1)) + (i % 2)) % 2 == 0 ?
					printf("\x1b[48;2;%i;%i;%im", DBoxColorR, DBoxColorB, DBoxColorB) :
					printf("\x1b[48;2;%i;%i;%im", LBoxColorR, LBoxColorB, LBoxColorB);

				//Spieler einfärben
				('a' <= Board[index] && Board[index] <= 'z') ?
					printf("\x1b[38;2;%i;%i;%im", DPieceColorR, DPieceColorB, DPieceColorB) :
					printf("\x1b[38;2;%i;%i;%im", LPieceColorR, LPieceColorB, LPieceColorB);



				printf("%c", Board[index]);

				printf("\x1b[48;2;0;0;0m");
				printf("\x1b[38;2;255;255;255m");


			}
		}

		printf("\n");

	}
}


int ReadFenStringToBoard(char* FEN)
{
	for (size_t i = 0; i < 64; i++)
	{
		Board[i] = ' ';
	}

	int index = 0;
	for (int i = 0; FEN[i] != '\0'; i++)
	{
		if ('A' <= FEN[i] && FEN[i] <= 'z')
		{
			Board[index++] = FEN[i];
		}
		else if ('0' <= FEN[i] && FEN[i] <= '9')
		{
			index += (FEN[i] - 48);
		}
	}
}


int main()
{

	ReadFenStringToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");




	while (1)
	{

		printboard();
		printf("\033[%d;%dH", GlobalY, GlobalX);


		char ctrl = getch();

		switch (ctrl)
		{
		case 'w':
			MoveInBoard(0, 1);
			break;
		case 'a':
			MoveInBoard(-1, 0);
			break;
		case 's':
			MoveInBoard(0, -1);
			break;
		case 'd':
			MoveInBoard(1, 0);
			break;
		default:
			break;
		}
		printf("\033[H\033[J");
	}

	return 0;
}