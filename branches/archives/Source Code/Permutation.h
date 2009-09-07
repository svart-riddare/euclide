#ifndef __PERMUTATION_H
#define __PERMUTATION_H

#include "Constantes.h"
#include "Mouvements.h"
#include "Position.h"

/*************************************************************/

typedef struct _scenario {
	hommes Homme;
	pieces Piece;
	cases CaseFinale;
	unsigned int Coups;
	unsigned int Captures;
	colonnes Promotion;
} scenario;

/**************/

typedef struct _destin {
	unsigned int NumCasesPossibles;
	cases CasesPossibles[MaxHommes];

	bool PossiblementDisparu;
	bool Disparitions[MaxCases];

	bool PossiblementPromu;
	bool Promotions[MaxColonnes][MaxPromotions];
	bool PromuSurCetteCase[MaxHommes];
	promotions PieceSurCetteCase[MaxHommes];

	scenario Scenarios[MaxHommes * MaxColonnes];
	scenario ScenariosMorts[MaxCases * MaxColonnes * MaxPromotions + MaxCases];

	unsigned int NombreDeScenarios;
	unsigned int NombreDeScenariosMorts;

} destin;

/**************/

typedef struct _permutations {
	destin Destins[MaxHommes];

	scenario ScenariosSuicides[MaxCases][MaxHommes + MaxColonnes * MaxColonnes * MaxPromotions];

	bool GrandRoquePossible;
	bool PetitRoquePossible;
	bool AucunRoquePossible;

} permutations;

/**************/

typedef struct _vie {
	cases Depart;
	const scenario *Scenario;
	unsigned int Coups;

	bool Capturee;
	bool Promue;
	bool SwitchbackEvident;
	bool Switchback;
    cases CaseDuSwitchback;

	unsigned int NombreAssassinats;
	_vie *Assassinats[MaxHommes];
	_vie *Assassin;

	const trajetpion *TrajetSiPion;
	const scenario *ScenariosSiPion[MaxRangees];

    
} vie;

/**************/

typedef struct _strategie {
	unsigned int IDPhaseA;
	unsigned int IDPhaseB;
	unsigned int IDPhaseC;
	unsigned int IDPhaseD;
	unsigned int IDPhaseE;
	unsigned int IDFinal;

	vie PiecesBlanches[MaxHommes];
	vie PiecesNoires[MaxHommes];

	bool PetitRoqueBlanc;
	bool GrandRoqueBlanc;
	bool PetitRoqueNoir;
	bool GrandRoqueNoir;

	unsigned int NombreDeCapturesBlanches;
	unsigned int NombreDeCapturesNoires;

	unsigned int CoupsLibresBlancs;
	unsigned int CoupsLibresNoirs;

	unsigned int CapturesLibresPourLesPionsBlancs;
	unsigned int CapturesLibresPourLesPionsNoirs;

	cases Prise[MaxCases];
} strategie;

/**************/

typedef struct _strategies {
	permutations PermutationsBlanches;
	permutations PermutationsNoires;
	unsigned int NombreDePiecesBlanches;
	unsigned int NombreDePiecesNoires;
	unsigned int DemiCoups;

	unsigned int CapturesPourLesPionsBlancs;
	unsigned int CapturesPourLesPionsNoirs;

	unsigned int CoupsParLesMortsBlancs;
	unsigned int CoupsParLesMortsNoirs;

	strategie StrategieActuelle;

} strategies;

/*************************************************************/
/* On a un gros problème avec l'inclusion de output.h        */
/*************************************************************/

typedef enum _texte;

strategies *ExamenDesStrategies(const position *Position, _texte Texte);
bool ProchaineStrategie(strategies *Strategies, _texte Texte, bool PremiereFois = false);
void Delete(strategies *Strategies);

/*************************************************************/

#endif
