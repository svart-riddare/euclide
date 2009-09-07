#ifndef __PARTIE_H
#define __PARTIE_H

#include "Constantes.h"
#include "Coups.h"

/*************************************************************/

typedef struct _deplacement {
	pieces TypePiece;

	hommes Qui;
	cases De;
	cases Vers;

	hommes Mort;
	bool Roque;
	bool EnPassant;
	bool Promotion;
} deplacement;

/*************************************************************/

typedef struct _contraintes {
	coup *PremiersCoupsBlancs[MaxHommes];
	coup *PremiersCoupsNoirs[MaxHommes];

	coup *DerniersCoupsBlancs[MaxHommes];
	coup *DerniersCoupsNoirs[MaxHommes];

	coup *CoupsBlancsEnOrdre[MaxCoups];
	coup *CoupsNoirsEnOrdre[MaxCoups];

	cases PositionsFinalesBlanches[MaxHommes];
	cases PositionsFinalesNoires[MaxHommes];

	unsigned int NombreCoupsBlancs;
	unsigned int NombreCoupsNoirs;
	unsigned int DemiCoups;

	unsigned int CoupsBlancsJoues[MaxHommes];
	unsigned int CoupsNoirsJoues[MaxHommes];

	unsigned int CoupsBlancsRequis[MaxHommes];
	unsigned int CoupsNoirsRequis[MaxHommes];

	unsigned int ProchaineEcheanceBlanche;
	unsigned int ProchaineEcheanceNoire;

	unsigned int CoupsLibresBlancs;
	unsigned int CoupsLibresNoirs;

} contraintes;

/*************************************************************/

typedef struct _etatdujeu {
	pieces Pieces[MaxCases];
	hommes Hommes[MaxCases];
	couleurs Couleurs[MaxCases];

	bool CettePieceBlanchePeutJouer[MaxHommes];
	bool CettePieceNoirePeutJouer[MaxHommes];
	bool PositionFinaleBlancheAtteinte[MaxHommes];
	bool PositionFinaleNoireAtteinte[MaxHommes];

	bool GrandRoqueBlancPossible;
	bool PetitRoqueBlancPossible;
	bool GrandRoqueNoirPossible;
	bool PetitRoqueNoirPossible;

	deplacement *Deplacements[MaxCoups];
	unsigned int DemiCoups;
	couleurs Trait;

	colonnes PriseEnPassantPossible;
} etatdujeu;

/*************************************************************/

typedef enum { UndoUnsignedInt, UndoBool, UndoCoup, UndoVie } undos;

typedef struct _undo {
	undos TypeUndo;

	union {
		unsigned int *UnsignedInt;
		bool *Bool;
		coup **Coup;
		const vie **Vie;
	} Pointeur;

	union {
		unsigned int UnsignedInt;
		bool Bool;
		coup *Coup;
		const vie *Vie;
	} AncienneValeur;
} undo;

/*************************************************************/

typedef struct _solution {
	deplacement Deplacements[MaxCoups];
	unsigned int DemiCoups;
} solution;

/*************************************************************/

unsigned int GenerationDesSolutions(pseudopartie *PseudoPartie, unsigned int DemiCoups, solution *Solutions, unsigned int MaxSolutions);

/*************************************************************/

#endif
