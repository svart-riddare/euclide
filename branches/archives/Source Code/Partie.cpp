#include <limits.h>
#include <memory.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "Constantes.h"
#include "Coups.h"
#include "Erreur.h"
#include "HashTables.h"
#include "Output.h"
#include "Partie.h"

/*************************************************************/

static undo UndoTable[MaxUndos];

/*************************************************************/

etatdujeu *CreerPositionInitiale(unsigned int Strategie);
contraintes *CreerContraintes(pseudopartie *PseudoPartie, unsigned int DemiCoups);
void FigerCertainesPieces(const pseudopartie *PseudoPartie, etatdujeu *Position);
int TriDesCoups(const void *A, const void *B);
bool IsEchec(const etatdujeu *Position, cases Case, couleurs CouleurRoi);
bool IsEncoreEchec(const etatdujeu *Position, cases Case, couleurs CouleurRoi, const deplacement *Deplacement);
bool IsEchecDecouverte(const etatdujeu *Position, cases CaseRoi, couleurs CouleurRoi, cases AncienneCase, cases NouvelleCase);
bool IsEchecDecouverteDouble(const etatdujeu *Position, cases PositionDuRoi, couleurs CouleurRoi, colonnes ColonneA, colonnes ColonneB, rangees Rangee);
void DeterminerLesCoupsPossibles(const etatdujeu *Position, deplacement *Deplacements, unsigned int *NombreDeplacements);
bool FonctionRecursivePrincipale(etatdujeu *Partie, contraintes *Contraintes, undo *Undos, solution *Solutions, unsigned int MaxSolutions, unsigned int *NombreSolutions, bool PriseEnPassantObligatoire);
void ModifyWithUndo(undo *Undos, unsigned int *Nombre, unsigned int *Pointeur, unsigned int NouvelleValeur);
void ModifyWithUndo(undo *Undos, unsigned int *Nombre, bool *Pointeur, bool NouvelleValeur);
void ModifyWithUndo(undo *Undos, unsigned int *Nombre, coup **Pointeur, coup *NouvelleValeur);
void ModifyWithUndo(undo *Undos, unsigned int *Nombre, const vie **Pointeur, vie *NouvelleValeur);

/*************************************************************/

unsigned int GenerationDesSolutions(pseudopartie *PseudoPartie, unsigned int DemiCoups, solution *Solutions, unsigned int MaxSolutions)
{
	etatdujeu *PositionInitiale = CreerPositionInitiale(PseudoPartie->Strategie->IDFinal);
	contraintes *Contraintes = CreerContraintes(PseudoPartie, DemiCoups);
	FigerCertainesPieces(PseudoPartie, PositionInitiale);
	InitialisationDesTablesDeDeplacementsBis();

	if (PositionInitiale->Strategie == 1)
		InitHashTables();

	unsigned int NombreSolutions = 0;

	try {
		FonctionRecursivePrincipale(PositionInitiale, Contraintes, UndoTable, Solutions, MaxSolutions, &NombreSolutions, false);
	}
	catch (int NombreSolutions) {
		return (unsigned int)NombreSolutions;
	}

	return NombreSolutions;
}

/*************************************************************/

void ModifyWithUndo(undo *Undos, unsigned int *Nombre, unsigned int *Pointeur, unsigned int NouvelleValeur)
{
	Undos[*Nombre].TypeUndo = UndoUnsignedInt;
	Undos[*Nombre].Pointeur.UnsignedInt = Pointeur;
	Undos[*Nombre].AncienneValeur.UnsignedInt = *Pointeur;

	*Pointeur = NouvelleValeur;
	*Nombre += 1;
}

void ModifyWithUndo(undo *Undos, unsigned int *Nombre, bool *Pointeur, bool NouvelleValeur)
{
	Undos[*Nombre].TypeUndo = UndoBool;
	Undos[*Nombre].Pointeur.Bool = Pointeur;
	Undos[*Nombre].AncienneValeur.Bool = *Pointeur;

	*Pointeur = NouvelleValeur;
	*Nombre += 1;
}

void ModifyWithUndo(undo *Undos, unsigned int *Nombre, coup **Pointeur, coup *NouvelleValeur)
{
	Undos[*Nombre].TypeUndo = UndoCoup;
	Undos[*Nombre].Pointeur.Coup = Pointeur;
	Undos[*Nombre].AncienneValeur.Coup = *Pointeur;

	*Pointeur = NouvelleValeur;
	*Nombre += 1;
}

void ModifyWithUndo(undo *Undos, unsigned int *Nombre, const vie **Pointeur, vie *NouvelleValeur)
{
	Undos[*Nombre].TypeUndo = UndoVie;
	Undos[*Nombre].Pointeur.Vie = Pointeur;
	Undos[*Nombre].AncienneValeur.Vie = *Pointeur;

	*Pointeur = NouvelleValeur;
	*Nombre += 1;
}

/*************************************************************/
/* To prevent stack overflows ! ******************************/
/*************************************************************/

static deplacement MemoireDeplacements[MaxCoups][MaxDeplacements];

/*************************************************************/

