#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include "Coups.h"
#include "Epd.h"
#include "Erreur.h"
#include "Main.h"
#include "Mouvements.h"
#include "Output.h"
#include "Partie.h"
#include "Permutation.h"
#include "Position.h"
#include "Timer.h"

/*************************************************************/

bool Main(const char *PositionEPD, unsigned int DemiCoups)
{
	FILE *Debug = fopen("Debug.txt", "w");
	fclose(Debug);

	StartTimer();

	try {
		diagramme *Diagramme = EPDToDiagramme(PositionEPD, DemiCoups);
		position *Position = ExamenDuDiagramme(Diagramme);
		strategies *Strategies = ExamenDesStrategies(Position);
		pseudopartie *Partie = NULL;
		unsigned int NombreSolutions = 0;
		bool Duals = false;
		bool Escape = false;

		if (Strategies) {
			do {
				bool Possible = true;

				//OutputStrategie(&Strategies->StrategieActuelle, NULL);
				Possible &= AnalysePhaseA(&Strategies->StrategieActuelle);

				if (Possible) {
					Partie = AnalysePhaseB(&Strategies->StrategieActuelle);
					if (!Partie)
						Possible = false;
				}

				if (Possible)
					Possible &= AnalysePhaseC(Partie);

				//OutputStrategie(&Strategies->StrategieActuelle, Partie);

				if (Possible)
					Possible &= AnalysePhaseD(Partie, Strategies->DemiCoups);

				if (Possible) {
					OutputStrategie(&Strategies->StrategieActuelle, Partie);
					OutputMessage(MESSAGE_ANALYSE);

					solution Solutions[4];
					unsigned int NouvellesSolutions = GenerationDesSolutions(Partie, Strategies->DemiCoups, Solutions, 4);
					OutputDebutPartie(NULL);

					if (NouvellesSolutions == UINT_MAX) {
						NouvellesSolutions = 0;
						Escape = true;
					}

					for (unsigned int k = 0; k < NouvellesSolutions; k++)
						OutputSolution(&Solutions[k], NombreSolutions + k + 1);

					NombreSolutions += NouvellesSolutions;
					if (NouvellesSolutions > 1)
						Duals = true;

					if (NombreSolutions > 0)
						OutputNombreSolutions(NombreSolutions, Duals);
				}

				AnalysePhaseX(&Strategies->StrategieActuelle);

				if (IsEscape())
					Escape = true;

			} while (!Escape && !Duals && ProchaineStrategie(Strategies));

			Debug = fopen("Debug.txt", "a");
			if (Debug) {
				fprintf(Debug, "\n");
				fprintf(Debug, "Stratégies, phase A : %u\n", Strategies->StrategieActuelle.IDPhaseA);
				fprintf(Debug, "Stratégies, phase B : %u\n", Strategies->StrategieActuelle.IDPhaseB);
				fprintf(Debug, "Stratégies, phase C : %u\n", Strategies->StrategieActuelle.IDPhaseC);
				fprintf(Debug, "Stratégies, phase D : %u\n", Strategies->StrategieActuelle.IDPhaseD);
				fprintf(Debug, "Stratégies, phase E : %u\n", Strategies->StrategieActuelle.IDPhaseE);
				fprintf(Debug, "\n");
				fclose(Debug);
			}

			if (!Escape) {
				OutputNombreSolutions(NombreSolutions, Duals);
				OutputDebutPartie(NULL);
			}
		}
		else {
			OutputNombreSolutions(0, false);
		}

		Delete(Partie);
		Delete(Strategies);
		Delete(Position);
		Delete(Diagramme);
	}
	catch (texte /*Texte*/) {
	}

	OutputMessage(GetTexte(MESSAGE_FIN, 256, false));
	OutputChrono(GetElapsedTime());

	bool Escape = WaitForInput();
	OutputClear();

	return !Escape;
}

/*************************************************************/

void MainStart()
{
	OutputCreate();
	ChoixDeLangue();
	CreationDesTablesDeMouvement();
}

/*************************************************************/

void MainEnd()
{
	DestructionDesTablesDeMouvement();
	DestructionDesTextes();
	OutputDestroy();
}

/*************************************************************/