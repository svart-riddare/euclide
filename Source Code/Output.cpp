#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "Constantes.h"
#include "Erreur.h"
#include "Output.h"
#include "Partie.h"
#include "System.h"
#include "Timer.h"
#include "Version.h"

/*************************************************************/

static const char *FichiersTexte[] = {
	"Euclide.txt",
	"Français.txt",
	"English.txt"
};

static char *Textes[MaxTextes];

static const char *ChiffresRomains[] = {
	"",
	"",
	" II",
	" III",
	" IV",
	" V",
	" VI",
	" VII",
	" VIII",
	" IX"
};

/*************************************************************/

void ChoixDeLangue(langue Langue)
{
	if (Langue >= MaxLangues)
		Langue = EUCLIDE;

	for (unsigned int i = 0; i < MaxTextes; i++)
		Textes[i] = NULL;

	FILE *Source = fopen(FichiersTexte[Langue], "r");
	if (!Source)
		Source = fopen(CheminAlternatif(FichiersTexte[Langue]), "r");
	if (!Source)
		ErreurFichierLangue(FichiersTexte[Langue]);

	unsigned int TextesLus = 0;
	char Tampon[1024];
	while (fgets(Tampon, 1024, Source)) {
		unsigned int Length = strlen(Tampon);
		while ((Length > 0) && isspace(Tampon[Length - 1]))
			Tampon[--Length] = '\0';
			
		Textes[TextesLus] = new char[Length + 1];
		strcpy(Textes[TextesLus], Tampon);

		if (++TextesLus >= MaxTextes)
			break;
	}
	
	if (TextesLus < MaxTextes)
		ErreurFichierLangue(FichiersTexte[Langue]);

	fclose(Source);
}

/*************************************************************/

void DestructionDesTextes()
{
	for (unsigned int i = 0; i < MaxTextes; i++)
		delete[] Textes[i];
}

/*************************************************************/

const char *GetTexte(texte Texte, unsigned int LongueurMaximale, bool TailleExacte)
{
	static char Tampon[1024];

	unsigned int Longueur = strlen(Textes[Texte]);
	
	if (LongueurMaximale > 1023)
		LongueurMaximale = 1023;
	
	if (Longueur > LongueurMaximale)
		Longueur = LongueurMaximale;

	memcpy(Tampon, Textes[Texte], Longueur);
	
	if (TailleExacte) {
		if (LongueurMaximale > Longueur)
			memset(&Tampon[Longueur], ' ', LongueurMaximale - Longueur);

		Tampon[LongueurMaximale] = '\0';
	}
	else {
		Tampon[Longueur] = '\0';
	}

	return Tampon;
}

/*************************************************************/

char HommeToChar(hommes Homme)
{
	switch (Homme) {
		case XROI :
			return PieceToChar(ROI);
		case XDAME :
			return PieceToChar(DAME);
		case TOURDAME :
		case TOURROI :
			return PieceToChar(TOUR);
		case FOUDAME :
		case FOUROI :
			return PieceToChar(FOUBLANC);
		case CAVALIERDAME :
		case CAVALIERROI :
			return PieceToChar(CAVALIER);
		default :
			break;
	}

	if (Homme <= PIONH)
		return PieceToChar(PION);

	return '?';
}

/*************************************************************/

char PieceToChar(pieces Piece)
{
	static char Symboles[MaxPieces];
	static char Lire = true;

	if (Lire) {
		memcpy(Symboles, GetTexte(MESSAGE_SYMBOLES, MaxPieces, true), MaxPieces);
		Lire = false;
	}

	return (Piece < MaxPieces) ? Symboles[Piece] : '?';
}

/*************************************************************/

char ColonneToChar(colonnes Colonne)
{
	if (Colonne >= MaxColonnes)
		return '?';

	return (char)('a' + Colonne);
}

/*************************************************************/

char RangeeToChar(rangees Rangee)
{
	if (Rangee >= MaxRangees)
		return '?';

	return (char)('1' + Rangee);
}

/*************************************************************/

const char *CaseToString(cases Case)
{
	static char Tampon[4];

	Tampon[0] = (Case < MaxCases) ? ColonneToChar(QuelleColonne(Case)) : '?';
	Tampon[1] = (Case < MaxCases) ? RangeeToChar(QuelleRangee(Case)) : '?';
	Tampon[2] = '\0';

	return Tampon;
}