bool FonctionRecursivePrincipale(etatdujeu *Partie, contraintes *Contraintes, undo *Undos, solution *Solutions, unsigned int MaxSolutions, unsigned int *NombreSolutions, bool PriseEnPassantObligatoire)
{
	static FILE *Debug = NULL;
/*
	if (!Partie->DemiCoups)
		Debug = fopen("Debug.txt", "a");*/

	static unsigned int Bonjour = 0;
	static unsigned int UseHashTables = true;

	Bonjour++;

	static bool AfficherDebutPartie = false;
	if (AfficherDebutPartie && (Partie->DemiCoups == 10)) {
		AfficherDebutPartie = false;
		OutputDebutPartie(&Partie->Deplacements[0]);

		int escape = IsEscape();
		if (escape)
			throw -escape;
	}

	if (Partie->DemiCoups >= Contraintes->DemiCoups) {
		bool Solution = true;

		for (unsigned int k = 0; k < MaxHommes; k++)
			if (!Partie->PositionFinaleBlancheAtteinte[k] || !Partie->PositionFinaleNoireAtteinte[k])
				Solution = false;

		for (unsigned int k = 0; k < MaxCases; k++)
			if (Partie->Couleurs[k] != Contraintes->Couleurs[k])
				Solution = false;

		if (Solution) {
			Solutions[*NombreSolutions].DemiCoups = Partie->DemiCoups;
			for (unsigned int k = 0; k < Partie->DemiCoups; k++)
				Solutions[*NombreSolutions].Deplacements[k] = *Partie->Deplacements[k];

			*NombreSolutions += 1;

			if (Debug) {
				char Space[MaxCoups + 2];
				memset(Space, ' ', Partie->DemiCoups + 1);
				Space[Partie->DemiCoups + 1] = '\0';

				fprintf(Debug, "%sSolution #%u : ", Space, *NombreSolutions);

				for (unsigned int k = 0; k < Partie->DemiCoups; k++)
					fprintf(Debug, "%c%s%c%c%c ", Partie->Deplacements[k]->Promotion ? HommeToChar(Partie->Deplacements[k]->Qui) : PieceToChar(Partie->Deplacements[k]->TypePiece), CaseToString(Partie->Deplacements[k]->De), (Partie->Deplacements[k]->Mort == MaxHommes) ? '-' : 'x', ColonneToChar(QuelleColonne(Partie->Deplacements[k]->Vers)), RangeeToChar(QuelleRangee(Partie->Deplacements[k]->Vers)));

				fprintf(Debug, "\n");
				fflush(Debug);
			}

			if (*NombreSolutions >= MaxSolutions)
				throw (int)MaxSolutions;
		}

		return true;
	}

	deplacement *Deplacements = MemoireDeplacements[Partie->DemiCoups];
	unsigned int NombreDeplacements = 0;
	unsigned int NombreUndos = 0;

	DeterminerLesCoupsPossibles(Partie, Deplacements, &NombreDeplacements);
	Verifier(NombreDeplacements < MaxDeplacements);

	coup **CoupsEnOrdre = (Partie->Trait == BLANCS) ? Contraintes->CoupsBlancsEnOrdre : Contraintes->CoupsNoirsEnOrdre;
	unsigned int *NombreCoupsJoues = (Partie->Trait == BLANCS) ? Contraintes->CoupsBlancsJoues : Contraintes->CoupsNoirsJoues;
	unsigned int *NombreCoupsRequis = (Partie->Trait == BLANCS) ? Contraintes->CoupsBlancsRequis : Contraintes->CoupsNoirsRequis;
	unsigned int *CoupsLibres = (Partie->Trait == BLANCS) ? &Contraintes->CoupsLibresBlancs : &Contraintes->CoupsLibresNoirs;
	coup **PremiersCoups = (Partie->Trait == BLANCS) ? Contraintes->PremiersCoupsBlancs : Contraintes->PremiersCoupsNoirs;
	coup **DerniersCoups = (Partie->Trait == BLANCS) ? Contraintes->DerniersCoupsBlancs : Contraintes->DerniersCoupsNoirs;
	unsigned int *ProchaineEcheance = (Partie->Trait == BLANCS) ? &Contraintes->ProchaineEcheanceBlanche : &Contraintes->ProchaineEcheanceNoire;
	unsigned int NombreCoupsRepertories = (Partie->Trait == BLANCS) ? Contraintes->NombreCoupsBlancs : Contraintes->NombreCoupsNoirs;
	coup *CoupSuivant = (Partie->Trait == BLANCS) ? ((*ProchaineEcheance < NombreCoupsRepertories) ? Contraintes->CoupsBlancsEnOrdre[*ProchaineEcheance] : NULL) : ((*ProchaineEcheance < NombreCoupsRepertories) ? Contraintes->CoupsNoirsEnOrdre[*ProchaineEcheance] : NULL);
	bool *PositionFinaleAtteinte = (Partie->Trait == BLANCS) ? Partie->PositionFinaleBlancheAtteinte : Partie->PositionFinaleNoireAtteinte;
	bool *GrandRoquePossible = (Partie->Trait == BLANCS) ? &Partie->GrandRoqueBlancPossible : &Partie->GrandRoqueNoirPossible;
	bool *PetitRoquePossible = (Partie->Trait == BLANCS) ? &Partie->PetitRoqueBlancPossible : &Partie->PetitRoqueNoirPossible;
	cases *PositionsFinales = (Partie->Trait == BLANCS) ? Contraintes->PositionsFinalesBlanches : Contraintes->PositionsFinalesNoires;
	bool *CettePiecePeutJouer = (Partie->Trait == BLANCS) ? Partie->CettePieceBlanchePeutJouer : Partie->CettePieceNoirePeutJouer;
	bool AuMoinsUneSolution = false;

	for (unsigned int k = 0; k < NombreDeplacements; k++) {
		Partie->Deplacements[Partie->DemiCoups] = &Deplacements[k];
		hommes Qui = Deplacements[k].Qui;
		hommes Mort = Deplacements[k].Mort;
		cases De = Deplacements[k].De;
		cases Vers = Deplacements[k].Vers;
		bool PriseEnPassantAuProchainCoup = false;

		Verifier(Qui < MaxHommes);
		coup *Coup = PremiersCoups[Qui];

		if (PriseEnPassantObligatoire && !Deplacements[k].EnPassant)
			continue;

		if (Partie->DemiCoups == 6)
			AfficherDebutPartie = true;

		if (Coup && CoupSuivant && (Coup != CoupSuivant))
			if (CoupSuivant->AuPlusTard <= Partie->DemiCoups)
				continue;

		if ((Deplacements[k].TypePiece == PION) && !Deplacements[k].Promotion) {
			// Cas #1 : La pièce qui se déplace est un pion

			if (!Coup)
				continue;

			if (Coup->AuPlusTot > (Partie->DemiCoups + 1))
				continue;

			bool Prerequis = true;
			for (unsigned int s = 0; s < Coup->NombreDoitSuivre; s++)
				if (!Coup->DoitSuivre[s]->CoupJoue)
					Prerequis = false;

			if (Coup->Vers < MaxCases) {
				// Cas #1a : Ce coup n'est pas possiblement un double pas

				if (Vers != Coup->Vers) {
					// Cas #1a-bis : Double pas pour prise en passant.

					if (Coup->Assassin && (abs((int)Vers - (int)De) == 2) && (abs((int)Vers - (int)Coup->Vers) == 1) && (Coup->Assassin->Scenario->Homme >= PIONA)) {
						PriseEnPassantAuProchainCoup = true;
					}
					else {
						continue;
					}
				}

				Verifier((Coup->De >= MaxCases) || (Coup->De == De));
				Verifier(Coup->NombreDeCoups == 1);

				if (Mort < MaxHommes)
					if (!Coup->Victime || (Mort != Coup->Victime->Scenario->Homme))
						continue;

				if (!Prerequis)
					continue;

				ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);

				if (Coup == DerniersCoups[Qui]) {
					ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], NULL);
					ModifyWithUndo(Undos, &NombreUndos, &PositionFinaleAtteinte[Qui], true);
					ModifyWithUndo(Undos, &NombreUndos, &CettePiecePeutJouer[Qui], false);
				}
				else {
					ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], PremiersCoups[Qui] + 1);
				}
			}
			else if (abs((int)Vers - (int)De) == 1) {
				// Cas #1b : Le double pas est possible mais on effectue un pas simple

				Verifier(Coup != DerniersCoups[Qui]);

				if (Mort < MaxHommes)
					continue;

				if (*CoupsLibres < 1)
					continue;

				if (!Prerequis)
					continue;

				ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);
				ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], PremiersCoups[Qui] + 1);
				ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - 1);
				ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + 1);
			}
			else {
				// Cas #1c : On effectue le double pas

				coup *CoupBis = Coup + 1;

				if ((Coup->De != De) || (CoupBis->Vers != Vers))
					continue;

				for (unsigned int t = 0; t < CoupBis->NombreDoitSuivre; t++)
					if (!CoupBis->DoitSuivre[t]->CoupJoue)
						if (CoupBis->DoitSuivre[t] != Coup)
							Prerequis = false;

				if (!Prerequis)
					continue;

				if (CoupBis->AuPlusTot > (Partie->DemiCoups + 1))
					continue;

				ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);
				ModifyWithUndo(Undos, &NombreUndos, &CoupBis->CoupJoue, true);

				if (CoupBis == DerniersCoups[Qui]) {
					ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], NULL);
					ModifyWithUndo(Undos, &NombreUndos, &PositionFinaleAtteinte[Qui], true);
					ModifyWithUndo(Undos, &NombreUndos, &CettePiecePeutJouer[Qui], false);
				}
				else {
					ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], PremiersCoups[Qui] + 2);
				}
			}
		}
		else if (Coup) {
			// Cas #2 : On peut considérer que la pièce qui joue n'est pas un pion

			bool Prerequis = true;
			for (unsigned int s = 0; s < Coup->NombreDoitSuivre; s++)
				if (!Coup->DoitSuivre[s]->CoupJoue)
					Prerequis = false;

			if (Coup->AuPlusTot > (Partie->DemiCoups + 1))
				Prerequis = false;

			if (Coup->Vers < MaxCases) {
				if (Coup->Vers == Vers) {
					// Cas #2a : On complète le coup

					if (Mort < MaxHommes) {
						if (!Coup->Victime || (Mort != Coup->Victime->Scenario->Homme))
							continue;
					}
					else {
						if (Coup->Victime)
							Prerequis = false;
					}

					if (!Prerequis) {
						if (*CoupsLibres < 2)
							continue;

						if (Mort < MaxHommes)
							ModifyWithUndo(Undos, &NombreUndos, &Coup->Victime, NULL);

						ModifyWithUndo(Undos, &NombreUndos, &Coup->NombreDeCoups, 2);
						ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - 2);
						ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + 2);
					}
					else {
						Verifier(Coup->NombreDeCoups == 1);
						ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);

						if (Coup == DerniersCoups[Qui]) {
							ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], NULL);
							ModifyWithUndo(Undos, &NombreUndos, &PositionFinaleAtteinte[Qui], true);

							if (*CoupsLibres < 2)
								ModifyWithUndo(Undos, &NombreUndos, &CettePiecePeutJouer[Qui], false);
						}
						else {
							ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], PremiersCoups[Qui] + 1);
						}
					}
				}
				else {
					// Cas #2b : On joue un coup intermédiaire

					if (Mort < MaxHommes)
						continue;

					if (Deplacements[k].Promotion)
						continue;

					if (Coup->Roque && !Deplacements[k].Roque)
						continue;

					unsigned int CoupsRestants = Coup->NombreDeCoups;
					Verifier(CoupsRestants > 0);
					CoupsRestants--;

					unsigned int NouveauxCoupsRestants = CombienDeDeplacementsBis(Deplacements[k].TypePiece, Vers, Coup->Vers);
					if (NouveauxCoupsRestants == UINT_MAX)
						continue;

					if (CoupsRestants < NouveauxCoupsRestants) {
						unsigned int CoupsExtra = NouveauxCoupsRestants - CoupsRestants;
						if (CoupsExtra > *CoupsLibres)
							continue;

						ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + CoupsExtra);
						ModifyWithUndo(Undos, &NombreUndos, &Coup->NombreDeCoups, Coup->NombreDeCoups + CoupsExtra - 1);
						ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - CoupsExtra);
					}
					else {
						ModifyWithUndo(Undos, &NombreUndos, &Coup->NombreDeCoups, CoupsRestants);
					}
				}
			}
			else {
				Verifier(Coup->De < MaxCases);
				coup *CoupBis = Coup + 1;

				if (CoupBis->Vers == Vers) {
					// Cas #2c : On joue un coup composé d'une seule traite

					for (unsigned int t = 0; t < CoupBis->NombreDoitSuivre; t++)
						if (!CoupBis->DoitSuivre[t]->CoupJoue)
							if (CoupBis->DoitSuivre[t] != Coup)
								Prerequis = false;

					if (CoupBis->AuPlusTot > (Partie->DemiCoups + 1))
						Prerequis = false;

					if (Mort < MaxHommes)
						if (!CoupBis->Victime || (Mort != CoupBis->Victime->Scenario->Homme))
							continue;

					if (!Prerequis) {
						if (*CoupsLibres < 2)
							continue;

						if (Mort < MaxHommes)
							ModifyWithUndo(Undos, &NombreUndos, &CoupBis->Victime, NULL);

						ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);
						ModifyWithUndo(Undos, &NombreUndos, &CoupBis->NombreDeCoups, 2);
						ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - 2);
						ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + 2);
					}
					else {
						Verifier(CoupBis->NombreDeCoups == 1);
						ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);
						ModifyWithUndo(Undos, &NombreUndos, &CoupBis->CoupJoue, true);

						if (CoupBis == DerniersCoups[Qui]) {
							ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], NULL);
							ModifyWithUndo(Undos, &NombreUndos, &PositionFinaleAtteinte[Qui], true);

							if (*CoupsLibres < 2)
								ModifyWithUndo(Undos, &NombreUndos, &CettePiecePeutJouer[Qui], false);
						}
						else {
							ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], PremiersCoups[Qui] + 2);
						}
					}
				}
				else {
					// Cas #2d : On ne fait que quitter une case

					if (!Prerequis)
						continue;

					if (Mort < MaxHommes)
						continue;

					unsigned int CoupsRestants = CoupBis->NombreDeCoups;
					Verifier(CoupsRestants > 0);
					CoupsRestants--;

					unsigned int NouveauxCoupsRestants = CombienDeDeplacementsBis(Deplacements[k].TypePiece, Vers, CoupBis->Vers);
					if (NouveauxCoupsRestants == UINT_MAX)
						continue;

					Verifier(NouveauxCoupsRestants > 0);
					if (CoupsRestants < NouveauxCoupsRestants) {
						unsigned int CoupsExtra = NouveauxCoupsRestants - CoupsRestants;
						if (CoupsExtra > *CoupsLibres)
							continue;

						ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + CoupsExtra);
						ModifyWithUndo(Undos, &NombreUndos, &CoupBis->NombreDeCoups, CoupBis->NombreDeCoups + CoupsExtra - 1);
						ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - CoupsExtra);
						ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], CoupBis);
						ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);
					}
					else {
						ModifyWithUndo(Undos, &NombreUndos, &CoupBis->NombreDeCoups, CoupsRestants);
						ModifyWithUndo(Undos, &NombreUndos, &PremiersCoups[Qui], CoupBis);						
						ModifyWithUndo(Undos, &NombreUndos, &Coup->CoupJoue, true);
					}
				}
			}
		}
		else {
			// Cas #3 : Il n'y a aucun coup correspondant (switchback / circuit)

			if (Mort < MaxHommes)
				continue;

			if (PositionFinaleAtteinte[Qui]) {
				if (*CoupsLibres < 2)
					continue;

				ModifyWithUndo(Undos, &NombreUndos, &PositionFinaleAtteinte[Qui], false);
				ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + 2);
				ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - 2);
			}
			else {
				unsigned int CoupsRestants = NombreCoupsRequis[Qui] - NombreCoupsJoues[Qui];
				CoupsRestants--;

				unsigned int NouveauxCoupsRestants = CombienDeDeplacementsBis(Deplacements[k].TypePiece, Vers, PositionsFinales[Qui]);
				if (NouveauxCoupsRestants == UINT_MAX)
					continue;

				Verifier(NouveauxCoupsRestants >= CoupsRestants);

				if (CoupsRestants < NouveauxCoupsRestants) {
					unsigned int CoupsExtra = NouveauxCoupsRestants - CoupsRestants;
					if (CoupsExtra > *CoupsLibres)
						continue;

					ModifyWithUndo(Undos, &NombreUndos, &NombreCoupsRequis[Qui], NombreCoupsRequis[Qui] + CoupsExtra);
					ModifyWithUndo(Undos, &NombreUndos, CoupsLibres, *CoupsLibres - CoupsExtra);
				}

				if (Vers == PositionsFinales[Qui])
					ModifyWithUndo(Undos, &NombreUndos, &PositionFinaleAtteinte[Qui], true);
			}
		}

		/* Jouer le coup retenu */

		unsigned int NouvelleProchaineEcheance = *ProchaineEcheance;
		while ((NouvelleProchaineEcheance < NombreCoupsRepertories) && (CoupsEnOrdre[NouvelleProchaineEcheance]->CoupJoue))
			NouvelleProchaineEcheance++;

		if (NouvelleProchaineEcheance != *ProchaineEcheance)
			ModifyWithUndo(Undos, &NombreUndos, ProchaineEcheance, NouvelleProchaineEcheance);

		ModifyWithUndo(Undos, &NombreUndos, &Partie->Pieces[Vers], Partie->Pieces[De]);
		ModifyWithUndo(Undos, &NombreUndos, &Partie->Hommes[Vers], Qui);
		ModifyWithUndo(Undos, &NombreUndos, &Partie->Couleurs[Vers], Partie->Trait);
		ModifyWithUndo(Undos, &NombreUndos, &Partie->Pieces[De], VIDE);
		ModifyWithUndo(Undos, &NombreUndos, &Partie->Hommes[De], MaxHommes);
		ModifyWithUndo(Undos, &NombreUndos, &Partie->Couleurs[De], NEUTRE);

		if (Deplacements[k].Roque) {
			cases TourDe = QuelleCase(A, QuelleRangee(Vers));
			cases TourVers = QuelleCase(D, QuelleRangee(Vers));

			if (QuelleColonne(Vers) == G) {
				TourDe = QuelleCase(H, QuelleRangee(Vers));
				TourVers = QuelleCase(F, QuelleRangee(Vers));
			}

			ModifyWithUndo(Undos, &NombreUndos, &Partie->Pieces[TourVers], Partie->Pieces[TourDe]);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Hommes[TourVers], Partie->Hommes[TourDe]);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Couleurs[TourVers], Partie->Trait);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Pieces[TourDe], VIDE);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Hommes[TourDe], MaxHommes);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Couleurs[TourDe], NEUTRE);
		}

		if (Deplacements[k].Promotion) {
			Verifier(Coup);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Pieces[Vers], Coup->Piece->Scenario->Piece);
			Deplacements[k].TypePiece = Coup->Piece->Scenario->Piece;
		}

		if (Deplacements[k].EnPassant) {
			cases Mort = QuelleCase(Partie->PriseEnPassantPossible, CINQ);
			if (Partie->Trait == NOIRS)
				Mort = QuelleCase(Partie->PriseEnPassantPossible, QUATRE);

			ModifyWithUndo(Undos, &NombreUndos, &Partie->Pieces[Mort], VIDE);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Hommes[Mort], MaxHommes);
			ModifyWithUndo(Undos, &NombreUndos, &Partie->Couleurs[Mort], NEUTRE);
		}

		colonnes NouvellePriseEnPassantPossible = MaxColonnes;
		if (Deplacements[k].TypePiece == PION)
			if (abs((int)QuelleRangee(De) - (int)QuelleRangee(Vers)) == 2)
				NouvellePriseEnPassantPossible = QuelleColonne(De);

		if (NouvellePriseEnPassantPossible != Partie->PriseEnPassantPossible)
			ModifyWithUndo(Undos, &NombreUndos, &Partie->PriseEnPassantPossible, NouvellePriseEnPassantPossible);

		if ((Qui == TOURDAME) || (Qui == XROI))
			if (*GrandRoquePossible)
				ModifyWithUndo(Undos, &NombreUndos, GrandRoquePossible, false);

		if ((Qui == TOURROI) || (Qui == XROI))
			if (*PetitRoquePossible)
				ModifyWithUndo(Undos, &NombreUndos, PetitRoquePossible, false);

		Partie->DemiCoups++;
		Partie->Trait = (Partie->Trait == BLANCS) ? NOIRS : BLANCS;
		NombreCoupsJoues[Qui]++;

		if (Debug) {
			char Output[MaxCoups + 2];
			memset(Output, ' ', Partie->DemiCoups);
			Output[Partie->DemiCoups] = '\0';

			fprintf(Debug, "%s%c%s%c%c%c%c%c\n", Output, Deplacements[k].Promotion ? HommeToChar(Qui) : PieceToChar(Deplacements[k].TypePiece), CaseToString(De), (Deplacements[k].Mort == MaxHommes) ? '-' : 'x', ColonneToChar(QuelleColonne(Vers)), RangeeToChar(QuelleRangee(Vers)), Deplacements[k].Promotion ? '=' : ' ', Deplacements[k].Promotion ? PieceToChar(Deplacements[k].TypePiece) : ' ');
			fflush(Debug);
		}

		if (UseHashTables) {
			unsigned int HashIndex = ComputeHashIndex(Partie->Couleurs, Partie->DemiCoups);

			if (PriseEnPassantAuProchainCoup || !IsPositionIn(Partie, HashIndex)) {
				bool UneSolution = FonctionRecursivePrincipale(Partie, Contraintes, &Undos[NombreUndos], Solutions, MaxSolutions, NombreSolutions, PriseEnPassantAuProchainCoup);

				if (!UneSolution && !PriseEnPassantAuProchainCoup)
					InsertPosition(Partie, HashIndex);

				AuMoinsUneSolution |= UneSolution;
			}
			else if (Debug) {
				char Output[MaxCoups + 2];
				memset(Output, ' ', Partie->DemiCoups);
				Output[Partie->DemiCoups] = '\0';

				fprintf(Debug, "%sPosition déja examinée !\n", Output);
			}
		}
		else {
			AuMoinsUneSolution |= FonctionRecursivePrincipale(Partie, Contraintes, &Undos[NombreUndos], Solutions, MaxSolutions, NombreSolutions, PriseEnPassantAuProchainCoup);
		}

		NombreCoupsJoues[Qui]--;
		Partie->DemiCoups--;
		Partie->Trait = (Partie->Trait == BLANCS) ? NOIRS : BLANCS;

		for (unsigned int n = NombreUndos; n-- > 0; ) {
			switch (Undos[n].TypeUndo) {
				case UndoUnsignedInt :
					*Undos[n].Pointeur.UnsignedInt = Undos[n].AncienneValeur.UnsignedInt;
					break;
				case UndoBool :
					*Undos[n].Pointeur.Bool = Undos[n].AncienneValeur.Bool;
					break;
				case UndoCoup :
					*Undos[n].Pointeur.Coup = Undos[n].AncienneValeur.Coup;
					break;
				case UndoVie :
					*Undos[n].Pointeur.Vie = Undos[n].AncienneValeur.Vie;
					break;
				default :
					Verifier(Undos[n].TypeUndo < 0);  // false!
					break;
			}
		}

		NombreUndos = 0;
	}

	if (Partie->DemiCoups == 0) {
		if (!Debug)
			Debug = fopen("Debug.txt", "a");

		fprintf(Debug, "\nNombre d'appels à la fonction récursive : %u\n", Bonjour);
		fclose(Debug);
		Debug = NULL;
	}

	return AuMoinsUneSolution;
}

