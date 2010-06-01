#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Constantes.h"
#include "Erreur.h"
#include "Mouvements.h"
#include "Output.h"
#include "Position.h"

/*************************************************************/

void DecompteDesPieces(position* Position);
void VerificationDesEchecs(const position *Position);
void ProvenanceDesPions(bonhomme Pieces[MaxHommes], couleurs Couleur);
void ProvenanceDesPionsII(bonhomme Pieces[MaxHommes], unsigned int CapturesDisponibles, unsigned int *CapturesLibres);
void RestrictionDesMouvementsParLesPions(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CapturesLibres);
void RestrictionDeLaProvenanceDesPions(bonhomme Pieces[MaxHommes]);
void FousDePromotion(position *Position, couleurs Couleur);
void CalculDesTrajectoiresMinimales(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CoupsDisponibles);
cases CaseDepart(pieces Piece, couleurs Couleur, bool ExAileDame, bool ExAileRoi, bool Roque);
int TriDesPieces(const void *DataA, const void *DataB);
unsigned int DecompteDesCoupsLibres(const position *Position, couleurs Couleur);
void EliminationDesTrajectoiresTropLongues(bonhomme Pieces[MaxHommes], constatations *Deductions);
void AnalyseSommaireDesCapturesParLesPions(position *Position, couleurs Couleur);
void TrajectoiresDesPiecesCapturees(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CapturesLibres);
void AnalyseDesRoques(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CoupsLibres);
void VerificationDesDeductions(bonhomme Pieces[MaxHommes]);
void PiecesImmobiles(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CoupsLibres);
void ExamenAttentifDesPiecesDePromotion(bonhomme Pieces[MaxHommes], couleurs Couleur, constatations *Deductions);
void RejetDesPossibilitesMutuellementExclusives(bonhomme Pieces[MaxHommes], constatations *Deductions);
void DeplacementMinimum(bonhomme *Piece);
void IdentificationDeToutesLesPiecesCapturees(bonhomme Pieces[MaxHommes]);
void RecalculDesMeilleuresTrajectoires(bonhomme Pieces[MaxHommes], couleurs Couleur);

/*************************************************************/

position *ExamenDuDiagramme(const diagramme *Diagramme)
{
	position *Position = new position;
	memset(Position, 0, sizeof(position));
	Position->Diagramme = *Diagramme;

	OutputDiagramme(Diagramme);
	if (Diagramme->DemiCoups >= MaxCoups)
		ErreurEnonce(MESSAGE_TROPDECOUPS);

	OutputMessage(MESSAGE_DECOMPTE);
	DecompteDesPieces(Position);
	OutputNombreDePieces(Position->Blancs.NombreTotalDePieces, Position->Noirs.NombreTotalDePieces);

	unsigned int MaxStep = 54;
	
	unsigned int StepPions = 0;
	unsigned int StepMortes = 0;
	unsigned int StepFous = 0;
	unsigned int StepPath = 0;
	unsigned int StepRejet = 0;
	unsigned int StepCaptures = 0;
	unsigned int StepMorts = 0;
	unsigned int StepRoques = 0;
	unsigned int StepPromotions = 0;
	unsigned int StepAbsurdes = 0;
	unsigned int StepRestrictions = 0;

	for (unsigned int Step = 1; Step <= MaxStep; Step++) {
		switch (Step) {
			case 1 :
				OutputMessage(MESSAGE_ANALYSEPIONS, ++StepPions);
				ProvenanceDesPions(Position->PiecesBlanches, BLANCS);
				ProvenanceDesPions(Position->PiecesNoires, NOIRS);
				break;
			case 2 :
			case 6 :
				OutputMessage(MESSAGE_ANALYSEPIONS, ++StepPions);
				RestrictionDeLaProvenanceDesPions(Position->PiecesBlanches);
				RestrictionDeLaProvenanceDesPions(Position->PiecesNoires);
				break;
			case 3 :
				OutputMessage(MESSAGE_CASESMORTES, ++StepMortes);
				IdentificationDesCasesMortes(Position->PiecesBlanches);
				IdentificationDesCasesMortes(Position->PiecesNoires);
				break;
			case 4 :
				OutputMessage(MESSAGE_ANALYSEFOUS, ++StepFous);
				FousDePromotion(Position, BLANCS);
				FousDePromotion(Position, NOIRS);
				break;
			case 5 :
				OutputMessage(MESSAGE_ANALYSEPIONS, ++StepPions);
				ProvenanceDesPionsII(Position->PiecesBlanches, Position->Blancs.NombreDeCapturesPourLesPions, &Position->Blancs.NombreDeCapturesLibres);
				ProvenanceDesPionsII(Position->PiecesNoires, Position->Noirs.NombreDeCapturesPourLesPions, &Position->Noirs.NombreDeCapturesLibres);
				break;
			case 7 :
			case 36 :
				OutputMessage(MESSAGE_RESTRICTIONS, ++StepRestrictions);
				RestrictionDesMouvementsParLesPions(Position->PiecesBlanches, BLANCS, Position->Blancs.NombreDeCapturesLibres);
				RestrictionDesMouvementsParLesPions(Position->PiecesNoires, NOIRS, Position->Noirs.NombreDeCapturesLibres);
				break;
			case 8 :
				OutputMessage(MESSAGE_MEILLEURESTRAJECTOIRES, ++StepPath);
				CalculDesTrajectoiresMinimales(Position->PiecesBlanches, BLANCS, (Position->Diagramme.DemiCoups + 1) / 2);
				CalculDesTrajectoiresMinimales(Position->PiecesNoires, NOIRS, Position->Diagramme.DemiCoups / 2);
				break;
			case 9 :
			case 13 :
			case 16 :
			case 18 :
			case 21 :
			case 24 :
			case 27 :
			case 31 :
			case 34 :
			case 37 :
			case 40 :
			case 43 :
			case 46 :
			case 49 :
			case 51 :
			case 53 :
				OutputDeplacementsMinimaux(Position->PiecesBlanches, Position->PiecesNoires);
				break;
			case 11 :
			case 19 :
			case 25 :
			case 32 :
			case 38 :
				Position->Blancs.CoupsLibres = DecompteDesCoupsLibres(Position, BLANCS);
				Position->Noirs.CoupsLibres = DecompteDesCoupsLibres(Position, NOIRS);
				Position->Blancs.CoupsLibresReels = Position->Blancs.CoupsLibres;
				Position->Noirs.CoupsLibresReels = Position->Noirs.CoupsLibres;
				OutputCoupsLibres(Position->Blancs.CoupsLibres, Position->Noirs.CoupsLibres);
				break;				
			case 44 :
				OutputCoupsLibres(Position->Blancs.CoupsLibresReels, Position->Noirs.CoupsLibresReels);
				break;				
			case 12 :
			case 20 :
			case 26 :
			case 33 :
			case 39 :
			case 45 :
				OutputMessage(MESSAGE_REJETTRAJECTOIRES, ++StepRejet);
				EliminationDesTrajectoiresTropLongues(Position->PiecesBlanches, &Position->Blancs);
				EliminationDesTrajectoiresTropLongues(Position->PiecesNoires, &Position->Noirs);
				break;
			case 10 :
			case 14 :
			case 22 :
			case 28 :
			case 35 :
			case 41 :
			case 47 :
				VerificationDesDeductions(Position->PiecesBlanches);
				VerificationDesDeductions(Position->PiecesNoires);
				break;
			case 15 :
				OutputMessage(MESSAGE_ANALYSESOMMAIRECAPTURES, ++StepCaptures);
				AnalyseSommaireDesCapturesParLesPions(Position, BLANCS);
				AnalyseSommaireDesCapturesParLesPions(Position, NOIRS);
				break;
			case 17 :
				OutputMessage(MESSAGE_TRAJECTOIRESDESMORTS, ++StepMorts);
				TrajectoiresDesPiecesCapturees(Position->PiecesBlanches, BLANCS, Position->Blancs.NombreDeCapturesLibres);
				TrajectoiresDesPiecesCapturees(Position->PiecesNoires, NOIRS, Position->Noirs.NombreDeCapturesLibres);
				break;
			case 23 :
				OutputMessage(MESSAGE_ANALYSEROQUES, ++StepRoques);
				AnalyseDesRoques(Position->PiecesBlanches, BLANCS, Position->Blancs.CoupsLibres);
				AnalyseDesRoques(Position->PiecesNoires, NOIRS, Position->Noirs.CoupsLibres);
				break;
			case 29 :
				OutputMessage(MESSAGE_CASESMORTES, ++StepMortes);
				PiecesImmobiles(Position->PiecesBlanches, BLANCS, Position->Blancs.CoupsLibres);
				PiecesImmobiles(Position->PiecesNoires, NOIRS, Position->Noirs.CoupsLibres);
				break;
			case 30 :
				OutputMessage(MESSAGE_PROMOTIONS, ++StepPromotions);
				ExamenAttentifDesPiecesDePromotion(Position->PiecesBlanches, BLANCS, &Position->Blancs);
				ExamenAttentifDesPiecesDePromotion(Position->PiecesNoires, NOIRS, &Position->Noirs);
				break;
			case 42 :
			case 52 :
				OutputMessage(MESSAGE_REJETABSURDES, ++StepAbsurdes);
				Position->Blancs.CoupsLibresReels = DecompteDesCoupsLibres(Position, BLANCS);
				Position->Noirs.CoupsLibresReels = DecompteDesCoupsLibres(Position, NOIRS);
				RejetDesPossibilitesMutuellementExclusives(Position->PiecesBlanches, &Position->Blancs);
				RejetDesPossibilitesMutuellementExclusives(Position->PiecesNoires, &Position->Noirs);
				break;
			case 48 :
				OutputMessage(MESSAGE_ANALYSESOMMAIRECAPTURES, ++StepCaptures);
				IdentificationDeToutesLesPiecesCapturees(Position->PiecesBlanches);
				IdentificationDeToutesLesPiecesCapturees(Position->PiecesNoires);
				break;
			case 50 :
				OutputMessage(MESSAGE_MEILLEURESTRAJECTOIRES, ++StepPath);
				RecalculDesMeilleuresTrajectoires(Position->PiecesBlanches, BLANCS);
				RecalculDesMeilleuresTrajectoires(Position->PiecesNoires, NOIRS);
				break;
			default :
				break;
		}
	}

	return Position;
}

