#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constantes.h"
#include "Coups.h"
#include "Erreur.h"
#include "Mouvements.h"
#include "Output.h"
#include "Permutation.h"

/*************************************************************/

static pseudopartie LaPseudoPartie;

static unsigned int CopieDuNombreDeCoupsBlancs[MaxHommes];
static unsigned int CopieDuNombreDeCoupsNoirs[MaxHommes];
static unsigned int CopieDesCoupsLibres[MaxCouleurs];

/*************************************************************/

bool InitialisationDesBlocages(const strategie *Strategie);
unsigned int CalculDesCoupsRequisAvecBlocages(const vie *Piece);
void DeterminerLesCoups(vie *Piece, const vie PiecesAdverses[MaxHommes], coup *Coups, unsigned int *NombreCoups, unsigned int CoupsLibres);
pseudopartie *CreationDeLaPseudoPartie(strategie *Strategie);
void CalculDesCoups(pseudopartie *Partie);
void CalculDesPenalites(pseudopartie *Partie);
bool PropagationDesPenalites(pseudopartie *Partie, bool *Encore);
void CalculDesLiensDePriorite(pseudopartie *Partie);
bool YATIlDesBoucles(pseudopartie *Partie);
void EliminerPrioritesEnDouble(coup *Priorites[], unsigned int *NombrePriorites, bool LePlusLoin);
void NormaliserLesPriorites(pseudopartie *Partie);
int TriDesPointeurs(const void *A, const void *B);
void PrioritesPourLesPions(pseudopartie *Partie);
void DeductionsSurLesPriorites(pseudopartie *Partie);
bool DeterminerLesSwitchback(pseudopartie *Partie);
unsigned int CombienDeSwitchback(hommes Homme, couleurs Couleur, cases De, cases *LaCase);
bool RangDesCoups(pseudopartie *Partie, unsigned int DemiCoups);

/*************************************************************/

bool AnalysePhaseA(strategie *Strategie)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		CopieDuNombreDeCoupsBlancs[i] = Strategie->PiecesBlanches[i].Coups;
		CopieDuNombreDeCoupsNoirs[i] = Strategie->PiecesNoires[i].Coups;
	}

	CopieDesCoupsLibres[BLANCS] = Strategie->CoupsLibresBlancs;
	CopieDesCoupsLibres[NOIRS] = Strategie->CoupsLibresNoirs;

	if (!InitialisationDesBlocages(Strategie))
		return false;

	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		vie *Pieces = (Couleur == BLANCS) ? Strategie->PiecesBlanches : Strategie->PiecesNoires;
		unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Strategie->CoupsLibresBlancs : &Strategie->CoupsLibresNoirs;

		for (unsigned int i = 0; i < MaxHommes; i++) {
			vie *Piece = &Pieces[i];

			unsigned int Coups = CalculDesCoupsRequisAvecBlocages(Piece);
			if (Coups == UINT_MAX)
				return false;

			Verifier(Coups >= Piece->Coups);
			unsigned int Extra = Coups - Piece->Coups;

			if (Extra > *CoupsLibres)
				return false;

			Piece->Coups = Coups;
			*CoupsLibres -= Extra;
		}
	}

	Strategie->IDPhaseB++;
	return true;
}

/*************************************************************/

pseudopartie *AnalysePhaseB(strategie *Strategie)
{
	pseudopartie *Partie = CreationDeLaPseudoPartie(Strategie);
	CalculDesCoups(Partie);
	CalculDesPenalites(Partie);

	bool Encore = true;
	while (Encore) {
		if (!PropagationDesPenalites(Partie, &Encore))
			return NULL;
	}

	if (!DeterminerLesSwitchback(Partie))
		return NULL;

	Strategie->IDPhaseC++;
	return Partie;
}

/*************************************************************/

bool AnalysePhaseC(pseudopartie *Partie)
{
	CalculDesLiensDePriorite(Partie);
	PrioritesPourLesPions(Partie);
	NormaliserLesPriorites(Partie);
	DeductionsSurLesPriorites(Partie);
	NormaliserLesPriorites(Partie);

	if (YATIlDesBoucles(Partie))
		return false;

	Partie->Strategie->IDPhaseD++;
	return true;
}

/*************************************************************/

bool AnalysePhaseD(pseudopartie *Partie, unsigned int DemiCoups)
{
	if (!RangDesCoups(Partie, DemiCoups))
		return false;

	Partie->Strategie->IDPhaseE++;
	Partie->Strategie->IDFinal = Partie->Strategie->IDPhaseE;
	return true;
}

/*************************************************************/

void Delete(pseudopartie *Partie)
{
	if (Partie)
		Partie->Strategie = NULL;
}

/*************************************************************/

bool RangDesCoups(pseudopartie *Partie, unsigned int DemiCoups)
{
	unsigned int NombreTotalCoups = Partie->NombreCoupsBlancs + Partie->NombreCoupsNoirs;

	for (unsigned int k = 0; k < NombreTotalCoups; k++) {
		couleurs Couleur = BLANCS;
		coup *Coup = &Partie->CoupsBlancs[k];

		if (k >= Partie->NombreCoupsBlancs) {
			Couleur = NOIRS;
			Coup = &Partie->CoupsNoirs[k - Partie->NombreCoupsBlancs];
		}

		Coup->AuPlusTot = (Couleur == BLANCS) ? 1 : 2;
		Coup->AuPlusTard = ((Couleur == BLANCS) ^ (DemiCoups % 2)) ? DemiCoups - 1 : DemiCoups;
	}

	bool Changements;
	do {
		Changements = false;

		for (unsigned int k = 0; k < NombreTotalCoups; k++) {
			couleurs Couleur = BLANCS;
			coup *Coup = &Partie->CoupsBlancs[k];

			if (k >= Partie->NombreCoupsBlancs) {
				Couleur = NOIRS;
				Coup = &Partie->CoupsNoirs[k - Partie->NombreCoupsBlancs];
			}

			for (unsigned int i = 0; i < Coup->NombreDoitSuivre; i++) {
				coup *Avant = Coup->DoitSuivre[i];

				Verifier(Avant != Coup);
				unsigned int Delta = 2;

				if (Avant->Vers == MaxCases) {
					if (Avant + 1 == Coup)
						Delta = 2 * (Coup->NombreDeCoups - 1);
				}

				couleurs CouleurAvant = BLANCS;
				if (Avant >= &Partie->CoupsBlancs[Partie->NombreCoupsBlancs])
					CouleurAvant = NOIRS;

				if (Couleur != CouleurAvant) {
					Verifier(Delta >= 2);
					Delta--;
				}

				if ((Delta + Avant->AuPlusTot) <= Coup->AuPlusTot)
					continue;

				Coup->AuPlusTot = Delta + Avant->AuPlusTot;
				Changements = true;

				if (Coup->AuPlusTot > DemiCoups)
					return false;
			}

			for (unsigned int j = 0; j < Coup->NombreDoitPreceder; j++) {
				coup *Apres = Coup->DoitPreceder[j];

				Verifier(Apres != Coup);
				unsigned int Delta = 2;

				if (Apres->De == MaxCases) {
					if (Coup == Apres - 1)
						Delta = 2 * (Apres->NombreDeCoups - 1);
				}

				couleurs CouleurApres = BLANCS;
				if (Apres >= &Partie->CoupsBlancs[Partie->NombreCoupsBlancs])
					CouleurApres = NOIRS;

				if (Couleur != CouleurApres) {
					Verifier(Delta >= 2);
					Delta--;
				}

				if (Apres->AuPlusTard <= Delta)
					return false;

				if (Coup->AuPlusTard <= (Apres->AuPlusTard - Delta))
					continue;

				Coup->AuPlusTard = Apres->AuPlusTard - Delta;
				Changements = true;
			}

			if (Coup->AuPlusTot > Coup->AuPlusTard)
				return false;
		}
	} while (Changements);

	return true;
}

