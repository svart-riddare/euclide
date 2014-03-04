#ifndef __COUPS_H
#define __COUPS_H

#include "Constantes.h"
#include "Permutation.h"

/*************************************************************/

typedef struct _coup {
	vie *Piece;
	cases De;
	cases Vers;
	pieces Type;
	const vie *Victime;
	const vie *Assassin;

	bool Premier;
	bool Promotion;
	bool Dernier;
	bool Roque;

	bool CoupJoue;

	unsigned int NombreDeCoups;
	unsigned int NombreDeCoupsSiCaseBloquee[MaxCases];

	unsigned int AuPlusTot;
	unsigned int AuPlusTard;

	unsigned int NombreDoitSuivre;
	unsigned int NombreDoitPreceder;
	_coup *DoitSuivre[4 * MaxCoups];
	_coup *DoitPreceder[4 * MaxCoups];

	unsigned int NombrePeutSuivre;
	unsigned int NombrePeutPreceder;
	_coup *PeutSuivre[2 * MaxCoups];
	_coup *PeutPreceder[2 * MaxCoups];

	// Les valeurs des prochains champs sont calculées mais inutilisées pour l'instant

	unsigned int NombreDoitSuivreSiPossible;
	unsigned int NombreDoitPrecederSiPossible;
	_coup *DoitSuivreSiPossible[2 * MaxCoups];
	_coup *DoitPrecederSiPossible[2 * MaxCoups];
} coup;

typedef struct _pseudopartie {
	unsigned int NombreCoupsBlancs;
	unsigned int NombreCoupsNoirs;

	coup CoupsBlancs[MaxCoups];
	coup CoupsNoirs[MaxCoups];

	unsigned int IndexPremiersCoupsBlancs[MaxHommes];
	unsigned int IndexPremiersCoupsNoirs[MaxHommes];

	strategie *Strategie;
} pseudopartie;


/*************************************************************/

bool AnalysePhaseA(strategie *Strategie);
pseudopartie *AnalysePhaseB(strategie *Strategie);
bool AnalysePhaseC(pseudopartie *Partie);
bool AnalysePhaseD(pseudopartie *Partie, unsigned int DemiCoups);
bool AnalysePhaseE(pseudopartie *Partie);
void AnalysePhaseX(strategie *Strategie);

void Delete(pseudopartie *Partie);

/*************************************************************/

#endif
