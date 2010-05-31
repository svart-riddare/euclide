#ifndef __CONSTANTES_H
#define __CONSTANTES_H

/*************************************************************/

#define MaxCoups         (128)
#define MaxDeplacements  (256)
#define MaxUndos         (256 * MaxCoups)

/*************************************************************/

enum { VIDE, PION, CAVALIER, FOUNOIR, FOUBLANC, TOUR, DAME, ROI, MaxPieces };
typedef unsigned int pieces;

extern const unsigned int NombreMaximum[MaxPieces];

enum { NEUTRE, BLANCS, NOIRS, MaxCouleurs };
typedef unsigned int couleurs;

/*************************************************************/

enum { PROMOTIONDAME, PROMOTIONTOUR, PROMOTIONFOU, PROMOTIONCAVALIER, MaxPromotions };
typedef unsigned int promotions;

enum { XROI, XDAME, TOURDAME, TOURROI, FOUDAME, FOUROI, CAVALIERDAME, CAVALIERROI, PIONA, PIONB, PIONC, PIOND, PIONE, PIONF, PIONG, PIONH, MaxHommes };
typedef unsigned int hommes;

/*************************************************************/

enum { A, B, C, D, E, F, G, H, MaxColonnes };
enum { UN, DEUX, TROIS, QUATRE, CINQ, SIX, SEPT, HUIT, MaxRangees };

enum { 
	A1, A2, A3, A4, A5, A6, A7, A8,
	B1, B2, B3, B4, B5, B6, B7, B8,
	C1, C2, C3, C4, C5, C6, C7, C8,
	D1, D2, D3, D4, D5, D6, D7, D8,
	E1, E2, E3, E4, E5, E6, E7, E8,
	F1, F2, F3, F4, F5, F6, F7, F8,
	G1, G2, G3, G4, G5, G6, G7, G8,
	H1, H2, H3, H4, H5, H6, H7, H8, MaxCases
};

typedef unsigned int colonnes;
typedef unsigned int rangees;
typedef unsigned int cases;

/*************************************************************/

colonnes QuelleColonne(cases Case);
rangees QuelleRangee(cases Case);
cases QuelleCase(colonnes Colonne, rangees Rangee);
pieces QuelFou(cases Case);

pieces QuellePiece(hommes Homme, couleurs Couleur);
pieces QuellePiece(promotions PiecePromotion, colonnes ColonnePromotion, couleurs Couleur);
hommes QuelHomme(pieces Piece, bool ExAileDame, bool ExAileRoi);
promotions QuellePromotion(hommes Homme);

/*************************************************************/

#endif