/*************************************************************/

int TriDesPieces(const void *DataA, const void *DataB)
{
	const bonhomme *PieceA = (const bonhomme *)DataA;
	const bonhomme *PieceB = (const bonhomme *)DataB;

	if (PieceA->Piece > PieceB->Piece)
		return -1;

	if (PieceA->Piece < PieceB->Piece)
		return 1;

	return 0;
}

/*************************************************************/

void DecompteDesPieces(position* Position)
{
	constatations *Pieces[MaxCouleurs];
	bonhomme *Listes[MaxCouleurs];

	Pieces[NEUTRE] = NULL;
	Pieces[BLANCS] = &Position->Blancs;
	Pieces[NOIRS] = &Position->Noirs;
	Listes[NEUTRE] = NULL;
	Listes[BLANCS] = Position->PiecesBlanches;
	Listes[NOIRS] = Position->PiecesNoires;

	const diagramme *Diagramme = &Position->Diagramme;

	for (cases Case = A1; Case < MaxCases; Case++) {
		couleurs Couleur = Diagramme->Couleurs[Case];
		pieces Piece = Diagramme->Pieces[Case];

		if (Couleur != NEUTRE) {
			Pieces[Couleur]->NombreTotalDePieces++;
			Pieces[Couleur]->NombreDePieces[Piece]++;
			
			Listes[Couleur]->CaseActuelle = Case;
			Listes[Couleur]->CaseActuelleMin = Case;
			Listes[Couleur]->CaseActuelleMax = Case;
			Listes[Couleur]->Piece = Piece;
			Listes[Couleur]++;

			Verifier(Piece != VIDE);
		}
	}

	qsort(Position->PiecesBlanches, MaxHommes, sizeof(bonhomme), TriDesPieces);
	qsort(Position->PiecesNoires, MaxHommes, sizeof(bonhomme), TriDesPieces);

	constatations *Blancs = Pieces[BLANCS];
	constatations *Noirs = Pieces[NOIRS];

	if ((Blancs->NombreTotalDePieces > MaxHommes) || (Noirs->NombreTotalDePieces > MaxHommes))
		ErreurEnonce(MESSAGE_TROPDEPIECES);

	Blancs->NombreDeCaptures = MaxHommes - Noirs->NombreTotalDePieces;
	Blancs->NombreDeCapturesPourLesPions = Blancs->NombreDeCaptures;
	Blancs->NombreDeCapturesLibres = Blancs->NombreDeCaptures;
	Noirs->NombreDeCaptures = MaxHommes - Blancs->NombreTotalDePieces;
	Noirs->NombreDeCapturesPourLesPions = Noirs->NombreDeCaptures;
	Noirs->NombreDeCapturesLibres = Noirs->NombreDeCaptures;

	if ((Blancs->NombreDePieces[PION] > NombreMaximum[PION]) || (Noirs->NombreDePieces[PION] > NombreMaximum[PION]))
		ErreurEnonce(MESSAGE_TROPDEPIONS);

	Blancs->NombreMaximumDePromotions = NombreMaximum[PION] - Blancs->NombreDePieces[PION];
	Noirs->NombreMaximumDePromotions = NombreMaximum[PION] - Noirs->NombreDePieces[PION];

	if (Blancs->NombreMaximumDePromotions == 0)
		for (unsigned int i = 0; i < MaxHommes; i++)
			Position->PiecesBlanches[i].PromotionImpossible = true;

	if (Noirs->NombreMaximumDePromotions == 0)
		for (unsigned int i = 0; i < MaxHommes; i++)
			Position->PiecesNoires[i].PromotionImpossible = true;

	if ((Blancs->NombreDePieces[ROI] != 1) || (Noirs->NombreDePieces[ROI] != 1))
		ErreurEnonce(MESSAGE_TROPDEROIS);

	for (pieces Piece = CAVALIER; Piece <= DAME; Piece = Piece + 1) {
		unsigned int Max = 1;
		if ((Piece == CAVALIER) || (Piece == TOUR))
			Max = 2;

		if (Blancs->NombreDePieces[Piece] > Max)
			Blancs->NombreMinimumDePromotions += (Blancs->NombreDePieces[Piece] - Max);

		if (Noirs->NombreDePieces[Piece] > Max)
			Noirs->NombreMinimumDePromotions += (Noirs->NombreDePieces[Piece] - Max);		
	}

	if ((Blancs->NombreMinimumDePromotions > Blancs->NombreMaximumDePromotions) || (Noirs->NombreMinimumDePromotions > Noirs->NombreMaximumDePromotions))
		ErreurEnonce(MESSAGE_TROPDEPIECESDEPROMOTION);

	VerificationDesEchecs(Position);
}

/*************************************************************/

void VerificationDesEchecs(const position *Position)
{
	const diagramme *Diagramme = &Position->Diagramme;

	bool Echecs[MaxCouleurs];

	for (couleurs Couleur = BLANCS; Couleur <= NOIRS; Couleur++) {
		Echecs[Couleur] = false;

		cases CaseRoi = (Couleur == BLANCS) ? Position->PiecesBlanches[0].CaseActuelle : Position->PiecesNoires[0].CaseActuelle;

		colonnes ColonneRoi = QuelleColonne(CaseRoi);
		rangees RangeeRoi = QuelleRangee(CaseRoi);

		for (int DeltaX = -2; DeltaX <= 2; DeltaX++) {
			for (int DeltaY = -2; DeltaY <= 2; DeltaY++) {
				if ((ColonneRoi + DeltaX < A) || (ColonneRoi + DeltaX > H))
					continue;

				if ((RangeeRoi + DeltaY < UN) || (RangeeRoi + DeltaY > HUIT))
					continue;

				cases Case = QuelleCase(ColonneRoi + DeltaX, RangeeRoi + DeltaY);
				if (Diagramme->Couleurs[Case] == Couleur)
					continue;

				if (abs(DeltaX * DeltaY) == 2)
					if (Diagramme->Pieces[Case] == CAVALIER)
						Echecs[Couleur] = true;

				if ((abs(DeltaX) <= 1) && (abs(DeltaY) <= 1))
					if (Diagramme->Pieces[Case] == ROI)
						Echecs[Couleur] = true;

				if ((abs(DeltaX) == 1) && (DeltaY == ((Couleur == BLANCS) ? 1 : -1)))
					if (Diagramme->Pieces[Case] == PION)
						Echecs[Couleur] = true;
			}
		}

		for (int DeltaX = -1; DeltaX <= 1; DeltaX++) {
			for (int DeltaY = -1; DeltaY <= 1; DeltaY++) {
				if (!DeltaX && !DeltaY)
					continue;

				pieces Piece = (DeltaX && DeltaY) ? QuelFou(CaseRoi) : TOUR;

				for (int Distance = 1; Distance <= 7; Distance++) {
					if ((ColonneRoi + DeltaX * Distance < A) || (ColonneRoi + DeltaX * Distance > H))
						break;

					if ((RangeeRoi + DeltaY * Distance < UN) || (RangeeRoi + DeltaY * Distance > HUIT))
						break;

					cases Case = QuelleCase(ColonneRoi + DeltaX * Distance, RangeeRoi + DeltaY * Distance);
					if (Diagramme->Couleurs[Case] == Couleur)
						break;

					if ((Diagramme->Pieces[Case] == Piece) || (Diagramme->Pieces[Case] == DAME))
						Echecs[Couleur] = true;

					if (Diagramme->Couleurs[Case] != VIDE)
						break;
				}
			}
		}
	}

	if (Echecs[BLANCS] && Echecs[NOIRS])
		ErreurEnonce(MESSAGE_AUCUNESOLUTION);

	if (Echecs[BLANCS] && ((Diagramme->DemiCoups % 2) == 1))
		ErreurEnonce(MESSAGE_AUCUNESOLUTION);

	if (Echecs[NOIRS] && ((Diagramme->DemiCoups % 2) == 0))
		ErreurEnonce(MESSAGE_AUCUNESOLUTION);
}

/*************************************************************/

