#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "Output.h"
#include "Position.h"
#include "Timer.h"
#include "Version.h"

/*************************************************************/

static char ESCAPE = 27;

/*************************************************************/

void OutputStrategieId(unsigned int ID);

/*************************************************************/

void OutputCreate()
{
	OutputClear();
}

/*************************************************************/

void OutputClear()
{
	char Tampon[128];
	strcpy(Tampon, EUCLIDE_VERSION);

	char *CopySign = strchr(Tampon, '©');
	if (CopySign)
		 *CopySign = 'c';

	char *EAcute = strchr(Tampon, 'É');
	if (EAcute)
		*EAcute = 'E';

	printf("%s\n", Tampon);
}

/*************************************************************/

void OutputDestroy()
{
	printf("\n");
}

/*************************************************************/

void OutputMessage(const char * /*Message*/)
{
}

/*************************************************************/

void OutputMessageErreur(const char *Message)
{
	printf("\n\t%s\n\a\n", Message);
}

/*************************************************************/

bool WaitForInput()
{
	printf("\n%s\n", GetTexte(MESSAGE_APPUYEZSURUNETOUCHE, 80, false));
	char Input = (char)getchar();

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
	for (int Rangee = HUIT; Rangee >= UN; Rangee--) {
		for (colonnes Colonne = A; Colonne <= H; Colonne++) {
			cases Case = QuelleCase(Colonne, Rangee);

			char Out = PieceToChar(Diagramme->Pieces[Case]);
			if (Out == ' ')
				Out = '.';

			if (Diagramme->Couleurs[Case] == NOIRS)
				Out = (char)tolower(Out);

			printf("%c", Out);
		}

		printf("\n");
	}

	printf("  %u%s%c\n", Diagramme->DemiCoups / 2, GetTexte(MESSAGE_VIRGULE, 1, true), (Diagramme->DemiCoups % 2) ? '5' : '0');
}

/*************************************************************/

void OutputNombreDePieces(unsigned int Blancs, unsigned int Noirs)
{
	printf("(%u+%u)\n", Blancs, Noirs);
}

/*************************************************************/

void OutputDeplacementsMinimaux(const bonhomme /*PiecesBlanches*/[MaxHommes], const bonhomme /*PiecesNoires*/[MaxHommes])
{
}

/*************************************************************/

void OutputCoupsLibres(unsigned int /*CoupsBlancs*/, unsigned int /*CoupsNoirs*/)
{
}

/*************************************************************/

void OutputStrategieId(unsigned int Id)
{
	printf("#%u\n", Id);
}

/*************************************************************/

void OutputChrono(const char * /*Chrono*/)
{
}

/*************************************************************/

void OutputResultat(const char *Resultat)
{
	printf("  [%s]\n", Resultat);
}

/*************************************************************/

void OutputStrategie(const strategie *Strategie, const pseudopartie * /**/)
{
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
	Tampon[DemiCoups * 5]=  '\0';

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

	printf("%s\n", Tampon);
}

/*************************************************************/