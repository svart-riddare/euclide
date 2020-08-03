#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "Constantes.h"
#include "Erreur.h"
#include "Mouvements.h"
#include "Output.h"
#include "Permutation.h"
#include "Position.h"

/*************************************************************/

static strategies LaStrategie;

/*************************************************************/

void DePositionAPermutation(const bonhomme Pieces[MaxHommes], permutations *Permutations, couleurs Couleur);
bool ChoisirLesRoques(permutations *Permutations, vie Pieces[MaxHommes], couleurs Couleur, bool *GrandRoque, bool *PetitRoque, bool PremiereFois);
bool ChoisirLesCasesFinales(permutations *Permutations, cases CasesPrises[MaxCases], vie Pieces[MaxHommes], unsigned int NombreDePieces, unsigned int *CoupsLibres, unsigned int *CapturesLibres, couleurs Couleur, bool PremiereFois);
unsigned int NombreDeCoupsNecessaires(cases CaseFinale, hommes Homme, cases Depart, promotions PiecePromotion = MaxPromotions, colonnes ColonnePromotion = MaxColonnes);
unsigned int NombreDeCapturesNecessaires(cases CaseFinale, hommes Homme, cases Depart);
bool PremiereStrategie(strategies *Strategies, _texte Texte);
void InitialisationDesStrategies(strategies *Strategies, const position *Position);
int TriDesScenarios(const void *DataA, const void *DataB);
void ConstructionDesScenarios(destin Destins[MaxHommes], const vie Pieces[MaxHommes], couleurs Couleur);
void ConstructionDesScenariosSuicides(permutations *Permutations, const vie Pieces[MaxHommes], couleurs Couleur);
bool SwitchbacksEvidents(vie Pieces[MaxHommes], unsigned int *CoupsLibres, bool GrandRoque, bool PetitRoque, bool PremiereFois);
bool TrajectoiresDesPions(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], couleurs Couleur, const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool PremiereFois);
bool XTrajectoiresDesPions(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], couleurs Couleur, const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool Calculer[MaxHommes], bool PremiereFois);
bool PiecesCaptureesParLePion(vie *Pion, const permutations *Permutations, vie PiecesAdverses[MaxHommes], unsigned int *CoupsLibres, unsigned int *CapturesLibres, bool PremiereFois);
bool TrajectoiresDesPionsCaptures(strategies *Strategies, bool PremiereFois);
bool PionsDisparusSansTrace(vie Pieces[MaxHommes], const destin Destins[MaxHommes], unsigned int *CoupsLibres, unsigned int *CapturesLibres, couleurs Couleur, bool PremiereFois);
bool TrajectoiresDesPionsBlancsDisparusSansTrace(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool PremiereFois);
bool TrajectoiresDesPionsNoirsDisparusSansTrace(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool PremiereFois);
unsigned int CalculDesCoupsAvecMeurtres(const vie *Assassin, hommes Homme, cases Depart);
bool ChoisirAssassin(vie *Victime, vie AssassinsPlausibles[MaxHommes], unsigned int *CoupsLibresAdverses, bool PremiereFois);
bool ChoisirLesAssassinats(strategies *Strategies, bool PremiereFois);
bool SwitchbackPossible(const vie *Piece);
unsigned int CalculPrecisDesCoupsRequisAvecMeurtres(const vie *Piece);
bool AutresSwitchbacks(vie PiecesBlanches[MaxHommes], vie PiecesNoires[MaxHommes], unsigned int *CoupsLibresBlancs, unsigned int *CoupsLibresNoirs, bool PremiereFois);

/*************************************************************/

strategies *ExamenDesStrategies(const position *Position, _texte Texte)
{
	strategies *Strategies = &LaStrategie;
	memset(Strategies, 0, sizeof(strategies));

	unsigned int StepPreparation = 0;

	OutputMessage(MESSAGE_PREPARATION, ++StepPreparation);
	InitialisationDesTablesDeDeplacements();

	OutputMessage(MESSAGE_PREPARATION, ++StepPreparation);
	InitialisationDesTrajetsDePion();

	OutputMessage(MESSAGE_PREPARATION, ++StepPreparation);
	InitialisationDesStrategies(Strategies, Position);

	OutputMessage(MESSAGE_PREPARATION, ++StepPreparation);
	DePositionAPermutation(Position->PiecesBlanches, &Strategies->PermutationsBlanches, BLANCS);
	DePositionAPermutation(Position->PiecesNoires, &Strategies->PermutationsNoires, NOIRS);

	OutputMessage((texte)Texte);
	if (!PremiereStrategie(Strategies, Texte))
		return NULL;

	return Strategies;
}

/*************************************************************/

int TriDesScenarios(const void *DataA, const void *DataB)
{
	const scenario *ScenarioA = (const scenario *)DataA;
	const scenario *ScenarioB = (const scenario *)DataB;

	if (ScenarioA->Coups < ScenarioB->Coups)
		return -1;

	if (ScenarioA->Coups > ScenarioB->Coups)
		return 1;

	return 0;
}

/*************************************************************/

void ConstructionDesScenarios(destin Destins[MaxHommes], const vie Pieces[MaxHommes], couleurs Couleur)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		destin *Destin = &Destins[i];

		unsigned int k = 0;

		for (unsigned int j = 0; j < Destin->NumCasesPossibles; j++) {
			if (!Destin->PromuSurCetteCase[j]) {
				Destin->Scenarios[k].Homme = i;
				Destin->Scenarios[k].Piece = QuellePiece(i, Couleur);
				Destin->Scenarios[k].Coups = NombreDeCoupsNecessaires(Destin->CasesPossibles[j], i, Pieces[i].Depart);
				Destin->Scenarios[k].Captures = NombreDeCapturesNecessaires(Destin->CasesPossibles[j], i, Pieces[i].Depart);
				Destin->Scenarios[k].CaseFinale = Destin->CasesPossibles[j];
				Destin->Scenarios[k].Promotion = MaxColonnes;
				k++;
			}
			else {
				Verifier(Destin->PossiblementPromu);

				for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
					if (!Destin->Promotions[Colonne][Destin->PieceSurCetteCase[j]])
						continue;

					Destin->Scenarios[k].Homme = i;
					Destin->Scenarios[k].Piece = QuellePiece(Destin->PieceSurCetteCase[j], Colonne, Couleur);
					Destin->Scenarios[k].Coups = NombreDeCoupsNecessaires(Destin->CasesPossibles[j], i, Pieces[i].Depart, Destin->PieceSurCetteCase[j], Colonne);
					Destin->Scenarios[k].Captures = NombreDeCapturesNecessaires(QuelleCase(Colonne, (Couleur == BLANCS) ? HUIT : UN), i, Pieces[i].Depart);
					Destin->Scenarios[k].CaseFinale = Destin->CasesPossibles[j];
					Destin->Scenarios[k].Promotion = Colonne;
					k++;
				}
			}
		}

		Verifier(k < sizeof(Destin->Scenarios) / sizeof(scenario));

		qsort(Destin->Scenarios, k, sizeof(scenario), TriDesScenarios);
		Destin->Scenarios[k].Coups = UINT_MAX;
		Destin->NombreDeScenarios = k;
	}
}

/*************************************************************/

void ConstructionDesScenariosSuicides(permutations *Permutations, const vie Pieces[MaxHommes], couleurs Couleur)
{
	unsigned int NombreDeScenarios[MaxCases];
	unsigned int k;
	for (k = 0; k < MaxCases; k++)
		NombreDeScenarios[k] = 0;

	for (unsigned int i = 0; i < MaxHommes; i++) {
		destin *Destin = &Permutations->Destins[i];
		const vie *Piece = &Pieces[i];

		Destin->NombreDeScenariosMorts = 0;

		if (!Piece->Capturee)
			continue;

		for (cases Case = A1; Case < MaxCases; Case++) {
			if (!Destin->Disparitions[Case])
				continue;

			scenario *Scenario = &Permutations->ScenariosSuicides[Case][NombreDeScenarios[Case]];
			NombreDeScenarios[Case]++;

			Scenario->Homme = i;
			Scenario->Piece = QuellePiece(i, Couleur);
			Scenario->Coups = NombreDeCoupsNecessaires(Case, i, Piece->Depart);
			Scenario->Captures = NombreDeCapturesNecessaires(Case, i, Piece->Depart);
			Scenario->CaseFinale = Case;
			Scenario->Promotion = MaxColonnes;

			Destin->ScenariosMorts[Destin->NombreDeScenariosMorts++] = *Scenario;
			Verifier(Destin->NombreDeScenariosMorts <= MaxCases * MaxColonnes * MaxPromotions + MaxCases);

			if (Destin->PossiblementPromu) {
				for (colonnes Promotion = A; Promotion < MaxColonnes; Promotion++) {
					for (promotions PromuEn = PROMOTIONDAME; PromuEn < MaxPromotions; PromuEn++) {
						if (!Destin->Promotions[Promotion][PromuEn])
							continue;

						scenario *Scenario = &Permutations->ScenariosSuicides[Case][NombreDeScenarios[Case]];
						NombreDeScenarios[Case]++;

						Scenario->Homme = i;
						Scenario->Piece = QuellePiece(PromuEn, Promotion, Couleur);
						Scenario->Coups = NombreDeCoupsNecessaires(Case, i, Piece->Depart, PromuEn, Promotion);
						Scenario->Captures = NombreDeCapturesNecessaires(QuelleCase(Promotion, (Couleur == BLANCS) ? HUIT : UN), i, Piece->Depart);
						Scenario->CaseFinale = Case;
						Scenario->Promotion = Promotion;

						Destin->ScenariosMorts[Destin->NombreDeScenariosMorts++] = *Scenario;
						Verifier(Destin->NombreDeScenariosMorts <= MaxCases * MaxColonnes * MaxPromotions + MaxCases);
					}
				}
			}
		}

		Verifier(Destin->NombreDeScenariosMorts < sizeof(Destin->ScenariosMorts) / sizeof(scenario));

		Destin->ScenariosMorts[Destin->NombreDeScenariosMorts].Coups = UINT_MAX;
		qsort(Destin->ScenariosMorts, Destin->NombreDeScenariosMorts, sizeof(scenario), TriDesScenarios);

		for (k = Destin->NombreDeScenariosMorts; k-- > 0; )
			if (Destin->ScenariosMorts[k].Coups == UINT_MAX)
				Destin->NombreDeScenariosMorts--;
	}

	for (k = 0; k < MaxCases; k++) {
		Verifier(NombreDeScenarios[k] < sizeof(Permutations->ScenariosSuicides[k]) / sizeof(scenario));

		Permutations->ScenariosSuicides[k][NombreDeScenarios[k]].Coups = UINT_MAX;
		qsort(Permutations->ScenariosSuicides[k], NombreDeScenarios[k], sizeof(scenario), TriDesScenarios);
	}
}

/*************************************************************/