void ProvenanceDesPions(bonhomme Pieces[MaxHommes], couleurs Couleur)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != PION)
			continue;

		unsigned int Colonne = QuelleColonne(Pieces[i].CaseActuelle);
		unsigned int Rangee = QuelleRangee(Pieces[i].CaseActuelle);
		unsigned int Pas = (Couleur == BLANCS) ? (Rangee - DEUX) : (SEPT - Rangee);

		Pieces[i].ExAileDame = false;
		Pieces[i].ExAileRoi = false;
		Pieces[i].ExGrandRoque = false;
		Pieces[i].ExPetitRoque = false;
		Pieces[i].PasDePion = Pas;
		Pieces[i].CapturesSiPion = 0;
		Pieces[i].PromotionImpossible = true;

		if (!Pas) {
			Pieces[i].ExPion[Colonne] = true;
		}
		else {
			for (colonnes De = A; De < MaxColonnes; De++)
				if((unsigned int)abs((int)De - (int)Colonne) <= Pas)
					Pieces[i].ExPion[De] = true;
		}
	}
}

/*************************************************************/

void RestrictionDeLaProvenanceDesPions(bonhomme Pieces[MaxHommes])
{
	bool Unique[MaxColonnes];
	cases QuelPion[MaxColonnes];
	
	for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++)
		Unique[Colonne] = false;

	bool Encore = true;
	while (Encore) {
		Encore = false;

		for (unsigned int i = 0; i < MaxHommes; i++) {
			if (Pieces[i].Piece != PION)
				continue;

			cases DernierPion = A1;
			colonnes DerniereColonne = A;
			unsigned int Possibilites = 0;
			Pieces[i].CapturesSiPion = UINT_MAX;

			for (colonnes Colonne = A; Colonne <= H; Colonne++) {
				if (Pieces[i].ExPion[Colonne]) {
					if (Unique[Colonne] && (QuelPion[Colonne] != Pieces[i].CaseActuelle)) {
						Pieces[i].ExPion[Colonne] = false;
					}
					else {
						DerniereColonne = Colonne;
						DernierPion = Pieces[i].CaseActuelle;
						Possibilites++;

						unsigned int CapturesRequises = abs((int)Colonne - (int)QuelleColonne(Pieces[i].CaseActuelle));
						if (CapturesRequises < Pieces[i].CapturesSiPion)
							Pieces[i].CapturesSiPion = CapturesRequises;
					}
				}
			}

			if (Possibilites == 0)
				ErreurEnonce(MESSAGE_STRUCTUREDEPIONSILLEGALE);

			if (Possibilites == 1) {
				if (!Unique[DerniereColonne]) {
					QuelPion[DerniereColonne] = DernierPion;
					Unique[DerniereColonne] = true;
					Encore = true;
				}
			}
		}
	}

	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece == PION)
			continue;

		if (Pieces[i].Piece == ROI) {
			for (colonnes k = A; k < MaxColonnes; k++)
				Pieces[i].ExPion[k] = false;
		}
		else {
			for (colonnes k = A; k < MaxColonnes; k++)
				Pieces[i].ExPion[k] = !Unique[k];
		}
	}
}

/*************************************************************/

void FousDePromotion(position *Position, couleurs Couleur)
{
	rangees Rangee = (Couleur == BLANCS) ? UN : HUIT;
	rangees RangeePion = (Couleur == BLANCS) ? DEUX : SEPT;

	for (colonnes Colonne = C; Colonne < H; Colonne += 3) {
		cases CaseGauche = QuelleCase(Colonne - 1, RangeePion);
		cases CaseDroite = QuelleCase(Colonne + 1, RangeePion);

		if (Position->Diagramme.Pieces[CaseGauche] != PION)
			continue;
		if (Position->Diagramme.Pieces[CaseDroite] != PION)
			continue;
		if (Position->Diagramme.Couleurs[CaseGauche] != Couleur)
			continue;
		if (Position->Diagramme.Couleurs[CaseDroite] != Couleur)
			continue;

		cases Case = QuelleCase(Colonne, Rangee);

		if ((Position->Diagramme.Pieces[Case] == QuelFou(Case)) && (Position->Diagramme.Couleurs[Case] == Couleur)) {
			AjouteUneCaseMorte(Case);
			continue;
		}

		bool PromotionSupplementaire = false;
		bonhomme *Bonhomme = (Couleur == BLANCS) ? Position->PiecesBlanches : Position->PiecesNoires;
		
		for (unsigned int i = 0; i < MaxHommes; i++, Bonhomme++) {
			if (Bonhomme->Piece == VIDE)
				break;

			if (Bonhomme->Piece == QuelFou(Case)) {
				Bonhomme->PieceDePromotion = true;
				PromotionSupplementaire = true;
			}
		}

		if (PromotionSupplementaire) {
			if (Couleur == BLANCS) {
				if (++Position->Blancs.NombreMinimumDePromotions > Position->Blancs.NombreMaximumDePromotions)
					ErreurEnonce(MESSAGE_TROPDEPIECESDEPROMOTION);
			}
			else {
				if (++Position->Noirs.NombreMinimumDePromotions > Position->Noirs.NombreMaximumDePromotions)
					ErreurEnonce(MESSAGE_TROPDEPIECESDEPROMOTION);
			}
		}

		Bonhomme->CaseActuelle = Case;
		Bonhomme->CaseActuelleMin = Case;
		Bonhomme->CaseActuelleMax = Case;
		Bonhomme->PieceDisparue = QuelFou(Case);
		Bonhomme->ExPiece[Bonhomme->PieceDisparue] = true;
		Bonhomme->PromotionImpossible = true;

		if (Couleur == BLANCS) {
			if (Position->Noirs.NombreDeCapturesPourLesPions-- == 0)
				ErreurEnonce(MESSAGE_TROPDEPIECES);

			Position->Noirs.NombreDeCapturesLibres--;
		}
		else {
			if (Position->Blancs.NombreDeCapturesPourLesPions-- == 0)
				ErreurEnonce(MESSAGE_TROPDEPIECES);

			Position->Blancs.NombreDeCapturesLibres--;
		}
	}
}

/*************************************************************/

void ProvenanceDesPionsII(bonhomme Pieces[MaxHommes], unsigned int CapturesDisponibles, unsigned int *CapturesLibres)
{
	unsigned int k;

	unsigned int NombreDePions = 0;
	bonhomme *Pions[MaxColonnes];

	for (unsigned int i = 0; i < MaxHommes; i++)
		if (Pieces[i].Piece == PION)
			Pions[NombreDePions++] = &Pieces[i];

	*CapturesLibres = CapturesDisponibles;
	if (!NombreDePions)
		return;

	for (k = 0; k < NombreDePions; k++)
		for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++)
			if ((unsigned int)abs((int)Colonne - (int)QuelleColonne(Pions[k]->CaseActuelle)) > CapturesDisponibles)
				Pions[k]->ExPion[Colonne] = false;

	colonnes Start[MaxColonnes];
	colonnes Stop[MaxColonnes];
	for (k = 0; k < NombreDePions; k++) {
		Start[k] = A;
		Stop[k] = H;

		while (!Pions[k]->ExPion[Start[k]]) {
			if (++Start[k] >= MaxColonnes)
				break;
		}

		while (!Pions[k]->ExPion[Stop[k]]) {
			if (Stop[k] == A)
				break;

			Stop[k]--;
		}

		if (Start[k] > Stop[k])
			ErreurEnonce(MESSAGE_STRUCTUREDEPIONSILLEGALE);
	}

	colonnes Compteurs[MaxColonnes];
	for (k = 0; k < NombreDePions; k++)
		Compteurs[k] = Start[k];

	bool Possibilites[MaxColonnes][MaxColonnes];
	memset(Possibilites, 0, sizeof(Possibilites));
	unsigned int MinCaptures = UINT_MAX;

	while (Compteurs[0] <= Stop[0]) {
		bool ColonnesPrises[MaxColonnes];
		memset(ColonnesPrises, 0, sizeof(ColonnesPrises));

		unsigned int CapturesRequises = 0;
		bool Possible = true;

		for (k = 0; k < NombreDePions; k++) {
			if (ColonnesPrises[Compteurs[k]]) {
				Possible = false;
			}
			else {
				ColonnesPrises[Compteurs[k]] = true;
				CapturesRequises += abs((int)QuelleColonne(Pions[k]->CaseActuelle) - (int)Compteurs[k]);
			}
		}

		if (CapturesRequises > CapturesDisponibles)
			Possible = false;

		if (Possible) {
			for (k = 0; k < NombreDePions; k++)
				Possibilites[k][Compteurs[k]] = true;
			
			if (CapturesRequises < MinCaptures)
				MinCaptures = CapturesRequises;
		}

		unsigned int X = NombreDePions - 1;
		do {
			Compteurs[X]++;
			while ((Compteurs[X] <= Stop[X]) && !Pions[X]->ExPion[Compteurs[X]])
				Compteurs[X]++;

			if (Compteurs[X] <= Stop[X])
				break;

			if (X)
				Compteurs[X] = Start[X];
		} while (X-- >= 1);
	}

	if (MinCaptures == UINT_MAX)
		ErreurEnonce(MESSAGE_STRUCTUREDEPIONSILLEGALE);

	*CapturesLibres -= MinCaptures;

	for (k = 0; k < NombreDePions; k++)
		for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++)
			Pions[k]->ExPion[Colonne] = Possibilites[k][Colonne];
}

/*************************************************************/