/*************************************************************/

void FigerCertainesPieces(const pseudopartie *PseudoPartie, etatdujeu *Position)
{
	for (unsigned int k = 0; k < MaxHommes; k++) {
		const vie *PieceBlanche = &PseudoPartie->Strategie->PiecesBlanches[k];
		const vie *PieceNoire = &PseudoPartie->Strategie->PiecesNoires[k];

		Position->CettePieceBlanchePeutJouer[k] = true;
		Position->CettePieceNoirePeutJouer[k] = true;

		if (!PieceBlanche->Coups) {
			if (k >= PIONA)
				Position->CettePieceBlanchePeutJouer[k] = false;

			if (PseudoPartie->Strategie->CoupsLibresBlancs < 2)
				Position->CettePieceBlanchePeutJouer[k] = false;
		}

		if (!PieceNoire->Coups) {
			if (k >= PIONA)
				Position->CettePieceNoirePeutJouer[k] = false;

			if (PseudoPartie->Strategie->CoupsLibresNoirs < 2)
				Position->CettePieceNoirePeutJouer[k] = false;
		}

		Position->PositionFinaleBlancheAtteinte[k] = (PieceBlanche->Coups == 0);
		Position->PositionFinaleNoireAtteinte[k] = (PieceNoire->Coups == 0);
	}

	Position->GrandRoqueBlancPossible = PseudoPartie->Strategie->GrandRoqueBlanc;
	Position->PetitRoqueBlancPossible = PseudoPartie->Strategie->PetitRoqueBlanc;
	Position->GrandRoqueNoirPossible = PseudoPartie->Strategie->GrandRoqueNoir;
	Position->PetitRoqueNoirPossible = PseudoPartie->Strategie->PetitRoqueNoir;
}

