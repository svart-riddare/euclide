#pragma warning(disable: 4201 4514)  // In Windows.h

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "Erreur.h"
#include "Output.h"
#include "Position.h"
#include "Timer.h"
#include "Version.h"

/*************************************************************/

static HANDLE Console = INVALID_HANDLE_VALUE;
static HANDLE InputConsole = INVALID_HANDLE_VALUE;

static CONSOLE_SCREEN_BUFFER_INFO InitialState;
static DWORD InitialOutputConsoleMode;
static DWORD InitialInputConsoleMode;
static bool InitialCursorState;

static unsigned int WIDTH = 80;
static unsigned int HEIGHT = 25;

#define COULEUR_BLANC  (WORD)(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define COULEUR_ROUGE  (WORD)(FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COULEUR_BLEU   (WORD)(FOREGROUND_BLUE)
#define COULEUR_BRUN   (WORD)(FOREGROUND_RED | FOREGROUND_GREEN)
#define COULEUR_VERT   (WORD)(FOREGROUND_GREEN)
#define COULEUR_GRIS   (WORD)(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define COULEUR_VIOLET (WORD)(FOREGROUND_RED | FOREGROUND_BLUE)
#define COULEUR_JAUNE  (WORD)(FOREGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define COULEUR_NOIR   (0)

#define COULEURFOND_GRIS   (WORD)(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)
#define COULEURFOND_VIOLET (WORD)(BACKGROUND_RED | BACKGROUND_BLUE)

/*************************************************************/

void PrintIt(CHAR_INFO *Output, unsigned int ToPrint);
void OutputStrategieId(unsigned int Id);

/*************************************************************/

void OutputCreate()
{
	Console = GetStdHandle(STD_OUTPUT_HANDLE);
	InputConsole = GetStdHandle(STD_INPUT_HANDLE);

	if ((Console == INVALID_HANDLE_VALUE) || (InputConsole == INVALID_HANDLE_VALUE))
		FatalOutputError();

	GetConsoleScreenBufferInfo(Console, &InitialState);
	COORD MaxWindowSize = InitialState.dwSize;

	if ((MaxWindowSize.Y < (int)HEIGHT) || (MaxWindowSize.X < (int)64))
		FatalOutputError();

	if (MaxWindowSize.X < (int)WIDTH)
		WIDTH = MaxWindowSize.X;

	GetConsoleMode(Console, &InitialOutputConsoleMode);
	GetConsoleMode(InputConsole, &InitialInputConsoleMode);
	SetConsoleMode(InputConsole, ENABLE_PROCESSED_INPUT);
		
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(Console, &CursorInfo);
	InitialCursorState = (CursorInfo.bVisible != 0);
	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(Console, &CursorInfo);

	SMALL_RECT W;
	W.Bottom = (short)(HEIGHT - 1);
	W.Right = (short)(WIDTH - 1);
	W.Left = 0;
	W.Top = 0;
	SetConsoleWindowInfo(Console, true, &W);

	OutputClear();
	FlushConsoleInputBuffer(InputConsole);
}

/*************************************************************/

void OutputClear()
{
	COORD Curseur = { 0, 0 };
	SetConsoleCursorPosition(Console, Curseur);

	DWORD Written;
	FillConsoleOutputAttribute(Console, COULEUR_BLANC, WIDTH * HEIGHT, Curseur, &Written);
	FillConsoleOutputCharacter(Console, ' ', WIDTH * HEIGHT, Curseur, &Written);

	COORD Copyright;
	Copyright.X = (SHORT)(WIDTH - strlen(EUCLIDE_VERSION) - 1);
	Copyright.Y = 0;

	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];
	
	strcpy(Tampon, EUCLIDE_VERSION);
	char *CopySign = strchr(Tampon, '©');
	CharToOem(EUCLIDE_VERSION, Tampon);
	if (CopySign)
		 *CopySign = '\002';
	
	SetConsoleCursorPosition(Console, Copyright);
	SetConsoleTextAttribute(Console, COULEUR_VIOLET);
	WriteConsole(Console, Tampon, strlen(EUCLIDE_VERSION), &Written, NULL);
	SetConsoleTextAttribute(Console, InitialState.wAttributes);
	SetConsoleCursorPosition(Console, Curseur);
}

/*************************************************************/