void CalculDesTrajectoiresMinimales(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CoupsDisponibles)
{
	cases De;

	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece == VIDE)
			continue;

		if (Pieces[i].Piece == PION) {
			Pieces[i].DeplacementsSiPion = UINT_MAX;

			for (De = (Couleur == BLANCS) ? A2 : A7; De < MaxCases; De += 8) {
				if (Pieces[i].ExPion[QuelleColonne(De)]) {
					unsigned int Meilleur = MeilleureTrajectoire(Pieces[i].Piece, De, Pieces[i].CaseActuelle, Couleur);
					if (Meilleur < Pieces[i].DeplacementsSiPion)
						Pieces[i].DeplacementsSiPion = Meilleur;
				}
			}

			Pieces[i].Deplacements = Pieces[i].DeplacementsSiPion;
			continue;
		}

		Pieces[i].Deplacements = UINT_MAX;

		De = CaseDepart(Pieces[i].Piece, Couleur, true, false, false);
		if ((De >= MaxCases) || Pieces[i].PieceDePromotion) {
			Pieces[i].ExAileDame = false;
			Pieces[i].DeplacementsSiAileDame = 0;
		}
		else {
			Pieces[i].ExAileDame = true;
			Pieces[i].DeplacementsSiAileDame = MeilleureTrajectoire(Pieces[i].Piece, De, Pieces[i].CaseActuelle);
			if (Pieces[i].DeplacementsSiAileDame > CoupsDisponibles)
				Pieces[i].ExAileDame = false;
			else if (Pieces[i].DeplacementsSiAileDame < Pieces[i].Deplacements)
				Pieces[i].Deplacements = Pieces[i].DeplacementsSiAileDame;
		}

		De = CaseDepart(Pieces[i].Piece, Couleur, false, true, false);
		if ((De >= MaxCases) || Pieces[i].PieceDePromotion) {
			Pieces[i].ExAileRoi = false;
			Pieces[i].DeplacementsSiAileRoi = 0;
		}
		else {
			Pieces[i].ExAileRoi = true;
			Pieces[i].DeplacementsSiAileRoi = MeilleureTrajectoire(Pieces[i].Piece, De, Pieces[i].CaseActuelle);
			if (Pieces[i].DeplacementsSiAileRoi > CoupsDisponibles)
				Pieces[i].ExAileRoi = false;
			else if (Pieces[i].DeplacementsSiAileRoi < Pieces[i].Deplacements)
				Pieces[i].Deplacements = Pieces[i].DeplacementsSiAileRoi;
		}

		De = CaseDepart(Pieces[i].Piece, Couleur, true, false, true);
		if (De < MaxCases) {
			Pieces[i].DeplacementsSiGrandRoque = MeilleureTrajectoire(Pieces[i].Piece, De, Pieces[i].CaseActuelle);
			
			if (Pieces[i].DeplacementsSiGrandRoque < UINT_MAX) {
				Pieces[i].ExGrandRoque = true;

				if (Pieces[i].Piece == ROI)
					Pieces[i].DeplacementsSiGrandRoque++;

				if (Pieces[i].DeplacementsSiGrandRoque < Pieces[i].Deplacements)
					Pieces[i].Deplacements = Pieces[i].DeplacementsSiGrandRoque;
			}
			else {
				Pieces[i].ExGrandRoque = false;
				Pieces[i].DeplacementsSiGrandRoque = 0;
			}				
		}
		else {
			Pieces[i].ExGrandRoque = false;
			Pieces[i].DeplacementsSiGrandRoque = 0;
		}

		De = CaseDepart(Pieces[i].Piece, Couleur, false, true, true);
		if (De < MaxCases) {
			Pieces[i].DeplacementsSiPetitRoque = MeilleureTrajectoire(Pieces[i].Piece, De, Pieces[i].CaseActuelle);
			
			if (Pieces[i].DeplacementsSiPetitRoque < UINT_MAX) {
				Pieces[i].ExPetitRoque = true;
	
				if (Pieces[i].Piece == ROI)
					Pieces[i].DeplacementsSiPetitRoque++;

				if (Pieces[i].DeplacementsSiPetitRoque < Pieces[i].Deplacements)
					Pieces[i].Deplacements = Pieces[i].DeplacementsSiPetitRoque;
			}
			else {
				Pieces[i].ExPetitRoque = false;
				Pieces[i].DeplacementsSiPetitRoque = 0;
			}
		}
		else {
			Pieces[i].ExPetitRoque = false;
			Pieces[i].DeplacementsSiPetitRoque = 0;
		}
		
		Pieces[i].PasDePion = 6;
		rangees RangeePromotion = (Couleur == BLANCS) ? HUIT : UN;
		if (QuelleRangee(Pieces[i].CaseActuelle) == RangeePromotion) {
			Pieces[i].DeplacementsSiPion = 5;
		}
		else {
			unsigned int MinimumSiPromotion = UINT_MAX - 5;
			for (colonnes Colonne = A; Colonne <= H; Colonne++) {
				De = QuelleCase(Colonne, RangeePromotion);
				unsigned int Coups = MeilleureTrajectoire(Pieces[i].Piece, De, Pieces[i].CaseActuelle);
				
				if (Coups < MinimumSiPromotion)
					MinimumSiPromotion = Coups;
				
				if (Coups <= 1)
					break;
			}

			Pieces[i].DeplacementsSiPion = 5 + MinimumSiPromotion;
			if ((MinimumSiPromotion > CoupsDisponibles) || (Pieces[i].Piece == ROI))
				Pieces[i].PromotionImpossible = true;

		}

		if (!Pieces[i].PromotionImpossible && (Pieces[i].DeplacementsSiPion < Pieces[i].Deplacements))
			Pieces[i].Deplacements = Pieces[i].DeplacementsSiPion;

		if (Pieces[i].Deplacements == UINT_MAX)
			Pieces[i].Deplacements = 0;
	}
}

/*************************************************************/

cases CaseDepart(pieces Piece, couleurs Couleur, bool ExAileDame, bool ExAileRoi, bool Roque)
{
	Verifier(ExAileDame ^ ExAileRoi);
	Verifier(Piece != PION);

	rangees Rangee = (Couleur == BLANCS) ? UN : HUIT;
	colonnes Colonne = MaxColonnes;

	switch (Piece) {
		case ROI :
			if (!Roque) {
				if (ExAileRoi)
					Colonne = E;
			}
			else  {
				Colonne = ExAileDame ? C : G;
			}
			break;
		case DAME :
			if (ExAileDame && !Roque)
				Colonne = D;
			break;
		case TOUR :
			if (ExAileDame) {
				if (Roque)
					Colonne = D;
				else
					Colonne = A;
			}
			else {
				if (Roque)
					Colonne = F;
				else
					Colonne = H;
			}
			break;
		case CAVALIER :
			if (!Roque)
				Colonne = ExAileDame ? B : G;
			break;
		case FOUNOIR :
			if (!Roque && ExAileDame && (Couleur == BLANCS))
				Colonne = C;
			if (!Roque && ExAileRoi && (Couleur == NOIRS))
				Colonne = F;
			break;
		case FOUBLANC :
			if (!Roque && ExAileDame && (Couleur == NOIRS))
				Colonne = C;
			if (!Roque && ExAileRoi && (Couleur == BLANCS))
				Colonne = F;
			break;
		default :
			break;
	}
			
	if (Colonne == MaxColonnes)
		return MaxCases;
		
	return QuelleCase(Colonne, Rangee);		
}

/*************************************************************/

void RestrictionDesMouvementsParLesPions(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CapturesLibres)
{
	rangees RangeeDepart = (Couleur == BLANCS) ? DEUX : SEPT;
			
	for (unsigned int k = 0; k < MaxHommes; k++) {
		if (Pieces[k].Piece != PION)
			continue;

		colonnes ColonneActuelle = QuelleColonne(Pieces[k].CaseActuelle);
		rangees RangeeActuelle = QuelleRangee(Pieces[k].CaseActuelle);

		if (RangeeActuelle == RangeeDepart)
			continue;
		
		colonnes ColonneDepart = MaxColonnes;
		unsigned int Possibilites = 0;
		for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
			if (Pieces[k].ExPion[Colonne]) {
				ColonneDepart = Colonne;
				Possibilites++;
			}
		}

		Verifier(Possibilites);

		if (Possibilites > 1)
			continue;

		if (ColonneDepart == ColonneActuelle) {
			if ((CapturesLibres < 2) || (abs((int)RangeeActuelle - (int)RangeeDepart) < 2))
				RestrictionDesMouvementsVerticauxTraversant(RangeeDepart, RangeeActuelle, ColonneActuelle);
		}
		else if (abs((int)ColonneActuelle - (int)ColonneDepart) == abs((int)RangeeActuelle - (int)RangeeDepart))
			RestrictionDesMouvementsDiagonauxTraversant(RangeeDepart, RangeeActuelle, ColonneDepart, ColonneActuelle);
	}
}

/*************************************************************/

unsigned int DecompteDesCoupsLibres(const position *Position, couleurs Couleur)
{
	unsigned int CoupsDisponibles = (Couleur == BLANCS) ? (Position->Diagramme.DemiCoups + 1) / 2 : Position->Diagramme.DemiCoups / 2;
	const bonhomme *Pieces = (Couleur == BLANCS) ? Position->PiecesBlanches : Position->PiecesNoires;

	unsigned int CoupsUtilises = 0;

	for (unsigned int i = 0; i < MaxHommes; Pieces++, i++)
		CoupsUtilises += Pieces->Deplacements;

	if (CoupsUtilises > CoupsDisponibles)
		ErreurEnonce(MESSAGE_PASASSEZDECOUPS);		

	return CoupsDisponibles - CoupsUtilises;
}

