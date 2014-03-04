#ifndef __MOUVEMENTS_H
#define __MOUVEMENTS_H

#include "Constantes.h"
#include "Position.h"

/*************************************************************/

extern bool CasesMortes[MaxCases];

/*************************************************************/

typedef struct _trajetpion {
	cases CaseDepart;
	cases CaseFinale;
	unsigned int NombreDeCoupsMin;
	unsigned int NombreDeCoupsMax;
	unsigned int NombreDeCaptures;
	cases Captures[MaxRangees];
} trajetpion;

/*************************************************************/

void CreationDesTablesDeMouvement();
void DestructionDesTablesDeMouvement();

void IdentificationDesCasesMortes(bonhomme Pieces[16]);
void AjouteUneCaseMorte(cases Case);
void AjouteUneCaseMorteTemporaire(cases Case);
void EnleveLaCaseMorteTemporaire(cases Case);

void RestrictionDesMouvementsVerticauxTraversant(rangees RangeeX, rangees RangeeY, colonnes ColonneXY);
void RestrictionDesMouvementsDiagonauxTraversant(rangees RangeeX, rangees RangeeY, colonnes ColonneX, colonnes ColonneY);

unsigned int MeilleureTrajectoire(pieces Piece, cases De, cases Vers, couleurs Couleur = NEUTRE);

void InitialisationDesTablesDeDeplacements();
unsigned int CombienDeDeplacements(hommes Homme, cases De, cases Vers);
void InitialisationDesTablesDeDeplacementsBis();
unsigned int CombienDeDeplacementsBis(pieces Piece, cases De, cases Vers);

bool AvancePossibleSansCapture(cases De, cases Vers);
const trajetpion *ObtenirLesTrajetsDePion(colonnes De, cases Vers, couleurs Couleur);
void InitialisationDesTrajetsDePion();

void InitialisationDesBloqueurs();
void BloquerLaCase(cases Case, bool BloquerDe, bool BloquerVers);
void DebloquerLaCase(cases Case, bool DebloquerDe, bool DebloquerVers);
void BloquerUneLigne(cases CaseX, cases CaseY, pieces Piece);
unsigned int CombienDeCoups(pieces Piece, cases De, cases Vers, bool DeuxPourSwitchback);
bool TrajetPionPossible(const trajetpion *Trajet);
bool CheminUniqueSiMinimumDeCoups(pieces Piece, cases De, cases Vers, cases Chemin[MaxCases]);
bool IsCoupPossible(pieces Piece, cases De, cases Vers);
bool IsEnEchecImparable(cases CaseRoi, pieces Type, cases Case, couleurs Couleur);

/*************************************************************/

#endif