/*************************************************************/

bool PropagationDesPenalites(pseudopartie *Partie, bool *Encore)
{
	*Encore = false;

	for (couleurs CouleurBloqueur = BLANCS; CouleurBloqueur <= NOIRS; CouleurBloqueur++) {
		unsigned int *IndexDesPremiersCoups = (CouleurBloqueur == BLANCS) ? Partie->IndexPremiersCoupsBlancs : Partie->IndexPremiersCoupsNoirs;
		coup *CoupsCouleurBloqueur = (CouleurBloqueur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
		unsigned int *CoupsLibresBloqueur = (CouleurBloqueur == BLANCS) ? &Partie->Strategie->CoupsLibresBlancs : &Partie->Strategie->CoupsLibresNoirs;
		unsigned int CoupsLibresAdversesDuBloqueur = (CouleurBloqueur == BLANCS) ? Partie->Strategie->CoupsLibresNoirs : Partie->Strategie->CoupsLibresBlancs;

		for (unsigned int k = 0; k < MaxHommes; k++) {
			if (IndexDesPremiersCoups[k] == UINT_MAX)
				continue;

			const coup *CoupBloqueur = &CoupsCouleurBloqueur[IndexDesPremiersCoups[k]];
			const vie *Bloqueur = CoupBloqueur->Piece;

			bool TempoBloqueurPermis = (*CoupsLibresBloqueur > (unsigned int)((CoupBloqueur->Type == CAVALIER) ? 1 : 0));
			bool Unique = true;

			bool IsCritique[MaxCases];
			for (int j = 0; j < MaxCases; j++)
				IsCritique[j] = false;

			cases CasesCritiques[MaxCases];
			unsigned int NumCasesCritiques = 0;
			cases DoublePas = MaxCases;

			if (!CoupBloqueur->Premier) {
				Verifier(CoupBloqueur->Type == TOUR);
				Verifier(CoupsCouleurBloqueur[0].Roque);
				cases VraiDepart = (QuelleColonne(CoupBloqueur->De) == D) ? CoupBloqueur->De - 24 : CoupBloqueur->De + 16;
				IsCritique[VraiDepart] = true;
				CasesCritiques[NumCasesCritiques++] = VraiDepart;
			}

			while (CoupBloqueur->Piece == Bloqueur) {
				if ((CoupBloqueur->De < MaxCases) && !IsCritique[CoupBloqueur->De]) {
					IsCritique[CoupBloqueur->De] = true;
					CasesCritiques[NumCasesCritiques++] = CoupBloqueur->De;
				}

				if ((CoupBloqueur->Vers < MaxCases) && !IsCritique[CoupBloqueur->Vers]) {
					IsCritique[CoupBloqueur->Vers] = true;
					CasesCritiques[NumCasesCritiques++] = CoupBloqueur->Vers;
				}

				if ((CoupBloqueur->De >= MaxCases) && (CoupBloqueur->Vers <= MaxCases))
					if (CoupBloqueur->Type != PION)
						Unique = false;

				if (DoublePas < MaxCases) {
					cases ExtraCritique = (DoublePas + CoupBloqueur->Vers) / 2;
					Verifier(!IsCritique[ExtraCritique]);
					DoublePas = MaxCases;

					if (TempoBloqueurPermis)
						CasesCritiques[NumCasesCritiques++] = ExtraCritique;
				}

				if ((CoupBloqueur->Type == PION) && (CoupBloqueur->Vers == MaxCases))
					DoublePas = CoupBloqueur->De;

				CoupBloqueur++;
			}

			if (Bloqueur->Assassin) {
				if (CoupsLibresAdversesDuBloqueur > 1)
					continue;

				if (!Bloqueur->Assassin->NombreAssassinats)
					continue;

				if (Bloqueur->Assassin->Assassinats[Bloqueur->Assassin->NombreAssassinats - 1] != Bloqueur)
					continue;

				if (Bloqueur->Assassin->Scenario->CaseFinale != Bloqueur->Scenario->CaseFinale)
					continue;

				if (Bloqueur->Assassin->Capturee)
					continue;
			}

			if (!Unique)
				continue;

			for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
				unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
				coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
				unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Partie->Strategie->CoupsLibresBlancs : &Partie->Strategie->CoupsLibresNoirs;

				if (TempoBloqueurPermis && (Bloqueur->Scenario->Piece != PION))
					continue;

				for (unsigned int i = 0; i < NombreCoups; i++) {
					coup *Coup = &Coups[i];

					if (Coup->Piece == Bloqueur)
						continue;

					if (Coup->Victime == Bloqueur)
						continue;

					if (!Coup->NombreDeCoups)
						continue;

					if (Coup->Roque && (Couleur == CouleurBloqueur) && !(CoupBloqueur - 1)->Premier)
						continue;

					unsigned int NombreMinimumDeCoups = UINT_MAX;

					for (unsigned int l = 0; l < NumCasesCritiques; l++) {
						unsigned int NCoups = Coup->NombreDeCoupsSiCaseBloquee[CasesCritiques[l]];
						if (NCoups < NombreMinimumDeCoups)
							NombreMinimumDeCoups = NCoups;
					}

					Verifier(NombreMinimumDeCoups > 0);

					if (NombreMinimumDeCoups <= Coup->NombreDeCoups)
						continue;

					unsigned int Extra = NombreMinimumDeCoups - Coup->NombreDeCoups;					

					if (Coup->De == MaxCases)
						continue;

					if (Extra > *CoupsLibres)
						return false;

					Coup->NombreDeCoups += Extra;
					Coup->Piece->Coups += Extra;
					*CoupsLibres -= Extra;
					*Encore = true;
				}
			}
		}
	}

	return true;
}

/*************************************************************/

void CalculDesCoups(pseudopartie *Partie)
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;

		for (unsigned int k = 0; k < NombreCoups; k++) {
			coup *Coup = &Coups[k];

			Verifier(Coup->De < MaxCases);

			if (Coup->Vers >= MaxCases) {
				coup *CoupDe = &Coups[k++];
				coup *CoupVers = &Coups[k];

				Verifier(CoupVers->De >= MaxCases);
				Verifier(CoupVers->Vers < MaxCases);

				if (Coup->Type != PION) {
					CoupVers->NombreDeCoups = CombienDeCoups(Coup->Type, CoupDe->De, CoupVers->Vers, true);
				}
				else {
					CoupVers->NombreDeCoups = 1;
				}
			}
			else {
				Coup->NombreDeCoups = 1;
			}
		}
	}
}

/*************************************************************/

void CalculDesPenalites(pseudopartie *Partie)
{
	for (cases Case = A1; Case < MaxCases; Case++) {
		if (CasesMortes[Case])
			continue;

		BloquerLaCase(Case, true, true);

		for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
			unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
			coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;

			for (unsigned int k = 0; k < NombreCoups; k++) {
				coup *Coup = &Coups[k];

				Verifier(Coup->De < MaxCases);

				if (Coup->Vers >= MaxCases) {
					coup *CoupDe = &Coups[k];
					coup *CoupVers = &Coups[++k];

					if (Coup->Type != PION) {
						CoupVers->NombreDeCoupsSiCaseBloquee[Case] = CombienDeCoups(Coup->Type, CoupDe->De, CoupVers->Vers, true);
						Verifier(CoupVers->NombreDeCoupsSiCaseBloquee[Case] > 0);
					}
					else {
						unsigned int NombreDeCoups = CoupVers->NombreDeCoups;
						if (Case == CoupDe->De)
							NombreDeCoups = UINT_MAX;
						if (Case == CoupVers->Vers)
							NombreDeCoups = UINT_MAX;
						if (QuelleColonne(CoupDe->De) == QuelleColonne(CoupVers->Vers))
							if (Case == QuelleCase(QuelleColonne(CoupDe->De), (QuelleRangee(CoupDe->De) + QuelleRangee(CoupVers->Vers)) / 2))
								NombreDeCoups = UINT_MAX;

						CoupVers->NombreDeCoupsSiCaseBloquee[Case] = NombreDeCoups;
					}
				}
				else {
					if (!IsCoupPossible(Coup->Type, Coup->De, Coup->Vers))
						Coup->NombreDeCoupsSiCaseBloquee[Case] = UINT_MAX;
					else
						Coup->NombreDeCoupsSiCaseBloquee[Case] = Coup->NombreDeCoups;
				}
			}
		}

		DebloquerLaCase(Case, true, true);
	}
}