/*************************************************************/

etatdujeu *CreerPositionInitiale(unsigned int Strategie)
{
	static etatdujeu Position;

	Position.Strategie = Strategie;
	Position.DemiCoups = 0;
	Position.Trait = BLANCS;

	Position.GrandRoqueBlancPossible = true;
	Position.PetitRoqueBlancPossible = true;
	Position.GrandRoqueNoirPossible = true;
	Position.PetitRoqueNoirPossible = true;
	Position.PriseEnPassantPossible = MaxColonnes;

	for (cases Case = A1; Case <= H8; Case++) {
		rangees Rangee = QuelleRangee(Case);

		Position.Pieces[Case] = VIDE;
		Position.Hommes[Case] = MaxHommes;
		Position.Couleurs[Case] = NEUTRE;

		if (Rangee <= DEUX)
			Position.Couleurs[Case] = BLANCS;

		if (Rangee >= SEPT)
			Position.Couleurs[Case] = NOIRS;

		if (Rangee == DEUX) {
			Position.Pieces[Case] = PION;
			Position.Hommes[Case] = PIONA + QuelleColonne(Case);
		}

		if (Rangee == SEPT) {
			Position.Pieces[Case] = PION;
			Position.Hommes[Case] = PIONA + QuelleColonne(Case);
		}
	}

	Position.Pieces[A1] = TOUR;
	Position.Pieces[B1] = CAVALIER;
	Position.Pieces[C1] = FOUNOIR;
	Position.Pieces[D1] = DAME;
	Position.Pieces[E1] = ROI;
	Position.Pieces[F1] = FOUBLANC;
	Position.Pieces[G1] = CAVALIER;
	Position.Pieces[H1] = TOUR;

	Position.Pieces[A8] = TOUR;
	Position.Pieces[B8] = CAVALIER;
	Position.Pieces[C8] = FOUBLANC;
	Position.Pieces[D8] = DAME;
	Position.Pieces[E8] = ROI;
	Position.Pieces[F8] = FOUNOIR;
	Position.Pieces[G8] = CAVALIER;
	Position.Pieces[H8] = TOUR;

	Position.Hommes[A1] = TOURDAME;
	Position.Hommes[B1] = CAVALIERDAME;
	Position.Hommes[C1] = FOUDAME;
	Position.Hommes[D1] = XDAME;
	Position.Hommes[E1] = XROI;
	Position.Hommes[F1] = FOUROI;
	Position.Hommes[G1] = CAVALIERROI;
	Position.Hommes[H1] = TOURROI;

	Position.Hommes[A8] = TOURDAME;
	Position.Hommes[B8] = CAVALIERDAME;
	Position.Hommes[C8] = FOUDAME;
	Position.Hommes[D8] = XDAME;
	Position.Hommes[E8] = XROI;
	Position.Hommes[F8] = FOUROI;
	Position.Hommes[G8] = CAVALIERROI;
	Position.Hommes[H8] = TOURROI;

	return &Position;
}