/*************************************************************/

void OutputMessageErreur(texte Message)
{
	OutputMessageErreur(GetTexte(Message, 256, false));
	OutputChrono(GetElapsedTime());
}

/*************************************************************/

void OutputNombreSolutions(unsigned int NombreSolutions, bool Duals, bool Final)
{
	if (Duals) {
		OutputResultat(GetTexte(MESSAGE_COOKED, 32, false));
	}
	else if (NombreSolutions == 0) {
		OutputResultat(GetTexte(MESSAGE_ZEROSOLUTION, 32, false));
	}
	else if (NombreSolutions == 1) {
		OutputResultat(GetTexte(Final ? MESSAGE_UNESOLUTIONUNIQUE : MESSAGE_UNESOLUTION, 32, false));
	}
	else {
		Verifier(NombreSolutions < 10000);

		char Texte[32];
		sprintf(Texte, "%u %s", NombreSolutions, GetTexte(Final ? MESSAGE_NSOLUTIONSDISTINCTES : MESSAGE_NSOLUTIONS, 27, false));
		OutputResultat(Texte);
	}

	OutputChrono(GetElapsedTime());
}

/*************************************************************/

void OutputMessage(texte Message, unsigned int Compte)
{
	Verifier(Compte < 10);

	char Tampon[1024];
	sprintf(Tampon, "%s%s...", GetTexte(Message, 256, false), ChiffresRomains[Compte]);

	OutputMessage(Tampon);
	OutputChrono(GetElapsedTime());
}

/*************************************************************/

static FILE *Output = NULL;

/*************************************************************/

void OutputFile(const char *Fichier)
{
	if (!Output && Fichier)
		Output = fopen(Fichier, "w");
}

/*************************************************************/

void OutputEntete(const char *PositionEPD, unsigned int DemiCoups) 
{
	if (!Output)
		Output = fopen("Output.txt", "w");

	if (!Output)
		return;

	char Tampon[1024];
	unsigned int NombreDieses = strlen(EUCLIDE_VERSION) + 6;
	memset(Tampon, '#', NombreDieses);
	Tampon[NombreDieses] = '\0';

	fprintf(Output, "\n%s\n## %s ##\n%s\n\n", Tampon, EUCLIDE_VERSION, Tampon);

	unsigned int Blancs = 0;
	unsigned int Noirs = 0;
	unsigned int i = 0;
	unsigned int k = 0;
	do {
		if (i >= sizeof(Tampon) - 1)
			break;

		if (!isspace(PositionEPD[k]))
			Tampon[i++] = PositionEPD[k];
	
		Blancs += ((PositionEPD[k] >= 'A') && (PositionEPD[k] <= 'Z')) ? 1 : 0;
		Noirs += ((PositionEPD[k] >= 'a') && (PositionEPD[k] <= 'z')) ? 1 : 0;

	} while (PositionEPD[++k]);

	Tampon[i] = '\0';		
	fprintf(Output, "%s\n\t%s\n\t%u\n\n", GetTexte(MESSAGE_PROBLEMEANALYSE, 256, false), Tampon, DemiCoups);

	fprintf(Output, "\t+---+---+---+---+---+---+---+---+\n\t");
	for (k = 0; PositionEPD[k]; k++) {
		if (isspace(PositionEPD[k]))
			continue;

		if (isdigit(PositionEPD[k]))
			for (i = 0; i < (unsigned int)(PositionEPD[k] - '0'); i++)
				fprintf(Output, "|   ");

		if (isalpha(PositionEPD[k]))
			fprintf(Output, "| %c ", PositionEPD[k]);
			
		if (PositionEPD[k] == '/')
			fprintf(Output, "|\n\t+---+---+---+---+---+---+---+---+\n\t");
	}

	while (isspace(PositionEPD[--k]) && (k > 0))
		;

	if (PositionEPD[k] != '/')
		fprintf(Output, "|\n\t+---+---+---+---+---+---+---+---+\n\t");

	sprintf(Tampon, "%u%s%c ", DemiCoups / 2, GetTexte(MESSAGE_VIRGULE, 1, true), (DemiCoups % 2) ? '5' : '0');
	strcat(Tampon, GetTexte(MESSAGE_COUPS, 32, false));
	strcat(Tampon, "                                 ");
	sprintf(&Tampon[33 - (5 + ((Blancs >= 10) ? 1 : 0) + ((Noirs >= 10) ? 1 : 0))], "(%d+%d)", Blancs, Noirs);
	fprintf(Output, "%s\n\n", Tampon);

	fflush(Output);
}