/*************************************************************/

void EliminationDesTrajectoiresTropLongues(bonhomme Pieces[MaxHommes], constatations *Deductions)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece == VIDE) {
			if (Pieces[i].PieceDisparue == VIDE)
				continue;

			for (pieces Piece = PION; Piece < ROI; Piece++) {
				if (Pieces[i].ExPiece[Piece]) {
					unsigned int Extra = Pieces[i].DeplacementsSiExPiece[Piece] - Pieces[i].Deplacements;
					if (Extra > Deductions->CoupsLibres)
						Pieces[i].ExPiece[Piece] = false;
				}
			}
		}
		else {
			if (Pieces[i].Piece == PION)
				continue;

			if (Pieces[i].ExAileDame) {
				unsigned int Extra = Pieces[i].DeplacementsSiAileDame - Pieces[i].Deplacements;
				if (Extra > Deductions->CoupsLibres)
					Pieces[i].ExAileDame = false;
			}

			if (Pieces[i].ExAileRoi) {
				unsigned int Extra = Pieces[i].DeplacementsSiAileRoi - Pieces[i].Deplacements;
				if (Extra > Deductions->CoupsLibres)
					Pieces[i].ExAileRoi = false;
			}

			if (Pieces[i].ExGrandRoque) {
				unsigned int Extra = Pieces[i].DeplacementsSiGrandRoque - Pieces[i].Deplacements;
				if (Extra > Deductions->CoupsLibres)
					Pieces[i].ExGrandRoque = false;
			}
			
			if (Pieces[i].ExPetitRoque) {
				unsigned int Extra = Pieces[i].DeplacementsSiPetitRoque - Pieces[i].Deplacements;
				if (Extra > Deductions->CoupsLibres)
					Pieces[i].ExPetitRoque = false;
			}

			if (!Pieces[i].PromotionImpossible) {
				unsigned int Extra = Pieces[i].DeplacementsSiPion - Pieces[i].Deplacements;
				if (Extra > Deductions->CoupsLibres)
					Pieces[i].PromotionImpossible = true;
			}
		}
	}
}

/*************************************************************/

void VerificationDesDeductions(bonhomme Pieces[MaxHommes])
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE) {
			if (!Pieces[i].ExAileDame && !Pieces[i].ExAileRoi && !Pieces[i].ExGrandRoque && !Pieces[i].ExPetitRoque) {
				if (Pieces[i].Piece == PION) {
					bool ExPion = false;
					for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++)
						ExPion |= Pieces[i].ExPion[Colonne];

					if (!ExPion)
						ErreurEnonce(MESSAGE_STRUCTUREDEPIONSILLEGALE);
				}
				else if (Pieces[i].PromotionImpossible) {
					ErreurEnonce(MESSAGE_AUCUNEPOSSIBILITE);
				}				
			}
		}
		else if (Pieces[i].PieceDisparue != VIDE) {
			if (Pieces[i].PromotionImpossible && !Pieces[i].ExPiece[TOUR] && !Pieces[i].ExPiece[CAVALIER] && !Pieces[i].ExPiece[DAME] && !Pieces[i].ExPiece[FOUNOIR] && !Pieces[i].ExPiece[FOUBLANC])
				ErreurEnonce(MESSAGE_AUCUNECAPTUREPOSSIBLE);
		}
	}
}

/*************************************************************/

void AnalyseSommaireDesCapturesParLesPions(position *Position, couleurs Couleur)
{
	bonhomme *Mort = (Couleur == BLANCS) ? Position->PiecesBlanches : Position->PiecesNoires;
	bonhomme *Vivant = (Couleur == BLANCS) ? Position->PiecesNoires : Position->PiecesBlanches;

	bonhomme *MaxMort = Mort + MaxHommes;
	bonhomme *MaxVivant = Vivant + MaxHommes;

	for ( ; Vivant < MaxVivant; Vivant++) {
		if (Vivant->Piece != PION)
			continue;

		if (Vivant->CapturesSiPion == 0)
			continue;

		bool Gauche = false;
		bool Droite = false;
		colonnes ColonneActuelle = QuelleColonne(Vivant->CaseActuelle);

		for (colonnes ColonneGauche = A; ColonneGauche < ColonneActuelle; ColonneGauche++)
			if (Vivant->ExPion[ColonneGauche])
				Gauche = true;

		for (colonnes ColonneDroite = ColonneActuelle + 1; ColonneDroite < MaxColonnes; ColonneDroite++)
			if (Vivant->ExPion[ColonneDroite])
				Droite = true;

		Verifier(Mort < MaxMort);
		Verifier(Gauche || Droite);
		Verifier(Vivant->DeplacementsSiPion >= Vivant->CapturesSiPion);

		for (unsigned int k = 0; k < Vivant->CapturesSiPion; k++) {
			if (k && Gauche && Droite)
				break;

			cases CaseCapture = Vivant->CaseActuelle;
			if ((CaseCapture < MaxCases) && (k > 0)) {
				if ((Couleur == BLANCS) && Gauche)
					CaseCapture += (-7 * k);
				if ((Couleur == BLANCS) && Droite)
					CaseCapture += (9 * k);
				if ((Couleur == NOIRS) && Gauche)
					CaseCapture += (-9 * k);
				if ((Couleur == NOIRS) && Droite)
					CaseCapture += (7 * k);
			}


			for ( ; Mort < MaxMort; Mort++) {
				if (Mort->Piece != VIDE)
					continue;

				if (Mort->PieceDisparue == VIDE)
					break;
			}

			Verifier(Mort < MaxMort);

			Mort->PieceDisparue = MaxPieces;
			for (pieces Piece = PION; Piece < ROI; Piece++)
				Mort->ExPiece[Piece] = true;

			if (Vivant->CapturesSiPion == Vivant->PasDePion) {
				Mort->CaseActuelle = CaseCapture;
				Mort->CaseActuelleMin = Mort->CaseActuelle;
				Mort->CaseActuelleMax = Mort->CaseActuelle;
			}
			else {
				Mort->CaseActuelle = MaxCases;
				int DeltaRangee = (Couleur == NOIRS) ? -(int)k : k;
				int DeltaColonne = Gauche ? -(int)k : k;

				if (Couleur == NOIRS) {
					Mort->CaseActuelleMax = QuelleCase(QuelleColonne(Vivant->CaseActuelle) + DeltaColonne, QuelleRangee(Vivant->CaseActuelle) + DeltaRangee);
					Mort->CaseActuelleMin = Mort->CaseActuelleMax - (Vivant->PasDePion - Vivant->CapturesSiPion);
				}
				else {
					Mort->CaseActuelleMin = QuelleCase(QuelleColonne(Vivant->CaseActuelle) + DeltaColonne, QuelleRangee(Vivant->CaseActuelle) + DeltaRangee);
					Mort->CaseActuelleMax = Mort->CaseActuelleMin + (Vivant->PasDePion - Vivant->CapturesSiPion);
				}
			}
		}
	}
}

/*************************************************************/