/*************************************************************/

int TriDesCoups(const void *A, const void *B)
{
	const coup *CoupA = *(const coup **)A;
	const coup *CoupB = *(const coup **)B;

	if (CoupA->AuPlusTard < CoupB->AuPlusTard)
		return -1;

	if (CoupA->AuPlusTard > CoupB->AuPlusTard)
		return 1;

	return 0;
}

/*************************************************************/

contraintes *CreerContraintes(pseudopartie *PseudoPartie, unsigned int DemiCoups)
{
	static contraintes Contraintes;

	for (cases Case = A1; Case < MaxCases; Case++)
		Contraintes.Couleurs[Case] = NEUTRE;

	for (unsigned int k = 0; k < MaxHommes; k++) {
		unsigned int IndexBlanc = PseudoPartie->IndexPremiersCoupsBlancs[k];
		unsigned int IndexNoir = PseudoPartie->IndexPremiersCoupsNoirs[k];

		coup *CoupBlanc = NULL;
		coup *CoupNoir = NULL;

		if (IndexBlanc < UINT_MAX)
			CoupBlanc = &PseudoPartie->CoupsBlancs[IndexBlanc];

		if (IndexNoir < UINT_MAX)
			CoupNoir = &PseudoPartie->CoupsNoirs[IndexNoir];

		Contraintes.PremiersCoupsBlancs[k] = CoupBlanc;
		Contraintes.PremiersCoupsNoirs[k] = CoupNoir;

		while (CoupBlanc && !CoupBlanc->Dernier)
			if ((CoupBlanc->Type != PION) && (CoupBlanc->Type != CoupBlanc[1].Type))
				CoupBlanc->Dernier = true;
			else
				CoupBlanc++;

		while (CoupNoir && !CoupNoir->Dernier)
			if ((CoupNoir->Type != PION) && (CoupNoir->Type != CoupNoir[1].Type))
				CoupNoir->Dernier = true;
			else
				CoupNoir++;

		Contraintes.DerniersCoupsBlancs[k] = CoupBlanc;
		Contraintes.DerniersCoupsNoirs[k] = CoupNoir;

		Contraintes.NombreCoupsBlancs = PseudoPartie->NombreCoupsBlancs;
		Contraintes.NombreCoupsNoirs = PseudoPartie->NombreCoupsNoirs;
		Contraintes.DemiCoups = DemiCoups;
		unsigned int i;

		for (i = 0; i < Contraintes.NombreCoupsBlancs; i++)
			Contraintes.CoupsBlancsEnOrdre[i] = &PseudoPartie->CoupsBlancs[i];

		for (i = 0; i < Contraintes.NombreCoupsNoirs; i++)
			Contraintes.CoupsNoirsEnOrdre[i] = &PseudoPartie->CoupsNoirs[i];

		qsort(Contraintes.CoupsBlancsEnOrdre, Contraintes.NombreCoupsBlancs, sizeof(coup *), TriDesCoups);
		qsort(Contraintes.CoupsNoirsEnOrdre, Contraintes.NombreCoupsNoirs, sizeof(coup *), TriDesCoups);

		Contraintes.CoupsBlancsJoues[k] = 0;
		Contraintes.CoupsNoirsJoues[k] = 0;

		Contraintes.CoupsBlancsRequis[k] = PseudoPartie->Strategie->PiecesBlanches[k].Coups;
		Contraintes.CoupsNoirsRequis[k] = PseudoPartie->Strategie->PiecesNoires[k].Coups;

		Contraintes.PositionsFinalesBlanches[k] = PseudoPartie->Strategie->PiecesBlanches[k].Scenario->CaseFinale;
		Contraintes.PositionsFinalesNoires[k] = PseudoPartie->Strategie->PiecesNoires[k].Scenario->CaseFinale;

		if (!PseudoPartie->Strategie->PiecesBlanches[k].Capturee)
			Contraintes.Couleurs[PseudoPartie->Strategie->PiecesBlanches[k].Scenario->CaseFinale] = BLANCS;

		if (!PseudoPartie->Strategie->PiecesNoires[k].Capturee)
			Contraintes.Couleurs[PseudoPartie->Strategie->PiecesNoires[k].Scenario->CaseFinale] = NOIRS;
	}

	Contraintes.CoupsLibresBlancs = PseudoPartie->Strategie->CoupsLibresBlancs;
	Contraintes.CoupsLibresNoirs = PseudoPartie->Strategie->CoupsLibresNoirs;

	Contraintes.ProchaineEcheanceBlanche = 0;
	Contraintes.ProchaineEcheanceNoire = 0;

	return &Contraintes;
}

/*************************************************************/

bool IsEchecDecouverte(const etatdujeu *Position, cases CaseRoi, couleurs CouleurRoi, cases AncienneCase, cases NouvelleCase)
{
	colonnes AncienneColonne = QuelleColonne(AncienneCase);
	rangees AncienneRangee = QuelleRangee(AncienneCase);
	colonnes ColonneRoi = QuelleColonne(CaseRoi);
	rangees RangeeRoi = QuelleRangee(CaseRoi);

	int DeltaColonne = (int)AncienneColonne - (int)ColonneRoi;
	int DeltaRangee = (int)AncienneRangee - (int)RangeeRoi;

	int AbsDeltaColonne = abs(DeltaColonne);
	int AbsDeltaRangee = abs(DeltaRangee);

	if ((DeltaColonne != 0) && (DeltaRangee != 0) && (AbsDeltaColonne != AbsDeltaRangee))
		return false;

	int StepColonne = DeltaColonne ? DeltaColonne / AbsDeltaColonne : 0;
	int StepRangee = DeltaRangee ? DeltaRangee / AbsDeltaRangee : 0;
	int k;

	for (k = 1; ; k++) {
		int Colonne = (int)ColonneRoi + StepColonne * k;
		int Rangee = (int)RangeeRoi + StepRangee * k;

		if ((Colonne < (int)A) || (Colonne > (int)H))
			break;

		if ((Rangee < (int)UN) || (Rangee > (int)HUIT))
			break;

		cases Case = QuelleCase(Colonne, Rangee);

		if (Case == AncienneCase)
			break;

		if (Case == NouvelleCase)
			return false;

		if (Position->Pieces[Case] != VIDE)
			return false;
	}

	for (k = 1; ; k++) {
		int DeltaColonneTotal = DeltaColonne + StepColonne * k;
		int DeltaRangeeTotal = DeltaRangee + StepRangee * k;

		int NouvelleColonne = (int)ColonneRoi + DeltaColonneTotal;
		int NouvelleRangee = (int)RangeeRoi + DeltaRangeeTotal;

		if ((NouvelleColonne < (int)A) || (NouvelleColonne > (int)H))
			break;

		if ((NouvelleRangee < (int)UN) || (NouvelleRangee > (int)HUIT))
			break;

		cases Case = QuelleCase(NouvelleColonne, NouvelleRangee);

		if (NouvelleCase == Case)
			return false;

		if (Position->Pieces[Case] == VIDE)
			continue;

		if (Position->Couleurs[Case] == CouleurRoi)
			return false;

		if (StepColonne * StepRangee == 0) {
			if (Position->Pieces[Case] == TOUR)
				return true;
		}
		else {
			if ((Position->Pieces[Case] == FOUBLANC) || (Position->Pieces[Case] == FOUNOIR))
				return true;
		}

		if (Position->Pieces[Case] == DAME)
			return true;

		break;
	}

	return false;
}

