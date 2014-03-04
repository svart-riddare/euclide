#ifndef __POSITION_H
#define __POSITION_H

#include "Constantes.h"

/*************************************************************/

typedef struct _diagramme {
	pieces Pieces[MaxCases];
	couleurs Couleurs[MaxCases];
	unsigned int DemiCoups;
} diagramme;

typedef struct _constatations {
	unsigned int NombreDePieces[MaxPieces];
	unsigned int NombreMinimumDePromotions;
	unsigned int NombreMaximumDePromotions;
	unsigned int NombreTotalDePieces;
	unsigned int NombreDeCaptures;
	unsigned int NombreDeCapturesPourLesPions;
	unsigned int NombreDeCapturesLibres;
	unsigned int CoupsLibres;
	unsigned int CoupsLibresReels;
} constatations;

typedef struct _bonhomme {
	cases CaseActuelle;
	pieces Piece;

	bool PieceDePromotion;
	bool PromotionImpossible;

	bool ExPion[MaxColonnes];
	bool ExAileDame;
	bool ExAileRoi;
	bool ExGrandRoque;
	bool ExPetitRoque;

	unsigned int PasDePion;
	unsigned int Deplacements;
	unsigned int DeplacementsSiPion;
	unsigned int DeplacementsSiAileDame;
	unsigned int DeplacementsSiAileRoi;
	unsigned int DeplacementsSiPetitRoque;
	unsigned int DeplacementsSiGrandRoque;

	unsigned int CapturesSiPion;

	pieces PieceDisparue;
	cases CaseActuelleMin;
	cases CaseActuelleMax;
	bool ExPiece[MaxPieces];
	unsigned int DeplacementsSiExPiece[MaxPieces];
	unsigned int DeplacementsSiCasePromotion[MaxColonnes][MaxColonnes];

} bonhomme;

typedef struct _position {
	diagramme Diagramme;
	constatations Blancs;
	constatations Noirs;
	bonhomme PiecesBlanches[MaxHommes];
	bonhomme PiecesNoires[MaxHommes];
} position;

/*************************************************************/

position *ExamenDuDiagramme(const diagramme *Diagramme);
void Delete(position *Position);

/*************************************************************/

#endif
