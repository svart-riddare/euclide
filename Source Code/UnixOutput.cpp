#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include "Erreur.h"
#include "Output.h"
#include "Position.h"
#include "Timer.h"
#include "Version.h"

#include <iconv.h>
#include <langinfo.h>
#include <ncurses.h>

/*************************************************************/

static unsigned int WIDTH = 80;
static unsigned int HEIGHT = 25;

#define COULEUR_BLANC  (COLOR_PAIR(COLOR_WHITE) | A_BOLD)
#define COULEUR_ROUGE  (COLOR_PAIR(COLOR_RED) | A_BOLD)
#define COULEUR_BLEU   (COLOR_PAIR(COLOR_BLUE))
#define COULEUR_BRUN   (COLOR_PAIR(COLOR_YELLOW))
#define COULEUR_VERT   (COLOR_PAIR(COLOR_GREEN))
#define COULEUR_GRIS   (COLOR_PAIR(COLOR_WHITE))
#define COULEUR_VIOLET (COLOR_PAIR(COLOR_MAGENTA))
#define COULEUR_JAUNE  (COLOR_PAIR(COLOR_YELLOW) | A_BOLD)
#define COULEUR_NOIR   (COLOR_PAIR(COLOR_BLACK))

#define COULEUR_BLANC_SUR_BLANC (COLOR_PAIR(12) | A_BOLD)
#define COULEUR_BLANC_SUR_NOIR  (COLOR_PAIR(13) | A_BOLD)
#define COULEUR_NOIR_SUR_BLANC  (COLOR_PAIR(14) | A_BOLD)
#define COULEUR_NOIR_SUR_NOIR   (COLOR_PAIR(15) | A_BOLD)

static char ESCAPE = 27;

/*************************************************************/

static iconv_t ICONV = (iconv_t)(-1);

/*************************************************************/

void OutputStrategieId(unsigned int ID);

/*************************************************************/

void OutputCreate()
{
	setlocale(LC_ALL, "");
	ICONV = iconv_open(nl_langinfo(CODESET), "ISO_8859-1");

	initscr();
	start_color();

	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(6, COLOR_CYAN, COLOR_BLACK);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);

	init_pair(12, COLOR_WHITE, COLOR_WHITE);
	init_pair(13, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(14, COLOR_BLACK, COLOR_WHITE);
	init_pair(15, COLOR_BLACK, COLOR_MAGENTA);

	curs_set(0);
	cbreak();
	noecho();

	WIDTH = getmaxx(stdscr);
	HEIGHT = getmaxy(stdscr);

	OutputClear();
}

/*************************************************************/

void OutputClear()
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH + 1];	

	memset(Tampon, ' ', WIDTH);
	Tampon[WIDTH] = '\0';
	
	attrset(COULEUR_BLANC);
	for (unsigned int y = 0; y < HEIGHT; y++)
		mvprintw(y, 0, Tampon);
	
	strcpy(Tampon, EUCLIDE_VERSION);

   char *CopySign = strchr(Tampon, '\xA9');
	if (CopySign)
		 *CopySign = 'c';

   char *EAcute = strchr(Tampon, '\xC9');
	if (EAcute)
		*EAcute = 'E';

	attrset(COULEUR_VIOLET);
	mvprintw(0, WIDTH - strlen(EUCLIDE_VERSION) - 1, Tampon);
	refresh();
}

/*************************************************************/

void OutputDestroy()
{
	endwin();

	iconv_close(ICONV);
}

/*************************************************************/

const char *TranslateString(const char *String)
{
	static char Translation[512];

	char *Input = (char *)String;
	char *Output = Translation;

	size_t InputSize = strlen(String) + 1;
	size_t OutputSize = sizeof(Translation);

	if (iconv(ICONV, &Input, &InputSize, &Output, &OutputSize) == (size_t)(-1))
		return String;

	if (InputSize > 0)
		return String;

	return Translation;
}

/*************************************************************/

