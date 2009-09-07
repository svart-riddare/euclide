#include "Constantes.h"
#include "Erreur.h"

/*************************************************************/

const unsigned int NombreMaximum[MaxPieces] = {
	0, 8, 2, 1, 1, 2, 1, 1 
};

/*************************************************************/

colonnes QuelleColonne(cases Case)
{
	Verifier(Case < MaxCases);

	return (Case / 8);
}

/*************************************************************/

rangees QuelleRangee(cases Case)
{
	Verifier(Case < MaxCases);

	return (Case % 8);
}

/*************************************************************/

cases QuelleCase(colonnes Colonne, rangees Rangee)
{
	Verifier(Colonne < MaxColonnes);
	Verifier(Rangee < MaxRangees);

	return (Colonne * 8 + Rangee);
}

/*************************************************************/

pieces QuelFou(cases Case)
{
	Verifier(Case < MaxCases);

	unsigned int A = Case % 2;
	unsigned int B = Case / 8;
	unsigned int C = (A + B) % 2;

	return C ? FOUBLANC : FOUNOIR;
}

/*************************************************************/

pieces QuellePiece(hommes Homme, couleurs Couleur)
{
	pieces Piece = VIDE;

	switch (Homme) {
		case XROI :
			Piece = ROI;
			break;
		case XDAME :
			Piece = DAME;
			break;
		case TOURDAME :
		case TOURROI :
			Piece = TOUR;
			break;
		case FOUDAME :
			Piece = (Couleur == BLANCS) ? FOUNOIR : FOUBLANC;
			break;
		case FOUROI :
			Piece = (Couleur == BLANCS) ? FOUBLANC : FOUNOIR;
			break;
		case CAVALIERDAME :
		case CAVALIERROI :
			Piece = CAVALIER;
			break;
		case PIONA :
		case PIONB :
		case PIONC :
		case PIOND :
		case PIONE :
		case PIONF :
		case PIONG :
		case PIONH :
			Piece = PION;
			break;
		default :
			break;
	}

	return Piece;
}

/*************************************************************/

pieces QuellePiece(promotions PiecePromotion, colonnes ColonnePromotion, couleurs Couleur)
{
	cases CasePromotion = QuelleCase(ColonnePromotion, (Couleur == BLANCS) ? HUIT : UN);
	pieces Piece = VIDE;

	switch (PiecePromotion) {
		case PROMOTIONDAME :
			Piece = DAME;
			break;
		case PROMOTIONTOUR :
			Piece = TOUR;
			break;
		case PROMOTIONFOU :
			Piece = QuelFou(CasePromotion);
			break;
		case PROMOTIONCAVALIER :
			Piece = CAVALIER;
			break;
		default :
			break;
	}

	return Piece;
}

/*************************************************************/

hommes QuelHomme(pieces Piece, bool ExAileDame, bool ExAileRoi)
{
	Verifier(ExAileDame ^ ExAileRoi);
	hommes Homme = MaxHommes;

	switch (Piece) {
		case ROI :
			Verifier(ExAileRoi);
			Homme = XROI;
			break;
		case DAME :
			Verifier(ExAileDame);
			Homme = XDAME;
			break;
		case TOUR :
			Homme = ExAileDame ? TOURDAME : TOURROI;
			break;
		case FOUBLANC :
		case FOUNOIR :
			Homme = ExAileDame ? FOUDAME : FOUROI;
			break;
		case CAVALIER :
			Homme = ExAileDame ? CAVALIERDAME : CAVALIERROI;
			break;
		default :
			Verifier(Homme != MaxHommes);  // false!
			break;
	}

	return Homme;
}

/*************************************************************/

promotions QuellePromotion(pieces Piece)
{
	promotions Promotion = MaxPromotions;

	switch (Piece) {
		case DAME :
			Promotion = PROMOTIONDAME;
			break;
		case TOUR :
			Promotion = PROMOTIONTOUR;
			break;
		case FOUBLANC : 
		case FOUNOIR :
			Promotion = PROMOTIONFOU;
			break;
		case CAVALIER :
			Promotion = PROMOTIONCAVALIER;
			break;
		default :
			Verifier(Promotion != MaxPromotions);  // false!
			break;
	}

	return Promotion;
}

/*************************************************************/