void OutputDestroy()
{
	COORD Curseur = { 0, 8 };
	SetConsoleCursorPosition(Console, Curseur);

	DWORD Written;
	FillConsoleOutputAttribute(Console, InitialState.wAttributes, WIDTH * (HEIGHT - 8), Curseur, &Written);
	FillConsoleOutputCharacter(Console, ' ', WIDTH * (HEIGHT - 8), Curseur, &Written);

	Curseur.Y++;

	SetConsoleWindowInfo(Console, true, &InitialState.srWindow);
	SetConsoleTextAttribute(Console, InitialState.wAttributes);
	SetConsoleCursorPosition(Console, Curseur);

	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(Console, &CursorInfo);
	CursorInfo.bVisible = InitialCursorState;
	SetConsoleCursorInfo(Console, &CursorInfo);

	SetConsoleMode(Console, InitialOutputConsoleMode);
	SetConsoleMode(InputConsole, InitialInputConsoleMode);
}

/*************************************************************/

void OutputMessage(const char *Message)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	memset(Tampon, ' ', WIDTH - 10);

	unsigned int L = strlen(Message);
	if (L > WIDTH - 10)
		L = WIDTH - 10;

	memcpy(Tampon, Message, L);
	CharToOemBuff(Tampon, Tampon, WIDTH - 10);

	DWORD Written;
	COORD Position = { 9, 1 };
	
	SetConsoleCursorPosition(Console, Position);
	SetConsoleTextAttribute(Console, COULEUR_BLANC);
	WriteConsole(Console, Tampon, WIDTH - 10, &Written, NULL);
}

/*************************************************************/

void OutputMessageErreur(const char *Message)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	memset(Tampon, ' ', WIDTH - 10);
	
	unsigned int L = strlen(Message);
	if (L > WIDTH - 10)
		L = WIDTH - 10;

	DWORD Written;
	COORD Position = { 9, 4 };

	memcpy(Tampon, Message, L);
	CharToOemBuff(Tampon, Tampon, WIDTH - 10);
	SetConsoleCursorPosition(Console, Position);
	SetConsoleTextAttribute(Console, COULEUR_ROUGE);
	WriteConsole(Console, Tampon, WIDTH - 10, &Written, NULL);
}
/*************************************************************/
/* Calling ReadConsole() on Windows NT 4 does not return if  */
/* user presses ESCAPE. Bug???                               */
/*************************************************************/

bool WaitForInput()
{
	const char *Texte = GetTexte(MESSAGE_APPUYEZSURUNETOUCHE, WIDTH - 1, true);
	static char *Vide = NULL;
	if (!Vide)
		Vide = new char[WIDTH];

	COORD Position = { 0, (short)(HEIGHT - 1) };

	TCHAR Input;
	DWORD Read;
	DWORD Written;

	memset(Vide, ' ', WIDTH);
	SetConsoleCursorPosition(Console, Position);
	SetConsoleTextAttribute(Console, COULEUR_GRIS);
	WriteConsole(Console, Texte, WIDTH - 1, &Written, NULL);
	FlushConsoleInputBuffer(InputConsole);
	WaitForSingleObject(InputConsole, INFINITE);
	ReadConsole(InputConsole, &Input, 1, &Read, NULL);
	SetConsoleCursorPosition(Console, Position);
	WriteConsole(Console, Vide, WIDTH - 1, &Written, NULL);

	return (Input == VK_ESCAPE) || (Input == 'x') || (Input == 'X');
}

/*************************************************************/

bool IsEscape()
{
	DWORD Events = 0;
	GetNumberOfConsoleInputEvents(InputConsole, &Events);

	if (Events > 0) {
		INPUT_RECORD *InputRecords = new INPUT_RECORD[Events];
		DWORD Read = 0;

		ReadConsoleInput(InputConsole, InputRecords, Events, &Read);
		for (unsigned int k = 0; k < Read; k++) {
			if (InputRecords[k].EventType == KEY_EVENT) {
				if (InputRecords[k].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
					OutputMessageErreur(MESSAGE_UTILISATEUR);
					delete[] InputRecords;
					return true;
				}
			}
		}

		delete[] InputRecords;
	}

	return false;
}

/*************************************************************/