void OutputMessage(const char *Message)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	memset(Tampon, ' ', WIDTH - 10);
	Tampon[WIDTH - 10] = '\0';

	unsigned int L = strlen(Message);
	if (L > WIDTH - 10)
		L = WIDTH - 10;

	memcpy(Tampon, Message, L);

	attrset(COULEUR_BLANC);
	mvprintw(1, 9, TranslateString(Tampon));
	refresh();
}

/*************************************************************/

void OutputMessageErreur(const char *Message)
{
	static char *Tampon = NULL;
	if (!Tampon)
		Tampon = new char[WIDTH];

	memset(Tampon, ' ', WIDTH - 10);
	Tampon[WIDTH - 10] = '\0';
	
	unsigned int L = strlen(Message);
	if (L > WIDTH - 10)
		L = WIDTH - 10;

	memcpy(Tampon, Message, L);

	attrset(COULEUR_ROUGE);
	mvprintw(4, 9, TranslateString(Tampon));
	refresh();
}

/*************************************************************/

bool WaitForInput()
{
	const char *Texte = GetTexte(MESSAGE_APPUYEZSURUNETOUCHE, WIDTH - 1, true);
	static char *Vide = NULL;
	if (!Vide) {
		Vide = new char[WIDTH + 1];
		memset(Vide, ' ', WIDTH);
		Vide[WIDTH] = '\0';
	}
	
	attrset(COULEUR_GRIS);
	mvprintw(HEIGHT - 1, 0, TranslateString(Texte));
	refresh();

	char Input = (char)getch();

	attrset(COULEUR_GRIS);
	mvprintw(HEIGHT - 1, 0, Vide);
	refresh();

	return (Input == ESCAPE) || (Input == 'x') || (Input == 'X');
}

/*************************************************************/

int IsEscape()
{
	return ESCAPE_NO;
}

/*************************************************************/

void OutputDiagramme(const diagramme *Diagramme)
{
	for (rangees Rangee = HUIT; (int)Rangee >= UN; Rangee--) {
		move(7 - Rangee, 0);
		
		for (colonnes Colonne = A; Colonne <= H; Colonne++) {
			cases Case = QuelleCase(Colonne, Rangee);

			chtype Couleur = 0;

			if (QuelFou(Case) == FOUNOIR)
				Couleur = (Diagramme->Couleurs[Case] == BLANCS) ? COULEUR_BLANC_SUR_NOIR : COULEUR_NOIR_SUR_NOIR;
			else
				Couleur = (Diagramme->Couleurs[Case] == BLANCS) ? COULEUR_BLANC_SUR_BLANC : COULEUR_NOIR_SUR_BLANC;

			addch(PieceToChar(Diagramme->Pieces[Case]) | Couleur);
		}
	}

	char Tampon[64];
	sprintf(Tampon, "%u%s%c ", Diagramme->DemiCoups / 2, GetTexte(MESSAGE_VIRGULE, 1, true), (Diagramme->DemiCoups % 2) ? '5' : '0');
	strcat(Tampon, GetTexte(MESSAGE_COUPS, 32, true));

	attrset(COULEUR_BLANC);
	mvprintw(7, 9, TranslateString(Tampon));
	refresh();
}

/*************************************************************/

void OutputNombreDePieces(unsigned int Blancs, unsigned int Noirs)
{
	char Tampon[16];
	sprintf(Tampon, "(%u+%u)", Blancs, Noirs);

	attrset(COULEUR_BLANC);
	mvprintw(6, 9, Tampon);
	refresh();
}

/*************************************************************/

void OutputDeplacementsMinimaux(const bonhomme /*PiecesBlanches*/[MaxHommes], const bonhomme /*PiecesNoires*/[MaxHommes])
{
}

/*************************************************************/

void OutputCoupsLibres(unsigned int CoupsBlancs, unsigned int CoupsNoirs)
{
	char Tampon[24];
	sprintf(Tampon, "%u - %u           ", CoupsBlancs, CoupsNoirs);

	attrset(COULEUR_VERT);
	mvprintw(2, 11, Tampon);
	refresh();
}

/*************************************************************/