void DePositionAPermutation(const bonhomme Pieces[MaxHommes], permutations *Permutations, couleurs Couleur)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE) {
			if (Pieces[i].Piece == PION) {
				for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
					if (Pieces[i].ExPion[Colonne]) {
						destin *Destin = &Permutations->Destins[PIONA + Colonne];
						Destin->CasesPossibles[Destin->NumCasesPossibles++] = Pieces[i].CaseActuelle;
					}
				}
			}
			else if (Pieces[i].Piece == ROI) {
				Verifier(!Permutations->Destins[XROI].NumCasesPossibles);

				Permutations->Destins[XROI].CasesPossibles[0] = Pieces[i].CaseActuelle;
				Permutations->Destins[XROI].NumCasesPossibles = 1;
				Permutations->GrandRoquePossible = Pieces[i].ExGrandRoque;
				Permutations->PetitRoquePossible = Pieces[i].ExPetitRoque;
				Permutations->AucunRoquePossible = Pieces[i].ExAileRoi;
			}
			else {
				if (Pieces[i].ExAileDame || Pieces[i].ExGrandRoque) {
					hommes Homme = QuelHomme(Pieces[i].Piece, true, false);
					Permutations->Destins[Homme].CasesPossibles[Permutations->Destins[Homme].NumCasesPossibles++] = Pieces[i].CaseActuelle;
				}

				if (Pieces[i].ExAileRoi || Pieces[i].ExPetitRoque) {
					hommes Homme = QuelHomme(Pieces[i].Piece, false, true);
					Permutations->Destins[Homme].CasesPossibles[Permutations->Destins[Homme].NumCasesPossibles++] = Pieces[i].CaseActuelle;
				}

				if (!Pieces[i].PromotionImpossible) {
					for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
						if (Pieces[i].ExPion[Colonne]) {
							destin *Destin = &Permutations->Destins[PIONA + Colonne];
							promotions Promotion = QuellePromotion(Pieces[i].Piece);

							Destin->CasesPossibles[Destin->NumCasesPossibles] = Pieces[i].CaseActuelle;
							Destin->PromuSurCetteCase[Destin->NumCasesPossibles] = true;
							Destin->PieceSurCetteCase[Destin->NumCasesPossibles] = Promotion;
							Destin->PossiblementPromu = true;
							Destin->NumCasesPossibles++;

							for (colonnes ColonnePromotion = A; ColonnePromotion < MaxColonnes; ColonnePromotion++)
								if (Pieces[i].DeplacementsSiCasePromotion[Colonne][ColonnePromotion] < UINT_MAX)
									Destin->Promotions[ColonnePromotion][Promotion] = true;
						}
					}
				}
			}
		}
		else {
			cases CaseMin = Pieces[i].CaseActuelleMin;
			cases CaseMax = Pieces[i].CaseActuelleMax;

			if ((CaseMin == MaxCases) || (CaseMax == MaxCases)) {
				CaseMin = A1;
				CaseMax = H8;
			}

			for (pieces ExPiece = PION; ExPiece < ROI; ExPiece++) {
				if (!Pieces[i].ExPiece[ExPiece])
					continue;

				if (ExPiece == PION) {
					for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
						if (Pieces[i].ExPion[Colonne]) {
							destin *Destin = &Permutations->Destins[PIONA + Colonne];

							Destin->PossiblementDisparu = true;
							for (cases Case = CaseMin; Case <= CaseMax; Case++)
								if (!CasesMortes[Case])
									Destin->Disparitions[Case] = true;

							if (!Pieces[i].PromotionImpossible) {
								Destin->PossiblementPromu = true;
								for (colonnes ColonnePromotion = A; ColonnePromotion < MaxColonnes; ColonnePromotion++)
									if (Pieces[i].DeplacementsSiCasePromotion[Colonne][ColonnePromotion] < UINT_MAX)
										for (promotions Promotion = PROMOTIONDAME; Promotion < MaxPromotions; Promotion++)
											Destin->Promotions[ColonnePromotion][Promotion] = true;
							}
						}
					}
				}
				else {
					bool ExAileDame = (Couleur == BLANCS) ? (ExPiece != FOUBLANC) : (ExPiece != FOUNOIR);
					bool ExAileRoi = (ExPiece != DAME) && ((Couleur == BLANCS) ? (ExPiece != FOUNOIR) : (ExPiece != FOUBLANC));

					if (ExAileDame) {
						destin *Destin = &Permutations->Destins[QuelHomme(ExPiece, true, false)];

						Destin->PossiblementDisparu = true;
						for (cases Case = CaseMin; Case <= CaseMax; Case++)
							if (!CasesMortes[Case])
								Destin->Disparitions[Case] = true;
					}

					if (ExAileRoi) {
						destin *Destin = &Permutations->Destins[QuelHomme(ExPiece, false, true)];

						Destin->PossiblementDisparu = true;
						for (cases Case = CaseMin; Case <= CaseMax; Case++)
							if (!CasesMortes[Case])
								Destin->Disparitions[Case] = true;
					}
				}
			}
		}
	}

	for (unsigned int k = 0; k < MaxHommes; k++)
		Verifier(Permutations->Destins[k].NumCasesPossibles || Permutations->Destins[k].PossiblementDisparu);
}

/*************************************************************/

bool ChoisirLesRoques(permutations *Permutations, vie Pieces[MaxHommes], couleurs Couleur, bool *GrandRoque, bool *PetitRoque, bool PremiereFois)
{
	if (PremiereFois) {
		*GrandRoque = false;
		*PetitRoque = false;
	}
	else {
		Verifier(!*GrandRoque || !*PetitRoque);
	}

	if (!PremiereFois || !Permutations->AucunRoquePossible) {
		for ( ; ; ) {
			if (!*GrandRoque) {
				*GrandRoque = true;
			}
			else {
				*GrandRoque = false;
				*PetitRoque = true;
			}

			if (*GrandRoque && *PetitRoque)
				return false;

			if (*GrandRoque && Permutations->GrandRoquePossible)
				break;

			if (*PetitRoque && Permutations->PetitRoquePossible)
				break;
		}
	}

	rangees Un = (Couleur == BLANCS) ? UN : HUIT;
	rangees Deux = (Couleur == BLANCS) ? DEUX : SEPT;

	if (*GrandRoque) {
		Pieces[XROI].Depart = QuelleCase(C, Un);
		Pieces[TOURDAME].Depart = QuelleCase(D, Un);
		Pieces[TOURROI].Depart = QuelleCase(H, Un);
	}
	else if (*PetitRoque) {
		Pieces[XROI].Depart = QuelleCase(G, Un);
		Pieces[TOURDAME].Depart = QuelleCase(A, Un);
		Pieces[TOURROI].Depart = QuelleCase(F, Un);
	}
	else {
		Pieces[XROI].Depart = QuelleCase(E, Un);
		Pieces[TOURDAME].Depart = QuelleCase(A, Un);
		Pieces[TOURROI].Depart = QuelleCase(H, Un);
	}

	if (PremiereFois) {
		Pieces[XDAME].Depart = QuelleCase(D, Un);
		Pieces[FOUDAME].Depart = QuelleCase(C, Un);
		Pieces[FOUROI].Depart = QuelleCase(F, Un);
		Pieces[CAVALIERDAME].Depart = QuelleCase(B, Un);
		Pieces[CAVALIERROI].Depart = QuelleCase(G, Un);
		Pieces[PIONA].Depart = QuelleCase(A, Deux);
		Pieces[PIONB].Depart = QuelleCase(B, Deux);
		Pieces[PIONC].Depart = QuelleCase(C, Deux);
		Pieces[PIOND].Depart = QuelleCase(D, Deux);
		Pieces[PIONE].Depart = QuelleCase(E, Deux);
		Pieces[PIONF].Depart = QuelleCase(F, Deux);
		Pieces[PIONG].Depart = QuelleCase(G, Deux);
		Pieces[PIONH].Depart = QuelleCase(H, Deux);
	}

	ConstructionDesScenarios(Permutations->Destins, Pieces, Couleur);
	return true;
}

/*************************************************************/

unsigned int NombreDeCoupsNecessaires(cases CaseFinale, hommes Homme, cases Depart, promotions PiecePromotion, colonnes ColonnePromotion)
{
	unsigned int CoupsRequis = UINT_MAX;

	if (Homme < PIONA) {
		CoupsRequis = CombienDeDeplacements(Homme, Depart, CaseFinale);
		if (Homme == XROI)
			if (QuelleColonne(Depart) != E)
				CoupsRequis++;
	}
	else if (PiecePromotion >= MaxPromotions) {
		rangees Actuelle = QuelleRangee(CaseFinale);
		rangees RangeeDepart = QuelleRangee(Depart);
		Verifier((RangeeDepart == DEUX) || (RangeeDepart == SEPT));

		CoupsRequis = abs((int)Actuelle - (int)RangeeDepart);

		if ((Actuelle == UN) || (Actuelle == HUIT))
			CoupsRequis = UINT_MAX;

		unsigned int Offset = abs((int)QuelleColonne(CaseFinale) - (int)QuelleColonne(Depart));

		if (Offset > CoupsRequis)
			CoupsRequis = UINT_MAX;

		if (CoupsRequis >= 2)
			CoupsRequis--;
	}
	else {
		cases VraiDepart = QuelleCase(ColonnePromotion, (QuelleRangee(Depart) == DEUX) ? HUIT : UN);

		switch (PiecePromotion) {
			case PROMOTIONDAME :
				CoupsRequis = CombienDeDeplacements(XDAME, VraiDepart, CaseFinale);
				break;
			case PROMOTIONTOUR :
				CoupsRequis = CombienDeDeplacements(TOURDAME, VraiDepart, CaseFinale);
				break;
			case PROMOTIONFOU :
				CoupsRequis = CombienDeDeplacements(FOUDAME, VraiDepart, CaseFinale);
				break;
			case PROMOTIONCAVALIER :
				CoupsRequis = CombienDeDeplacements(CAVALIERDAME, VraiDepart, CaseFinale);
				break;
			default :
				break;
		}

		if (CoupsRequis < UINT_MAX)
			CoupsRequis += 5;

	}

	return CoupsRequis;
}

/*************************************************************/

unsigned int NombreDeCapturesNecessaires(cases CaseFinale, hommes Homme, cases Depart)
{
	unsigned int Captures = 0;

	if ((Homme >= PIONA) && (Homme <= PIONH)) {
		Verifier((QuelleRangee(Depart) == DEUX) || (QuelleRangee(Depart) == SEPT));

		if (abs((int)CaseFinale - (int)Depart) > 1) {
			Captures = abs((int)QuelleColonne(CaseFinale) - (int)QuelleColonne(Depart));

			cases FausseFinale = CaseFinale;
			if (QuelleRangee(Depart) == DEUX)
				FausseFinale--;
			else
				FausseFinale++;

			if (!Captures && !AvancePossibleSansCapture(Depart, FausseFinale))
				Captures += 2;
		}
	}

	return Captures;
}

/*************************************************************/