/*************************************************************/

bool IsEncoreEchec(const etatdujeu *Position, cases Case, couleurs CouleurRoi, const deplacement *Deplacement)
{
	etatdujeu *XPosition = const_cast<etatdujeu *>(Position);

	couleurs ExCouleur = Position->Couleurs[Deplacement->Vers];
	pieces ExPiece = Position->Pieces[Deplacement->Vers];
	hommes ExHomme = Position->Hommes[Deplacement->Vers];
	hommes ExPion = MaxHommes;

	XPosition->Couleurs[Deplacement->De] = NEUTRE;
	XPosition->Pieces[Deplacement->De] = VIDE;
	XPosition->Hommes[Deplacement->De] = MaxHommes;

	XPosition->Couleurs[Deplacement->Vers] = CouleurRoi;
	XPosition->Pieces[Deplacement->Vers] = Deplacement->TypePiece;
	XPosition->Hommes[Deplacement->Vers] = Deplacement->Qui;

	if (Deplacement->EnPassant) {
		cases Mort = QuelleCase(QuelleColonne(Deplacement->Vers), QuelleRangee(Deplacement->De));
		ExPion = XPosition->Hommes[Mort];

		XPosition->Couleurs[Mort] = NEUTRE;
		XPosition->Pieces[Mort] = VIDE;
		XPosition->Hommes[Mort] = MaxHommes;
	}

	bool Resultat = IsEchec(Position, Case, CouleurRoi);

	if (Deplacement->EnPassant) {
		cases Mort = QuelleCase(QuelleColonne(Deplacement->Vers), QuelleRangee(Deplacement->De));

		XPosition->Couleurs[Mort] = (CouleurRoi == BLANCS) ? NOIRS : BLANCS;
		XPosition->Pieces[Mort] = PION;
		XPosition->Hommes[Mort] = ExPion;
	}

	XPosition->Couleurs[Deplacement->Vers] = ExCouleur;
	XPosition->Pieces[Deplacement->Vers] = ExPiece;
	XPosition->Hommes[Deplacement->Vers] = ExHomme;

	XPosition->Couleurs[Deplacement->De] = CouleurRoi;
	XPosition->Pieces[Deplacement->De] = Deplacement->TypePiece;
	XPosition->Hommes[Deplacement->De] = Deplacement->Qui;

	return Resultat;
}

/*************************************************************/

bool IsEchec(const etatdujeu *Position, cases Case, couleurs CouleurRoi)
{
	colonnes ColonneRoi = QuelleColonne(Case);
	rangees RangeeRoi = QuelleRangee(Case);

	if ((ColonneRoi > A) && (RangeeRoi < SEPT)) {
		cases Cavalier = QuelleCase(ColonneRoi - 1, RangeeRoi + 2);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi < H) && (RangeeRoi < SEPT)) {
		cases Cavalier = QuelleCase(ColonneRoi + 1, RangeeRoi + 2);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi < G) && (RangeeRoi < HUIT)) {
		cases Cavalier = QuelleCase(ColonneRoi + 2, RangeeRoi + 1);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi < G) && (RangeeRoi > UN)) {
		cases Cavalier = QuelleCase(ColonneRoi + 2, RangeeRoi - 1);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi < H) && (RangeeRoi > DEUX)) {
		cases Cavalier = QuelleCase(ColonneRoi + 1, RangeeRoi - 2);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi > A) && (RangeeRoi > DEUX)) {
		cases Cavalier = QuelleCase(ColonneRoi - 1, RangeeRoi - 2);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi > B) && (RangeeRoi > UN)) {
		cases Cavalier = QuelleCase(ColonneRoi - 2, RangeeRoi - 1);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	if ((ColonneRoi > B) && (RangeeRoi < HUIT)) {
		cases Cavalier = QuelleCase(ColonneRoi - 2, RangeeRoi + 1);

		if (Position->Pieces[Cavalier] == CAVALIER)
			if (Position->Couleurs[Cavalier] != CouleurRoi)
				return true;
	}

	for (int DeltaR = -1; DeltaR <= 1; DeltaR++) {
		for (int DeltaC = -1; DeltaC <= 1; DeltaC++) {
			if ((DeltaR == 0) && (DeltaC == 0))
				continue;

			for (int k = 1; ; k++) {
				int Colonne = (int)ColonneRoi + DeltaC * k;
				int Rangee = (int)RangeeRoi + DeltaR * k;

				if ((Colonne < (int)A) || (Colonne > (int)H))
					break;

				if ((Rangee < (int)UN) || (Rangee > (int)HUIT))
					break;

				cases Case = QuelleCase(Colonne, Rangee);
				pieces Piece = Position->Pieces[Case];

				if (Piece == VIDE)
					continue;

				if (Position->Couleurs[Case] == CouleurRoi) {
					if (Piece == ROI)
						continue;
					else
						break;
				}

				if (k == 1)
					if (Piece == ROI)
						return true;

				if (DeltaR * DeltaC == 0) {
					if ((Piece == TOUR) || (Piece == DAME))
						return true;
				}
				else {
					if ((Piece == FOUBLANC) || (Piece == FOUNOIR) || (Piece == DAME))
						return true;

					if ((Piece == PION) && (k == 1)) {
						if ((CouleurRoi == BLANCS) && (DeltaR == 1))
							return true;

						if ((CouleurRoi == NOIRS) && (DeltaR == -1))
							return true;
					}
				}

				break;
			}
		}
	}

	return false;
}

/*************************************************************/

bool IsEchecDecouverteDouble(const etatdujeu *Position, cases PositionDuRoi, couleurs CouleurRoi, colonnes ColonneA, colonnes ColonneB, rangees Rangee)
{
	colonnes ColonneGauche = (ColonneA < ColonneB) ? ColonneA : ColonneB;
	colonnes ColonneDroite = ColonneGauche + 1;

	colonnes ColonneRoi = QuelleColonne(PositionDuRoi);
	rangees RangeeRoi = QuelleRangee(PositionDuRoi);

	if (RangeeRoi != Rangee)
		return false;

	colonnes Colonne;
	if (ColonneRoi < ColonneGauche) {
		for (Colonne = ColonneRoi; Colonne < ColonneGauche; Colonne++)
			if (Position->Pieces[QuelleCase(Colonne, Rangee)] != VIDE)
				return false;

		for (Colonne = ColonneDroite + 1; Colonne < MaxColonnes; Colonne++) {
			cases Case = QuelleCase(Colonne, Rangee);

			if (Position->Pieces[Case] == VIDE)
				continue;

			if (Position->Couleurs[Case] == CouleurRoi)
				return false;

			if ((Position->Pieces[Case] == TOUR) || (Position->Pieces[Case] == DAME))
				return true;

			break;
		}
	}
	else {
		for (Colonne = ColonneRoi; Colonne > ColonneDroite; Colonne++)
			if (Position->Pieces[QuelleCase(Colonne, Rangee)] != VIDE)
				return false;

		for (Colonne = ColonneGauche; ColonneGauche-- > A; ) {
			cases Case = QuelleCase(Colonne, Rangee);

			if (Position->Pieces[Case] == VIDE)
				continue;

			if (Position->Couleurs[Case] == CouleurRoi)
				return false;

			if ((Position->Pieces[Case] == TOUR) || (Position->Pieces[Case] == DAME))
				return true;

			break;
		}
	}

	return false;
}

/*************************************************************/