void TrajectoiresDesPiecesCapturees(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CapturesLibres)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE)
			continue;

		if (Pieces[i].PieceDisparue == VIDE)
			continue;

		for (pieces ExPiece = PION; ExPiece < ROI; ExPiece++) {
			if (!Pieces[i].ExPiece[ExPiece])
				continue;

			unsigned int Nombre = 0;

			for (unsigned int k = 0; k < MaxHommes; k++) {
				if (Pieces[k].Piece == ExPiece)
					if (Pieces[k].PromotionImpossible)
						Nombre++;

				if (Pieces[k].PieceDePromotion && (ExPiece == PION))
					Nombre++;
			}
				
			Verifier(Nombre <= NombreMaximum[ExPiece]);

			if (Nombre >= NombreMaximum[ExPiece]) {
				Pieces[i].ExPiece[ExPiece] = false;
				continue;
			}

			if (ExPiece != PION) {
				unsigned int Deplacements = UINT_MAX;
				cases De;

				for (cases Vers = Pieces[i].CaseActuelleMin; Vers <= Pieces[i].CaseActuelleMax; Vers++) {
					De = CaseDepart(ExPiece, Couleur, true, false, false);
					if (De < MaxCases) {
						unsigned int NouveauDeplacements = MeilleureTrajectoire(ExPiece, De, Vers);
						if (NouveauDeplacements < Deplacements)
							Deplacements = NouveauDeplacements;
					}

					De = CaseDepart(ExPiece, Couleur, false, true, false);
					if (De < MaxCases) {
						unsigned int NouveauDeplacements = MeilleureTrajectoire(ExPiece, De, Vers);
						if (NouveauDeplacements < Deplacements)
							Deplacements = NouveauDeplacements;
					}

					De = CaseDepart(ExPiece, Couleur, true, false, true);
					if (De < MaxCases) {
						unsigned int NouveauDeplacements = MeilleureTrajectoire(ExPiece, De, Vers);
						if (NouveauDeplacements < Deplacements)
							Deplacements = NouveauDeplacements;
					}

					De = CaseDepart(ExPiece, Couleur, false, true, true);
					if (De < MaxCases) {
						unsigned int NouveauDeplacements = MeilleureTrajectoire(ExPiece, De, Vers);
						if (NouveauDeplacements < Deplacements)
							Deplacements = NouveauDeplacements;
					}
				}

				if (Deplacements == UINT_MAX)
					Pieces[i].ExPiece[ExPiece] = false;
				else 
					Pieces[i].DeplacementsSiExPiece[ExPiece] = Deplacements;
			}
			else {
				colonnes ColonneCapture = QuelleColonne(Pieces[i].CaseActuelleMin);
				colonnes ColonneGauche = ColonneCapture - CapturesLibres;
				colonnes ColonneDroite = ColonneCapture + CapturesLibres;

				if (ColonneCapture < CapturesLibres)
					ColonneGauche = A;
				if (ColonneDroite >= MaxColonnes)
					ColonneDroite = H;

				unsigned int Deplacements = UINT_MAX;

				for (colonnes Colonne = ColonneGauche; Colonne <= ColonneDroite; Colonne++) {
					if (!Pieces[i].ExPion[Colonne])
						continue;

					cases Depart = QuelleCase(Colonne, (Couleur == BLANCS) ? DEUX : SEPT);

					for (cases Case = Pieces[i].CaseActuelleMin; Case <= Pieces[i].CaseActuelleMax; Case++) {
						unsigned int Meilleur = MeilleureTrajectoire(PION, Depart, Case, Couleur);
						if (Meilleur < Deplacements)
							Deplacements = Meilleur;
					}
				}

				Pieces[i].DeplacementsSiExPiece[PION] = 6;

				if (Deplacements < Pieces[i].DeplacementsSiExPiece[PION])
					Pieces[i].DeplacementsSiExPiece[PION] = Deplacements;
				/*else
					Pieces[i].PieceDePromotion = true;*/
			}
		}

		pieces SeulePiece = VIDE;
		unsigned int PiecesPossible = 0;
		Pieces[i].Deplacements = UINT_MAX;
		for (pieces Piece = PION; Piece < ROI; Piece++) {
			if (Pieces[i].ExPiece[Piece]) { 
				PiecesPossible++;
				SeulePiece = Piece;

				if (Pieces[i].DeplacementsSiExPiece[Piece] < Pieces[i].Deplacements)
					Pieces[i].Deplacements = Pieces[i].DeplacementsSiExPiece[Piece];
			}
		}

		Verifier(PiecesPossible > 0);

		if (PiecesPossible == 1) {
			Pieces[i].PieceDisparue = SeulePiece;
			if (SeulePiece != PION)
				Pieces[i].PromotionImpossible = true;
		}
	}
}	

/*************************************************************/

void AnalyseDesRoques(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CoupsLibres)
{
	bonhomme *Roi = &Pieces[0];
	Verifier(Roi->Piece == ROI);

	rangees Depart = (Couleur == BLANCS) ? UN : HUIT;

	unsigned int ExtraGrandRoque = 0;
	unsigned int ExtraPetitRoque = 0;

	unsigned int CompteurTours = 0;
	unsigned int CompteurGrandRoque = 0;
	unsigned int CompteurPetitRoque = 0;
	unsigned int ToursExtraGrandRoque = UINT_MAX;
	unsigned int ToursExtraPetitRoque = UINT_MAX;

	for (unsigned int k = 0; k < MaxHommes; k++) {
		if (Pieces[k].Piece == TOUR) {
			if (Pieces[k].PromotionImpossible)
				CompteurTours++;
				
			if (Pieces[k].ExGrandRoque) {
				CompteurGrandRoque++;
				unsigned int Extra = Pieces[k].DeplacementsSiGrandRoque - Pieces[k].Deplacements;
				if (Extra < ToursExtraGrandRoque)
					ToursExtraGrandRoque = Extra;
			}

			if (Pieces[k].ExPetitRoque) {
				CompteurPetitRoque++;
				unsigned int Extra = Pieces[k].DeplacementsSiPetitRoque - Pieces[k].Deplacements;
				if (Extra < ToursExtraPetitRoque)
					ToursExtraPetitRoque = Extra;
			}
		}
		else if ((Pieces[k].Piece > PION) && (Pieces[k].Piece < ROI)) {
			if (Pieces[k].ExAileDame && (Pieces[k].DeplacementsSiAileDame == 0))
				ExtraGrandRoque += 2;
				
			if (Pieces[k].ExAileRoi && (Pieces[k].DeplacementsSiAileRoi == 0))
				ExtraPetitRoque += 2;
		}
	}

	if (CompteurTours == NombreMaximum[TOUR]) {
		if (CompteurGrandRoque == 0)
			Roi->ExGrandRoque = false;

		if (CompteurPetitRoque == 0)
			Roi->ExPetitRoque = false;
	}

	if (Roi->ExGrandRoque)
		ExtraGrandRoque += Roi->DeplacementsSiGrandRoque - Roi->Deplacements;

	if (Roi->ExPetitRoque)
		ExtraPetitRoque += Roi->DeplacementsSiPetitRoque - Roi->Deplacements;

	if (ToursExtraGrandRoque < UINT_MAX)
		ExtraGrandRoque += ToursExtraGrandRoque;

	if (ToursExtraPetitRoque < UINT_MAX)
		ExtraPetitRoque += ToursExtraPetitRoque;

	if (Roi->ExGrandRoque && CasesMortes[QuelleCase(C, Depart)])
		Roi->ExGrandRoque = false;

	if (Roi->ExPetitRoque && CasesMortes[QuelleCase(F, Depart)])
		Roi->ExPetitRoque = false;

	if (ExtraGrandRoque > CoupsLibres)
		Roi->ExGrandRoque = false;

	if (ExtraPetitRoque > CoupsLibres)
		Roi->ExPetitRoque = false;

	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece == TOUR) {
			Pieces[i].ExGrandRoque &= Roi->ExGrandRoque;
			Pieces[i].ExPetitRoque &= Roi->ExPetitRoque;
		}

		if ((Pieces[i].Piece == ROI) || (Pieces[i].Piece == TOUR))
			DeplacementMinimum(&Pieces[i]);
	}

	if (!Roi->ExAileDame && !Roi->ExAileRoi) {
		if (Roi->ExGrandRoque && !Roi->ExPetitRoque) {
			for (unsigned int i = 1; i < MaxHommes; i++) {
				if ((Pieces[i].Piece <= PION) || (Pieces[i].Piece == TOUR))
					continue;

				if (Pieces[i].CaseActuelle == CaseDepart(Pieces[i].Piece, Couleur, true, false, false))
					if (Pieces[i].ExAileDame && !Pieces[i].DeplacementsSiAileDame)
						Pieces[i].DeplacementsSiAileDame = 2;
			}
		}
		else if (Roi->ExPetitRoque && !Roi->ExGrandRoque) {
			for (unsigned int i = 1; i < MaxHommes; i++) {
				if ((Pieces[i].Piece <= PION) || (Pieces[i].Piece == TOUR))
					continue;

				if (Pieces[i].CaseActuelle == CaseDepart(Pieces[i].Piece, Couleur, true, false, false))
					if (Pieces[i].ExAileRoi && !Pieces[i].DeplacementsSiAileRoi)
						Pieces[i].DeplacementsSiAileRoi = 2;
			}
		}
	}
}

/*************************************************************/