bool ChoisirLesCasesFinales(permutations *Permutations, cases CasesPrises[MaxCases], vie Pieces[MaxHommes], unsigned int NombreDePieces, unsigned int *CoupsLibres, unsigned int *CapturesLibres, couleurs Couleur, bool PremiereFois)
{
	hommes Homme = PremiereFois ? MaxHommes : XROI;
	unsigned int HommesSurLeJeu = PremiereFois ? 0 : NombreDePieces;

	do {
		for ( ; Homme < MaxHommes; Homme++) {
			const destin *Destin = &Permutations->Destins[Homme];
			vie *Piece = &Pieces[Homme];

			if (Piece->Capturee) {
				Verifier(!Piece->Coups);
				continue;
			}

			Verifier(Piece->Scenario);
			Verifier(Piece->Scenario->Coups == Piece->Coups);

			*CapturesLibres += Piece->Scenario->Captures;
			*CoupsLibres += Piece->Scenario->Coups;
			CasesPrises[Piece->Scenario->CaseFinale] = false;
			Piece->Capturee = false;
			Piece->Promue = false;
			Piece->Coups = 0;
			HommesSurLeJeu--;

			Piece->Scenario++;
			while (Piece->Scenario->Coups <= *CoupsLibres) {
				if (!CasesPrises[Piece->Scenario->CaseFinale])
					if (Piece->Scenario->Captures <= *CapturesLibres)
						break;

				Piece->Scenario++;
			}

			if (Piece->Scenario->Coups <= *CoupsLibres) {
				Piece->Promue = (Piece->Scenario->Promotion != MaxColonnes);
				CasesPrises[Piece->Scenario->CaseFinale] = true;
				Piece->Coups = Piece->Scenario->Coups;
				*CoupsLibres -= Piece->Scenario->Coups;
				*CapturesLibres -= Piece->Scenario->Captures;
				HommesSurLeJeu++;
				break;
			}

			Piece->Scenario = NULL;
			if (Destin->PossiblementDisparu) {
				if (HommesSurLeJeu + Homme >= NombreDePieces) {
					Verifier(!Piece->Coups);
					Piece->Capturee = true;
					break;
				}
			}
		}

		if (!PremiereFois && (Homme >= MaxHommes))
			return false;

		for ( ; Homme-- > 0; ) {
			const destin *Destin = &Permutations->Destins[Homme];
			vie *Piece = &Pieces[Homme];

			Verifier(!Piece->Scenario);
			Piece->Scenario = &Destin->Scenarios[0];
			Piece->Capturee = false;

			while (Piece->Scenario->Coups <= *CoupsLibres) {
				if (!CasesPrises[Piece->Scenario->CaseFinale])
					if (Piece->Scenario->Captures <= *CapturesLibres)
						break;

				Piece->Scenario++;
			}

			if (Piece->Scenario->Coups <= *CoupsLibres) {
				Piece->Promue = (Piece->Scenario->Promotion != MaxColonnes);
				CasesPrises[Piece->Scenario->CaseFinale] = true;
				Piece->Coups = Piece->Scenario->Coups;
				*CoupsLibres -= Piece->Scenario->Coups;
				*CapturesLibres -= Piece->Scenario->Captures;
				HommesSurLeJeu++;
				continue;
			}

			Piece->Scenario = NULL;
			Piece->Capturee = true;
			Verifier(!Piece->Coups);

			if (Destin->PossiblementDisparu && (HommesSurLeJeu + Homme >= NombreDePieces))
				continue;

			break;
		}

		PremiereFois = false;

	} while ((signed int)Homme >= 0);

	ConstructionDesScenariosSuicides(Permutations, Pieces, Couleur);
	return true;
}

/*************************************************************/

bool PremiereStrategie(strategies *Strategies, _texte Texte)
{
	strategie *Strategie = &Strategies->StrategieActuelle;

	Verifier(Strategies->PermutationsBlanches.AucunRoquePossible || Strategies->PermutationsBlanches.GrandRoquePossible || Strategies->PermutationsBlanches.PetitRoquePossible);
	Verifier(Strategies->PermutationsNoires.AucunRoquePossible || Strategies->PermutationsNoires.GrandRoquePossible || Strategies->PermutationsNoires.PetitRoquePossible);

	Strategie->IDPhaseA = 0;
	Strategie->IDPhaseB = 0;

	Strategie->GrandRoqueBlanc = false;
	Strategie->PetitRoqueBlanc = false;
	Strategie->GrandRoqueNoir = false;
	Strategie->GrandRoqueNoir = false;

	Strategie->NombreDeCapturesBlanches = 0;
	Strategie->NombreDeCapturesNoires = 0;

	Strategie->CoupsLibresBlancs = (Strategies->DemiCoups + 1) / 2;
	Strategie->CoupsLibresNoirs = (Strategies->DemiCoups / 2);

	Verifier(Strategie->CoupsLibresBlancs >= Strategies->CoupsParLesMortsBlancs);
	Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);

	Strategie->CapturesLibresPourLesPionsBlancs = Strategies->CapturesPourLesPionsBlancs;
	Strategie->CapturesLibresPourLesPionsNoirs = Strategies->CapturesPourLesPionsNoirs;

	for (cases Case = A1; Case < MaxCases; Case++)
		Strategie->Prise[Case] = false;

	return ProchaineStrategie(Strategies, Texte, true);
}

/*************************************************************/

bool ProchaineStrategie(strategies *Strategies, _texte Texte, bool PremiereFois)
{
	strategie *Strategie = &Strategies->StrategieActuelle;
	OutputMessage((texte)Texte);

	static const int MaxEtapes = 15;
	int Etape = PremiereFois ? 0 : MaxEtapes;

	static couleurs QuiCommencePourLesPions = BLANCS;

	static unsigned int SauvegardeCoupsLibresBlancs[MaxEtapes];
	static unsigned int SauvegardeCoupsLibresNoirs[MaxEtapes];

	do {
		for ( ; --Etape >= 0; ) {
			bool Possible = true;

			Verifier(SauvegardeCoupsLibresBlancs[Etape] == Strategie->CoupsLibresBlancs);
			Verifier(SauvegardeCoupsLibresNoirs[Etape] == Strategie->CoupsLibresNoirs);

			switch (Etape) {
				case 0 :
					Possible &= ChoisirLesRoques(&Strategies->PermutationsBlanches, Strategie->PiecesBlanches, BLANCS, &Strategie->GrandRoqueBlanc, &Strategie->PetitRoqueBlanc, false);
					break;
				case 1 :
					Possible &= ChoisirLesRoques(&Strategies->PermutationsNoires, Strategie->PiecesNoires, NOIRS, &Strategie->GrandRoqueNoir, &Strategie->PetitRoqueNoir, false);
					break;
				case 2 :
					Strategie->CoupsLibresBlancs -= Strategies->CoupsParLesMortsBlancs;
					Possible &= ChoisirLesCasesFinales(&Strategies->PermutationsBlanches, Strategie->Prise, Strategie->PiecesBlanches, Strategies->NombreDePiecesBlanches, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsBlancs, BLANCS, false);
					Strategie->CoupsLibresBlancs += Strategies->CoupsParLesMortsBlancs;
					break;
				case 3 :
					Strategie->CoupsLibresBlancs -= Strategies->CoupsParLesMortsBlancs;
					Possible &= SwitchbacksEvidents(Strategie->PiecesBlanches, &Strategie->CoupsLibresBlancs, Strategie->GrandRoqueBlanc, Strategie->PetitRoqueBlanc, false);
					Strategie->CoupsLibresBlancs += Strategies->CoupsParLesMortsBlancs;
					break;
				case 4 :
					Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);
					Strategie->CoupsLibresNoirs -= Strategies->CoupsParLesMortsNoirs;
					Possible &= ChoisirLesCasesFinales(&Strategies->PermutationsNoires, Strategie->Prise, Strategie->PiecesNoires, Strategies->NombreDePiecesNoires, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsNoirs, NOIRS, false);
					Strategie->CoupsLibresNoirs += Strategies->CoupsParLesMortsNoirs;
					break;
				case 5 :
					Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);
					Strategie->CoupsLibresNoirs -= Strategies->CoupsParLesMortsNoirs;
					Possible &= SwitchbacksEvidents(Strategie->PiecesNoires, &Strategie->CoupsLibresNoirs, Strategie->GrandRoqueNoir, Strategie->PetitRoqueNoir, false);
					Strategie->CoupsLibresNoirs += Strategies->CoupsParLesMortsNoirs;
					break;
				case 6 :
					if (QuiCommencePourLesPions == BLANCS) {
						Strategie->CoupsLibresBlancs -= Strategies->CoupsParLesMortsBlancs;
						Possible &= TrajectoiresDesPions(Strategie->PiecesBlanches, Strategie->PiecesNoires, BLANCS, &Strategies->PermutationsNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, false);
						Strategie->CoupsLibresBlancs += Strategies->CoupsParLesMortsBlancs;
					}
					else {
						Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);
						Strategie->CoupsLibresNoirs -= Strategies->CoupsParLesMortsNoirs;
						Possible &= TrajectoiresDesPions(Strategie->PiecesNoires, Strategie->PiecesBlanches, NOIRS, &Strategies->PermutationsBlanches, &Strategie->CoupsLibresNoirs, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, false);
						Strategie->CoupsLibresNoirs += Strategies->CoupsParLesMortsNoirs;
					}
					break;
				case 7 :
					if (QuiCommencePourLesPions == BLANCS)
						Possible &= TrajectoiresDesPions(Strategie->PiecesNoires, Strategie->PiecesBlanches, NOIRS, &Strategies->PermutationsBlanches, &Strategie->CoupsLibresNoirs, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, false);
					else
						Possible &= TrajectoiresDesPions(Strategie->PiecesBlanches, Strategie->PiecesNoires, BLANCS, &Strategies->PermutationsNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, false);
					break;
				case 8 :
					Possible &= TrajectoiresDesPionsCaptures(Strategies, false);
					break;
				case 9 :
					Possible &= PionsDisparusSansTrace(Strategie->PiecesBlanches, Strategies->PermutationsBlanches.Destins, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsBlancs, BLANCS, false);
					break;
				case 10 :
					Possible &= TrajectoiresDesPionsBlancsDisparusSansTrace(Strategie->PiecesBlanches, Strategie->PiecesNoires, &Strategies->PermutationsNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, false);
					break;
				case 11 :
					Possible &= PionsDisparusSansTrace(Strategie->PiecesNoires, Strategies->PermutationsNoires.Destins, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsNoirs, NOIRS, false);
					break;
				case 12 :
					Possible &= TrajectoiresDesPionsNoirsDisparusSansTrace(Strategie->PiecesNoires, Strategie->PiecesBlanches, &Strategies->PermutationsBlanches, &Strategie->CoupsLibresNoirs, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, false);
					break;
				case 13 :
					Possible &= ChoisirLesAssassinats(Strategies, false);
					break;
				case 14 :
					Possible &= AutresSwitchbacks(Strategie->PiecesBlanches, Strategie->PiecesNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, false);
					break;
				default :
					break;
			}

			SauvegardeCoupsLibresBlancs[Etape] = Strategie->CoupsLibresBlancs;
			SauvegardeCoupsLibresNoirs[Etape] = Strategie->CoupsLibresNoirs;

			if (Possible)
				break;

			if (!Etape)
				return false;
		}

		for ( ; ++Etape < MaxEtapes; ) {
			bool Possible = true;

			switch (Etape) {
				case 0 :
					Possible &= ChoisirLesRoques(&Strategies->PermutationsBlanches, Strategie->PiecesBlanches, BLANCS, &Strategie->GrandRoqueBlanc, &Strategie->PetitRoqueBlanc, true);
					break;
				case 1 :
					Possible &= ChoisirLesRoques(&Strategies->PermutationsNoires, Strategie->PiecesNoires, NOIRS, &Strategie->GrandRoqueNoir, &Strategie->PetitRoqueNoir, true);
					break;
				case 2 :
					Strategie->CoupsLibresBlancs -= Strategies->CoupsParLesMortsBlancs;
					Possible &= ChoisirLesCasesFinales(&Strategies->PermutationsBlanches, Strategie->Prise, Strategie->PiecesBlanches, Strategies->NombreDePiecesBlanches, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsBlancs, BLANCS, true);
					Strategie->CoupsLibresBlancs += Strategies->CoupsParLesMortsBlancs;
					break;
				case 3 :
					Strategie->CoupsLibresBlancs -= Strategies->CoupsParLesMortsBlancs;
					Possible &= SwitchbacksEvidents(Strategie->PiecesBlanches, &Strategie->CoupsLibresBlancs, Strategie->GrandRoqueBlanc, Strategie->PetitRoqueBlanc, true);
					Strategie->CoupsLibresBlancs += Strategies->CoupsParLesMortsBlancs;
					break;
				case 4 :
					Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);
					Strategie->CoupsLibresNoirs -= Strategies->CoupsParLesMortsNoirs;
					Possible &= ChoisirLesCasesFinales(&Strategies->PermutationsNoires, Strategie->Prise, Strategie->PiecesNoires, Strategies->NombreDePiecesNoires, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsNoirs, NOIRS, true);
					Strategie->CoupsLibresNoirs += Strategies->CoupsParLesMortsNoirs;
					break;
				case 5 :
					Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);
					Strategie->CoupsLibresNoirs -= Strategies->CoupsParLesMortsNoirs;
					Possible &= SwitchbacksEvidents(Strategie->PiecesNoires, &Strategie->CoupsLibresNoirs, Strategie->GrandRoqueNoir, Strategie->PetitRoqueNoir, true);
					Strategie->CoupsLibresNoirs += Strategies->CoupsParLesMortsNoirs;
					break;
				case 6 :
					QuiCommencePourLesPions = (Strategie->CoupsLibresBlancs > Strategie->CoupsLibresNoirs) ? BLANCS : NOIRS;
					if (QuiCommencePourLesPions == BLANCS) {
						Strategie->CoupsLibresBlancs -= Strategies->CoupsParLesMortsBlancs;
						Possible &= TrajectoiresDesPions(Strategie->PiecesBlanches, Strategie->PiecesNoires, BLANCS, &Strategies->PermutationsNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, true);
						Strategie->CoupsLibresBlancs += Strategies->CoupsParLesMortsBlancs;
					}
					else {
						Verifier(Strategie->CoupsLibresNoirs >= Strategies->CoupsParLesMortsNoirs);
						Strategie->CoupsLibresNoirs -= Strategies->CoupsParLesMortsNoirs;
						Possible &= TrajectoiresDesPions(Strategie->PiecesNoires, Strategie->PiecesBlanches, NOIRS, &Strategies->PermutationsBlanches, &Strategie->CoupsLibresNoirs, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, true);
						Strategie->CoupsLibresNoirs += Strategies->CoupsParLesMortsNoirs;
					}
					break;
				case 7 :
					if (QuiCommencePourLesPions == BLANCS)
						Possible &= TrajectoiresDesPions(Strategie->PiecesNoires, Strategie->PiecesBlanches, NOIRS, &Strategies->PermutationsBlanches, &Strategie->CoupsLibresNoirs, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, true);
					else
						Possible &= TrajectoiresDesPions(Strategie->PiecesBlanches, Strategie->PiecesNoires, BLANCS, &Strategies->PermutationsNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, true);
					break;
				case 8 :
					Possible &= TrajectoiresDesPionsCaptures(Strategies, true);
					break;
				case 9 :
					Possible &= PionsDisparusSansTrace(Strategie->PiecesBlanches, Strategies->PermutationsBlanches.Destins, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsBlancs, BLANCS, true);
					break;
				case 10 :
					Possible &= TrajectoiresDesPionsBlancsDisparusSansTrace(Strategie->PiecesBlanches, Strategie->PiecesNoires, &Strategies->PermutationsNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, true);
					break;
				case 11 :
					Possible &= PionsDisparusSansTrace(Strategie->PiecesNoires, Strategies->PermutationsNoires.Destins, &Strategie->CoupsLibresNoirs, &Strategie->CapturesLibresPourLesPionsNoirs, NOIRS, true);
					break;
				case 12 :
					Possible &= TrajectoiresDesPionsNoirsDisparusSansTrace(Strategie->PiecesNoires, Strategie->PiecesBlanches, &Strategies->PermutationsBlanches, &Strategie->CoupsLibresNoirs, &Strategie->CoupsLibresBlancs, &Strategie->CapturesLibresPourLesPionsNoirs, &Strategie->CapturesLibresPourLesPionsBlancs, true);
					break;
				case 13 :
					Possible &= ChoisirLesAssassinats(Strategies, true);
					break;
				case 14 :
					Possible &= AutresSwitchbacks(Strategie->PiecesBlanches, Strategie->PiecesNoires, &Strategie->CoupsLibresBlancs, &Strategie->CoupsLibresNoirs, true);
					break;
				default :
					break;
			}

			SauvegardeCoupsLibresBlancs[Etape] = Strategie->CoupsLibresBlancs;
			SauvegardeCoupsLibresNoirs[Etape] = Strategie->CoupsLibresNoirs;

			if (!Possible)
				break;
		}

	} while (Etape < MaxEtapes);

	Strategie->IDFinal = 0;
	Strategie->IDPhaseA++;

	unsigned int SommeCoupsBlancs = 0;
	unsigned int SommeCoupsNoirs = 0;
	for (unsigned int k = 0; k < MaxHommes; k++) {
		SommeCoupsBlancs += Strategie->PiecesBlanches[k].Coups;
		SommeCoupsNoirs += Strategie->PiecesNoires[k].Coups;
	}

	Verifier(SommeCoupsBlancs + Strategie->CoupsLibresBlancs == (Strategies->DemiCoups + 1) / 2);
	Verifier(SommeCoupsNoirs + Strategie->CoupsLibresNoirs == (Strategies->DemiCoups / 2));

	return true;
}