/*************************************************************/

bool InitialisationDesBlocages(const strategie *Strategie)
{
	InitialisationDesBloqueurs();

	for (cases Case = A1; Case < MaxCases; Case++) {
		if (CasesMortes[Case])
			BloquerLaCase(Case, true, true);
	}

	for (unsigned int i = 0; i < MaxHommes; i++) {
		for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
			const vie *Piece = (Couleur == BLANCS) ? &Strategie->PiecesBlanches[i] : &Strategie->PiecesNoires[i];
			unsigned int CoupsLibres = (Couleur == BLANCS) ? Strategie->CoupsLibresBlancs : Strategie->CoupsLibresNoirs;
			unsigned int CoupsLibresAdverses = (Couleur == BLANCS) ? Strategie->CoupsLibresNoirs : Strategie->CoupsLibresBlancs;
			bool GrandRoque = (Couleur == BLANCS) ? Strategie->GrandRoqueBlanc : Strategie->GrandRoqueNoir;
			bool PetitRoque = (Couleur == BLANCS) ? Strategie->PetitRoqueBlanc : Strategie->PetitRoqueNoir;

			if (i >= PIONA)
				if (!TrajetPionPossible(Piece->TrajetSiPion))
					return false;

			if (!Piece->Coups && !Piece->Capturee)
				if (Piece->TrajetSiPion || (CoupsLibres < 2))
					if (((i != TOURDAME) || !GrandRoque) && ((i != TOURROI) || !PetitRoque))
						BloquerLaCase(Piece->Scenario->CaseFinale, true, true);

			if (!Piece->Coups && Piece->Capturee && (Piece->Scenario->CaseFinale == Piece->Assassin->Scenario->CaseFinale) && !Piece->Assassin->Capturee) {
				if (Piece->Assassin->NombreAssassinats && (Piece->Assassin->Assassinats[Piece->Assassin->NombreAssassinats - 1] == Piece)) {
					if ((CoupsLibresAdverses < 2) && !Piece->Assassin->Capturee)
						if (((i != TOURDAME) || !GrandRoque) && ((i != TOURROI) || !PetitRoque))
							BloquerLaCase(Piece->Scenario->CaseFinale, true, false);
				}
				else if (Piece->Assassin->TrajetSiPion) {
					if (!Piece->Assassin->Promue)
						BloquerLaCase(Piece->Scenario->CaseFinale, true, false);
				}
			}

			if (Piece->TrajetSiPion && Piece->Coups && !Piece->Capturee) {
				if (!Piece->TrajetSiPion->NombreDeCaptures) {
					if (!Piece->Promue) {
						BloquerUneLigne(Piece->Depart, Piece->Scenario->CaseFinale, PION);
					}
					else  {
						if (Piece->Scenario->CaseFinale == Piece->TrajetSiPion->CaseFinale)
							if ((Piece->Coups <= 6) && (CoupsLibres < 2))
								BloquerUneLigne(Piece->Depart, Piece->Scenario->CaseFinale, PION);
					}
				}
				else {
					if (!Piece->Promue) {
						unsigned int DeltaR = abs((int)QuelleRangee(Piece->Depart) - (int)QuelleRangee(Piece->TrajetSiPion->CaseFinale));
						unsigned int DeltaC = abs((int)QuelleColonne(Piece->Depart) - (int)QuelleColonne(Piece->TrajetSiPion->CaseFinale));
						if ((DeltaC == DeltaR) && (DeltaC == Piece->TrajetSiPion->NombreDeCaptures))
							BloquerUneLigne(Piece->Depart, Piece->TrajetSiPion->CaseFinale, PION);
					}
				}
			}

			if ((Piece->Coups == 1) && !CoupsLibres && !Piece->Capturee) {
				bool Bloquer = false;
				switch (Piece->Scenario->Homme) {
					case XROI :
						if (!GrandRoque && !PetitRoque)
							Bloquer = true;
						break;
					case XDAME :
					case FOUDAME :
					case FOUROI :
						Bloquer = true;
						break;
					case TOURDAME :
						Bloquer = !GrandRoque;
						break;
					case TOURROI :
						Bloquer = !PetitRoque;
						break;
					default :
						break;
				}

				if (Bloquer)
					BloquerUneLigne(Piece->Depart, Piece->Scenario->CaseFinale, Piece->Scenario->Piece);	
			}

			if (Piece->Scenario->Homme == XROI) {
				if (GrandRoque || PetitRoque) {
					if (!CoupsLibres && (Piece->Coups <= 1)) {
						BloquerUneLigne(QuelleCase(E, QuelleRangee(Piece->Depart)), Piece->Depart, ROI);
					}
					else if ((CoupsLibres < 2) && (Piece->Scenario->CaseFinale == Piece->Depart)) {
						BloquerUneLigne(QuelleCase(E, QuelleRangee(Piece->Depart)), Piece->Depart, ROI);
					}
				}
			}
		}
	}

	return true;
}

/*************************************************************/

unsigned int CalculDesCoupsRequisAvecBlocages(const vie *Piece)
{
	unsigned int Coups = 0;
	cases Depart = Piece->Depart;

	if (Piece->Scenario->Homme >= PIONA) {
		Coups += Piece->TrajetSiPion->NombreDeCoupsMin;

		if (!Piece->Promue)
			return Piece->Coups;

		Depart = Piece->TrajetSiPion->CaseFinale;
	}

	cases De = Depart;

	for (unsigned int k = 0; k < Piece->NombreAssassinats; k++) {
		cases Vers = Piece->Assassinats[k]->Scenario->CaseFinale;

		unsigned int Ajout = CombienDeCoups(Piece->Scenario->Piece, De, Vers, false);
		if (Ajout == UINT_MAX)
			return UINT_MAX;

		if (!Ajout)
			Ajout = 2;

		Coups += Ajout;
		De = Vers;
	}

	unsigned int Ajout = CombienDeCoups(Piece->Scenario->Piece, De, Piece->Scenario->CaseFinale, false);
	if (Ajout == UINT_MAX)
		return UINT_MAX;

	Coups += Ajout;

	if ((Piece->Scenario->Homme == XROI) && (QuelleColonne(Depart) != E))
		Coups++;

	if (!Coups && Piece->Switchback)
		Coups = 2;

	return Coups;
}

/*************************************************************/