/*************************************************************/

void OutputStrategieOmise(const strategie *Strategie)
{
	if (!Output)
		return;

	fprintf(Output, "%s #%u\n", GetTexte(MESSAGE_STRATEGIEOMISE, 32, false), Strategie->IDFinal);
	fflush(Output);
}

/*************************************************************/

void OutputContinuerDe(unsigned int ContinuerDe)
{
	if (!Output)
		return;

	if (ContinuerDe <= 1)
		return;

	fprintf(Output, "%s #%u\n", GetTexte(MESSAGE_REPRISE, 64, false), ContinuerDe);
	fflush(Output);
}	

/*************************************************************/

void OutputPiedDePage(unsigned int NombreSolutions, bool Duals, bool Escape)
{
	if (!Output)
		return;

	fprintf(Output, "\n%s\n", GetTexte(MESSAGE_VERDICT, 32, false));
	
	if (Duals) {
		fprintf(Output, "\t%s\n", GetTexte(MESSAGE_COOKED, 32, false));
	}
	else if (Escape) {
		fprintf(Output, "\t%s\n", GetTexte(MESSAGE_ANALYSEINTERROMPUE, 32, false));
	}
	else if (NombreSolutions == 0) {
		fprintf(Output, "\t%s\n", GetTexte(MESSAGE_ZEROSOLUTION, 32, false));
	}
	else if (NombreSolutions == 1) {
	fprintf(Output, "\t%s\n", GetTexte(MESSAGE_UNESOLUTIONUNIQUE, 32, false));
	}
	else {
		fprintf(Output, "\t%u %s\n", NombreSolutions, GetTexte(MESSAGE_NSOLUTIONSDISTINCTES, 32, false));
	}

	fprintf(Output, "\t%s %s\n\n", GetElapsedTime(), GetTexte(MESSAGE_SECONDES, 32, false));
	fflush(Output);
}

/*************************************************************/

const solution *SolutionReference = NULL;

void OutputSolution(const solution *Solution, unsigned int Numero, bool Dual)
{
	if (!Output)
		return;

	if (!Dual)
		SolutionReference = Solution;

	fprintf(Output, "\n%s #%u :\n", GetTexte(MESSAGE_SOLUTION, 64, false), Numero);
	fprintf(Output, "---------------------------------------------------------------------\n");

	for (unsigned int k = 0; k < Solution->DemiCoups; k++) {
		const deplacement *Deplacement = &Solution->Deplacements[k];

		if ((k % 2) == 0)
			fprintf(Output, "%2u. ", (k / 2) + 1);

		if (Dual && (memcmp(Deplacement, &SolutionReference->Deplacements[k], sizeof(deplacement)) == 0)) {
			fprintf(Output, "...      ");
		}
		else if (Deplacement->Roque) {
			if (QuelleColonne(Deplacement->Vers) == C)
				fprintf(Output, "0-0-0    ");
			else
				fprintf(Output, "0-0      ");
		}
		else {
			fprintf(Output, "%c%s%c%c%c%c%c ", Deplacement->Promotion ? HommeToChar(Deplacement->Qui) : PieceToChar(Deplacement->TypePiece), CaseToString(Deplacement->De), (Deplacement->Mort == MaxHommes) ? '-' : 'x', ColonneToChar(QuelleColonne(Deplacement->Vers)), RangeeToChar(QuelleRangee(Deplacement->Vers)), Deplacement->Promotion ? '=' : (Deplacement->EnPassant ? 'e' : ' '), Deplacement->Promotion ? PieceToChar(Deplacement->TypePiece) : (Deplacement->EnPassant ? 'p' : ' '));
		}

		if ((k % 2) == 1)
			fprintf(Output, " ");

		if ((k % 6) == 5)
			if (k != (Solution->DemiCoups - 1))
				fprintf(Output, "\n");
	}

	fprintf(Output, "\n\n");
	fflush(Output);
}

/*************************************************************/