/*************************************************************/

void InitialisationDesStrategies(strategies *Strategies, const position *Position)
{
	unsigned int i;

	Strategies->NombreDePiecesBlanches = Position->Blancs.NombreTotalDePieces;
	Strategies->NombreDePiecesNoires = Position->Noirs.NombreTotalDePieces;
	Strategies->DemiCoups = Position->Diagramme.DemiCoups;

	Strategies->CapturesPourLesPionsBlancs = Position->Blancs.NombreDeCapturesPourLesPions;
	Strategies->CapturesPourLesPionsNoirs = Position->Noirs.NombreDeCapturesPourLesPions;

	Strategies->CoupsParLesMortsBlancs = 0;
	for (i = 0; i < MaxHommes; i++)
		if (Position->PiecesBlanches[i].Piece == VIDE)
			Strategies->CoupsParLesMortsBlancs += Position->PiecesBlanches[i].Deplacements;

	Strategies->CoupsParLesMortsNoirs = 0;
	for (i = 0; i < MaxHommes; i++)
		if (Position->PiecesNoires[i].Piece == VIDE)
			Strategies->CoupsParLesMortsNoirs += Position->PiecesNoires[i].Deplacements;
}

/*************************************************************/

bool AutresSwitchbacks(vie PiecesBlanches[MaxHommes], vie PiecesNoires[MaxHommes], unsigned int *CoupsLibresBlancs, unsigned int *CoupsLibresNoirs, bool PremiereFois)
{
	if (!PremiereFois) {
		for (unsigned int i = 0; i < PIONA; i++) {
			if (PiecesBlanches[i].Switchback) {
				if (!PiecesBlanches[i].SwitchbackEvident) {
					Verifier(PiecesBlanches[i].Coups == 2);
					PiecesBlanches[i].Coups = 0;
					*CoupsLibresBlancs += 2;
				}

				PiecesBlanches[i].Switchback = false;
			}

			if (PiecesNoires[i].Switchback) {
				if (!PiecesNoires[i].SwitchbackEvident) {
					Verifier(PiecesNoires[i].Coups == 2);
					PiecesNoires[i].Coups = 0;
					*CoupsLibresNoirs += 2;
				}

				PiecesNoires[i].Switchback = false;
			}
		}

		return false;
	}

	for (unsigned int i = 0; i < PIONA; i++) {
		vie *Piece = &PiecesBlanches[i];
		unsigned int *CoupsLibres = CoupsLibresBlancs;

		for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
			if (Couleur == NOIRS) {
				Piece = &PiecesNoires[i];
				CoupsLibres = CoupsLibresNoirs;
			}

			Piece->CaseDuSwitchback = MaxCases;
			Piece->Switchback = Piece->SwitchbackEvident;
			if (Piece->Switchback)
				continue;

			if (Piece->Capturee)
				continue;

			if (Piece->Coups)
				continue;

			if ((i == TOURDAME) && (QuelleColonne(Piece->Scenario->CaseFinale) != A))
				continue;

			if ((i == TOURROI) && (QuelleColonne(Piece->Scenario->CaseFinale) != H))
				continue;

			if (CasesMortes[Piece->Scenario->CaseFinale])
				continue;

			unsigned int ExtraBlancs = 0;
			unsigned int ExtraNoirs = 0;

			AjouteUneCaseMorteTemporaire(Piece->Scenario->CaseFinale);

			for (unsigned int j = 0; j < MaxHommes; j++) {
				unsigned int CoupsRequisBlancs = CalculPrecisDesCoupsRequisAvecMeurtres(&PiecesBlanches[j]);
				Verifier(CoupsRequisBlancs >= PiecesBlanches[j].Coups);

				if (CoupsRequisBlancs == UINT_MAX) {
					ExtraBlancs = UINT_MAX;
					break;
				}

				ExtraBlancs += CoupsRequisBlancs - PiecesBlanches[j].Coups;

				unsigned int CoupsRequisNoirs = CalculPrecisDesCoupsRequisAvecMeurtres(&PiecesNoires[j]);
				Verifier(CoupsRequisNoirs >= PiecesNoires[j].Coups);

				ExtraNoirs += CoupsRequisNoirs - PiecesNoires[j].Coups;

				if (CoupsRequisNoirs == UINT_MAX) {
					ExtraNoirs = UINT_MAX;
					break;
				}
			}

			EnleveLaCaseMorteTemporaire(Piece->Scenario->CaseFinale);

			bool Possible = true;

			if ((ExtraBlancs > *CoupsLibresBlancs) || (ExtraNoirs > *CoupsLibresNoirs)) {
				if (*CoupsLibres < 2)
					Possible = false;

				if (Possible && SwitchbackPossible(Piece)) {
					Piece->CaseDuSwitchback = MaxCases;
					Piece->Switchback = true;
					Piece->Coups = 2;
					*CoupsLibres -= 2;
				}
			}

			if (!Possible) {
				for (unsigned int k = 0; k <= i ; k++) {
					if (PiecesBlanches[k].Switchback) {
						if (!PiecesBlanches[k].SwitchbackEvident) {
							Verifier(PiecesBlanches[k].Coups == 2);
							PiecesBlanches[k].Coups = 0;
							*CoupsLibresBlancs += 2;
						}

						PiecesBlanches[k].Switchback = false;
					}

					if (PiecesNoires[k].Switchback) {
						if (!PiecesNoires[k].SwitchbackEvident) {
							Verifier(PiecesNoires[k].Coups == 2);
							PiecesNoires[k].Coups = 0;
							*CoupsLibresNoirs += 2;
						}

						PiecesNoires[k].Switchback = false;
					}
				}

				return false;
			}
		}
	}

	bool ToujoursPossible = true;

	if (!PiecesBlanches[XROI].Coups) {
		bool SwitchbackRoyal = false;

		for (unsigned int k = 0; k < MaxHommes; k++) {
			for (unsigned int i = 0; i < PiecesNoires[k].NombreAssassinats; i++) {
				cases Case = PiecesNoires[k].Assassinats[i]->Scenario->CaseFinale;
				if (((i + 1) == PiecesNoires[k].NombreAssassinats) && (Case == PiecesNoires[k].Scenario->CaseFinale))
					continue;

				switch (PiecesNoires[k].Scenario->Piece) {
					case ROI :
					case DAME :
						if ((Case == D1) || (Case == D2) || (Case == E2) || (Case == F2) || (Case == F1))
							SwitchbackRoyal = true;
						break;
					case TOUR :
						if ((Case == D1) || (Case == E2) || (Case == F1))
							SwitchbackRoyal = true;
						break;
					case FOUBLANC :
						if ((Case == D2) || (Case == F2))
							SwitchbackRoyal = true;
						break;
					case CAVALIER :
						if ((Case == C2) || (Case == D3) || (Case == F3) || (Case == G2))
							SwitchbackRoyal = true;
						break;
					case FOUNOIR :
					case PION :
					default :
						break;
				}

				if (SwitchbackRoyal)
					break;
			}

			if (k >= PIONA) {
				const trajetpion *Trajet = PiecesNoires[k].TrajetSiPion;
				if ((Trajet->CaseFinale == D1) || (Trajet->CaseFinale == F1)) {
					if (Trajet->NombreDeCaptures) {
						if (QuelleRangee(Trajet->Captures[Trajet->NombreDeCaptures - 1]) > UN)
							SwitchbackRoyal = true;
					}
					else {
						SwitchbackRoyal = true;
					}
				}
				else if ((Trajet->CaseFinale == C1) || (Trajet->CaseFinale == G1)) {
					if (Trajet->NombreDeCaptures && (QuelleRangee(Trajet->Captures[Trajet->NombreDeCaptures - 1]) == UN)) {
						if (Trajet->NombreDeCaptures == 1) {
							if ((Trajet->CaseDepart == D7) || (Trajet->CaseDepart == F7))
								SwitchbackRoyal = true;
						}
						else {
							if ((QuelleColonne(Trajet->Captures[Trajet->NombreDeCaptures - 2]) == D) || (QuelleColonne(Trajet->Captures[Trajet->NombreDeCaptures - 2]) == F))
								SwitchbackRoyal = true;
						}
					}
				}
			}
		}

		if (SwitchbackRoyal) {
			if (*CoupsLibresBlancs >= 2) {
				*CoupsLibresBlancs -= 2;
				PiecesBlanches[XROI].CaseDuSwitchback = MaxCases;
				PiecesBlanches[XROI].Switchback = true;
				PiecesBlanches[XROI].Coups = 2;
			}
			else {
				ToujoursPossible = false;
			}
		}
	}

	if (!PiecesNoires[XROI].Coups) {
		bool SwitchbackRoyal = false;

		for (unsigned int k = 0; k < MaxHommes; k++) {
			for (unsigned int i = 0; i < PiecesBlanches[k].NombreAssassinats; i++) {
				cases Case = PiecesBlanches[k].Assassinats[i]->Scenario->CaseFinale;
				if (((i + 1) == PiecesBlanches[k].NombreAssassinats) && (Case == PiecesBlanches[k].Scenario->CaseFinale))
					continue;

				switch (PiecesBlanches[k].Scenario->Piece) {
					case ROI :
					case DAME :
						if ((Case == D8) || (Case == D7) || (Case == E7) || (Case == F7) || (Case == F8))
							SwitchbackRoyal = true;
						break;
					case TOUR :
						if ((Case == D8) || (Case == E7) || (Case == F8))
							SwitchbackRoyal = true;
						break;
					case FOUNOIR :
						if ((Case == D7) || (Case == F7))
							SwitchbackRoyal = true;
						break;
					case CAVALIER :
						if ((Case == C7) || (Case == D6) || (Case == F6) || (Case == G7))
							SwitchbackRoyal = true;
						break;
					case FOUBLANC :
					case PION :
					default :
						break;
				}

				if (SwitchbackRoyal)
					break;
			}

			if (k >= PIONA) {
				const trajetpion *Trajet = PiecesBlanches[k].TrajetSiPion;
				if ((Trajet->CaseFinale == D8) || (Trajet->CaseFinale == F8)) {
					if (Trajet->NombreDeCaptures) {
						if (QuelleRangee(Trajet->Captures[Trajet->NombreDeCaptures - 1]) < HUIT)
							SwitchbackRoyal = true;
					}
					else {
						SwitchbackRoyal = true;
					}
				}
				else if ((Trajet->CaseFinale == C8) || (Trajet->CaseFinale == G8)) {
					if (Trajet->NombreDeCaptures && (QuelleRangee(Trajet->Captures[Trajet->NombreDeCaptures - 1]) == HUIT)) {
						if (Trajet->NombreDeCaptures == 1) {
							if ((Trajet->CaseDepart == D2) || (Trajet->CaseDepart == F2))
								SwitchbackRoyal = true;
						}
						else {
							if ((QuelleColonne(Trajet->Captures[Trajet->NombreDeCaptures - 2]) == D) || (QuelleColonne(Trajet->Captures[Trajet->NombreDeCaptures - 2]) == F))
								SwitchbackRoyal = true;
						}
					}
				}
			}
		}

		if (SwitchbackRoyal) {
			if (*CoupsLibresNoirs >= 2) {
				*CoupsLibresNoirs -= 2;
				PiecesNoires[XROI].CaseDuSwitchback = MaxCases;
				PiecesNoires[XROI].Switchback = true;
				PiecesNoires[XROI].Coups = 2;
			}
			else {
				ToujoursPossible = false;
			}
		}
	}

	if (!ToujoursPossible) {
		for (unsigned int k = 0; k < PIONA ; k++) {
			if (PiecesBlanches[k].Switchback) {
				if (!PiecesBlanches[k].SwitchbackEvident) {
					Verifier(PiecesBlanches[k].Coups == 2);
					PiecesBlanches[k].Coups = 0;
					*CoupsLibresBlancs += 2;
				}

				PiecesBlanches[k].Switchback = false;
			}

			if (PiecesNoires[k].Switchback) {
				if (!PiecesNoires[k].SwitchbackEvident) {
					Verifier(PiecesNoires[k].Coups == 2);
					PiecesNoires[k].Coups = 0;
					*CoupsLibresNoirs += 2;
				}

				PiecesNoires[k].Switchback = false;
			}
		}

		return false;
	}

	return true;
}