void OutputDiagramme(const diagramme *Diagramme)
{
	CHAR_INFO Echiquier[MaxCases];

	for (cases Case = A1; Case <= H8; Case++) {
		colonnes Colonne = QuelleColonne(Case);
		rangees Rangee = QuelleRangee(Case);
		unsigned int Index = (7 - Rangee) * 8 + Colonne;

		Echiquier[Index].Char.AsciiChar = PieceToChar(Diagramme->Pieces[Case]);
		Echiquier[Index].Attributes = (QuelFou(Case) == FOUNOIR) ? COULEURFOND_VIOLET : COULEURFOND_GRIS;
		Echiquier[Index].Attributes |= (WORD)((Diagramme->Couleurs[Case] == BLANCS) ? COULEUR_BLANC : COULEUR_NOIR);
	}

	COORD Taille = { 8, 8 };
	COORD Origine = { 0, 0 };
	SMALL_RECT Fenetre = { 0, 0, 7, 7 };

	WriteConsoleOutput(Console, Echiquier, Taille, Origine, &Fenetre);

	char Tampon[64];
	sprintf(Tampon, "%u%s%c ", Diagramme->DemiCoups / 2, GetTexte(MESSAGE_VIRGULE, 1, true), (Diagramme->DemiCoups % 2) ? '5' : '0');
	strcat(Tampon, GetTexte(MESSAGE_COUPS, 32, true));
	CharToOem(Tampon, Tampon);

	DWORD Written;
	COORD Curseur = { 9, 7 };
	
	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_BLANC);
	WriteConsole(Console, Tampon, strlen(Tampon), &Written, NULL);
}

/*************************************************************/

void OutputNombreDePieces(unsigned int Blancs, unsigned int Noirs)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	sprintf(Tampon, "(%u+%u)", Blancs, Noirs);

	DWORD Written;
	COORD Curseur = { 9, 6 };

	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_BLANC);
	WriteConsole(Console, Tampon, strlen(Tampon), &Written, NULL);
}

/*************************************************************/

void OutputDeplacementsMinimaux(const bonhomme /*PiecesBlanches*/[MaxHommes], const bonhomme /*PiecesNoires*/[MaxHommes]) {}
/*
void OutputDeplacementsMinimaux(const bonhomme PiecesBlanches[MaxHommes], const bonhomme PiecesNoires[MaxHommes])
{
	CHAR_INFO *Output = new CHAR_INFO[MaxHommes * WIDTH];
	CHAR_INFO *Out = Output;

	for (unsigned int i = 0; i < MaxHommes; i++) {
		for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
			const bonhomme *Piece = (Couleur == BLANCS) ? &PiecesBlanches[i] : &PiecesNoires[i];

			for (unsigned int k = 0; k < WIDTH / 2; k++) {
				Out[k].Char.AsciiChar = ' ';
				Out[k].Attributes = (Piece->Piece != VIDE) ? COULEUR_GRIS : COULEUR_BRUN;
			}

			if (Piece->Piece != VIDE) {
				Out[2].Char.AsciiChar = PieceToChar(Piece->Piece);
				Out[3].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->CaseActuelle));
				Out[4].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->CaseActuelle));

				if (Piece->ExAileDame)
					PrintIt(&Out[8], Piece->DeplacementsSiAileDame);
				if (Piece->ExAileRoi)
					PrintIt(&Out[11], Piece->DeplacementsSiAileRoi);
				if (Piece->ExGrandRoque)
					PrintIt(&Out[14], Piece->DeplacementsSiGrandRoque);
				if (Piece->ExPetitRoque)
					PrintIt(&Out[17], Piece->DeplacementsSiPetitRoque);
				if ((Piece->Piece == PION) || !Piece->PromotionImpossible)
					PrintIt(&Out[23], Piece->DeplacementsSiPion);
			}
			else if (Piece->PieceDisparue != VIDE) {
				Out[2].Char.AsciiChar = PieceToChar(Piece->PieceDisparue);
				if (Piece->CaseActuelle != MaxCases) {
					Out[3].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->CaseActuelle));
					Out[4].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->CaseActuelle));
				}
				else {
					if (Piece->CaseActuelleMin != MaxCases) {
						Out[3].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->CaseActuelleMin));
						Out[4].Char.AsciiChar = RangeeToChar(QuelleRangee((Couleur == BLANCS) ? Piece->CaseActuelleMin : Piece->CaseActuelleMax));
						Out[5].Char.AsciiChar = '/';
						Out[6].Char.AsciiChar = RangeeToChar(QuelleRangee((Couleur == BLANCS) ? Piece->CaseActuelleMax : Piece->CaseActuelleMin));
					}
					else {
						Out[3].Char.AsciiChar = '?';
						Out[4].Char.AsciiChar = '?';
					}
				}
				
				unsigned int Position = 8;
				for (pieces PiecePossible = DAME; PiecePossible > VIDE; PiecePossible--, Position += 3)
					if (Piece->ExPiece[PiecePossible])
						PrintIt(&Out[Position], Piece->DeplacementsSiExPiece[PiecePossible]);			
			}

			if (Piece->Deplacements)
				PrintIt(&Out[30], Piece->Deplacements);

			Out[30].Attributes = COULEUR_BLANC;
			Out[31].Attributes = COULEUR_BLANC;

			Out += WIDTH / 2;
		}
	}

	COORD Taille = { (short)WIDTH, MaxHommes };
	COORD Origine = { 0, 0 };
	SMALL_RECT Fenetre = { 0, 8, (short)(WIDTH - 1), (short)(HEIGHT - 1) };

	WriteConsoleOutput(Console, Output, Taille, Origine, &Fenetre);
	OutputChrono(GetElapsedTime());

	delete[] Output;
}
*/
/*************************************************************/