pseudopartie *CreationDeLaPseudoPartie(strategie *Strategie)
{
	pseudopartie *Partie = &LaPseudoPartie;
	memset(Partie, 0, sizeof(pseudopartie));
	Partie->Strategie = Strategie;

	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		vie *Pieces = (Couleur == BLANCS) ? Strategie->PiecesBlanches : Strategie->PiecesNoires;
		vie *PiecesAdverses = (Couleur == BLANCS) ? Strategie->PiecesNoires : Strategie->PiecesBlanches;
		unsigned int CoupsLibres = (Couleur == BLANCS) ? Strategie->CoupsLibresBlancs : Strategie->CoupsLibresNoirs;
		bool GrandRoque = (Couleur == BLANCS) ? Strategie->GrandRoqueBlanc : Strategie->GrandRoqueNoir;
		bool PetitRoque = (Couleur == BLANCS) ? Strategie->PetitRoqueBlanc : Strategie->PetitRoqueNoir;

		unsigned int *PremiersCoups = (Couleur == BLANCS) ? Partie->IndexPremiersCoupsBlancs : Partie->IndexPremiersCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
		unsigned int *NombreCoups = (Couleur == BLANCS) ? &Partie->NombreCoupsBlancs : &Partie->NombreCoupsNoirs;

		unsigned int k = 0;

		if (GrandRoque || PetitRoque) {
			Coups[0].Piece = &Pieces[XROI];
			Coups[0].De = QuelleCase(E, QuelleRangee(Coups[0].Piece->Depart));
			Coups[0].Vers = Coups[0].Piece->Depart;
			Coups[0].Type = ROI;
			Coups[0].Roque = true;
			Coups[0].Premier = true;
			k++;
		}

		for (unsigned int i = 0; i < MaxHommes; i++) {
			vie *Piece = &Pieces[i];

			PremiersCoups[i] = k;
			DeterminerLesCoups(Piece, PiecesAdverses, &Coups[k], &k, CoupsLibres);
			Verifier(k < MaxCoups);

			if (k == PremiersCoups[i])
				PremiersCoups[i] = UINT_MAX;

			if (GrandRoque || PetitRoque) {
				if ((i == XROI) && (PremiersCoups[XROI] != UINT_MAX))
					Coups[PremiersCoups[XROI]].Premier = false;

				if (GrandRoque && (i == TOURDAME) && (PremiersCoups[TOURDAME] != UINT_MAX))
					Coups[PremiersCoups[TOURDAME]].Premier = false;

				if (PetitRoque && (i == TOURROI) && (PremiersCoups[TOURROI] != UINT_MAX))
					Coups[PremiersCoups[TOURROI]].Premier = false;

				if (i == XROI)
					PremiersCoups[XROI] = 0;
			}
		}

		*NombreCoups += k;

		Verifier((*NombreCoups + 1) < MaxCoups);
	}

	return Partie;
}

/*************************************************************/

void DeterminerLesCoups(vie *Piece, const vie PiecesAdverses[MaxHommes], coup *Coups, unsigned int *NombreCoups, unsigned int CoupsLibres)
{
	if (!Piece->Coups)
		return;

	cases Depart = Piece->Depart;
	unsigned int k = 0;
	Coups[0].Premier = true;

	if (Piece->Scenario->Homme >= PIONA) {
		int DeltaR = (QuelleRangee(Depart) == DEUX) ? 1 : -1;

		const trajetpion *Trajet = Piece->TrajetSiPion;
		unsigned int ProchaineCapture = 0;

		if ((Trajet->NombreDeCoupsMin < Trajet->NombreDeCoupsMax) && (CoupsLibres > 0)) {
			Coups[0].Piece = Piece;
			Coups[1].Piece = Piece;

			Coups[0].De = Depart;
			Coups[1].De = MaxCases;

			Coups[0].Vers = MaxCases;
			Coups[1].Vers = Depart + 2 * DeltaR;

			Coups[0].Type = PION;
			Coups[1].Type = PION;

			k += 2;
		}
		else {
			bool DeuxCases = true;

			if (Trajet->NombreDeCoupsMin == Trajet->NombreDeCoupsMax)
				DeuxCases = false;

			if (Trajet->NombreDeCaptures)
				if (abs((int)QuelleRangee(Depart) - (int)QuelleRangee(Trajet->Captures[0])) <= 2)
					DeuxCases = false;

			Coups[0].Type = PION;
			Coups[0].Piece = Piece;
			Coups[0].De = Depart;
			Coups[0].Vers = Depart + (DeuxCases ? 2 : 1) * DeltaR;
			k++;

			if (Trajet->NombreDeCaptures) {
				if (QuelleRangee(Coups[0].Vers) == QuelleRangee(Trajet->Captures[0])) {
					Coups[0].Vers = Trajet->Captures[ProchaineCapture++];
					for (unsigned int i = 0; i < MaxHommes; i++) {
						if (PiecesAdverses[i].Assassin == Piece) {
							if (PiecesAdverses[i].Scenario->CaseFinale == Coups[0].Vers) {
								Coups[0].Victime = &PiecesAdverses[i];

								bool CaptureParLaPromotion = false;
								for (unsigned int s = 0; s < Coups[0].Piece->NombreAssassinats; s++)
									if (Coups[0].Piece->Assassinats[s] == Coups[0].Victime)
										CaptureParLaPromotion = true;

								if (!CaptureParLaPromotion)
									break;
							}
						}
					}
				}
			}
		}

		while (Coups[k - 1].Vers != Trajet->CaseFinale) {
			Coups[k].Type = PION;
			Coups[k].Piece = Piece;
			Coups[k].De = Coups[k - 1].Vers;
			Coups[k].Vers = Coups[k].De + DeltaR;

			if (ProchaineCapture < Trajet->NombreDeCaptures) {
				if (QuelleRangee(Coups[k].Vers) == QuelleRangee(Trajet->Captures[ProchaineCapture])) {
					Coups[k].Vers = Trajet->Captures[ProchaineCapture++];
					for (unsigned int i = 0; i < MaxHommes; i++) {
						if (PiecesAdverses[i].Assassin == Piece) {
							if (PiecesAdverses[i].Scenario->CaseFinale == Coups[k].Vers) {
								Coups[k].Victime = &PiecesAdverses[i];

								bool CaptureParLaPromotion = false;
								for (unsigned int s = 0; s < Coups[k].Piece->NombreAssassinats; s++)
									if (Coups[k].Piece->Assassinats[s] == Coups[k].Victime)
										CaptureParLaPromotion = true;

								if (!CaptureParLaPromotion)
									break;
							}
						}
					}
				}
			}

			k++;
		}

		if (Piece->Promue)
			Coups[k - 1].Promotion = true;

		Depart = Trajet->CaseFinale;
	}

	cases De = Depart;
	cases Vers;

	for (unsigned int j = 0; j <= Piece->NombreAssassinats; j++) {
		if (j < Piece->NombreAssassinats)
			Vers = Piece->Assassinats[j]->Scenario->CaseFinale;
		else
			Vers = Piece->Scenario->CaseFinale;

		cases Chemin[MaxCases];
		bool Unique = false;

		if (!CoupsLibres || ((CoupsLibres <= 1) && (Piece->Scenario->Piece == CAVALIER)))
			if (CheminUniqueSiMinimumDeCoups(Piece->Scenario->Piece, De, Vers, Chemin))
				Unique = true;

		if (!Unique) {
			if ((De != Vers) || (j < Piece->NombreAssassinats) || Piece->Switchback) {
				if (Piece->Switchback && Piece->NombreAssassinats && (j >= Piece->NombreAssassinats) && (Coups[k - 1].Vers == Piece->Depart)) {
					Unique = false;
				}
				else {
					Coups[k].Type = Piece->Scenario->Piece;
					Coups[k].Piece = Piece;
					Coups[k].De = De;
					Coups[k].Vers = MaxCases;
					k++;
					Coups[k].Type = Piece->Scenario->Piece;
					Coups[k].Piece = Piece;
					Coups[k].De = MaxCases;
					Coups[k].Vers = Vers;
					Coups[k].Victime = (j < Piece->NombreAssassinats) ? Piece->Assassinats[j] : NULL;
					k++;
				}
			}
		}
		else {
			Verifier(De != Vers);
			for (unsigned int i = 0; Chemin[i] != Vers; i++) {
				Coups[k].Type = Piece->Scenario->Piece;
				Coups[k].Piece = Piece;
				Coups[k].De = Chemin[i];
				Coups[k].Vers = Chemin[i + 1];
				if ((j < Piece->NombreAssassinats) && (Coups[k].Vers == Vers))
					Coups[k].Victime = Piece->Assassinats[j];
				k++;
			}
		}

		De = Vers;
	}

	if (k) {
		Coups[k - 1].Dernier = true;
		Coups[k - 1].Assassin = Piece->Assassin;
		*NombreCoups += k;
	}
}