void ExamenAttentifDesPiecesDePromotion(bonhomme Pieces[MaxHommes], couleurs Couleur, constatations *Deductions)
{
	rangees RangeePromotion = (Couleur == BLANCS) ? HUIT : UN;
	rangees PionsAdverses = (Couleur == BLANCS) ? SEPT : DEUX;
	rangees RangeePion = (Couleur == BLANCS) ? DEUX : SEPT;

	unsigned int Captures[MaxColonnes][MaxColonnes];
	bool PromotionPossible[MaxColonnes][MaxColonnes];

	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE)
			Pieces[i].ExPiece[Pieces[i].Piece] = true;
		else if (Pieces[i].PieceDisparue != VIDE)
			Pieces[i].PromotionImpossible = !Pieces[i].ExPiece[PION];
		else
			continue;
	
		if (Pieces[i].PromotionImpossible || !Pieces[i].ExPion)
			continue;

		for (colonnes CI = A; CI < MaxColonnes; CI++) {
			for (colonnes CJ = A; CJ < MaxColonnes; CJ++) {
				PromotionPossible[CI][CJ] = false;
				Captures[CI][CJ] = UINT_MAX;
			}
		}

		for (colonnes ColonnePion = A; ColonnePion < MaxColonnes; ColonnePion++) {
			if (!Pieces[i].ExPion[ColonnePion])
				continue;

			for (colonnes ColonnePassage = A; ColonnePassage < MaxColonnes; ColonnePassage++) {
				if (CasesMortes[QuelleCase(ColonnePassage,PionsAdverses)])
					continue;

				unsigned int CapturesRequises = abs((int)ColonnePassage - (int)ColonnePion);

				if (!CapturesRequises && !AvancePossibleSansCapture(QuelleCase(ColonnePion, RangeePion), QuelleCase(ColonnePassage, PionsAdverses)))
					CapturesRequises += 2;

				if (CapturesRequises > Deductions->NombreDeCapturesLibres)
					continue;

				for (int Offset = -1; Offset <= 1; Offset++) {
					if ((Offset == -1) && (ColonnePassage == A))
						continue;
					if ((Offset == 1) && (ColonnePassage == H))
						continue;

					colonnes ColonnePromotion = ColonnePassage + Offset;
					if (CasesMortes[QuelleCase(ColonnePromotion, RangeePromotion)])
						continue;

					unsigned int CapturesVraimentRequises = CapturesRequises + abs(Offset);
					if (CapturesVraimentRequises > Deductions->NombreDeCapturesLibres)
						continue;

					PromotionPossible[ColonnePion][ColonnePromotion] = true;
					if (CapturesVraimentRequises < Captures[ColonnePion][ColonnePromotion] )
						Captures[ColonnePion][ColonnePromotion] = CapturesVraimentRequises;
				}
			}

			bool PionPossible = false;

			for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++)
				PionPossible |= PromotionPossible[ColonnePion][Colonne];
		}

		unsigned int MinDeplacements = UINT_MAX;

		for (colonnes ExPion = A; ExPion < MaxColonnes; ExPion++) {
			for (colonnes Promotion = A; Promotion < MaxColonnes; Promotion++) {
				Pieces[i].DeplacementsSiCasePromotion[ExPion][Promotion] = UINT_MAX;

				if (!PromotionPossible[ExPion][Promotion])
					continue;

				cases CasePromotion = QuelleCase(Promotion, RangeePromotion);

				for (cases CaseActuelle = Pieces[i].CaseActuelleMin; CaseActuelle <= Pieces[i].CaseActuelleMax; CaseActuelle++) {
					for (pieces Piece = CAVALIER; Piece <= DAME; Piece++) {
						unsigned int Deplacements = MeilleureTrajectoire(Piece, CasePromotion, CaseActuelle);
						if (Deplacements < UINT_MAX)
							Deplacements += 5;

						if (Deplacements < Pieces[i].DeplacementsSiCasePromotion[ExPion][Promotion])
							Pieces[i].DeplacementsSiCasePromotion[ExPion][Promotion] = Deplacements;

						if (Deplacements < MinDeplacements)
							MinDeplacements = Deplacements;
					}
				}
			
				if (Pieces[i].DeplacementsSiCasePromotion[ExPion][Promotion] == UINT_MAX)
					PromotionPossible[ExPion][Promotion] = false;
			}
		}

		unsigned int MinCaptures = UINT_MAX;
		for (colonnes CJ = A; CJ < MaxColonnes; CJ++)
			for (colonnes CI = A; CI < MaxColonnes; CI++)
				if (PromotionPossible[CJ][CI])
					if (Captures[CJ][CI] < MinCaptures)
						MinCaptures = Captures[CJ][CI];

		if ((Pieces[i].DeplacementsSiPion > 5) || ((Pieces[i].DeplacementsSiPion == 5) && (Pieces[i].CapturesSiPion < 5))) {
			Verifier(!Pieces[i].CapturesSiPion);

			Pieces[i].DeplacementsSiPion = MinDeplacements;
			Pieces[i].DeplacementsSiExPiece[PION] = MinDeplacements;
			Pieces[i].CapturesSiPion = MinCaptures;

			if (Pieces[i].DeplacementsSiPion == UINT_MAX) {
				Pieces[i].PromotionImpossible = true;
				Pieces[i].ExPiece[PION] = false;
			}

			if (Pieces[i].PieceDePromotion) {
				if (Deductions->NombreDeCapturesLibres > MinCaptures)
					Deductions->NombreDeCapturesLibres -= MinCaptures;
				else
					Deductions->NombreDeCapturesLibres = 0;
			}

			DeplacementMinimum(&Pieces[i]);
		}
	}

	for (unsigned int k = 0; k < MaxHommes; k++)
		if ((Pieces[k].Piece == VIDE) && (Pieces[k].PieceDisparue == VIDE))
			for (colonnes ExPion = A; ExPion < MaxColonnes; ExPion++)
				for (colonnes Promotion = A; Promotion < MaxColonnes; Promotion++)
					Pieces[k].DeplacementsSiCasePromotion[ExPion][Promotion] = PromotionPossible[ExPion][Promotion] ? 0 : 5;
}

/*************************************************************/

void PiecesImmobiles(bonhomme Pieces[MaxHommes], couleurs Couleur, unsigned int CoupsLibres)
{
	if (CoupsLibres >= 2)
		return;

	rangees RangeeDepart = (Couleur == BLANCS) ? UN : HUIT;
	
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece <= PION)
			continue;

		if (QuelleRangee(Pieces[i].CaseActuelle) != RangeeDepart)
			continue;

		if (!Pieces[i].PromotionImpossible)
			continue;

		if (Pieces[i].ExAileDame && (Pieces[i].DeplacementsSiAileDame == 0))
			if ((Pieces[i].Piece != TOUR) || !CoupsLibres)
				AjouteUneCaseMorte(Pieces[i].CaseActuelle);

		if (Pieces[i].ExAileRoi && (Pieces[i].DeplacementsSiAileRoi == 0))
			if ((Pieces[i].Piece != TOUR) || !CoupsLibres)
				AjouteUneCaseMorte(Pieces[i].CaseActuelle);
	}
}

/*************************************************************/

void RejetDesPossibilitesMutuellementExclusives(bonhomme Pieces[MaxHommes], constatations *Deductions)
{
	bonhomme *Groupes[MaxPieces][MaxHommes];
	unsigned int TailleDuGroupe[MaxPieces];
	unsigned int i, j, k;

	for (pieces Piece = VIDE; Piece < MaxPieces; Piece++)
		TailleDuGroupe[Piece] = 0;

	for (i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE)
			Groupes[Pieces[i].Piece][TailleDuGroupe[Pieces[i].Piece]++] = &Pieces[i];
		else if (Pieces[i].PieceDisparue < MaxPieces)
			Groupes[Pieces[i].PieceDisparue][TailleDuGroupe[Pieces[i].PieceDisparue]++] = &Pieces[i];
	}


	for (i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE)
			continue;

		if (Pieces[i].PieceDisparue != MaxPieces)
			continue;
		
		pieces SeulePossibilite = VIDE;
		unsigned int Possibilites = 0;

		for (pieces Promotion = PION; Promotion <= DAME; Promotion++) {
			if (Pieces[i].ExPiece[Promotion]) {
				
				unsigned int NombreNonPromu = 0;
				for (unsigned int k = 0; k < TailleDuGroupe[Promotion]; k++)
					if (Groupes[Promotion][k]->PromotionImpossible)
						NombreNonPromu++;
				
				if (NombreNonPromu >= NombreMaximum[Promotion]) {
					Pieces[i].ExPiece[Promotion] = false;
				}
				else {
					SeulePossibilite = Promotion;
					Possibilites++;
				}
			}
		}

		if (Possibilites == 1) {
			if ((SeulePossibilite != PION) || (Pieces[i].PromotionImpossible)) {
				Pieces[i].PieceDisparue = SeulePossibilite;
				Groupes[SeulePossibilite][TailleDuGroupe[SeulePossibilite]++] = &Pieces[i];
				i = 0;  // Attention! Ce n'est pas grave si on ne repasse pas sur le cas i = 0 car c'est le roi!
			}
		}
	}


	for (pieces Type = CAVALIER; Type <= DAME; Type++) {

		unsigned int Compteurs[10];
		for (i = 0; i < 10; i++)
			Compteurs[i] = 0;

		unsigned int Max = (Type == TOUR) ? 5 : 3;
		unsigned int NumPieces = TailleDuGroupe[Type];

		while ((NumPieces > 0) && (Groupes[Type][NumPieces - 1]->Piece == VIDE))
			NumPieces--;

		unsigned int CoupsPourCeType = 0;
		for (k = 0; k < NumPieces; k++)
			CoupsPourCeType += Groupes[Type][k]->Deplacements;

		unsigned int MinDeplacements = UINT_MAX;

		if (!NumPieces)
			continue;

		bool CasPossibles[10][5];
		for (i = 0; i < 10; i++)
			for (j = 0; j < 5; j++)
				CasPossibles[i][j] = false;

		bool Done = false;

		do {
			bool AileRoiOk = false;
			bool AileDameOk = false;
			bool Impossible = false;
			unsigned int Deplacements = 0;
			unsigned int Promotions = 0;

			for (k = 0; k < NumPieces; k++) {
				switch (Compteurs[k]) {
					case 0 :
						if (AileRoiOk || !Groupes[Type][k]->ExAileRoi)
							Impossible = true;
						
						AileRoiOk = true;
						Deplacements += Groupes[Type][k]->DeplacementsSiAileRoi;
						break;
					case 1 :
						if (AileDameOk || !Groupes[Type][k]->ExAileDame)
							Impossible = true;

						AileDameOk = true;
						Deplacements += Groupes[Type][k]->DeplacementsSiAileDame;
						break;
					case 2 :
						if (++Promotions > Deductions->NombreMaximumDePromotions)
							Impossible = true;

						if (Groupes[Type][k]->PromotionImpossible)
							Impossible = true;

						Deplacements += Groupes[Type][k]->DeplacementsSiPion;
						break;
					case 3 :
						if (AileDameOk || !Groupes[Type][k]->ExGrandRoque)
							Impossible = true;

						AileDameOk = true;
						Deplacements += Groupes[Type][k]->DeplacementsSiGrandRoque;
						break;
					case 4 :
						if (AileRoiOk || !Groupes[Type][k]->ExPetitRoque)
							Impossible = true;

						AileRoiOk = true;
						Deplacements += Groupes[Type][k]->DeplacementsSiPetitRoque;
						break;
					default :
						break;
				}

				if (Impossible)
					break;
			}

			if (!Impossible) {
				Verifier(Deplacements >= CoupsPourCeType);

				if (Deplacements - CoupsPourCeType > Deductions->CoupsLibresReels)
					Impossible = true;
			}

			if (!Impossible) {
				if (Deplacements < MinDeplacements)
					MinDeplacements = Deplacements;

				for (k = 0; k < NumPieces; k++)
					CasPossibles[k][Compteurs[k]] = true;
			}

			// Ajustement du compteur
								
			if (k >= NumPieces)
				k = NumPieces - 1;

			while (!Done) {
				if (++Compteurs[k] < Max)
					break;

				Compteurs[k] = 0;
				if (!k--)
					Done = true;
			}
		} while (!Done);

		if (MinDeplacements == UINT_MAX) {
			if (NumPieces <= NombreMaximum[Type])
				ErreurEnonce(MESSAGE_AUCUNEPOSSIBILITE);
			else
				ErreurEnonce(MESSAGE_AUCUNEPROMOTIONPOSSIBLE);
		}
		else {
			Deductions->CoupsLibresReels -= (MinDeplacements - CoupsPourCeType);

			for (k = 0; k < NumPieces; k++) {
				Groupes[Type][k]->ExAileRoi &= CasPossibles[k][0];
				Groupes[Type][k]->ExAileDame &= CasPossibles[k][1];
				Groupes[Type][k]->ExGrandRoque &= CasPossibles[k][3];
				Groupes[Type][k]->ExPetitRoque &= CasPossibles[k][4];

				if (!CasPossibles[k][2])
					Groupes[Type][k]->PromotionImpossible = true;
			}
		}
	}

	for (i = 0; i < MaxHommes; i++)
		DeplacementMinimum(&Pieces[i]);
}