void DeterminerLesCoupsPossibles(const etatdujeu *Position, deplacement *Deplacements, unsigned int *NombreDeplacements)
{
	couleurs Couleur = Position->Trait;
	deplacement *Deplacement = &Deplacements[0];

	cases CaseDuRoi = MaxCases;
	for (cases Case = A1; (CaseDuRoi == MaxCases) && (Case <= H8); Case++)
		if ((Position->Pieces[Case] == ROI) && (Position->Couleurs[Case] == Couleur))
			CaseDuRoi = Case;

	const bool *CettePiecePeutJouer = (Couleur == BLANCS) ? Position->CettePieceBlanchePeutJouer : Position->CettePieceNoirePeutJouer;

	for (cases De = A1; De <= H8; De++) {
		if (Position->Couleurs[De] != Couleur)
			continue;

		if (!CettePiecePeutJouer[Position->Hommes[De]])
			continue;

		colonnes ColonneDe = QuelleColonne(De);
		rangees RangeeDe = QuelleRangee(De);

		if (Position->Pieces[De] == ROI) {
			if (Position->GrandRoqueBlancPossible && (De == E1) && (Couleur == BLANCS) && (Position->Pieces[A1] == TOUR) && (Position->Couleurs[A1] == BLANCS) && (Position->Pieces[B1] == VIDE) && (Position->Pieces[C1] == VIDE) && (Position->Pieces[D1] == VIDE)) {
				Deplacement->TypePiece = ROI;
				Deplacement->De = De;
				Deplacement->Vers = C1;
				Deplacement->Qui = XROI;
				Deplacement->Mort = MaxHommes;
				Deplacement->Roque = true;
				Deplacement->EnPassant = false;
				Deplacement->Promotion = false;

				if (!IsEchec(Position, C1, BLANCS) && !IsEchec(Position, D1, BLANCS) && !IsEchec(Position, E1, BLANCS))
					Deplacement++;
			}

			if (Position->GrandRoqueNoirPossible && (De == E8) && (Couleur == NOIRS) && (Position->Pieces[A8] == TOUR) && (Position->Couleurs[A8] == NOIRS) && (Position->Pieces[B8] == VIDE) && (Position->Pieces[C8] == VIDE) && (Position->Pieces[D8] == VIDE)) {
				Deplacement->TypePiece = ROI;
				Deplacement->De = De;
				Deplacement->Vers = C8;
				Deplacement->Qui = XROI;
				Deplacement->Mort = MaxHommes;
				Deplacement->Roque = true;
				Deplacement->EnPassant = false;
				Deplacement->Promotion = false;

				if (!IsEchec(Position, C8, NOIRS) && !IsEchec(Position, D8, NOIRS) && !IsEchec(Position, E8, NOIRS))
					Deplacement++;
			}

			if (Position->PetitRoqueBlancPossible && (De == E1) && (Couleur == BLANCS) && (Position->Pieces[H1] == TOUR) && (Position->Couleurs[H1] == BLANCS) && (Position->Pieces[F1] == VIDE) && (Position->Pieces[G1] == VIDE)) {
				Deplacement->TypePiece = ROI;
				Deplacement->De = De;
				Deplacement->Vers = G1;
				Deplacement->Qui = XROI;
				Deplacement->Mort = MaxHommes;
				Deplacement->Roque = true;
				Deplacement->EnPassant = false;
				Deplacement->Promotion = false;

				if (!IsEchec(Position, E1, BLANCS) && !IsEchec(Position, F1, BLANCS) && !IsEchec(Position, G1, BLANCS))
					Deplacement++;
			}

			if (Position->PetitRoqueNoirPossible && (De == E8) && (Couleur == NOIRS) && (Position->Pieces[H8] == TOUR) && (Position->Couleurs[H8] == NOIRS) && (Position->Pieces[F8] == VIDE) && (Position->Pieces[G8] == VIDE)) {
				Deplacement->TypePiece = ROI;
				Deplacement->De = De;
				Deplacement->Vers = G8;
				Deplacement->Qui = XROI;
				Deplacement->Mort = MaxHommes;
				Deplacement->Roque = true;
				Deplacement->EnPassant = false;
				Deplacement->Promotion = false;

				if (!IsEchec(Position, E8, NOIRS) && !IsEchec(Position, F8, NOIRS) && !IsEchec(Position, G8, NOIRS))
					Deplacement++;
			}

			for (int DeltaR = -1; DeltaR <= 1; DeltaR++) {
				for (int DeltaC = -1; DeltaC <= 1; DeltaC++) {
					if ((DeltaR == 0) && (DeltaC == 0))
						continue;

					if ((DeltaC < 0) && (ColonneDe == A))
						continue;

					if ((DeltaR < 0) && (RangeeDe == UN))
						continue;

					if ((DeltaC > 0) && (ColonneDe == H))
						continue;

					if ((DeltaR > 0) && (RangeeDe == HUIT))
						continue;

					cases Vers = QuelleCase((int)ColonneDe + DeltaC, (int)RangeeDe + DeltaR);

					if ((Position->Couleurs[Vers] != Couleur) && !IsEchec(Position, Vers, Couleur)) {
						Deplacement->TypePiece = ROI;
						Deplacement->De = De;
						Deplacement->Vers = Vers;
						Deplacement->Qui = XROI;
						Deplacement->Mort = Position->Hommes[Vers];
						Deplacement->Roque = false;
						Deplacement->EnPassant = false;
						Deplacement->Promotion = false;
						Deplacement++;
					}
				}
			}
		}
		else {
			bool AttentionEchecDecouverte = IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, MaxCases);
			bool AttentionRoiEnEchec = IsEchec(Position, CaseDuRoi, Couleur);

			if (Position->Pieces[De] == CAVALIER) {
				if (AttentionEchecDecouverte)
					continue;

				for (int DeltaColonne = -2; DeltaColonne <= 2; DeltaColonne++) {
					for (int DeltaRangee = -2; DeltaRangee <= 2; DeltaRangee++) {
						if ((abs(DeltaColonne) + abs(DeltaRangee)) != 3)
							continue;

						int NouvelleColonne = (int)ColonneDe + DeltaColonne;
						int NouvelleRangee = (int)RangeeDe + DeltaRangee;

						if ((NouvelleColonne < (int)A) || (NouvelleColonne > (int)H))
							continue;

						if ((NouvelleRangee < (int)UN) || (NouvelleRangee > (int)HUIT))
							continue;

						int NouvelleCase = QuelleCase(NouvelleColonne, NouvelleRangee);
						if (Position->Couleurs[NouvelleCase] != Couleur) {
							Deplacement->TypePiece = CAVALIER;
							Deplacement->De = De;
							Deplacement->Vers = NouvelleCase;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = Position->Hommes[NouvelleCase];
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = false;

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;
						}
					}
				}
			}

			if ((Position->Pieces[De] == TOUR) || (Position->Pieces[De] == DAME)) {
				for (int DeltaColonne = -1; DeltaColonne <= 1; DeltaColonne += 2) {
					for (int Step = 1; ; Step++) {
						int NouvelleColonne = (int)ColonneDe + DeltaColonne * Step;

						if ((NouvelleColonne < (int)A) || (NouvelleColonne > (int)H))
							break;

						int NouvelleCase = QuelleCase(NouvelleColonne, RangeeDe);
						if (Position->Couleurs[NouvelleCase] != Couleur) {
							if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, NouvelleCase)) {
								Deplacement->TypePiece = Position->Pieces[De];
								Deplacement->De = De;
								Deplacement->Vers = NouvelleCase;
								Deplacement->Qui = Position->Hommes[De];
								Deplacement->Mort = Position->Hommes[NouvelleCase];
								Deplacement->Roque = false;
								Deplacement->EnPassant = false;
								Deplacement->Promotion = false;

								Verifier(Deplacement->Qui < MaxHommes);
								if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
									Deplacement++;
							}
						}

						if (Position->Pieces[NouvelleCase])
							break;
					}
				}

				for (int DeltaRangee = -1; DeltaRangee <= 1; DeltaRangee += 2) {
					for (int Step = 1; ; Step++) {
						int NouvelleRangee = (int)RangeeDe + DeltaRangee * Step;

						if ((NouvelleRangee < (int)UN) || (NouvelleRangee > (int)HUIT))
							break;

						int NouvelleCase = QuelleCase(ColonneDe, NouvelleRangee);
						if (Position->Couleurs[NouvelleCase] != Couleur) {
							if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, NouvelleCase)) {
								Deplacement->TypePiece = Position->Pieces[De];
								Deplacement->De = De;
								Deplacement->Vers = NouvelleCase;
								Deplacement->Qui = Position->Hommes[De];
								Deplacement->Mort = Position->Hommes[NouvelleCase];
								Deplacement->Roque = false;
								Deplacement->EnPassant = false;
								Deplacement->Promotion = false;

								Verifier(Deplacement->Qui < MaxHommes);
								if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
									Deplacement++;
							}
						}

						if (Position->Pieces[NouvelleCase])
							break;
					}
				}
			}

			if ((Position->Pieces[De] == DAME) || (Position->Pieces[De] == FOUBLANC) || (Position->Pieces[De] == FOUNOIR)) {
				for (int DeltaColonne = -1; DeltaColonne <= 1; DeltaColonne += 2) {
					for (int DeltaRangee = -1; DeltaRangee <= 1; DeltaRangee += 2) {
						for (int Step = 1; ; Step++) {
							int NouvelleColonne = (int)ColonneDe + DeltaColonne * Step;
							int NouvelleRangee = (int)RangeeDe + DeltaRangee * Step;

							if ((NouvelleColonne < (int)A) || (NouvelleColonne > (int)H))
								break;

							if ((NouvelleRangee < (int)UN) || (NouvelleRangee > (int)HUIT))
								break;

							int NouvelleCase = QuelleCase(NouvelleColonne, NouvelleRangee);
							if (Position->Couleurs[NouvelleCase] != Couleur) {
								if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, NouvelleCase)) {
									Deplacement->TypePiece = Position->Pieces[De];
									Deplacement->De = De;
									Deplacement->Vers = NouvelleCase;
									Deplacement->Qui = Position->Hommes[De];
									Deplacement->Mort = Position->Hommes[NouvelleCase];
									Deplacement->Roque = false;
									Deplacement->EnPassant = false;
									Deplacement->Promotion = false;

									Verifier(Deplacement->Qui < MaxHommes);
									if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
										Deplacement++;
								}
							}

							if (Position->Pieces[NouvelleCase])
								break;
						}
					}
				}
			}

			if (Position->Pieces[De] == PION) {
				if (Couleur == BLANCS) {
					if (Position->Pieces[De + 1] == VIDE) {
						if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, De + 1)) {
							Deplacement->TypePiece = PION;
							Deplacement->De = De;
							Deplacement->Vers = De + 1;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = MaxHommes;
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = (QuelleRangee(De) == SEPT);

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;

							if ((RangeeDe == DEUX) && (Position->Pieces[De + 2] == VIDE)) {
								Deplacement->TypePiece = PION;
								Deplacement->De = De;
								Deplacement->Vers = De + 2;
								Deplacement->Qui = Position->Hommes[De];
								Deplacement->Mort = MaxHommes;
								Deplacement->Roque = false;
								Deplacement->EnPassant = false;
								Deplacement->Promotion = false;

								Verifier(Deplacement->Qui < MaxHommes);
								if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
									Deplacement++;
							}
						}
					}

					if ((De >= 7) && (Position->Couleurs[De - 7] == NOIRS)) {
						if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, De - 7)) {
							Deplacement->TypePiece = PION;
							Deplacement->De = De;
							Deplacement->Vers = De - 7;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = Position->Hommes[De - 7];
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = (QuelleRangee(De) == SEPT);

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;
						}
					}

					if ((De < MaxCases - 9) && (Position->Couleurs[De + 9] == NOIRS)) {
						if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, De + 9)) {
							Deplacement->TypePiece = PION;
							Deplacement->De = De;
							Deplacement->Vers = De + 9;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = Position->Hommes[De + 9];
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = (QuelleRangee(De) == SEPT);

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;
						}
					}

					if ((RangeeDe == CINQ) && (Position->PriseEnPassantPossible < MaxColonnes)) {
						if (abs((int)Position->PriseEnPassantPossible - (int)ColonneDe) == 1) {
							cases Vers = QuelleCase(Position->PriseEnPassantPossible, SIX);

							if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, Vers)) {
								if (!IsEchecDecouverteDouble(Position, CaseDuRoi, Couleur, Position->PriseEnPassantPossible, ColonneDe, CINQ)) {
									Deplacement->TypePiece = PION;
									Deplacement->De = De;
									Deplacement->Vers = Vers;
									Deplacement->Qui = Position->Hommes[De];
									Deplacement->Mort = Position->Hommes[Vers - 1];
									Deplacement->Roque = false;
									Deplacement->EnPassant = true;
									Deplacement->Promotion = false;

									Verifier(Deplacement->Qui < MaxHommes);
									if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
										Deplacement++;
								}
							}
						}
					}
				}
				else {
					if (Position->Pieces[De - 1] == VIDE) {
						if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, De - 1)) {
							Deplacement->TypePiece = PION;
							Deplacement->De = De;
							Deplacement->Vers = De - 1;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = MaxHommes;
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = (QuelleRangee(De) == DEUX);

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;

							if ((RangeeDe == SEPT) && (Position->Pieces[De - 2] == VIDE)) {
								Deplacement->TypePiece = PION;
								Deplacement->De = De;
								Deplacement->Vers = De - 2;
								Deplacement->Qui = Position->Hommes[De];
								Deplacement->Mort = MaxHommes;
								Deplacement->Roque = false;
								Deplacement->EnPassant = false;
								Deplacement->Promotion = false;

								Verifier(Deplacement->Qui < MaxHommes);
								if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
									Deplacement++;
							}
						}
					}

					if ((De >= 9) && (Position->Couleurs[De - 9] == BLANCS)) {
						if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, De - 9)) {
							Deplacement->TypePiece = PION;
							Deplacement->De = De;
							Deplacement->Vers = De - 9;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = Position->Hommes[De - 9];
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = (QuelleRangee(De) == DEUX);

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;
						}
					}

					if ((De < MaxCases - 7) && (Position->Couleurs[De + 7] == BLANCS)) {
						if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, De + 7)) {
							Deplacement->TypePiece = PION;
							Deplacement->De = De;
							Deplacement->Vers = De + 7;
							Deplacement->Qui = Position->Hommes[De];
							Deplacement->Mort = Position->Hommes[De + 7];
							Deplacement->Roque = false;
							Deplacement->EnPassant = false;
							Deplacement->Promotion = (QuelleRangee(De) == DEUX);

							Verifier(Deplacement->Qui < MaxHommes);
							if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
								Deplacement++;
						}
					}

					if ((RangeeDe == QUATRE) && (Position->PriseEnPassantPossible < MaxColonnes)) {
						if (abs((int)Position->PriseEnPassantPossible - (int)ColonneDe) == 1) {
							cases Vers = QuelleCase(Position->PriseEnPassantPossible, TROIS);

							if (!AttentionEchecDecouverte || !IsEchecDecouverte(Position, CaseDuRoi, Couleur, De, Vers)) {
								if (!IsEchecDecouverteDouble(Position, CaseDuRoi, Couleur, Position->PriseEnPassantPossible, ColonneDe, QUATRE)) {
									Deplacement->TypePiece = PION;
									Deplacement->De = De;
									Deplacement->Vers = Vers;
									Deplacement->Qui = Position->Hommes[De];
									Deplacement->Mort = Position->Hommes[Vers + 1];
									Deplacement->Roque = false;
									Deplacement->EnPassant = true;
									Deplacement->Promotion = false;

									Verifier(Deplacement->Qui < MaxHommes);
									if (!AttentionRoiEnEchec || !IsEncoreEchec(Position, CaseDuRoi, Couleur, Deplacement))
										Deplacement++;
								}
							}
						}
					}
				}
			}
		}
	}

	*NombreDeplacements = Deplacement - &Deplacements[0];
}

/*************************************************************/