/*************************************************************/

void AnalysePhaseX(strategie *Strategie)
{
	Strategie->CoupsLibresBlancs = CopieDesCoupsLibres[BLANCS];
	Strategie->CoupsLibresNoirs = CopieDesCoupsLibres[NOIRS];

	for (unsigned int i = 0; i < MaxHommes; i++) {
		Strategie->PiecesBlanches[i].Coups = CopieDuNombreDeCoupsBlancs[i];
		Strategie->PiecesNoires[i].Coups = CopieDuNombreDeCoupsNoirs[i];

		Strategie->PiecesBlanches[i].CaseDuSwitchback = MaxCases;
		Strategie->PiecesNoires[i].CaseDuSwitchback = MaxCases;
	}
}

/*************************************************************/

void CalculDesLiensDePriorite(pseudopartie *Partie)
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
		const unsigned int *IndexDesCoups = (Couleur == BLANCS) ? Partie->IndexPremiersCoupsBlancs : Partie->IndexPremiersCoupsNoirs;
		unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Partie->Strategie->CoupsLibresBlancs : &Partie->Strategie->CoupsLibresNoirs;

		coup *CoupsAdverses = (Couleur == BLANCS) ? Partie->CoupsNoirs : Partie->CoupsBlancs;
		const unsigned int *IndexDesCoupsAdverses = (Couleur == BLANCS) ? Partie->IndexPremiersCoupsNoirs : Partie->IndexPremiersCoupsBlancs;

		for (unsigned int k = 0; k < NombreCoups; k++) {
			coup *Coup = &Coups[k];

			Verifier(Coup->Piece);

			if (!Coup->Premier && !Coup->Roque) {
				if (Coup->Piece == (Coup - 1)->Piece) {
					Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Coup - 1;
				}
				else {
					Coup->DoitSuivre[Coup->NombreDoitSuivre++] = &Coups[0];
					Verifier(Coup->Type == TOUR);
					Verifier(Coups[0].Roque);
				}
			}

			if (!Coup->Dernier && !Coup->Roque)
				Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Coup + 1;

			if (Coup->Roque) {
				Verifier(Coup == &Coups[0]);

				if (Coups[1].Type == ROI)
					Coup->DoitPreceder[Coup->NombreDoitPreceder++] = &Coups[1];

				if (QuelleColonne(Coup->Vers) == C)
					if (IndexDesCoups[TOURDAME] < UINT_MAX)
						Coup->DoitPreceder[Coup->NombreDoitPreceder++] = &Coups[IndexDesCoups[TOURDAME]];

				if (QuelleColonne(Coup->Vers) == G)
					if (IndexDesCoups[TOURROI] < UINT_MAX)
						Coup->DoitPreceder[Coup->NombreDoitPreceder++] = &Coups[IndexDesCoups[TOURROI]];
			}

			if (Coup->Victime) {
				unsigned int Index = IndexDesCoupsAdverses[Coup->Victime->Scenario->Homme];
				if (Index < UINT_MAX) {
					coup *C = &CoupsAdverses[Index];
					while (!C->Dernier)
						C++;

					Coup->DoitSuivre[Coup->NombreDoitSuivre++] = C;
					C->DoitPreceder[C->NombreDoitPreceder++] = Coup;

					Verifier(C->Piece);
					Verifier(C->NombreDoitPreceder <= (2 * MaxCoups));
				}
			}

			Verifier(Coup->NombreDoitSuivre <= (2 * MaxCoups));
			Verifier(Coup->NombreDoitPreceder <= (2 * MaxCoups));

			for (couleurs CouleurMorts = BLANCS; CouleurMorts <= NOIRS; CouleurMorts++) {
				vie *Pieces = (CouleurMorts == BLANCS) ? Partie->Strategie->PiecesBlanches : Partie->Strategie->PiecesNoires;
				unsigned int *IndexAssassins = (CouleurMorts == BLANCS) ? Partie->IndexPremiersCoupsNoirs : Partie->IndexPremiersCoupsBlancs;
				coup *CoupsAssassins = (CouleurMorts == BLANCS) ? Partie->CoupsNoirs : Partie->CoupsBlancs;
				bool VraimentMort = (CouleurMorts == BLANCS) ? (Partie->Strategie->CoupsLibresBlancs < 2) : (Partie->Strategie->CoupsLibresNoirs < 2);

				for (unsigned int h = 0; h < MaxHommes; h++) {
					if (Pieces[h].Coups)
						continue;

					if (!Pieces[h].Capturee)
						continue;

					if ((h == TOURDAME) && (QuelleColonne(Pieces[h].Depart) != A))
						continue;

					if ((h == TOURROI) && (QuelleColonne(Pieces[h].Depart) != H))
						continue;

					if ((h < PIONA) && (!VraimentMort))
						if (CombienDeSwitchback(h, CouleurMorts, Pieces[h].Depart, NULL) > 0)
							continue;

					if (Coup->Victime == &Pieces[h])
						continue;

					unsigned int Temps = Coup->NombreDeCoupsSiCaseBloquee[Pieces[h].Depart];
					if (Temps <= Coup->NombreDeCoups)
						continue;

					unsigned int Extra = Temps - Coup->NombreDeCoups;
					if (Extra <= *CoupsLibres)
						continue;

					Verifier(Pieces[h].Assassin);
					Verifier(IndexAssassins[Pieces[h].Assassin->Scenario->Homme] < UINT_MAX);

					coup *Assassinat = &CoupsAssassins[IndexAssassins[Pieces[h].Assassin->Scenario->Homme]];
					while (Assassinat->Victime != &Pieces[h])
						Assassinat++;

					if (Assassinat->Dernier || (Coup == (Assassinat + 1))) {
						Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Assassinat;
					}
					else {
						Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Assassinat + 1;
					}
				}
			}

			for (couleurs CouleurBloqueur = BLANCS; CouleurBloqueur <= NOIRS; CouleurBloqueur++) {
				unsigned int NombreBloqueurs = (CouleurBloqueur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
				coup *CoupsBloqueurs = (CouleurBloqueur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
				unsigned int CoupsLibresBloqueur = (CouleurBloqueur == BLANCS) ? Partie->Strategie->CoupsLibresBlancs : Partie->Strategie->CoupsLibresNoirs;

				for (unsigned int j = 0; j < NombreBloqueurs; j++) {
					coup *Bloqueur = &CoupsBloqueurs[j];

					if (Bloqueur->Piece->Scenario->Homme == Coup->Piece->Scenario->Homme)
						if (Couleur == CouleurBloqueur)
							continue;

					if (Bloqueur->Assassin == Coup->Piece)
						continue;

					if ((Coup->Type == ROI) && (Couleur != CouleurBloqueur)) {
						pieces BloqueurTypeVers = Bloqueur->Type;
						if (Bloqueur->Promotion)
							BloqueurTypeVers = Bloqueur->Piece->Scenario->Piece;

						if (IsEnEchecImparable(Coup->De, Bloqueur->Type, Bloqueur->De, CouleurBloqueur))
							if (Coup->Premier && !Bloqueur->Dernier)
								Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur + 1;

						if (IsEnEchecImparable(Coup->De, BloqueurTypeVers, Bloqueur->Vers, CouleurBloqueur)) {
							if (IsEnEchecImparable(Coup->Vers, BloqueurTypeVers, Bloqueur->Vers, CouleurBloqueur)) {
								if (Coup->Premier && !Bloqueur->Assassin)
									Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur;

								if (Coup->Dernier && !Bloqueur->Dernier)
									Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Bloqueur;
							}
						}

						if (IsEnEchecImparable(Coup->Vers, Bloqueur->Type, Bloqueur->De, CouleurBloqueur))
							if (Bloqueur->Premier)
								Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Bloqueur;

						if (IsEnEchecImparable(Coup->Vers, BloqueurTypeVers, Bloqueur->Vers, CouleurBloqueur)) {
							if (Bloqueur->Dernier && !Bloqueur->Assassin)
								if (!Bloqueur->Promotion || (CoupsLibresBloqueur < 2))
									Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur;

							if (Coup->Dernier && !Bloqueur->Dernier)
								Bloqueur->DoitPreceder[Bloqueur->NombreDoitPreceder++] = Coup;
						}
					}

					if ((Coup->De >= MaxCases) || (Coup->Vers >= MaxCases)) {
						cases CaseProbleme = (Coup->De < MaxCases) ? Coup->De : Coup->Vers;

						if (Bloqueur->De == CaseProbleme) {
							if (Bloqueur->Premier) {
								Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Bloqueur;
							}
							else {
								Coup->PeutSuivre[Coup->NombrePeutSuivre++] = Bloqueur;
							}
						}

						if (Bloqueur->Vers == CaseProbleme) {
							if (Bloqueur->Assassin) {
								Coup->PeutPreceder[Coup->NombrePeutPreceder++] = Bloqueur;

								unsigned int Index = (CouleurBloqueur == BLANCS) ? Partie->IndexPremiersCoupsNoirs[Bloqueur->Assassin->Scenario->Homme] : Partie->IndexPremiersCoupsBlancs[Bloqueur->Assassin->Scenario->Homme];
								Verifier(Index < UINT_MAX);
								coup *CoupsAssassins = (CouleurBloqueur == BLANCS) ? &Partie->CoupsNoirs[Index] : &Partie->CoupsBlancs[Index];
								unsigned int CoupsAssassinsLibres = (CouleurBloqueur == BLANCS) ? Partie->Strategie->CoupsLibresNoirs : Partie->Strategie->CoupsLibresBlancs;

								while (CoupsAssassins->Victime != Bloqueur->Piece)
									CoupsAssassins++;

								bool SuivrePossible = true;
								if (!CoupsAssassins->Dernier) {
									Coup->PeutSuivre[Coup->NombrePeutSuivre++] = CoupsAssassins + 1;
								}
								else {
									if ((CoupsAssassins->Type == PION) && !CoupsAssassins->Promotion && !CoupsAssassins->Assassin)
										SuivrePossible = false;

									if (!CoupsAssassins->Assassin && (CoupsAssassinsLibres < 2))
										SuivrePossible = false;

									if (!SuivrePossible) {
										Coup->DoitPreceder[Coup->NombreDoitPreceder++] = CoupsAssassins;
										Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur;
									}
								}
							}
							else {
								if (Bloqueur->Dernier) {
									Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur;
								}
								else {
									Coup->PeutPreceder[Coup->NombrePeutPreceder++] = Bloqueur;
								}
							}
						}

						if (Coup->Vers >= MaxCases)
							continue;
					}
					else if (Bloqueur->De < MaxCases) {
						unsigned int CoupsDe = Coup->NombreDeCoupsSiCaseBloquee[Bloqueur->De];
						unsigned int Extra = 0;
						if (CoupsDe >= Coup->NombreDeCoups)
							Extra = CoupsDe - Coup->NombreDeCoups;

						if (Extra > *CoupsLibres) {
							if (Bloqueur->Premier) {
								Coup->DoitSuivre[Coup->NombreDoitSuivre++] = Bloqueur;
							}
							else {
								Coup->PeutSuivre[Coup->NombrePeutSuivre++] = Bloqueur;
							}
						}
						else if (Extra > 0) {
							if (Bloqueur->Premier)
								Coup->DoitSuivreSiPossible[Coup->NombreDoitSuivreSiPossible++] = Bloqueur;
						}
					}

					if ((Bloqueur->Vers < MaxCases) && (Coup->Vers < MaxCases)) {
						unsigned int CoupsVers = Coup->NombreDeCoupsSiCaseBloquee[Bloqueur->Vers];
						unsigned int Extra = 0;
						if (CoupsVers >= Coup->NombreDeCoups)
							Extra = CoupsVers - Coup->NombreDeCoups;

						if (Extra > *CoupsLibres) {
							if (Coup->De >= MaxCases)
								continue;
/*
							if (Coup->De >= MaxCases) {
								unsigned int DeltaC = abs((int)QuelleColonne(Bloqueur->Vers) - (int)QuelleColonne(Coup->Vers));
								unsigned int DeltaR = abs((int)QuelleRangee(Bloqueur->Vers) - (int)QuelleRangee(Coup->Vers));
								if ((DeltaC > 1) || (DeltaR > 1))  // Cette ligne est incorrecte, on devrait toujours faire le continue
									continue;
							}*/

							if (Bloqueur->Dernier) {
								if (Bloqueur->Assassin) {
									Coup->PeutPreceder[Coup->NombrePeutPreceder++] = Bloqueur;

									unsigned int Index = (CouleurBloqueur == BLANCS) ? Partie->IndexPremiersCoupsNoirs[Bloqueur->Assassin->Scenario->Homme] : Partie->IndexPremiersCoupsBlancs[Bloqueur->Assassin->Scenario->Homme];
									Verifier(Index < UINT_MAX);
									coup *CoupsAssassins = (CouleurBloqueur == BLANCS) ? &Partie->CoupsNoirs[Index] : &Partie->CoupsBlancs[Index];
									unsigned int CoupsAssassinsLibres = (CouleurBloqueur == BLANCS) ? Partie->Strategie->CoupsLibresNoirs : Partie->Strategie->CoupsLibresBlancs;

									while (CoupsAssassins->Victime != Bloqueur->Piece) {
										CoupsAssassins++;
										Verifier(CoupsAssassins->Piece != NULL);
									}

									bool SuivrePossible = true;
									if (!CoupsAssassins->Dernier || CoupsAssassins->Assassin) {
										Coup->PeutSuivre[Coup->NombrePeutSuivre++] = CoupsAssassins + 1;
									}
									else {
										if ((CoupsAssassins->Type == PION) && !CoupsAssassins->Promotion)
											SuivrePossible = false;

										if (CoupsAssassinsLibres < 2)
											SuivrePossible = false;

										if (!SuivrePossible) {
											if ((CoupsLibresBloqueur < 2) || (!Bloqueur->Promotion && (Bloqueur->Type == PION))) {
												Coup->DoitPreceder[Coup->NombreDoitPreceder++] = CoupsAssassins;
												Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur;
											}
										}
									}
								}
								else {
									if ((CoupsLibresBloqueur < 2) || (!Bloqueur->Promotion && (Bloqueur->Type == PION)))
										Coup->DoitPreceder[Coup->NombreDoitPreceder++] = Bloqueur;
								}
							}
							else {
								Verifier(!Bloqueur->Assassin);
								Coup->PeutPreceder[Coup->NombrePeutPreceder++] = Bloqueur;
							}
						}
						else if (Extra > 0) {
							if (Bloqueur->Dernier && !Bloqueur->Assassin)
								Coup->DoitPrecederSiPossible[Coup->NombreDoitPrecederSiPossible++] = Bloqueur;
						}
					}
				}
			}

			Verifier(Coup->NombreDoitSuivre <= (2 * MaxCoups));
			Verifier(Coup->NombrePeutSuivre <= (2 * MaxCoups));
			Verifier(Coup->NombreDoitPreceder <= (2 * MaxCoups));
			Verifier(Coup->NombrePeutPreceder <= (2 * MaxCoups));
			Verifier(Coup->NombreDoitSuivreSiPossible <= (2 * MaxCoups));
			Verifier(Coup->NombreDoitPrecederSiPossible <= (2 * MaxCoups));
		}
	}
}