void PrintIt(CHAR_INFO *Output, unsigned int ToPrint)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	sprintf(Tampon, "%2u", ToPrint);
	Output[0].Char.AsciiChar = Tampon[0];
	Output[1].Char.AsciiChar = Tampon[1];
}

/*************************************************************/

void OutputCoupsLibres(unsigned int CoupsBlancs, unsigned int CoupsNoirs)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	sprintf(Tampon, "%u - %u           ", CoupsBlancs, CoupsNoirs);

	COORD Curseur = { 11, 2 };
	DWORD Written;

	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_VERT);
	WriteConsole(Console, Tampon, 16, &Written, NULL);
}

/*************************************************************/

void OutputStrategieId(unsigned int ID)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	sprintf(Tampon, "%8u", ID);

	COORD Curseur = { (short)(WIDTH - 9), 2 };
	DWORD Written;

	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_VERT);
	WriteConsole(Console, Tampon, 8, &Written, NULL);
}

/*************************************************************/

void OutputChrono(const char *Chrono)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	memset(Tampon, ' ', 16);
	Verifier(strlen(Chrono) <= 16);
	memcpy(&Tampon[16 - strlen(Chrono)], Chrono, strlen(Chrono));

	COORD Curseur = { (short)(WIDTH - 17), 7 };
	DWORD Written;

	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_VERT);
	WriteConsole(Console, Tampon, 16, &Written, NULL);
}

/*************************************************************/

void OutputResultat(const char *Resultat)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	memset(Tampon, ' ', 32);
	Verifier(strlen(Resultat) <= 32);
	memcpy(&Tampon[32 - strlen(Resultat)], Resultat, strlen(Resultat));

	COORD Curseur = { (short)(WIDTH - 33), 5 };
	DWORD Written;

	CharToOemBuff(Tampon, Tampon, 32);
	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_JAUNE);
	WriteConsole(Console, Tampon, 32, &Written, NULL);
}

/*************************************************************/

