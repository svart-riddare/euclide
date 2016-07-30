#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include "Coups.h"
#include "Epd.h"
#include "Erreur.h"
#include "HashTables.h"
#include "Main.h"
#include "Mouvements.h"
#include "Output.h"
#include "Partie.h"
#include "Permutation.h"
#include "Position.h"
#include "Save.h"
#include "Timer.h"

/*************************************************************/

bool Main(const char *PositionEPD, unsigned int DemiCoups, const char *Contraintes[], int NombreContaintes, bool Continuer, unsigned int ContinuerDe, bool ModeExpress)
{
#ifndef NDEBUG
	FILE *Debug = fopen("Debug.txt", "w");
	fclose(Debug);
#endif

	StartTimer();
	OutputEntete(PositionEPD, DemiCoups);

	try {
		diagramme *Diagramme = EPDToDiagramme(PositionEPD, DemiCoups);
		position *Position = ExamenDuDiagramme(Diagramme);

		if (Continuer && !ContinuerDe)
			ContinuerDe = GetSauvegarde(Diagramme);

		OutputContinuerDe(ContinuerDe);
		Sauvegarde(Diagramme, ContinuerDe);

		texte Texte = (ContinuerDe > 1) ? MESSAGE_SUITE : MESSAGE_RECHERCHE;
				
		strategies *Strategies = ExamenDesStrategies(Position, (_texte)Texte);
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

				// DO NOT COMMIT
				OutputStrategie(&Strategies->StrategieActuelle, Partie);

				if (Possible)
					Possible &= AnalysePhaseC(Partie);

				// DO NOT COMMIT
				OutputStrategie(&Strategies->StrategieActuelle, Partie);

				if (Possible)
					Possible &= AnalysePhaseD(Partie, Strategies->DemiCoups);

				// DO NOT COMMIT
				OutputStrategie(&Strategies->StrategieActuelle, Partie);

				if (Possible)
					Possible &= AnalysePhaseE(Partie);

				if (Possible) {
					OutputStrategie(&Strategies->StrategieActuelle, Partie);
					
					if (Strategies->StrategieActuelle.IDFinal >= ContinuerDe) {
						OutputMessage(MESSAGE_ANALYSE);
						Texte = MESSAGE_RECHERCHE;

						solution Solutions[4];
						unsigned int NouvellesSolutions = GenerationDesSolutions(Partie, Strategies->DemiCoups, Contraintes, NombreContaintes,  Solutions, 4);
						OutputDebutPartie(NULL);

						if (NouvellesSolutions == UINT_MAX) {
							NouvellesSolutions = 0;
							Escape = true;
						}

						if (NouvellesSolutions == (UINT_MAX - 1)) {
							OutputStrategieOmise(&Strategies->StrategieActuelle);
							NouvellesSolutions = 0;
						}

						for (unsigned int k = 0; k < NouvellesSolutions; k++)
							OutputSolution(&Solutions[k], NombreSolutions + k + 1, k != 0);

						NombreSolutions += NouvellesSolutions;
						if (NouvellesSolutions > 1)
							Duals = true;

						if (NombreSolutions > 0)
							OutputNombreSolutions(NombreSolutions, Duals, false);

						if (!Escape)
							SetSauvegarde(Strategies->StrategieActuelle.IDFinal);
					}
				}

				AnalysePhaseX(&Strategies->StrategieActuelle);

				if (IsEscape() == ESCAPE_ESCAPE)
					Escape = true;
				
			} while (!Escape && !Duals && ProchaineStrategie(Strategies, (_texte)Texte));

#ifndef NDEBUG
			Debug = fopen("Debug.txt", "a");
			if (Debug) {
				fprintf(Debug, "\n");
				fprintf(Debug, "Stratégies, phase A : %u\n", Strategies->StrategieActuelle.IDPhaseA);
				fprintf(Debug, "Stratégies, phase B : %u\n", Strategies->StrategieActuelle.IDPhaseB);
				fprintf(Debug, "Stratégies, phase C : %u\n", Strategies->StrategieActuelle.IDPhaseC);
				fprintf(Debug, "Stratégies, phase D : %u\n", Strategies->StrategieActuelle.IDPhaseD);
				fprintf(Debug, "Stratégies, phase E : %u\n", Strategies->StrategieActuelle.IDPhaseE);
				fprintf(Debug, "Stratégies, phase F : %u\n", Strategies->StrategieActuelle.IDPhaseF);
				fprintf(Debug, "\n");
				fclose(Debug);
			}
#endif

			if (!Escape) {
				OutputNombreSolutions(NombreSolutions, Duals, true);
				OutputDebutPartie(NULL);
				ClearSauvegarde();
			}
		}
		else {
			OutputNombreSolutions(0, false, true);
		}

		OutputPiedDePage(NombreSolutions, Duals, Escape);

		Delete(Partie);
		Delete(Strategies);
		Delete(Position);
		Delete(Diagramme);
	}
	catch (texte /*Texte*/) {
	}

	OutputMessage(GetTexte(MESSAGE_FIN, 256, false));
	OutputChrono(GetElapsedTime());

	bool Escape = ModeExpress ? false : WaitForInput();
	OutputClear();

	return !Escape;
}

/*************************************************************/

void MainStart(unsigned int MemoireDisponible)
{
	OutputCreate();
	ChoixDeLangue();
	CreationDesTablesDeMouvement();
	CreateHashTables(MemoireDisponible);
}

/*************************************************************/

void MainEnd()
{
	DestructionDesTablesDeMouvement();
	DestructionDesTextes();
	OutputDestroy();
}

/*************************************************************/