/*************************************************************/

void DeplacementMinimum(bonhomme *Piece)
{
	Piece->Deplacements = UINT_MAX;

	if (Piece->Piece != VIDE) {
		if (Piece->ExAileRoi)
			if (Piece->DeplacementsSiAileRoi < Piece->Deplacements)
				Piece->Deplacements = Piece->DeplacementsSiAileRoi;
		if (Piece->ExAileDame)
			if (Piece->DeplacementsSiAileDame < Piece->Deplacements)
				Piece->Deplacements = Piece->DeplacementsSiAileDame;
		if (Piece->ExGrandRoque)
			if (Piece->DeplacementsSiGrandRoque < Piece->Deplacements)
				Piece->Deplacements = Piece->DeplacementsSiGrandRoque;
		if (Piece->ExPetitRoque)
			if (Piece->DeplacementsSiPetitRoque < Piece->Deplacements)
				Piece->Deplacements = Piece->DeplacementsSiPetitRoque;
		if (Piece->ExPion)
			if (Piece->DeplacementsSiPion < Piece->Deplacements)
				Piece->Deplacements = Piece->DeplacementsSiPion;
	}
	else if (Piece->PieceDisparue != VIDE) {
		for (pieces ExPiece = PION; ExPiece < ROI; ExPiece++)
			if (Piece->ExPiece[ExPiece])
				if (Piece->DeplacementsSiExPiece[ExPiece] < Piece->Deplacements)
					Piece->Deplacements = Piece->DeplacementsSiExPiece[ExPiece];
	}

	if (Piece->Deplacements == UINT_MAX)
			Piece->Deplacements = 0;
}

/*************************************************************/

void IdentificationDeToutesLesPiecesCapturees(bonhomme Pieces[MaxHommes])
{
	unsigned int NombrePur[MaxPieces];
	unsigned int NombreSi[MaxPieces];
	unsigned int TotalSi = 0;
	bool ExPiece[MaxPieces];
	unsigned int i;
	pieces Piece;

	for (Piece = VIDE; Piece < MaxPieces; Piece++) {
		ExPiece[Piece] = ((Piece == ROI) || (Piece == VIDE)) ? false : true;
		NombrePur[Piece] = 0;
		NombreSi[Piece] = 0;
	}

	for (i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE) {
			if (Pieces[i].PromotionImpossible) {
				NombrePur[Pieces[i].Piece]++;
			}
			else if (Pieces[i].PieceDePromotion) {
				NombrePur[PION]++;
			}
			else {
				NombreSi[Pieces[i].Piece]++;
				TotalSi++;
			}
		}
		else if (Pieces[i].PieceDisparue < MaxPieces) {
			NombrePur[Pieces[i].PieceDisparue]++;
		}
		else {
			for (Piece = PION; Piece <= DAME; Piece++) {
				if (Pieces[i].ExPiece[Piece]) {
					NombreSi[Piece]++;
					TotalSi++;
				}
			}
		}
	}

	for (Piece = PION; Piece < MaxPieces; Piece++) {
		if (!ExPiece[Piece])
			continue;
			
		if (NombrePur[Piece] >= NombreMaximum[Piece])
			ExPiece[Piece] = false;
	}	
			
	for (i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE)
			continue;

		if (Pieces[i].PieceDisparue != VIDE)
			continue;

		Pieces[i].PieceDisparue = MaxPieces;
		Pieces[i].CaseActuelle = MaxCases;
		Pieces[i].CaseActuelleMin = MaxCases;
		Pieces[i].CaseActuelleMax = MaxCases;
		Pieces[i].Deplacements = 0;
		
		unsigned int Possibilites = 0;
		pieces LaSeule = VIDE;

		for (Piece = ROI; Piece >= PION; Piece--) {
			Pieces[i].ExPiece[Piece] = ExPiece[Piece];
			Pieces[i].DeplacementsSiExPiece[Piece] = 0;

			if (ExPiece[Piece] && !NombreSi[Piece]) {
				if ((Piece != PION) || ((TotalSi + NombrePur[PION]) < NombreMaximum[PION])) {
					Pieces[i].PieceDisparue = Piece;
					if (++NombrePur[Piece] >= NombreMaximum[Piece])
						ExPiece[Piece] = false;
/*
					if (Pieces[i].PieceDisparue == PION)
						Pieces[i].PromotionImpossible = true;
*/
					for (pieces k = DAME; k > Piece; k--)
						Pieces[i].ExPiece[k] = false;

					break;
				}
			}

			if (ExPiece[Piece]) {
				Possibilites++;
				LaSeule = Piece;
			}
		}

		if ((Possibilites == 1) && (Pieces[i].PieceDisparue == MaxPieces))
			Pieces[i].PieceDisparue = LaSeule;

		if ((Piece == VIDE) && (Possibilites > 1) && Pieces[i].ExPiece[PION])
			TotalSi++;
	}
}

/*************************************************************/

void RecalculDesMeilleuresTrajectoires(bonhomme Pieces[MaxHommes], couleurs Couleur)
{
	for (unsigned int i = 0; i < MaxHommes; i++) {
		if (Pieces[i].Piece != VIDE) {
			if (Pieces[i].ExAileDame) {
				cases Depart = CaseDepart(Pieces[i].Piece, Couleur, true, false, false);
				unsigned int Deplacements = MeilleureTrajectoire(Pieces[i].Piece, Depart, Pieces[i].CaseActuelle);
				if (Deplacements >= Pieces[i].DeplacementsSiAileDame)
					Pieces[i].DeplacementsSiAileDame = Deplacements;
				if (Deplacements == UINT_MAX)
					Pieces[i].ExAileDame = false;
			}

			if (Pieces[i].ExAileRoi) {
				cases Depart = CaseDepart(Pieces[i].Piece, Couleur, false, true, false);
				unsigned int Deplacements = MeilleureTrajectoire(Pieces[i].Piece, Depart, Pieces[i].CaseActuelle);
				if (Deplacements >= Pieces[i].DeplacementsSiAileRoi)
					Pieces[i].DeplacementsSiAileRoi = Deplacements;
				if (Deplacements == UINT_MAX)
					Pieces[i].ExAileRoi = false;
			}

			if (Pieces[i].ExGrandRoque) {
				cases Depart = CaseDepart(Pieces[i].Piece, Couleur, true, false, true);
				unsigned int Deplacements = MeilleureTrajectoire(Pieces[i].Piece, Depart, Pieces[i].CaseActuelle);
				if (Deplacements >= Pieces[i].DeplacementsSiGrandRoque)
					Pieces[i].DeplacementsSiGrandRoque = Deplacements;
				if (Deplacements == UINT_MAX)
					Pieces[i].ExGrandRoque = false;
			}

			if (Pieces[i].ExPetitRoque) {
				cases Depart = CaseDepart(Pieces[i].Piece, Couleur, false, true, true);
				unsigned int Deplacements = MeilleureTrajectoire(Pieces[i].Piece, Depart, Pieces[i].CaseActuelle);
				if (Deplacements >= Pieces[i].DeplacementsSiPetitRoque)
					Pieces[i].DeplacementsSiPetitRoque = Deplacements;
				if (Deplacements == UINT_MAX)
					Pieces[i].ExPetitRoque = false;
			}

			DeplacementMinimum(&Pieces[i]);
		}
	}
}

/*************************************************************/

void Delete(position *Position)
{
	delete Position;
}

/*************************************************************/