/*************************************************************/

bool SwitchbacksEvidents(vie Pieces[MaxHommes], unsigned int *CoupsLibres, bool GrandRoque, bool PetitRoque, bool PremiereFois)
{
	if (!PremiereFois) {
		for (unsigned int i = 0; i < PIONA; i++) {
			if (Pieces[i].SwitchbackEvident) {
				Verifier(Pieces[i].Coups == 2);

				Pieces[i].SwitchbackEvident = false;
				Pieces[i].Coups = 0;
				*CoupsLibres += 2;
			}
		}

		return false;
	}

	unsigned int CoupsNecessaires = 0;

	vie *TourDame = &Pieces[TOURDAME];
	vie *CavalierDame = &Pieces[CAVALIERDAME];
	vie *PionA = &Pieces[PIONA];

	vie *TourRoi = &Pieces[TOURROI];
	vie *CavalierRoi = &Pieces[CAVALIERROI];
	vie *PionH = &Pieces[PIONH];

	vie *FouDame = &Pieces[FOUDAME];
	vie *FouRoi = &Pieces[FOUROI];
	vie *Dame = &Pieces[XDAME];

	if (GrandRoque) {
		if (!Dame->Coups && !Dame->Capturee) {
			Dame->SwitchbackEvident = true;
			Dame->Coups = 2;
			CoupsNecessaires += 2;
		}

		if (!FouDame->Coups && !FouDame->Capturee) {
			FouDame->SwitchbackEvident = true;
			FouDame->Coups = 2;
			CoupsNecessaires += 2;
		}

		if (!CavalierDame->Coups && !CavalierDame->Capturee) {
			CavalierDame->SwitchbackEvident = true;
			CavalierDame->Coups = 2;
			CoupsNecessaires += 2;
		}
	}
	else if (PetitRoque) {
		if (!FouRoi->Coups && !FouRoi->Capturee) {
			FouRoi->SwitchbackEvident = true;
			FouRoi->Coups = 2;
			CoupsNecessaires += 2;
		}

		if (!CavalierRoi->Coups && !CavalierRoi->Capturee) {
			CavalierRoi->SwitchbackEvident = true;
			CavalierRoi->Coups = 2;
			CoupsNecessaires += 2;
		}
	}

	if (TourDame->Coups && !CavalierDame->Coups && !CavalierDame->Capturee && PionA->Scenario && !PionA->Scenario->Coups) {
		CavalierDame->SwitchbackEvident = true;
		CavalierDame->Coups = 2;
		CoupsNecessaires += 2;
	}

	if (TourRoi->Coups && !CavalierRoi->Coups && !CavalierRoi->Capturee && PionH->Scenario && !PionH->Scenario->Coups) {
		CavalierRoi->SwitchbackEvident = true;
		CavalierRoi->Coups = 2;
		CoupsNecessaires += 2;
	}

	if (CoupsNecessaires > *CoupsLibres) {
		for (unsigned int i = 0; i < PIONA; i++) {
			if (Pieces[i].SwitchbackEvident) {
				Pieces[i].SwitchbackEvident = false;
				Pieces[i].Coups = 0;
			}
		}

		return false;
	}

	*CoupsLibres -= CoupsNecessaires;
	return true;
}

/*************************************************************/