/*************************************************************/

void EliminerPrioritesEnDouble(coup *Priorites[], unsigned int *NombrePriorites, bool LePlusLoin)
{
	unsigned int Blancs[MaxHommes];
	unsigned int Noirs[MaxHommes];

	for (unsigned int i = 0; i < MaxHommes; i++) {
		Blancs[i] = UINT_MAX;
		Noirs[i] = UINT_MAX;
	}

	for (unsigned int k = 0; k < *NombrePriorites; k++) {
		coup *Coup = Priorites[k];
		hommes Homme = Coup->Piece->Scenario->Homme;
		unsigned int *Tableau = (QuelleRangee(Coup->Piece->Depart) <= QUATRE) ? Blancs : Noirs;

		if (Tableau[Homme] < UINT_MAX) {
			if ((LePlusLoin && (Priorites[Tableau[Homme]] < Coup)) || (!LePlusLoin && (Priorites[Tableau[Homme]] > Coup))) {
				Priorites[Tableau[Homme]] = NULL;
				Tableau[Homme] = k;
			}
			else {
				Priorites[k] = NULL;
			}
		}
		else {
			Tableau[Homme] = k;
		}
	}

	unsigned int j = 0;
	while (j < *NombrePriorites) {
		if (!Priorites[j]) {
			Priorites[j] = Priorites[--(*NombrePriorites)];
		}
		else {
			j++;
		}
	}
}

