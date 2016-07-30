#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Erreur.h"
#include "Epd.h"
#include "Main.h"
#include "Output.h"
#include "System.h"

/*************************************************************/

void LireLeFichier(const char *Nom, bool Continuer, unsigned int ContinuerDe, bool ModeExpress);

/*************************************************************/
/* Arguments :                                               */
/*   <input> [<out>]   : Fichier à analyser et de résultat.  */
/*   <epd> <halfmoves> : Problème à analyser.                */
/*   -s25              : Débuter à la stratégie 25.          */
/*   -batch            : Analyser tous les problèmes.        */
/*   -m18              : Utiliser 2^18 octets de mémoire.    */
/*************************************************************/

int main(int NombreArguments, char *Arguments[])
{
	bool Continuer = true;	
	bool ModeExpress = false;
	unsigned int ContinuerDe = 0;

	unsigned int UtilisationMemoire = MemoireDisponible();

	const char *PremierArgument = NULL;
	const char *SecondArgument = NULL;
	const char *TroisiemeArgument = NULL;

	bool ModeFichier = true;

	while (NombreArguments > 1)
	{
		if (memcmp(Arguments[1], "-s", strlen("-s")) == 0) {
			if (strlen(Arguments[1]) > strlen("-s")) {
				ContinuerDe = atoi(&Arguments[1][strlen("-s")]);
				if (ContinuerDe <= 0)
					Continuer = false;
			}
		}
		else if ((strcmp(Arguments[1], "-b") == 0) || (strcmp(Arguments[1], "-batch") == 0) || (strcmp(Arguments[1], "--batch") == 0)) {
			ModeExpress = true;
		}
		else if (memcmp(Arguments[1], "-m", strlen("-m")) == 0) {
			if (strlen(Arguments[1]) > strlen("-m"))
				UtilisationMemoire = atoi(&Arguments[1][strlen("-m")]);
		}
		else {
			if (SecondArgument)
				TroisiemeArgument = Arguments[1];
			else if (PremierArgument)
				SecondArgument = Arguments[1];
			else
				PremierArgument = Arguments[1];

			if (SecondArgument) {
				if (atoi(SecondArgument) > 0)
					ModeFichier = false;

				char Nombre[32];
				sprintf(Nombre, "%d", atoi(SecondArgument));
				if (strcmp(SecondArgument, Nombre))
					ModeFichier = true;
			}
		}

		NombreArguments--;
		Arguments++;
	}

	MainStart(UtilisationMemoire);

	if (!PremierArgument)
	{
		OutputMessageErreur(MESSAGE_MAUVAISARGUMENTS);
		WaitForInput();
	}
	else
	if (ModeFichier)
	{
		OutputFile(SecondArgument);
		LireLeFichier(PremierArgument, Continuer, ContinuerDe, ModeExpress);
	}
	else
	{
		OutputFile(TroisiemeArgument);
		Main(PremierArgument, atoi(SecondArgument), NULL, 0, Continuer, ContinuerDe, ModeExpress);
	}

    MainEnd();
	return 0;
}

/*************************************************************/

void LireLeFichier(const char *Nom, bool Continuer, unsigned int ContinuerDe, bool ModeExpress)
{
	Verifier(Nom && Nom[0]);

	FILE *File = fopen(Nom, "r");
	if (!File) {
		char Message[1024];
		sprintf(Message, "[%s] %s", Nom, GetTexte(MESSAGE_ERREURLECTURE, 512, false));
		OutputMessageErreur(Message);
		return;
	}

	char TamponA[1024];
	char TamponB[1024];

	unsigned int NombreTrouve = 0;
	bool Encore = true;

	if (!fgets(TamponA, sizeof(TamponA), File) || !fgets(TamponB, sizeof(TamponB), File))
		Encore = false;

	char *EPD = TamponA;
	char *DemiCoups = TamponB;

	while (Encore) {
		unsigned int Coups = 0;
		int Scan = 0;
				
		if (sscanf(DemiCoups, "%u%n", &Coups, &Scan) == 1) {
			diagramme *Diagramme = EPDToDiagramme(EPD, Coups, false);
			
			if (Diagramme) {
				int NombreContraintes = 0;
				const char *Contraintes[32];
				
				while (NombreContraintes < 32) {
					while (DemiCoups[Scan] && isspace(DemiCoups[Scan]))
						Scan++;

					if (DemiCoups[Scan] != '+' && DemiCoups[Scan] != '-')
						break;

					Contraintes[NombreContraintes++] = &DemiCoups[Scan];
					while (DemiCoups[Scan] && !isspace(DemiCoups[Scan]))
						Scan++;

					if (!DemiCoups[Scan])
						break;

					DemiCoups[Scan++] = '\0';
				}

				Encore = Main(EPD, Coups, Contraintes, NombreContraintes, Continuer, ContinuerDe, ModeExpress);
				Delete(Diagramme);				
				NombreTrouve++;
			}
		}

		EPD = DemiCoups;
		DemiCoups = (EPD == TamponA) ? TamponB : TamponA;

		if (Encore)
			if (!fgets(DemiCoups, sizeof(TamponB), File))
				Encore = false;
	}

	if (!NombreTrouve) {
		char Message[1024];
		sprintf(Message, "[%s] %s", Nom, GetTexte(MESSAGE_AUCUNPROBLEME, 512, false));
		OutputMessageErreur(Message);
	}

	fclose(File);
}

/*************************************************************/