bool PiecesCaptureesParLePion(vie *Pion, const permutations *Permutations, vie PiecesAdverses[MaxHommes], unsigned int *CoupsLibres, unsigned int *CapturesLibres, bool PremiereFois)
{
	unsigned int k = PremiereFois ? 0 : Pion->TrajetSiPion->NombreDeCaptures;

	do {
		for ( ; (int)(--k) >= 0; ) {
			const scenario *Scenario = Pion->ScenariosSiPion[k];

			Verifier(Scenario);

			bool ExSpecial = (Scenario->Homme >= PIONA) && (PiecesAdverses[Scenario->Homme].TrajetSiPion != NULL);

			if (ExSpecial) {
				Verifier(PiecesAdverses[Scenario->Homme].Scenario != Scenario);
				PiecesAdverses[Scenario->Homme].Assassin = NULL;
			}
			else {
				Verifier(PiecesAdverses[Scenario->Homme].Scenario == Scenario);

				*CoupsLibres += Scenario->Coups;
				*CapturesLibres += Scenario->Captures;
				PiecesAdverses[Scenario->Homme].Assassin = NULL;
				PiecesAdverses[Scenario->Homme].Scenario = NULL;
				PiecesAdverses[Scenario->Homme].Promue = false;
				PiecesAdverses[Scenario->Homme].Coups = 0;
			}

			bool Possible = false;
			while (!Possible) {
				Scenario++;

				bool Special = (Scenario->Coups < UINT_MAX) && PiecesAdverses[Scenario->Homme].Scenario && !PiecesAdverses[Scenario->Homme].Assassin;

				if (Special) {
					const scenario *VraiScenario = PiecesAdverses[Scenario->Homme].Scenario;
					Verifier(Scenario->Homme >= PIONA);
					if (Scenario->CaseFinale == VraiScenario->CaseFinale) {
						if ((Scenario->Piece == VraiScenario->Piece) && (Scenario->Promotion == VraiScenario->Promotion)) {
							Verifier(Scenario->Coups == VraiScenario->Coups);
							PiecesAdverses[Scenario->Homme].Assassin = Pion;
							Pion->ScenariosSiPion[k] = Scenario;
							Possible = true;
						}
					}
				}
				else {
					if (Scenario->Coups > *CoupsLibres) {
						Pion->ScenariosSiPion[k] = NULL;
						break;
					}

					if (!PiecesAdverses[Scenario->Homme].Assassin && (Scenario->Captures <= *CapturesLibres)) {
						*CoupsLibres -= Scenario->Coups;
						*CapturesLibres -= Scenario->Captures;
						PiecesAdverses[Scenario->Homme].Scenario = Scenario;
						PiecesAdverses[Scenario->Homme].Assassin = Pion;
						PiecesAdverses[Scenario->Homme].Coups = Scenario->Coups;
						PiecesAdverses[Scenario->Homme].Promue = (Scenario->Promotion < MaxColonnes) ? true : false;
						Pion->ScenariosSiPion[k] = Scenario;
						Possible = true;
					}
				}
			}

			if (!Possible && !k)
				return false;

			if (Possible)
				break;
		}

		for ( ; ++k < Pion->TrajetSiPion->NombreDeCaptures; ) {
			const scenario *Scenario = Pion->ScenariosSiPion[k];
			Verifier(!Scenario);

			Scenario = &Permutations->ScenariosSuicides[Pion->TrajetSiPion->Captures[k]][0];

			bool Possible = false;
			while (!Possible) {
				bool Special = (Scenario->Coups < UINT_MAX) && PiecesAdverses[Scenario->Homme].Scenario && !PiecesAdverses[Scenario->Homme].Assassin;

				if (Special) {
					const scenario *VraiScenario = PiecesAdverses[Scenario->Homme].Scenario;
					Verifier(Scenario->Homme >= PIONA);
					if (Scenario->CaseFinale == VraiScenario->CaseFinale) {
						if ((Scenario->Piece == VraiScenario->Piece) && (Scenario->Promotion == VraiScenario->Promotion)) {
							Verifier(Scenario->Coups == VraiScenario->Coups);
							PiecesAdverses[Scenario->Homme].Assassin = Pion;
							Pion->ScenariosSiPion[k] = Scenario;
							Possible = true;
						}
					}
				}
				else {
					if (Scenario->Coups >= UINT_MAX)
						break;

					if (Scenario->Coups <= *CoupsLibres) {
						if (!PiecesAdverses[Scenario->Homme].Assassin && (Scenario->Captures <= *CapturesLibres)) {
							*CoupsLibres -= Scenario->Coups;
							*CapturesLibres -= Scenario->Captures;
							PiecesAdverses[Scenario->Homme].Scenario = Scenario;
							PiecesAdverses[Scenario->Homme].Assassin = Pion;
							PiecesAdverses[Scenario->Homme].Coups = Scenario->Coups;
							PiecesAdverses[Scenario->Homme].Promue = (Scenario->Promotion < MaxColonnes) ? true : false;
							Pion->ScenariosSiPion[k] = Scenario;
							Possible = true;
						}
					}
				}

				Scenario++;
			}

			if (!Possible) {
				if (!k)
					return false;

				break;
			}
		}

	} while ((int)k < (int)Pion->TrajetSiPion->NombreDeCaptures);

	return true;
}

/*************************************************************/

bool TrajectoiresDesPionsCaptures(strategies *Strategies, bool PremiereFois)
{
	static bool Calculer[2 * MaxHommes][MaxHommes];
	static int MaxNiveau = -1;

	int Niveau = PremiereFois ? 0 : MaxNiveau + 1;

	if (PremiereFois)
		MaxNiveau = -1;
	else if (MaxNiveau < 0)
		return false;

	strategie *Strategie = &Strategies->StrategieActuelle;

	for ( ; ; ) {
		for ( ; --Niveau >= 0; ) {
			couleurs Couleur = (Niveau % 2) ? NOIRS : BLANCS;

			unsigned int *CapturesLibresAdverses = (Couleur == BLANCS) ? &Strategie->CapturesLibresPourLesPionsNoirs : &Strategie->CapturesLibresPourLesPionsBlancs;
			unsigned int *CapturesLibres = (Couleur == BLANCS) ? &Strategie->CapturesLibresPourLesPionsBlancs : &Strategie->CapturesLibresPourLesPionsNoirs;
			unsigned int *CoupsLibresAdverses = (Couleur == BLANCS) ? &Strategie->CoupsLibresNoirs : &Strategie->CoupsLibresBlancs;
			unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Strategie->CoupsLibresBlancs : &Strategie->CoupsLibresNoirs;

			vie *Pieces = (Couleur == BLANCS) ? Strategie->PiecesBlanches : Strategie->PiecesNoires;
			vie *PiecesAdverses = (Couleur == BLANCS) ? Strategie->PiecesNoires : Strategie->PiecesBlanches;

			const permutations *PermutationsAdverses = (Couleur == BLANCS) ? &Strategies->PermutationsNoires : &Strategies->PermutationsBlanches;

			if (Niveau > MaxNiveau)
				continue;

			bool Possible = XTrajectoiresDesPions(Pieces, PiecesAdverses, Couleur, PermutationsAdverses, CoupsLibres, CoupsLibresAdverses, CapturesLibres, CapturesLibresAdverses, Calculer[Niveau], false);

			if (Possible)
				break;

			MaxNiveau--;
		}

		if (!PremiereFois && (Niveau < 0))
			break;

		PremiereFois = false;
		for (Niveau++ ; ; Niveau++) {
			couleurs Couleur = (Niveau % 2) ? NOIRS : BLANCS;

			unsigned int *CapturesLibresAdverses = (Couleur == BLANCS) ? &Strategie->CapturesLibresPourLesPionsNoirs : &Strategie->CapturesLibresPourLesPionsBlancs;
			unsigned int *CapturesLibres = (Couleur == BLANCS) ? &Strategie->CapturesLibresPourLesPionsBlancs : &Strategie->CapturesLibresPourLesPionsNoirs;
			unsigned int *CoupsLibresAdverses = (Couleur == BLANCS) ? &Strategie->CoupsLibresNoirs : &Strategie->CoupsLibresBlancs;
			unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Strategie->CoupsLibresBlancs : &Strategie->CoupsLibresNoirs;

			vie *Pieces = (Couleur == BLANCS) ? Strategie->PiecesBlanches : Strategie->PiecesNoires;
			vie *PiecesAdverses = (Couleur == BLANCS) ? Strategie->PiecesNoires : Strategie->PiecesBlanches;

			const permutations *PermutationsAdverses = (Couleur == BLANCS) ? &Strategies->PermutationsNoires : &Strategies->PermutationsBlanches;

			bool Allonsy = false;
			for (hommes Pion = PIONA; Pion <= PIONH; Pion++) {
				Calculer[Niveau][Pion] = false;

				if (Pieces[Pion].Scenario && !Pieces[Pion].TrajetSiPion) {
					Calculer[Niveau][Pion] = true;
					Allonsy = true;
				}
			}

			if (!Allonsy && (Niveau > 0))
				return true;

			if (!Allonsy)
				continue;

			bool Possible = XTrajectoiresDesPions(Pieces, PiecesAdverses, Couleur, PermutationsAdverses, CoupsLibres, CoupsLibresAdverses, CapturesLibres, CapturesLibresAdverses, Calculer[Niveau], true);
			if (!Possible)
				break;

			MaxNiveau = Niveau;
			continue;
		}
	}

	return false;
}

/*************************************************************/

bool TrajectoiresDesPionsNoirsDisparusSansTrace(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool PremiereFois)
{
	static bool Calculer[MaxHommes];

	if (PremiereFois)
		for (hommes Pion = PIONA; Pion <= PIONH; Pion++)
			Calculer[Pion] = (Pieces[Pion].TrajetSiPion == NULL) ? true : false;

	return XTrajectoiresDesPions(Pieces, PiecesAdverses, NOIRS, PermutationsAdverses, CoupsLibres, CoupsLibresAdverses, CapturesLibres, CapturesLibresAdverses, Calculer, PremiereFois);
}

/*************************************************************/

bool TrajectoiresDesPionsBlancsDisparusSansTrace(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool PremiereFois)
{
	static bool Calculer[MaxHommes];

	if (PremiereFois)
		for (hommes Pion = PIONA; Pion <= PIONH; Pion++)
			Calculer[Pion] = Pieces[Pion].Capturee && !Pieces[Pion].Assassin;

	return XTrajectoiresDesPions(Pieces, PiecesAdverses, BLANCS, PermutationsAdverses, CoupsLibres, CoupsLibresAdverses, CapturesLibres, CapturesLibresAdverses, Calculer, PremiereFois);
}

/*************************************************************/

bool PionsDisparusSansTrace(vie Pieces[MaxHommes], const destin Destins[MaxHommes], unsigned int *CoupsLibres, unsigned int *CapturesLibres, couleurs Couleur, bool PremiereFois)
{
	static bool CalculerBlanc[MaxHommes];
	static bool CalculerNoir[MaxHommes];

	bool *Calculer = (Couleur == BLANCS) ? CalculerBlanc : CalculerNoir;

	if (PremiereFois)
		for (hommes Pion = PIONA; Pion <= PIONH; Pion++)
			Calculer[Pion] = Pieces[Pion].Capturee && !Pieces[Pion].Assassin;

	hommes Pion = PremiereFois ? PIONA : MaxHommes;

	do {
		for ( ; --Pion >= PIONA; ) {
			vie *Piece = &Pieces[Pion];

			if (!Calculer[Pion])
				continue;

			Verifier(Piece->Scenario);

			*CoupsLibres += Piece->Scenario->Coups;
			*CapturesLibres += Piece->Scenario->Captures;
			Piece->Promue = false;
			Piece->Coups = 0;

			bool Possible = false;

			do {
				Piece->Scenario++;

				if (Piece->Scenario->Coups > *CoupsLibres)
					break;

				if (Piece->Scenario->Captures <= *CapturesLibres)
					Possible = true;

			} while (!Possible);

			if (!Possible) {
				Piece->Scenario = NULL;
				continue;
			}

			*CoupsLibres -= Piece->Scenario->Coups;
			*CapturesLibres -= Piece->Scenario->Captures;
			Piece->Coups = Piece->Scenario->Coups;
			Piece->Promue = (Piece->Scenario->Promotion < MaxColonnes) ? true : false;
			break;
		}

		if (!PremiereFois && (Pion < PIONA))
			return false;

		PremiereFois = false;
		for ( ; ++Pion < MaxHommes; ) {
			vie *Piece = &Pieces[Pion];
            const destin *Destin = &Destins[Pion];

			if (!Calculer[Pion])
				continue;

			Verifier(!Piece->Scenario);

			Piece->Scenario = &Destin->ScenariosMorts[0];
			bool Possible = false;

			while (!Possible) {
				if (Piece->Scenario->Coups > *CoupsLibres)
					break;

				if (Piece->Scenario->Captures <= *CapturesLibres) {
					Possible = true;
				}
				else {
					Piece->Scenario++;
				}
			}

			if (!Possible) {
				Piece->Scenario = NULL;
				break;
			}

			*CoupsLibres -= Piece->Scenario->Coups;
			*CapturesLibres -= Piece->Scenario->Captures;
			Piece->Coups = Piece->Scenario->Coups;
			Piece->Promue = (Piece->Scenario->Promotion < MaxColonnes) ? true : false;
		}
	} while (Pion < MaxHommes);

	return true;
}