/*************************************************************/

int TriDesPointeurs(const void *A, const void *B)
{
	const coup *ObjetA = *(const coup **)A;
	const coup *ObjetB = *(const coup **)B;

	if (ObjetA < ObjetB)
		return -1;

	if (ObjetA > ObjetB)
		return 1;

	return 0;
}

/*************************************************************/

void NormaliserLesPriorites(pseudopartie *Partie)
{
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;

		for (unsigned int k = 0; k < NombreCoups; k++) {
			coup *Coup = &Coups[k];
			EliminerPrioritesEnDouble(Coup->DoitSuivre, &Coup->NombreDoitSuivre, true);

			for (unsigned int i = 0; i < Coup->NombreDoitSuivre; i++) {
				Coup->DoitSuivre[i]->DoitPreceder[Coup->DoitSuivre[i]->NombreDoitPreceder++] = Coup;
				Verifier(Coup->DoitSuivre[i]->NombreDoitPreceder < 4 * MaxCoups);
			}
		}
	}
}
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;

		for (unsigned int k = 0; k < NombreCoups; k++) {
			coup *Coup = &Coups[k];
			EliminerPrioritesEnDouble(Coup->DoitPreceder, &Coup->NombreDoitPreceder, false);

			for (unsigned int i = 0; i < Coup->NombreDoitPreceder; i++) {
				Coup->DoitPreceder[i]->DoitSuivre[Coup->DoitPreceder[i]->NombreDoitSuivre++] = Coup;
				Verifier(Coup->DoitPreceder[i]->NombreDoitSuivre < 4 * MaxCoups);
			}
		}
	}
}
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;

		for (unsigned int k = 0; k < NombreCoups; k++) {
			coup *Coup = &Coups[k];

			EliminerPrioritesEnDouble(Coup->DoitSuivre, &Coup->NombreDoitSuivre, true);
			EliminerPrioritesEnDouble(Coup->DoitPreceder, &Coup->NombreDoitPreceder, false);

			qsort(Coup->DoitSuivre, Coup->NombreDoitSuivre, sizeof(coup *), TriDesPointeurs);
			qsort(Coup->PeutSuivre, Coup->NombrePeutSuivre, sizeof(coup *), TriDesPointeurs);
			qsort(Coup->DoitPreceder, Coup->NombreDoitPreceder, sizeof(coup *), TriDesPointeurs);
			qsort(Coup->PeutPreceder, Coup->NombrePeutPreceder, sizeof(coup *), TriDesPointeurs);
		}
	}
}
}

/*************************************************************/

void PrioritesPourLesPions(pseudopartie *Partie)
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
		coup *CoupsAdverses = (Couleur != BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
		unsigned int *PremiersCoups = (Couleur == BLANCS) ? Partie->IndexPremiersCoupsBlancs : Partie->IndexPremiersCoupsNoirs;
		unsigned int *PremiersCoupsAdverses = (Couleur != BLANCS) ? Partie->IndexPremiersCoupsBlancs : Partie->IndexPremiersCoupsNoirs;

		for (hommes k = PIONA; k <= PIONH; k++) {
			if (PremiersCoups[k] == UINT_MAX)
				continue;

			if (PremiersCoupsAdverses[k] == UINT_MAX)
				continue;

			coup *Coup = &Coups[PremiersCoups[k]];
			while (!Coup->Dernier && !Coup->Victime && !Coup->Promotion)
				Coup++;

			cases CaseCritique = Coup->De;

			coup *CoupAdverse = &CoupsAdverses[PremiersCoupsAdverses[k]];
			if (CaseCritique == CoupAdverse->De) {
				//CoupAdverse->DoitPreceder[CoupAdverse->NombreDoitPreceder++] = &Coups[PremiersCoups[k]];

				while (!CoupAdverse->Dernier && !CoupAdverse->Victime && !CoupAdverse->Promotion)
					CoupAdverse++;
				
				if (CoupAdverse->Dernier && !CoupAdverse->Assassin && !CoupAdverse->Victime)
					Coups[PremiersCoups[k]].DoitPreceder[Coups[PremiersCoups[k]].NombreDoitPreceder++] = &CoupsAdverses[PremiersCoupsAdverses[k]];

				continue;
			}

			while (!CoupAdverse->Dernier && !CoupAdverse->Victime && !CoupAdverse->Promotion)
				CoupAdverse++;

			if (!Coup->Victime)
				continue;

			cases CaseCritiqueAdverse = MaxCases;
			if (CoupAdverse->Victime || CoupAdverse->Promotion) {
				if ((QuelleRangee(CoupAdverse->De) <= DEUX) || (QuelleRangee(CoupAdverse->De) >= SEPT))
					CaseCritiqueAdverse = CoupAdverse->De;
			}
			else if (!Coup->Piece->Assassin) {
				CaseCritiqueAdverse = CoupAdverse->Vers;
			}

			if (CaseCritiqueAdverse >= MaxCases)
				continue;

			CoupAdverse = &CoupsAdverses[PremiersCoupsAdverses[k]];
			for ( ; ; CoupAdverse++) {
				if (CoupAdverse->Victime)
					break;

				cases Vers = CoupAdverse->Vers;
				if (Vers >= MaxCases)
					Vers = CoupAdverse->De + ((Couleur == BLANCS) ? -1 : 1);

				if (Vers == CaseCritique) {
					CoupAdverse->DoitSuivre[CoupAdverse->NombreDoitSuivre++] = Coup;
					break;
				}

				if ((CoupAdverse->De < MaxCases) && (CoupAdverse->Vers < MaxCases)) {
					cases De = CoupAdverse->De;
					cases Vers = CoupAdverse->Vers;

					if (abs((int)QuelleRangee(De) - (int)QuelleRangee(Vers)) == 2) {
						cases Critique = (De + Vers) / 2;

						if (Critique == CaseCritique) {
							CoupAdverse->DoitSuivre[CoupAdverse->NombreDoitSuivre++] = Coup;
							break;
						}
					}
				}

				if (Vers == CaseCritiqueAdverse)
					break;
			}
		}
	}
}

/*************************************************************/