void OutputStrategieId(unsigned int Id)
{
	char Tampon[16];
	sprintf(Tampon, "%8u", Id);

	attrset(COULEUR_VERT);
	mvprintw(2, WIDTH - 9, Tampon);
	refresh();
}

/*************************************************************/

void OutputChrono(const char *Chrono)
{
	char Tampon[17];

	memset(Tampon, ' ', 16);
	Verifier(strlen(Chrono) <= 16);
	memcpy(&Tampon[16 - strlen(Chrono)], Chrono, strlen(Chrono));
	Tampon[16] = '\0';

	attrset(COULEUR_VERT);
	mvprintw(7, WIDTH - 17, Tampon);
	refresh();
}

/*************************************************************/

void OutputResultat(const char *Resultat)
{
	char Tampon[33];

	memset(Tampon, ' ', 32);
	Verifier(strlen(Resultat) <= 32);
	memcpy(&Tampon[32 - strlen(Resultat)], Resultat, strlen(Resultat));
	Tampon[32] = '\0';

	attrset(COULEUR_JAUNE);
	mvprintw(5, WIDTH - 33, TranslateString(Tampon));
	refresh();
}

/*************************************************************/

void OutputStrategie(const strategie *Strategie, const pseudopartie * /*Partie*/)
{
	static char *Out = NULL;
	if (!Out)
		Out = new char[(WIDTH >= 512) ? WIDTH / 2 + 1 : 256];

	for (unsigned int i = 0; i < MaxHommes; i++) {
		for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
			const vie *Piece = (Couleur == BLANCS) ? &Strategie->PiecesBlanches[i] : &Strategie->PiecesNoires[i];
			unsigned int k = 0;
			
			if (Piece->Scenario) {	
				Out[k++] = HommeToChar(i);
				Out[k++] = ColonneToChar(QuelleColonne(Piece->Depart));
				Out[k++] = RangeeToChar(QuelleRangee(Piece->Depart));
			
				if (Piece->TrajetSiPion) {
					for (unsigned int j = 0; j < Piece->TrajetSiPion->NombreDeCaptures; j++) {
						Out[k++] = 'x';
						Out[k++] = ColonneToChar(QuelleColonne(Piece->TrajetSiPion->Captures[j]));
						Out[k++] = RangeeToChar(QuelleRangee(Piece->TrajetSiPion->Captures[j]));
					}

					if (!Piece->TrajetSiPion->NombreDeCaptures || (Piece->TrajetSiPion->CaseFinale != Piece->TrajetSiPion->Captures[Piece->TrajetSiPion->NombreDeCaptures - 1])) {
						if (Piece->Coups) {
							Out[k++] = '-';
							Out[k++] = ColonneToChar(QuelleColonne(Piece->TrajetSiPion->CaseFinale));
							Out[k++] = RangeeToChar(QuelleRangee(Piece->TrajetSiPion->CaseFinale));
						}
					}

					if (Piece->Promue) {
						Out[k++] = '=';
						Out[k++] = PieceToChar(Piece->Scenario->Piece);
						if (Piece->NombreAssassinats > 0) {
							for (unsigned int s = 0; s < Piece->NombreAssassinats; s++) {
								Out[k++] = 'x';
								Out[k++] = PieceToChar(Piece->Assassinats[s]->Scenario->Piece);
								Out[k++] = ColonneToChar(QuelleColonne(Piece->Assassinats[s]->Scenario->CaseFinale));
								Out[k++] = RangeeToChar(QuelleRangee(Piece->Assassinats[s]->Scenario->CaseFinale));
							}

							if (!Piece->NombreAssassinats || (Piece->Assassinats[Piece->NombreAssassinats - 1]->Scenario->CaseFinale != Piece->Scenario->CaseFinale)) {
								Out[k++] = '-';
								Out[k++] = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
								Out[k++] = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
							}
						}
						else {
							if (QuelleCase(Piece->Scenario->Promotion, (Couleur == BLANCS) ? HUIT : UN) != Piece->Scenario->CaseFinale) {
								Out[k++] = '-';
								Out[k++] = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
								Out[k++] = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
							}
						}
					}
				}
				else if (Piece->Coups) {
					for (unsigned int s = 0; s < Piece->NombreAssassinats; s++) {
						Out[k++] = 'x';
						Out[k++] = PieceToChar(Piece->Assassinats[s]->Scenario->Piece);
						Out[k++] = ColonneToChar(QuelleColonne(Piece->Assassinats[s]->Scenario->CaseFinale));
						Out[k++] = RangeeToChar(QuelleRangee(Piece->Assassinats[s]->Scenario->CaseFinale));
					}
					
					if (!Piece->NombreAssassinats || (Piece->Assassinats[Piece->NombreAssassinats - 1]->Scenario->CaseFinale != Piece->Scenario->CaseFinale)) {
						if (Piece->NombreAssassinats || (Piece->Scenario->CaseFinale != Piece->Depart)) {
							Out[k++] = '-';
							Out[k++] = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
							Out[k++] = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
						}
					}
				}

				if (Piece->Switchback && !Piece->NombreAssassinats) {
					Out[k++] = '-';
					Out[k++] = (Piece->CaseDuSwitchback < MaxCases) ? ColonneToChar(QuelleColonne(Piece->CaseDuSwitchback)) : '?';
					Out[k++] = (Piece->CaseDuSwitchback < MaxCases) ? RangeeToChar(QuelleRangee(Piece->CaseDuSwitchback)) : '?';
					Out[k++] = '-';
					Out[k++] = ColonneToChar(QuelleColonne(Piece->Scenario->CaseFinale));
					Out[k++] = RangeeToChar(QuelleRangee(Piece->Scenario->CaseFinale));
				}

				if (Piece->Capturee && Piece->Assassin) {
					Out[k++] = ' ';
					Out[k++] = '(';
					Out[k++] = HommeToChar(Piece->Assassin->Scenario->Homme);
					Out[k++] = ColonneToChar(QuelleColonne(Piece->Assassin->Depart));
					Out[k++] = RangeeToChar(QuelleRangee(Piece->Assassin->Depart));
					Out[k++] = ')';
				}
			}

			while (k < WIDTH / 2)
				Out[k++] = ' ';

			Out[WIDTH / 2 - 5] = '\0';

			attrset(Piece->Capturee ? COULEUR_BRUN : COULEUR_GRIS);
			mvprintw(8 + i, 5 + ((Couleur == NOIRS) ? WIDTH / 2 : 0), Out);

			strcpy(Out, "  ");
			if (Piece->Scenario && Piece->Coups)
				sprintf(Out, "%2u", Piece->Coups);

			attrset(COULEUR_BLANC);
			mvprintw(8 + i, 1 + ((Couleur == NOIRS) ? WIDTH / 2 : 0), Out);
		}
	}

	OutputCoupsLibres(Strategie->CoupsLibresBlancs, Strategie->CoupsLibresNoirs);
	OutputStrategieId(Strategie->IDFinal);
	OutputChrono(GetElapsedTime());
}

/*************************************************************/

void OutputDebutPartie(deplacement **Deplacements)
{
	static const unsigned int DemiCoups = 6;

	char Tampon[DemiCoups * 5 + 1];
	memset(Tampon, ' ', DemiCoups * 5);
	Tampon[DemiCoups * 5] = '\0';

	if (Deplacements) {
		for (unsigned int k = 0; k < DemiCoups; k++) {
			char *Out = &Tampon[k * 5];
	
			*Out++ = HommeToChar(Deplacements[k]->Qui);
			if (Deplacements[k]->Mort != MaxHommes)
				*Out++ = 'x';
			*Out++ = ColonneToChar(QuelleColonne(Deplacements[k]->Vers));
			*Out++ = RangeeToChar(QuelleRangee(Deplacements[k]->Vers));
		}
	}

	attrset(COULEUR_GRIS);
	mvprintw(3, WIDTH - DemiCoups * 5, Tampon);
	refresh();

	OutputChrono(GetElapsedTime());
}

/*************************************************************/