/*************************************************************/

bool TrajectoiresDesPions(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], couleurs Couleur, const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool PremiereFois)
{
	static bool CalculerBlanc[MaxHommes];
	static bool CalculerNoir[MaxHommes];

	bool *Calculer = (Couleur == BLANCS) ? CalculerBlanc : CalculerNoir;

	if (PremiereFois)
		for (hommes Pion = PIONA; Pion <= PIONH; Pion++)
			Calculer[Pion] = !Pieces[Pion].Capturee;

	return XTrajectoiresDesPions(Pieces, PiecesAdverses, Couleur, PermutationsAdverses, CoupsLibres, CoupsLibresAdverses, CapturesLibres, CapturesLibresAdverses, Calculer, PremiereFois);
}

/*************************************************************/

bool XTrajectoiresDesPions(vie Pieces[MaxHommes], vie PiecesAdverses[MaxHommes], couleurs Couleur, const permutations *PermutationsAdverses, unsigned int *CoupsLibres, unsigned int *CoupsLibresAdverses, unsigned int *CapturesLibres, unsigned int *CapturesLibresAdverses, bool Calculer[MaxHommes], bool PremiereFois)
{
	hommes Pion = PremiereFois ? PIONA : MaxHommes;

	do {
		for ( ; --Pion >= PIONA; ) {
			vie *Piece = &Pieces[Pion];
			if (!Calculer[Pion])
				continue;

			Verifier(Piece->TrajetSiPion);

			if (Piece->TrajetSiPion->NombreDeCaptures)
				if (PiecesCaptureesParLePion(Piece, PermutationsAdverses, PiecesAdverses, CoupsLibresAdverses, CapturesLibresAdverses, false))
					break;

			*CapturesLibres += (Piece->TrajetSiPion->NombreDeCaptures - Piece->Scenario->Captures);
			*CoupsLibres += (Piece->Coups - Piece->Scenario->Coups);
			Piece->Coups = Piece->Scenario->Coups;

			bool Possible = false;

			while (!Possible) {
				Piece->TrajetSiPion++;

				unsigned int NouvellesCaptures = Piece->TrajetSiPion->NombreDeCaptures - Piece->Scenario->Captures;
				if (NouvellesCaptures > *CapturesLibres) {
					Piece->TrajetSiPion = NULL;
					break;
				}

				unsigned int CoupExtra = 0;
				if ((Piece->TrajetSiPion->NombreDeCoupsMin == Piece->TrajetSiPion->NombreDeCoupsMax) && (Piece->TrajetSiPion->NombreDeCoupsMax >= 2)) {
					if (!*CoupsLibres)
						continue;

					CoupExtra = 1;
				}

				Possible = Piece->TrajetSiPion->NombreDeCaptures ? PiecesCaptureesParLePion(Piece, PermutationsAdverses, PiecesAdverses, CoupsLibresAdverses, CapturesLibresAdverses, true) : true;

				if (Possible) {
					Piece->Coups = Piece->Scenario->Coups + CoupExtra;
					*CapturesLibres -= NouvellesCaptures;
					*CoupsLibres -= CoupExtra;
				}
			}

			if (Possible)
				break;
		}

		if (!PremiereFois && (Pion < PIONA))
			return false;

		PremiereFois = false;
		for ( ; ++Pion <= PIONH; ) {
			vie *Piece = &Pieces[Pion];
			if (!Calculer[Pion])
				continue;

			Verifier(!Piece->TrajetSiPion);
			Verifier(Piece->Promue || (Piece->Scenario->Coups < 6));

			cases CaseFinale = Piece->Scenario->CaseFinale;
			if (Piece->Promue)
				CaseFinale = QuelleCase(Piece->Scenario->Promotion, (Couleur == BLANCS) ? HUIT : UN);

			Piece->TrajetSiPion = ObtenirLesTrajetsDePion((colonnes)(Pion - PIONA), CaseFinale, Couleur);

			bool Possible = false;
			while (!Possible) {
				unsigned int NouvellesCaptures = Piece->TrajetSiPion->NombreDeCaptures - Piece->Scenario->Captures;

				if (Piece->TrajetSiPion->NombreDeCaptures < Piece->Scenario->Captures) {
					Piece->TrajetSiPion++;
					continue;
				}

				if (NouvellesCaptures > *CapturesLibres) {
					Piece->TrajetSiPion = NULL;
					break;
				}

				unsigned int CoupExtra = 0;
				if ((Piece->TrajetSiPion->NombreDeCoupsMin == Piece->TrajetSiPion->NombreDeCoupsMax) && (Piece->TrajetSiPion->NombreDeCoupsMax >= 2)) {
					if (!*CoupsLibres) {
						Piece->TrajetSiPion++;
						continue;
					}

					CoupExtra = 1;
				}

				Possible = Piece->TrajetSiPion->NombreDeCaptures ? PiecesCaptureesParLePion(Piece, PermutationsAdverses, PiecesAdverses, CoupsLibresAdverses, CapturesLibresAdverses, true) : true;
				if (Possible) {
					Piece->Coups = Piece->Scenario->Coups + CoupExtra;
					*CapturesLibres -= NouvellesCaptures;
					*CoupsLibres -= CoupExtra;
				}
				else {
					Piece->TrajetSiPion++;
				}
			}

			if (!Possible)
				break;
		}

	} while (Pion <= PIONH);

	return true;
}

/*************************************************************/

bool ChoisirLesAssassinats(strategies *Strategies, bool PremiereFois)
{
	static hommes MortsBlancs[MaxHommes];
	static hommes MortsNoirs[MaxHommes];
	static unsigned int NombreMortsBlancs = 0;
	static unsigned int NombreMortsNoirs = 0;
	static unsigned int NombreMorts = 0;

	static unsigned int IndexBlanc = 0;
	static unsigned int IndexNoir = 0;
	static couleurs Couleurs[2 * MaxHommes];


	strategie *Strategie = &Strategies->StrategieActuelle;

	if (PremiereFois) {
		NombreMortsBlancs = 0;
		NombreMortsNoirs = 0;
		IndexBlanc = 0;
		IndexNoir = 0;

		for (unsigned int i = 0; i < MaxHommes; i++) {
			if (Strategie->PiecesBlanches[i].Capturee && !Strategie->PiecesBlanches[i].Assassin)
				MortsBlancs[NombreMortsBlancs++] = i;

			if (Strategie->PiecesNoires[i].Capturee && !Strategie->PiecesNoires[i].Assassin)
				MortsNoirs[NombreMortsNoirs++] = i;
		}

		NombreMorts = NombreMortsBlancs + NombreMortsNoirs;
	}

	if (!NombreMorts)
		return PremiereFois;

	unsigned int Index = PremiereFois ? 0 : NombreMortsBlancs + NombreMortsNoirs;

	do {
		for ( ; Index-- > 0; ) {
			couleurs Couleur = Couleurs[Index];
			Verifier(IndexBlanc + IndexNoir == Index + 1);

			hommes Homme = (Couleur == BLANCS) ? MortsBlancs[IndexBlanc - 1] : MortsNoirs[IndexNoir - 1];
			vie *Piece = (Couleur == BLANCS) ? &Strategie->PiecesBlanches[Homme] : &Strategie->PiecesNoires[Homme];
			vie *Adversaires = (Couleur == BLANCS) ? Strategie->PiecesNoires : Strategie->PiecesBlanches;

			unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Strategie->CoupsLibresBlancs : &Strategie->CoupsLibresNoirs;
			unsigned int *CoupsLibresAdverses = (Couleur == BLANCS) ? &Strategie->CoupsLibresNoirs : &Strategie->CoupsLibresBlancs;

			Verifier(Homme != XROI);
			Verifier(Piece->Scenario);

			bool Possible = ChoisirAssassin(Piece, Adversaires, CoupsLibresAdverses, false);
			if (Possible)
				break;

			if (Homme < PIONA) {
				if (Piece->NombreAssassinats > 0) {
					const scenario *TempScenario = Piece->Scenario;
					Piece->Scenario = NULL;
					unsigned int CoupsRequis = CalculDesCoupsAvecMeurtres(Piece, Homme, Piece->Depart);
					Piece->Scenario = TempScenario;
					Verifier(CoupsRequis <= Piece->Coups);
					unsigned int Extra = Piece->Coups - CoupsRequis;
					Piece->Coups = CoupsRequis;
					*CoupsLibres += Extra;
				}
				else {
					*CoupsLibres += Piece->Coups;
					Piece->Coups = 0;
				}

				Verifier(Piece->Scenario->Coups < UINT_MAX);
				Piece->Scenario++;

				if (Piece->NombreAssassinats > 0) {
					while (!Possible) {
						if (Piece->Scenario->Coups == UINT_MAX)
							break;

						unsigned int CoupsRequis = CalculDesCoupsAvecMeurtres(Piece, Homme, Piece->Depart);
						if (CoupsRequis == UINT_MAX)
							break;

						Verifier(CoupsRequis >= Piece->Coups);
						unsigned int Extra = CoupsRequis - Piece->Coups;

						if (Extra <= *CoupsLibres) {
							Possible = ChoisirAssassin(Piece, Adversaires, CoupsLibresAdverses, true);
							if (Possible) {
								*CoupsLibres -= Extra;
								Piece->Coups = CoupsRequis;
								break;
							}
						}

						Piece->Scenario++;
					}

					if (!Possible)
						Piece->Scenario = NULL;
				}
				else {
					while (Piece->Scenario->Coups <= *CoupsLibres) {
						Possible = ChoisirAssassin(Piece, Adversaires, CoupsLibresAdverses, true);

						if (Possible) {
							Piece->Coups = Piece->Scenario->Coups;
							*CoupsLibres -= Piece->Coups;
							break;
						}

						Piece->Scenario++;
					}
				}

				if (!Possible)
					Piece->Scenario = NULL;
			}

			if (Possible)
				break;

			if (Couleur == BLANCS)
				IndexBlanc--;
			else
				IndexNoir--;
		}

		if (!PremiereFois && ((int)Index < 0))
			return false;

		PremiereFois = false;
		for ( ; ++Index < NombreMorts; ) {
			couleurs Couleur = (Strategie->CoupsLibresBlancs <= Strategie->CoupsLibresNoirs) ? BLANCS : NOIRS;

			if ((Couleur == BLANCS) && (IndexBlanc >= NombreMortsBlancs))
				Couleur = NOIRS;
			if ((Couleur == NOIRS) && (IndexNoir >= NombreMortsNoirs))
				Couleur = BLANCS;

			Couleurs[Index] = Couleur;
			Verifier(IndexBlanc + IndexNoir == Index);

			hommes Homme = (Couleur == BLANCS) ? MortsBlancs[IndexBlanc] : MortsNoirs[IndexNoir];
			vie *Piece = (Couleur == BLANCS) ? &Strategie->PiecesBlanches[Homme] : &Strategie->PiecesNoires[Homme];
			const destin *Destin = (Couleur == BLANCS) ? &Strategies->PermutationsBlanches.Destins[Homme] : &Strategies->PermutationsNoires.Destins[Homme];
			vie *Adversaires = (Couleur == BLANCS) ? Strategie->PiecesNoires : Strategie->PiecesBlanches;

			unsigned int *CoupsLibres = (Couleur == BLANCS) ? &Strategie->CoupsLibresBlancs : &Strategie->CoupsLibresNoirs;
			unsigned int *CoupsLibresAdverses = (Couleur == BLANCS) ? &Strategie->CoupsLibresNoirs : &Strategie->CoupsLibresBlancs;

			bool Possible = false;

			if (Homme < PIONA) {
				Verifier(!Piece->Scenario);
				Verifier(Homme != XROI);

				Piece->Scenario = &Destin->ScenariosMorts[0];

				if (Piece->NombreAssassinats > 0) {
					while (!Possible) {
						if (Piece->Scenario->Coups == UINT_MAX)
							break;

						unsigned int CoupsRequis = CalculDesCoupsAvecMeurtres(Piece, Homme, Piece->Depart);
						if (CoupsRequis == UINT_MAX)
							break;

						Verifier(CoupsRequis >= Piece->Coups);
						unsigned int Extra = CoupsRequis - Piece->Coups;

						if (Extra <= *CoupsLibres) {
							Possible = ChoisirAssassin(Piece, Adversaires, CoupsLibresAdverses, true);
							if (Possible) {
								*CoupsLibres -= Extra;
								Piece->Coups = CoupsRequis;
								break;
							}
						}

						Piece->Scenario++;
					}

					if (!Possible)
						Piece->Scenario = NULL;
				}
				else {
					while (Piece->Scenario->Coups <= *CoupsLibres) {
						Possible = ChoisirAssassin(Piece, Adversaires, CoupsLibresAdverses, true);

						if (Possible) {
							Piece->Coups = Piece->Scenario->Coups;
							*CoupsLibres -= Piece->Coups;
							break;
						}

						Piece->Scenario++;
					}
				}

				if (!Possible)
					Piece->Scenario = NULL;
			}
			else {
				Verifier(Piece->Scenario);
				Possible = ChoisirAssassin(Piece, Adversaires, CoupsLibresAdverses, true);
			}

			if (!Possible)
				break;

			if (Couleur == BLANCS)
				IndexBlanc++;
			else
				IndexNoir++;
		}

	} while (Index < NombreMorts);

	return true;
}

