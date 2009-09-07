#include <stdio.h>
#include <stdlib.h>
#include "Erreur.h"
#include "Epd.h"
#include "Main.h"
#include "Output.h"

/*************************************************************/

void LireLeFichier(const char *Nom);

/*************************************************************/

int main(int NombreArguments, char *Arguments[])
{
	MainStart();

	switch (NombreArguments) {
		case 2 :
			LireLeFichier(Arguments[1]);
			break;
		case 3 :
			Main(Arguments[1], atoi(Arguments[2]));
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

void LireLeFichier(const char *Nom)
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

	fgets(TamponA, sizeof(TamponA), File);
	if (!fgets(TamponB, sizeof(TamponB), File))
		Encore = false;

	char *EPD = TamponA;
	char *DemiCoups = TamponB;

	while (Encore) {
		unsigned int Coups = 0;
				
		if (sscanf(DemiCoups, "%u", &Coups) == 1) {
			diagramme *Diagramme = EPDToDiagramme(EPD, Coups, false);
			
			if (Diagramme) {
				Encore = Main(EPD, Coups);
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