void DeductionsSurLesPriorites(pseudopartie *Partie)
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int NombreCoups = (Couleur == BLANCS) ? Partie->NombreCoupsBlancs : Partie->NombreCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;

		for (unsigned int k = 0; k < NombreCoups; k++) {
			coup *Coup = &Coups[k];

			if (!Coup->NombreDoitSuivre && !Coup->NombreDoitPreceder)
				continue;

			unsigned int m = 0;
			unsigned int n = Coup->NombreDoitPreceder;

			for (unsigned int Forward = 0; Forward < Coup->NombrePeutSuivre; Forward++) {
				coup *CForward = Coup->PeutSuivre[Forward];

				if (m >= Coup->NombreDoitSuivre)
					break;

				if (m < Coup->NombreDoitSuivre)
					while ((Coup->DoitSuivre[m] < CForward) && (Coup->DoitSuivre[m]->Piece != CForward->Piece))
						if (++m >= Coup->NombreDoitSuivre)
							break;

				if (m < Coup->NombreDoitSuivre)
					if (Coup->DoitSuivre[m]->Piece == CForward->Piece)
						if ((CForward - 1) == Coup->DoitSuivre[m])
							Coup->DoitSuivre[m] = CForward;
			}

			for (unsigned int Backward = Coup->NombrePeutPreceder; Backward-- > 0; ) {
				coup *CBackward = Coup->PeutPreceder[Backward];

				if (n <= 0)
					break;

				if (n > 0)
					while ((Coup->DoitPreceder[n - 1] > CBackward) && (Coup->DoitPreceder[n - 1]->Piece != CBackward->Piece))
						if (--n <= 0)
							break;

				if (n > 0) {
					Verifier(n <= Coup->NombreDoitPreceder);
					if (Coup->DoitPreceder[n - 1]->Piece == CBackward->Piece)
						if ((CBackward + 1) == Coup->DoitPreceder[n - 1])
							Coup->DoitPreceder[n - 1] = CBackward;
				}
			}
		}
	}
}

/*************************************************************/

bool YATIlDesBoucles(pseudopartie *Partie)
{
	unsigned int NombrePredecesseurs;
	coup *Predecesseurs[2 * MaxCoups];
	bool DejaInclus[2 * MaxCoups];

	unsigned int NombreTotalDeCoups = Partie->NombreCoupsBlancs + Partie->NombreCoupsNoirs;

	for (unsigned int k = 0; k < NombreTotalDeCoups; k++) {
		coup *Coup = &Partie->CoupsBlancs[k];
		if (k >= Partie->NombreCoupsBlancs)
			Coup = &Partie->CoupsNoirs[k - Partie->NombreCoupsBlancs];

		unsigned int m = 0;
		unsigned int n = 0;
		while ((m < Coup->NombreDoitSuivre) && (n < Coup->NombreDoitPreceder)) {
			if (Coup->DoitSuivre[m] == Coup->DoitPreceder[n])
				return true;

			if (Coup->DoitSuivre[m] < Coup->DoitPreceder[n])
				m++;

			if (Coup->DoitSuivre[m] > Coup->DoitPreceder[n])
				n++;
		}

		Predecesseurs[0] = Coup;
		NombrePredecesseurs = 0;
		for (unsigned int i = 0; i < NombreTotalDeCoups; i++)
			DejaInclus[i] = false;

		unsigned int Index = 0;
		while (Index <= NombrePredecesseurs) {
			coup *Examiner = Predecesseurs[Index];

			for (unsigned int i = 0; i < Examiner->NombreDoitSuivre; i++) {
				coup *Nouveau = Examiner->DoitSuivre[i];
				if (Nouveau == Coup)
					return true;

				unsigned int IndexDuNouveau = Nouveau - &Partie->CoupsBlancs[0];
				if (IndexDuNouveau >= Partie->NombreCoupsBlancs) {
					IndexDuNouveau = Nouveau - &Partie->CoupsNoirs[0];
					IndexDuNouveau += Partie->NombreCoupsBlancs;

					Verifier((IndexDuNouveau >= Partie->NombreCoupsBlancs) && (IndexDuNouveau < NombreTotalDeCoups));
				}

				if (!DejaInclus[IndexDuNouveau]) {
					DejaInclus[IndexDuNouveau] = true;
					Predecesseurs[++NombrePredecesseurs] = Nouveau;
				}
			}

			Index++;
		}
	}

	return false;
}

/*************************************************************/

bool DeterminerLesSwitchback(pseudopartie *Partie)
{
	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		unsigned int *IndexCoups = (Couleur == BLANCS) ? Partie->IndexPremiersCoupsBlancs : Partie->IndexPremiersCoupsNoirs;
		coup *Coups = (Couleur == BLANCS) ? Partie->CoupsBlancs : Partie->CoupsNoirs;
		unsigned int CoupsLibres = (Couleur == BLANCS) ? Partie->Strategie->CoupsLibresBlancs : Partie->Strategie->CoupsLibresNoirs;

		if (CoupsLibres)
			continue;

		for (unsigned int k = 0; k < PIONA; k++) {
			if (IndexCoups[k] == UINT_MAX)
				continue;

			coup *PremierCoup = &Coups[IndexCoups[k]];
			if (!PremierCoup->Premier || PremierCoup->Dernier)
				continue;

			coup *DernierCoup = &Coups[IndexCoups[k] + 1];
			if (!DernierCoup->Dernier)
				continue;

			if (!DernierCoup->Piece->Switchback)
				continue;

			if (PremierCoup->Victime)
				continue;

			Verifier(PremierCoup->De == DernierCoup->Vers);
			Verifier(PremierCoup->Vers >= MaxCases);
			Verifier(DernierCoup->De >= MaxCases);

			unsigned int Nombre = CombienDeSwitchback(k, Couleur, PremierCoup->De, &PremierCoup->Piece->CaseDuSwitchback);

			if (Nombre == 0)
				return false;
		}
	}

	return true;
}

/*************************************************************/

unsigned int CombienDeSwitchback(hommes Homme, couleurs Couleur, cases De, cases *LaCase)
{
	cases TestTourDame[] = { A2, A3, B1, B2, MaxCases };
	cases TestCavalierDame[] = { A3, C3, D2, MaxCases };
	cases TestFouDame[] = { B2, A3, D2, E3, MaxCases };
	cases TestDame[] = { B1, C1, E1, F1, D2, D3, C2, B3, E2, F3, MaxCases };
	cases TestRoi[] = { D1, D2, E2, F2, F1, MaxCases };
	cases TestFouRoi[] = { E2, D3, G2, H3, MaxCases };
	cases TestCavalierRoi[] = { E2, F3, H3, MaxCases };
	cases TestTourRoi[] = { F1, G1, H2, H3, MaxCases };

	unsigned int Nombre = 0;

	if (LaCase)
		*LaCase = MaxCases;

	cases *ATester = NULL;
	switch (Homme) {
		case XROI :
			ATester = TestRoi;
			break;
		case XDAME :
			ATester = TestDame;
			break;
		case TOURROI :
			ATester = TestTourRoi;
			break;
		case TOURDAME :
			ATester = TestTourDame;
			break;
		case FOUROI :
			ATester = TestFouRoi;
			break;
		case FOUDAME :
			ATester = TestFouDame;
			break;
		case CAVALIERDAME :
			ATester = TestCavalierDame;
			break;
		case CAVALIERROI :
			ATester = TestCavalierRoi;
			break;
		default :
			Verifier(ATester != NULL);
			break;
	}

	while (*ATester != MaxCases) {
		cases Vers = *ATester++;
		if (Couleur == NOIRS)
			Vers = QuelleCase(QuelleColonne(Vers), HUIT - QuelleRangee(Vers));

		if (IsCoupPossible(QuellePiece(Homme, Couleur), De, Vers)) {
			Nombre++;

			if (LaCase)
				*LaCase = Vers;
		}
	}

	if (LaCase && (Nombre != 1))
		*LaCase = MaxCases;

	return Nombre;
}

/*************************************************************/