/*************************************************************/

bool ChoisirAssassin(vie *Victime, vie AssassinsPlausibles[MaxHommes], unsigned int *CoupsLibresAdverses, bool PremiereFois)
{
	unsigned int IndexVictime = UINT_MAX;

	Verifier(Victime->Scenario);
	Verifier(Victime->Scenario->Homme != XROI);

	if (PremiereFois) {
		Verifier(!Victime->Assassin);

		Victime->Assassin = &AssassinsPlausibles[XROI];
	}
	else {
		Verifier(Victime->Assassin);

		for (unsigned int k = 0; k < Victime->Assassin->NombreAssassinats; k++) {
			if (Victime->Assassin->Assassinats[k] == Victime) {
				IndexVictime = k;
				break;
			}
		}

		Verifier(IndexVictime < UINT_MAX);

		hommes Homme = (hommes)(Victime->Assassin - AssassinsPlausibles);
		cases Depart = Victime->Assassin->Depart;
		unsigned int CoupsPion = 0;
		if (Homme >= PIONA) {
			Homme = QuelHomme(Victime->Assassin->Scenario->Piece, true, false);
			Depart = QuelleCase(Victime->Assassin->Scenario->Promotion, (QuelleRangee(Depart) == DEUX) ? HUIT : UN);
			CoupsPion = Victime->Assassin->TrajetSiPion->NombreDeCoupsMin;
		}

		Victime->Assassin->Assassinats[IndexVictime] = NULL;
		unsigned int Coups = CoupsPion + CalculDesCoupsAvecMeurtres(Victime->Assassin, Homme, Depart);
		Verifier(Coups >= CoupsPion);
		unsigned int Extra = Victime->Assassin->Coups - Coups;
		Victime->Assassin->Coups = Coups;
		*CoupsLibresAdverses += Extra;
	}

	bool Possible = false;
	bool PremierEssai = PremiereFois;
	for ( ; !Possible && (Victime->Assassin < &AssassinsPlausibles[MaxHommes]); Victime->Assassin++, PremierEssai = true) {
		vie *Assassin = Victime->Assassin;
		hommes Homme = (hommes)(Victime->Assassin - AssassinsPlausibles);
		cases Depart = Assassin->Depart;
		unsigned int CoupsPion = 0;

		if ((Homme >= PIONA) && (!Assassin->Promue))
			continue;

		if (Assassin->Assassin == Victime)
			continue;

		if (Homme >= PIONA) {
			Homme = QuelHomme(Assassin->Scenario->Piece, true, false);
			Depart = QuelleCase(Assassin->Scenario->Promotion, (QuelleRangee(Depart) == DEUX) ? HUIT : UN);
			CoupsPion = Assassin->TrajetSiPion->NombreDeCoupsMin;
		}

		while (!Possible) {
			if (PremierEssai) {
				for (unsigned int k = Assassin->NombreAssassinats; k-- > 0; )
					Assassin->Assassinats[k + 1] = Assassin->Assassinats[k];

				Assassin->Assassinats[0] = Victime;
				Assassin->NombreAssassinats++;
				PremierEssai = false;
				IndexVictime = 0;
			}
			else {
				Assassin->Assassinats[IndexVictime] = Assassin->Assassinats[IndexVictime + 1];
				Assassin->Assassinats[IndexVictime + 1] = Victime;
				if (++IndexVictime >= Assassin->NombreAssassinats) {
					Assassin->NombreAssassinats--;
					break;
				}
			}

			unsigned int Coups = CalculDesCoupsAvecMeurtres(Assassin, Homme, Depart);
			if (Coups < UINT_MAX)
				Coups += CoupsPion;

			Verifier(Coups >= Assassin->Coups);
			unsigned int Extra = Coups - Assassin->Coups;

			if (Extra <= *CoupsLibresAdverses) {
				Assassin->Coups += Extra;
				*CoupsLibresAdverses -= Extra;
				Possible = true;
			}
		}
	}

	if (!Possible)
		Victime->Assassin = NULL;
	else {
		Victime->Assassin--;
		Verifier(Victime->Assassin->NombreAssassinats > 0);
	}

	return Possible;
}

/*************************************************************/

unsigned int CalculPrecisDesCoupsRequisAvecMeurtres(const vie *Piece)
{
	unsigned int Coups = 0;
	cases Depart = Piece->Depart;

	if (Piece->Scenario->Homme >= PIONA) {
		Coups += Piece->TrajetSiPion->NombreDeCoupsMin;

		if (!Piece->Promue)
			return Piece->Coups;

		Depart = QuelleCase(Piece->Scenario->Promotion, (QuelleRangee(Depart) == DEUX) ? HUIT : UN);
	}

	cases De = Depart;

	for (unsigned int k = 0; k < Piece->NombreAssassinats; k++) {
		cases Vers = Piece->Assassinats[k]->Scenario->CaseFinale;

		unsigned int Ajout = MeilleureTrajectoire(Piece->Scenario->Piece, De, Vers);
		if (Ajout == UINT_MAX)
			return UINT_MAX;

		if (!Ajout)
			Ajout = 2;

		Coups += Ajout;
		De = Vers;
	}

	unsigned int Ajout = MeilleureTrajectoire(Piece->Scenario->Piece, De, Piece->Scenario->CaseFinale);
	if (Ajout == UINT_MAX)
		return UINT_MAX;

	Coups += Ajout;

	if ((Piece->Scenario->Homme == XROI) && (QuelleColonne(Depart) != E))
		Coups++;

	if (!Coups && (Piece->SwitchbackEvident || Piece->Switchback)) {
		if (!SwitchbackPossible(Piece))
			return UINT_MAX;

		Coups = 2;
	}

	return Coups;
}

/*************************************************************/

bool SwitchbackPossible(const vie *Piece)
{
	if (CasesMortes[Piece->Depart])
		return false;

	if (Piece->Scenario->Homme >= PIONA)
		return false;

	bool Possible = true;

	rangees RDepart = QuelleRangee(Piece->Depart);
	rangees RPions = MaxRangees;

	if (RDepart == UN)
		RPions = DEUX;
	if (RDepart == HUIT)
		RPions = SEPT;

	Verifier(RPions < MaxRangees);

	switch (Piece->Scenario->Homme) {
		case TOURDAME :
			Verifier(QuelleColonne(Piece->Depart) == A);
			if (CasesMortes[QuelleCase(A, RPions)] && CasesMortes[QuelleCase(B, RDepart)])
				Possible = false;
			break;
		case TOURROI :
			Verifier(QuelleColonne(Piece->Depart) == H);
			if (CasesMortes[QuelleCase(A, RPions)] && CasesMortes[QuelleCase(G, RDepart)])
				Possible = false;
			break;
		case FOUDAME :
			if (CasesMortes[QuelleCase(B, RPions)] && CasesMortes[QuelleCase(D, RPions)])
				Possible = false;
			break;
		case FOUROI :
			if (CasesMortes[QuelleCase(E, RPions)] && CasesMortes[QuelleCase(G, RPions)])
				Possible = false;
			break;
		default :
			break;
	}

	return Possible;
}

/*************************************************************/

unsigned int CalculDesCoupsAvecMeurtres(const vie *Assassin, hommes Homme, cases Depart)
{
	Verifier(Homme < PIONA);
	unsigned int Coups = 0;

	cases De = Depart;

	for (unsigned int k = 0; k < Assassin->NombreAssassinats; k++) {
		if (!Assassin->Assassinats[k])
			continue;

		cases Vers = Assassin->Assassinats[k]->Scenario->CaseFinale;

		unsigned int Ajout = CombienDeDeplacements(Homme, De, Vers);
		if (Ajout == UINT_MAX)
			return UINT_MAX;

		if (!Ajout)
			Ajout = 2;

		Coups += Ajout;
		De = Vers;
	}

	if (Assassin->Scenario) {
		Verifier(Assassin->Scenario->CaseFinale < MaxCases);

		unsigned int Ajout = CombienDeDeplacements(Homme, De, Assassin->Scenario->CaseFinale);
		if (Ajout == UINT_MAX)
			return UINT_MAX;

		Coups += Ajout;
	}

	if ((Homme == XROI) && (QuelleColonne(Depart) != E))
		Coups++;

	Verifier(!Assassin->Switchback);
	if (!Coups && Assassin->SwitchbackEvident)
		Coups = 2;

	return Coups;
}

/*************************************************************/

void Delete(strategies *Strategies)
{
	Verifier(!Strategies || (Strategies == &LaStrategie));
}

/*************************************************************/