void OutputStrategie(const strategie *Strategie, const pseudopartie *Partie)
{
	CHAR_INFO *Output = new CHAR_INFO[MaxHommes * WIDTH + 100];
	CHAR_INFO *Out = Output;

	for (unsigned int i = 0; i < MaxHommes; i++) {
		for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
			const vie *Piece = (Couleur == BLANCS) ? &Strategie->PiecesBlanches[i] : &Strategie->PiecesNoires[i];

			for (unsigned int j = 0; j < WIDTH / 2; j++) {
				Out[j].Char.AsciiChar = ' ';
				Out[j].Attributes = Piece->Capturee ? COULEUR_BRUN : COULEUR_GRIS;
			}

			if (Piece->Scenario && Piece->Coups) {
				PrintIt(&Out[1], Piece->Coups);
				Out[1].Attributes = COULEUR_BLANC;
				Out[2].Attributes = COULEUR_BLANC;
			}
			
			unsigned int k = 5;
			
			if (Piece->Scenario) {	
				Out[k++].Char.AsciiChar = HommeToChar(i);
				Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Depart));
				Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Depart));
			
				if (Piece->TrajetSiPion) {
					for (unsigned int j = 0; j < Piece->TrajetSiPion->NombreDeCaptures; j++) {
						Out[k++].Char.AsciiChar = 'x';
						Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->TrajetSiPion->Captures[j]));
						Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->TrajetSiPion->Captures[j]));
					}

					if (!Piece->TrajetSiPion->NombreDeCaptures || (Piece->TrajetSiPion->CaseFinale != Piece->TrajetSiPion->Captures[Piece->TrajetSiPion->NombreDeCaptures - 1])) {
						if (Piece->Coups) {
							Out[k++].Char.AsciiChar = '-';
							Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->TrajetSiPion->CaseFinale));
							Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->TrajetSiPion->CaseFinale));
						}
					}

					if (Piece->Promue) {
						Out[k++].Char.AsciiChar = '=';
						Out[k++].Char.AsciiChar = PieceToChar(Piece->Scenario->Piece);
						if (Piece->NombreAssassinats > 0) {
							for (unsigned int s = 0; s < Piece->NombreAssassinats; s++) {
								Out[k++].Char.AsciiChar = 'x';
								Out[k++].Char.AsciiChar = PieceToChar(Piece->Assassinats[s]->Scenario->Piece);
								Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Assassinats[s]->Scenario->CaseFinale));
								Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Assassinats[s]->Scenario->CaseFinale));
							}

							if (!Piece->NombreAssassinats || (Piece->Assassinats[Piece->NombreAssassinats - 1]->Scenario->CaseFinale != Piece->Scenario->CaseFinale)) {
								Out[k++].Char.AsciiChar = '-';
								Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
								Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
							}
						}
						else {
							if (QuelleCase(Piece->Scenario->Promotion, (Couleur == BLANCS) ? HUIT : UN) != Piece->Scenario->CaseFinale) {
								Out[k++].Char.AsciiChar = '-';
								Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
								Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
							}
						}
					}
				}
				else if (Piece->Coups) {
					for (unsigned int s = 0; s < Piece->NombreAssassinats; s++) {
						Out[k++].Char.AsciiChar = 'x';
						Out[k++].Char.AsciiChar = PieceToChar(Piece->Assassinats[s]->Scenario->Piece);
						Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Assassinats[s]->Scenario->CaseFinale));
						Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Assassinats[s]->Scenario->CaseFinale));
					}
					
					if (!Piece->NombreAssassinats || (Piece->Assassinats[Piece->NombreAssassinats - 1]->Scenario->CaseFinale != Piece->Scenario->CaseFinale)) {
						if (Piece->NombreAssassinats || (Piece->Scenario->CaseFinale != Piece->Depart)) {
							Out[k++].Char.AsciiChar = '-';
							Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
							Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
						}
					}
				}

				if (Piece->Switchback && !Piece->NombreAssassinats) {
					Out[k++].Char.AsciiChar = '-';
					Out[k++].Char.AsciiChar = (Piece->CaseDuSwitchback < MaxCases) ? ColonneToChar(QuelleColonne(Piece->CaseDuSwitchback)) : '?';
					Out[k++].Char.AsciiChar = (Piece->CaseDuSwitchback < MaxCases) ? RangeeToChar(QuelleRangee(Piece->CaseDuSwitchback)) : '?';
					Out[k++].Char.AsciiChar = '-';
					Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
					Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
				}

				if (Piece->Capturee && Piece->Assassin) {
					Out[k++].Char.AsciiChar = ' ';
					Out[k++].Char.AsciiChar = '(';
					Out[k++].Char.AsciiChar = HommeToChar(Piece->Assassin->Scenario->Homme);
					Out[k++].Char.AsciiChar = ColonneToChar(QuelleColonne(Piece->Assassin->Depart));
					Out[k++].Char.AsciiChar = RangeeToChar(QuelleRangee(Piece->Assassin->Depart));
					Out[k].Char.AsciiChar = ')';
				}
			}

			Out += WIDTH / 2;
		}
	}

	COORD Taille = { (short)WIDTH, MaxHommes };
	COORD Origine = { 0, 0 };
	SMALL_RECT Fenetre = { 0, 8, (short)(WIDTH - 1), (short)(HEIGHT - 1) };

	WriteConsoleOutput(Console, Output, Taille, Origine, &Fenetre);
	OutputCoupsLibres(Strategie->CoupsLibresBlancs, Strategie->CoupsLibresNoirs);
	OutputStrategieId(Strategie->IDFinal);
	OutputChrono(GetElapsedTime());

	/* TEMPORAIRE */

	FILE *Debug = fopen("Debug.txt", "a");

	if (Debug) {
		fprintf(Debug, "\n********************************************************************************\n\n");
		fprintf(Debug, "Stratégie #%u (%u)   %u - %u\n\n", Strategie->IDFinal, Strategie->IDPhaseA, Strategie->CoupsLibresBlancs, Strategie->CoupsLibresNoirs);
		unsigned int z = 0;
		
		static char *String = NULL;
		if (!String)
			String = new char[WIDTH + 8];

		for (unsigned int y = 0; y < MaxHommes; y++) {			
			for (unsigned int x = 0; x < WIDTH; x++)
				String[x] = Output[z++].Char.AsciiChar;

			String[WIDTH] = '\0';

			fprintf(Debug, "%s\n", String);
		}
/*
		fprintf(Debug, "\n");
		for (couleurs Couleur = BLANCS; Partie && (Couleur <= NOIRS); Couleur++) {
			for (unsigned int k = 0; k < ((Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs); k++) {
				const coup *Coup = (Couleur == BLANCS) ? &Partie->CoupsBlancs[k] : &Partie->CoupsNoirs[k];

				char De[4];
				char Vers[4];
				strcpy(De, CaseToString(Coup->De));
				strcpy(Vers, CaseToString(Coup->Vers));

				fprintf(Debug, "  %c%s%c%s                             %2u - %2u\n", PieceToChar(Coup->Type), De, Coup->Victime ? 'x' : '-', Vers, Coup->AuPlusTot, Coup->AuPlusTard);

				fprintf(Debug, "    Après : ");
				for (unsigned int i = 0; i < Coup->NombreDoitSuivre; i++) {
					const coup *C = Coup->DoitSuivre[i];

					strcpy(De, CaseToString(C->De));
					strcpy(Vers, CaseToString(C->Vers));
					fprintf(Debug, "%c%s%c%s ", PieceToChar(C->Type), De, C->Victime ? 'x' : '-', Vers);
				}

				fprintf(Debug, "\n    Avant : ");
				for (unsigned int j = 0; j < Coup->NombreDoitPreceder; j++) {
					const coup *C = Coup->DoitPreceder[j];

					strcpy(De, CaseToString(C->De));
					strcpy(Vers, CaseToString(C->Vers));
					fprintf(Debug, "%c%s%c%s ", PieceToChar(C->Type), De, C->Victime ? 'x' : '-', Vers);
				}

				fprintf(Debug, "\n");
			}
		}*/

		fprintf(Debug, "\n");
		fclose(Debug);
	}

	/* FIN DU TEMPORAIRE */

	delete[] Output;
}

/*************************************************************/

void OutputDebutPartie(deplacement **Deplacements)
{
	static const unsigned int DemiCoups = 6;

	char Tampon[DemiCoups * 5];
	memset(Tampon, ' ', DemiCoups * 5);

	if (Deplacements) {
		for (unsigned int k = 0; k < DemiCoups; k++) {
			char *Out = &Tampon[k * 5];
	
			*Out++ = HommeToChar(Deplacements[k]->Qui);
			if (Deplacements[k]->Mort != MaxHommes)
				*Out++ = 'x';
			*Out++ = ColonneToChar(QuelleColonne(Deplacements[k]->Vers));
			*Out = RangeeToChar(QuelleRangee(Deplacements[k]->Vers));
		}
	}	

	COORD Curseur = { (short)(WIDTH - DemiCoups * 5), 3 };
	DWORD Written;

	CharToOemBuff(Tampon, Tampon, DemiCoups * 5);
	SetConsoleCursorPosition(Console, Curseur);
	SetConsoleTextAttribute(Console, COULEUR_GRIS);
	WriteConsole(Console, Tampon, DemiCoups * 5, &Written, NULL);

	OutputChrono(GetElapsedTime());
}

/*************************************************************/