#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Erreur.h"
#include "Epd.h"
#include "Main.h"
#include "Output.h"

/*************************************************************/

void LireLeFichier(const char *Nom, bool Continuer, unsigned int ContinuerDe, bool ModeExpress);

/*************************************************************/
/* Arguments :                                               */
/*   <filename>        : Nom du fichier à analyser           */
/*   <epd> <halfmoves> : Problème à analyser.                */
/*   -s25              : Débuter à la stratégie 25.          */
/*************************************************************/

int main(int NombreArguments, char *Arguments[])
{
	MainStart();
	
	bool Continuer = true;	
	bool ModeExpress = false;
	unsigned int ContinuerDe = 0;

	if (NombreArguments > 1) {
		if (memcmp(Arguments[1], "-s", strlen("-s")) == 0) {
			ContinuerDe = atoi(&Arguments[1][strlen("-s")]);
			if (!ContinuerDe)
				Continuer = false;

			NombreArguments--;
			Arguments++;
		}
		else if (strcmp(Arguments[1], "-batch") == 0) {
			ModeExpress = true;

			NombreArguments--;
			Arguments++;
		}
	}

	switch (NombreArguments) {
		case 2 :
			LireLeFichier(Arguments[1], Continuer, ContinuerDe, ModeExpress);
			break;
		case 3 :
			Main(Arguments[1], atoi(Arguments[2]), Continuer, ContinuerDe, ModeExpress);
			break;
			
		default :
			OutputMessageErreur(MESSAGE_MAUVAISARGUMENTS);
			WaitForInput();
			break;
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
				
		if (sscanf(DemiCoups, "%u", &Coups) == 1) {
			diagramme *Diagramme = EPDToDiagramme(EPD, Coups, false);
			
			if (Diagramme) {
				Encore = Main(EPD, Coups, Continuer, ContinuerDe, ModeExpress);
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


