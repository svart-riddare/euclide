#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "Constantes.h"
#include "Erreur.h"
#include "Mouvements.h"
#include "Position.h"

/*************************************************************/

typedef bool _tablemouvements[MaxCases][MaxCases];
typedef unsigned int _tablebloqueurs[MaxCases][MaxCases];
typedef unsigned int _tabledeplacements[MaxCases][MaxCases];
typedef bool _tablevalidation[MaxCases];

/*************************************************************/

void CreationDesTablesInterception();
void CalculDesDeplacementsPossibles(cases De, _tablemouvements *TableMouvements, _tabledeplacements *TableDeplacements, _tablevalidation *TableValidation);
void CalculDesDeplacementsPossibles(cases De, _tablebloqueurs *TableBloqueurs, _tabledeplacements *TableDeplacements, _tablevalidation *TableValidation);
void CalculDesTrajetsDePionPossibles(cases De, cases Vers, trajetpion *Trajets, couleurs Couleur);
int TriDesTrajets(const void *DataA, const void *DataB);

/*************************************************************/

static bool TousLesMouvementsTour[MaxCases][MaxCases];
static bool TousLesMouvementsDame[MaxCases][MaxCases];
static bool TousLesMouvementsFou[MaxCases][MaxCases];
static bool TousLesMouvementsCavalier[MaxCases][MaxCases];
static bool TousLesMouvementsRoi[MaxCases][MaxCases];
static bool TousLesMouvementsPionBlancs[MaxCases][MaxCases];
static bool TousLesMouvementsPionNoirs[MaxCases][MaxCases];

static bool **TablesInterception[MaxCases];
static unsigned int **TablesBlocages[MaxCases];

static const unsigned int MaxInterceptionsTemporaires = 512;
static bool *InterceptionsTemporaires[MaxInterceptionsTemporaires];
static unsigned int NombreInterceptionsTemporaires = 0;

static bool MouvementsTour[MaxCases][MaxCases];
static bool MouvementsFou[MaxCases][MaxCases];
static bool MouvementsCavalier[MaxCases][MaxCases];
static bool MouvementsDame[MaxCases][MaxCases];
static bool MouvementsRoi[MaxCases][MaxCases];

bool CasesMortes[MaxCases];

static unsigned int DeplacementsTour[MaxCases][MaxCases];
static unsigned int DeplacementsFou[MaxCases][MaxCases];
static unsigned int DeplacementsCavalier[MaxCases][MaxCases];
static unsigned int DeplacementsDame[MaxCases][MaxCases];
static unsigned int DeplacementsRoi[MaxCases][MaxCases];

static bool DeplacementsTourCorrects[MaxCases];
static bool DeplacementsFouCorrects[MaxCases];
static bool DeplacementsCavalierCorrects[MaxCases];
static bool DeplacementsDameCorrects[MaxCases];
static bool DeplacementsRoiCorrects[MaxCases];

static unsigned int DeplacementsTourBis[MaxCases][MaxCases];
static unsigned int DeplacementsFouBis[MaxCases][MaxCases];
static unsigned int DeplacementsCavalierBis[MaxCases][MaxCases];
static unsigned int DeplacementsDameBis[MaxCases][MaxCases];
static unsigned int DeplacementsRoiBis[MaxCases][MaxCases];

static bool DeplacementsTourCorrectsBis[MaxCases];
static bool DeplacementsFouCorrectsBis[MaxCases];
static bool DeplacementsCavalierCorrectsBis[MaxCases];
static bool DeplacementsDameCorrectsBis[MaxCases];
static bool DeplacementsRoiCorrectsBis[MaxCases];

static const unsigned int MaxTrajectoiresDePion = 256;

static trajetpion TrajetsPionsBlancs[MaxColonnes][MaxCases][MaxTrajectoiresDePion];
static trajetpion TrajetsPionsNoirs[MaxColonnes][MaxCases][MaxTrajectoiresDePion];

static bool TrajetsBlancsValides[MaxColonnes][MaxCases];
static bool TrajetsNoirsValides[MaxColonnes][MaxCases];

static unsigned int BloqueursPion[MaxCases];
static unsigned int BloqueursCavalier[MaxCases][MaxCases];
static unsigned int BloqueursTour[MaxCases][MaxCases];
static unsigned int BloqueursDame[MaxCases][MaxCases];
static unsigned int BloqueursFou[MaxCases][MaxCases];
static unsigned int BloqueursRoi[MaxCases][MaxCases];

/*************************************************************/

void CreationDesTablesDeMouvement()
{
	for (cases De = A1; De <= H8; De++) {
		colonnes ColonneDe = QuelleColonne(De);
		rangees RangeeDe = QuelleRangee(De);

		for (cases Vers = A1; Vers <= H8; Vers++) {
			colonnes ColonneVers = QuelleColonne(Vers);
			rangees RangeeVers = QuelleRangee(Vers);

			TousLesMouvementsTour[De][Vers] = false;
			TousLesMouvementsFou[De][Vers] = false;
			TousLesMouvementsCavalier[De][Vers] = false;
			TousLesMouvementsRoi[De][Vers] = false;
			TousLesMouvementsPionBlancs[De][Vers] = false;
			TousLesMouvementsPionNoirs[De][Vers] = false;
			
			if ((ColonneVers == ColonneDe) ^ (RangeeVers == RangeeDe))
				TousLesMouvementsTour[De][Vers] = true;

			if (abs((int)ColonneDe - (int)ColonneVers) == abs((int)RangeeDe - (int)RangeeVers))
				if (ColonneDe != ColonneVers)
					TousLesMouvementsFou[De][Vers] = true;

			if (abs((int)ColonneDe - (int)ColonneVers) * abs((int)RangeeDe - (int)RangeeVers) == 2)
				TousLesMouvementsCavalier[De][Vers] = true;

			if (TousLesMouvementsTour[De][Vers] || TousLesMouvementsFou[De][Vers])
				if ((abs((int)ColonneDe - (int)ColonneVers) <= 1) && (abs((int)RangeeDe - (int)RangeeVers) <= 1))
					TousLesMouvementsRoi[De][Vers] = true;
			
			if ((RangeeDe != UN) && (RangeeDe != HUIT)) {
				if (abs((int)RangeeDe - (int)RangeeVers) == 1)
					if (abs((int)ColonneDe - (int)ColonneVers) <= 1)
						(RangeeDe < RangeeVers) ? TousLesMouvementsPionBlancs[De][Vers] = true : TousLesMouvementsPionNoirs[De][Vers] = true;

				if ((RangeeDe == DEUX) && (RangeeVers == QUATRE) && (ColonneDe == ColonneVers))
					TousLesMouvementsPionBlancs[De][Vers] = true;

				if ((RangeeDe == SEPT) && (RangeeVers == CINQ) && (ColonneDe == ColonneVers))
					TousLesMouvementsPionNoirs[De][Vers] = true;
			}

			TousLesMouvementsDame[De][Vers] = TousLesMouvementsTour[De][Vers] || TousLesMouvementsFou[De][Vers];
		}
	}

	CreationDesTablesInterception();
}

/*************************************************************/

void CreationDesTablesInterception()
{
	bool *Interceptions[256];
	unsigned int *Blocages[256];

	for (cases Case = A1; Case <= H8; Case++) {
		unsigned int NombreInterceptions = 0;
		TablesInterception[Case] = NULL;
		TablesBlocages[Case] = NULL;
		
		colonnes Colonne = QuelleColonne(Case);
		rangees Rangee = QuelleRangee(Case);

{
		for (colonnes De = A; De <= H; De++) {
			for (colonnes Vers = A; Vers <= H; Vers++) {
				if ((De < Colonne && Colonne < Vers) || (Vers < Colonne && Colonne < De)) {
					Interceptions[NombreInterceptions] = &MouvementsTour[QuelleCase(De, Rangee)][QuelleCase(Vers, Rangee)];
					Blocages[NombreInterceptions] = &BloqueursTour[QuelleCase(De, Rangee)][QuelleCase(Vers, Rangee)];
					NombreInterceptions++;
					Interceptions[NombreInterceptions] = &MouvementsDame[QuelleCase(De, Rangee)][QuelleCase(Vers, Rangee)];
					Blocages[NombreInterceptions] = &BloqueursDame[QuelleCase(De, Rangee)][QuelleCase(Vers, Rangee)];
					NombreInterceptions++;
				}
			}
		}
}
{
		for (rangees De = A; De <= H; De++) {
			for (rangees Vers = A; Vers <= H; Vers++) {
				if ((De < Rangee && Rangee < Vers) || (Vers < Rangee && Rangee < De)) {
					Interceptions[NombreInterceptions] = &MouvementsTour[QuelleCase(Colonne, De)][QuelleCase(Colonne, Vers)];
					Blocages[NombreInterceptions] = &BloqueursTour[QuelleCase(Colonne, De)][QuelleCase(Colonne, Vers)];
					NombreInterceptions++;
					Interceptions[NombreInterceptions] = &MouvementsDame[QuelleCase(Colonne, De)][QuelleCase(Colonne, Vers)];
					Blocages[NombreInterceptions] = &BloqueursDame[QuelleCase(Colonne, De)][QuelleCase(Colonne, Vers)];
					NombreInterceptions++;
				}
			}
		}
}
{
		for (cases De = A1; De <= H8; De++) {
			if (TousLesMouvementsFou[De][Case]) {
				colonnes DeColonne = QuelleColonne(De);
				rangees DeRangee = QuelleRangee(De);

				for (cases Vers = A1; Vers <= H8; Vers++) {
					if (TousLesMouvementsFou[Case][Vers]) {
						colonnes VersColonne = QuelleColonne(Vers);
						rangees VersRangee = QuelleRangee(Vers);

						if (abs((int)DeColonne - (int)VersColonne) == abs((int)DeRangee - (int)VersRangee)) {
							if (((DeColonne < QuelleColonne(Case)) && (QuelleColonne(Case) < VersColonne)) || ((DeColonne > QuelleColonne(Case)) && (QuelleColonne(Case) > VersColonne))) {
								Interceptions[NombreInterceptions] = &MouvementsFou[De][Vers];
								Blocages[NombreInterceptions] = &BloqueursFou[De][Vers];
								NombreInterceptions++;
								Interceptions[NombreInterceptions] = &MouvementsDame[De][Vers];
								Blocages[NombreInterceptions] = &BloqueursDame[De][Vers];
								NombreInterceptions++;
							}
						}
					}
				}
			}
		}
}

		Verifier(NombreInterceptions <= 256);
		
		TablesInterception[Case] = new bool *[NombreInterceptions + 1];
		memcpy(TablesInterception[Case], Interceptions, NombreInterceptions * sizeof(bool *));
		TablesInterception[Case][NombreInterceptions] = NULL;

		TablesBlocages[Case] = new unsigned int *[NombreInterceptions + 1];
		memcpy(TablesBlocages[Case], Blocages, NombreInterceptions * sizeof(unsigned int *));
		TablesBlocages[Case][NombreInterceptions] = NULL;
	}
}

/*************************************************************/

void DestructionDesTablesDeMouvement()
{
	for (cases Case = A1; Case <= H8; Case++) {
		delete[] TablesInterception[Case];
		delete[] TablesBlocages[Case];
	}
}

/*************************************************************/

void AjouteUneCaseMorte(cases Case)
{
	if (CasesMortes[Case])
		return;

	CasesMortes[Case] = true;

	for (cases De = A1; De <= H8; De++) {
		MouvementsTour[De][Case] = false;
		MouvementsTour[Case][De] = false;
		MouvementsDame[De][Case] = false;
		MouvementsDame[Case][De] = false;
		MouvementsFou[De][Case] = false;
		MouvementsFou[Case][De] = false;
		MouvementsRoi[Case][De] = false;
		MouvementsRoi[De][Case] = false;
		MouvementsCavalier[De][Case] = false;
		MouvementsCavalier[Case][De] = false;
	}

	bool **Interception = TablesInterception[Case];
	while (*Interception) {
		**Interception = false;
		Interception++;
	}		
}

/*************************************************************/

void IdentificationDesCasesMortes(bonhomme Pieces[16])
{
	static bool RemiseAZero = true;

	if (RemiseAZero) {
		for (cases Case = A1; Case <= H8; Case++)
			CasesMortes[Case] = false;

		memcpy(MouvementsTour, TousLesMouvementsTour, sizeof(MouvementsTour) * sizeof(bool));
		memcpy(MouvementsDame, TousLesMouvementsDame, sizeof(MouvementsDame) * sizeof(bool));
		memcpy(MouvementsFou, TousLesMouvementsFou, sizeof(MouvementsFou) * sizeof(bool));
		memcpy(MouvementsCavalier, TousLesMouvementsCavalier, sizeof(MouvementsCavalier) * sizeof(bool));
		memcpy(MouvementsRoi, TousLesMouvementsRoi, sizeof(MouvementsRoi) * sizeof(bool));
	}

	RemiseAZero = !RemiseAZero;

	for (unsigned int i = 0; i < 16; i++) {
		if (Pieces[i].Piece != PION)
			continue;
		
		if (Pieces[i].PasDePion)
			continue;

		cases Case = Pieces[i].CaseActuelle;
		AjouteUneCaseMorte(Case);
	}
}

/*************************************************************/

unsigned int MeilleureTrajectoire(pieces Piece, cases De, cases Vers, couleurs Couleur)
{
	cases File[MaxCases];
	unsigned int Push = 0;
	unsigned int Pop = 0;

	Verifier(De < MaxCases);
	Verifier(Vers < MaxCases);

	if (De == Vers)
		return 0;

	if (CasesMortes[De] || CasesMortes[Vers])
		return UINT_MAX;

	unsigned int Coups[MaxCases];
	memset(Coups, 0, MaxCases * sizeof(unsigned int));

	_tablemouvements *TableMouvements = NULL;
	
	switch (Piece) {
		case PION :
			Verifier(Couleur != NEUTRE);
			TableMouvements = (Couleur == BLANCS) ? &TousLesMouvementsPionBlancs : &TousLesMouvementsPionNoirs;
			break;
		case CAVALIER :
			TableMouvements = &MouvementsCavalier;
			break;
		case FOUNOIR :
		case FOUBLANC :
			TableMouvements = &MouvementsFou;
			break;
		case TOUR :
			TableMouvements = &MouvementsTour;
			break;
		case DAME :
			TableMouvements = &MouvementsDame;
			break;
		case ROI :
			TableMouvements = &MouvementsRoi;
			break;
		default :
			break;
	}

	File[Push++] = De;

	while (Pop < Push) {
		cases Source = File[Pop++];
		unsigned int Longueur = 1 + Coups[Source];

		for (cases Sink = A1; Sink <= H8; Sink++) {
			if (!(*TableMouvements)[Source][Sink])
				continue;

			if (CasesMortes[Sink])
				continue;

			if (!Coups[Sink] && (Sink != De)) {
				Verifier(Push < MaxCases);

				Coups[Sink] = Longueur;
				File[Push++] = Sink;

				if (Sink == Vers)
					return Longueur;
			}
			else {
				Verifier(Longueur >= Coups[Sink]);
			}
		}
	}

	return UINT_MAX;
}

/*************************************************************/

void RestrictionDesMouvementsVerticauxTraversant(rangees RangeeX, rangees RangeeY, colonnes ColonneXY)
{
	Verifier(RangeeX != RangeeY);

	bool EnBas[MaxRangees];
	bool EnHaut[MaxRangees];
	for (rangees Rangee = A; Rangee < MaxRangees; Rangee++) {
		EnBas[Rangee] = false;
		EnHaut[Rangee] = false;
		if ((Rangee <= RangeeX) && (Rangee <= RangeeY))
			EnBas[Rangee] = true;
		if ((Rangee >= RangeeX) && (Rangee >= RangeeY))
			EnHaut[Rangee] = true;
	}

	for (rangees RangeeDe = A; RangeeDe < MaxRangees; RangeeDe++) {
		if (!EnBas[RangeeDe])
			continue;

		cases CaseDe = QuelleCase(ColonneXY, RangeeDe);
		
		for (rangees RangeeVers = A; RangeeVers < MaxRangees; RangeeVers++) {
			if (!EnHaut[RangeeVers])
				continue;

			cases CaseVers = QuelleCase(ColonneXY, RangeeVers);

			MouvementsTour[CaseDe][CaseVers] = false;
			MouvementsTour[CaseVers][CaseDe] = false;
			MouvementsDame[CaseDe][CaseVers] = false;
			MouvementsDame[CaseVers][CaseDe] = false;
			MouvementsRoi[CaseDe][CaseVers] = false;
			MouvementsRoi[CaseVers][CaseDe] = false;				
		}
	}		
}

/*************************************************************/

void RestrictionDesMouvementsDiagonauxTraversant(rangees RangeeX, rangees RangeeY, colonnes ColonneX, colonnes ColonneY)
{
	Verifier(RangeeX != RangeeY);
	Verifier(abs((int)RangeeX - (int)RangeeY) == abs((int)ColonneX - (int)ColonneY));

	colonnes ColonneGauche = (ColonneX < ColonneY) ? ColonneX : ColonneY;
	colonnes ColonneDroite = (ColonneX < ColonneY) ? ColonneY : ColonneX;
	rangees RangeeGauche = (ColonneX < ColonneY) ? RangeeX : RangeeY;
	rangees RangeeDroite = (ColonneX < ColonneY) ? RangeeY : RangeeX;

	for (colonnes ColonneDe = A; ColonneDe <= ColonneGauche; ColonneDe++) {
		for (colonnes ColonneVers = ColonneDroite; ColonneVers < MaxColonnes; ColonneVers++) {

			rangees RangeeDe = MaxRangees;
			rangees RangeeVers = MaxRangees;

			if (RangeeGauche > RangeeDroite) {
				RangeeDe = RangeeGauche + (ColonneGauche - ColonneDe);
				RangeeVers = RangeeDroite - (ColonneVers - ColonneDroite);  // Attention: UNDERFLOW
			}
			else if (RangeeGauche < RangeeDroite) {
				RangeeDe = RangeeGauche - (ColonneGauche - ColonneDe);
				RangeeVers = RangeeDroite + (ColonneVers - ColonneDroite);  // Attention: UNDERFLOW
			}
	
			if ((RangeeDe >= MaxRangees) || (RangeeVers >= MaxRangees))
				continue;
			
			cases CaseDe = QuelleCase(ColonneDe, RangeeDe);
			cases CaseVers = QuelleCase(ColonneVers, RangeeVers);
	
			MouvementsDame[CaseDe][CaseVers] = false;
			MouvementsDame[CaseVers][CaseDe] = false;
			MouvementsFou[CaseDe][CaseVers] = false;
			MouvementsFou[CaseVers][CaseDe] = false;
			MouvementsRoi[CaseDe][CaseVers] = false;
			MouvementsRoi[CaseVers][CaseDe] = false;				
		}
	}
}

/*************************************************************/

void InitialisationDesTablesDeDeplacements()
{
	for (cases i = A1; i < MaxCases; i++) {
		DeplacementsCavalierCorrects[i] = false;
		DeplacementsFouCorrects[i] = false;
		DeplacementsTourCorrects[i] = false;
		DeplacementsDameCorrects[i] = false;
		DeplacementsRoiCorrects[i] = false;		

		for (cases j = A1; j < MaxCases; j++) {
			DeplacementsCavalier[i][j] = UINT_MAX;
			DeplacementsFou[i][j] = UINT_MAX;
			DeplacementsTour[i][j] = UINT_MAX;
			DeplacementsDame[i][j] = UINT_MAX;
			DeplacementsRoi[i][j] = UINT_MAX;
		}
	}
}

/*************************************************************/

void InitialisationDesTablesDeDeplacementsBis()
{
	for (cases i = A1; i < MaxCases; i++) {
		DeplacementsCavalierCorrectsBis[i] = false;
		DeplacementsFouCorrectsBis[i] = false;
		DeplacementsTourCorrectsBis[i] = false;
		DeplacementsDameCorrectsBis[i] = false;
		DeplacementsRoiCorrectsBis[i] = false;
	}
}

/*************************************************************/

void CalculDesDeplacementsPossibles(cases De, _tablemouvements *TableMouvements, _tabledeplacements *TableDeplacements, _tablevalidation *TableValidation)
{
	(*TableValidation)[De] = true;

	cases File[MaxCases];
	unsigned int Push = 0;
	unsigned int Pop = 0;

	unsigned int *Coups = &((*TableDeplacements)[De][0]);
	File[Push++] = De;
	Coups[De] = 0;

	while (Pop < Push) {
		cases Source = File[Pop++];	
		unsigned int Longueur = 1 + Coups[Source];

		for (cases Sink = A1; Sink <= H8; Sink++) {
			if (!(*TableMouvements)[Source][Sink])
				continue;

			if (CasesMortes[Sink])
				continue;

			if (Coups[Sink] == UINT_MAX) {
				Verifier(Push < MaxCases);

				Coups[Sink] = Longueur;
				File[Push++] = Sink;
			}
			else {
				Verifier(Longueur >= Coups[Sink]);
			}
		}
	}
}

/*************************************************************/

void CalculDesDeplacementsPossiblesBis(cases De, _tablebloqueurs *TableBloqueurs, _tabledeplacements *TableDeplacements, _tablevalidation *TableValidation)
{
	(*TableValidation)[De] = true;

	cases File[MaxCases];
	unsigned int Push = 0;
	unsigned int Pop = 0;

	unsigned int *Coups = &((*TableDeplacements)[De][0]);
	for (unsigned int k = 0; k < MaxCases; k++)
		Coups[k] = UINT_MAX;

	File[Push++] = De;
	Coups[De] = 0;

	while (Pop < Push) {
		cases Source = File[Pop++];
		unsigned int Longueur = 1 + Coups[Source];

		for (cases Sink = A1; Sink <= H8; Sink++) {
			if ((*TableBloqueurs)[Source][Sink] > 0)
				continue;

			if (CasesMortes[Sink])
				continue;

			if (Coups[Sink] == UINT_MAX) {
				Verifier(Push < MaxCases);

				Coups[Sink] = Longueur;
				File[Push++] = Sink;
			}
			else {
				Verifier(Longueur >= Coups[Sink]);
			}
		}
	}
}

/*************************************************************/

unsigned int CombienDeDeplacements(hommes Homme, cases De, cases Vers)
{
	Verifier(Homme < PIONA);

	if (De == Vers)
		return 0;

	if (CasesMortes[De] || CasesMortes[Vers])
		return UINT_MAX;

	_tablemouvements *TableMouvements = NULL;
	_tabledeplacements *TableDeplacements = NULL;
	_tablevalidation *TableValidation = NULL;

	switch (Homme) {
		case CAVALIERDAME :
		case CAVALIERROI :
			TableMouvements = &MouvementsCavalier;
			TableDeplacements = &DeplacementsCavalier;
			TableValidation = &DeplacementsCavalierCorrects;
			break;
		case FOUDAME :
		case FOUROI :
			TableMouvements = &MouvementsFou;
			TableDeplacements = &DeplacementsFou;
			TableValidation = &DeplacementsFouCorrects;
			break;
		case TOURDAME :
		case TOURROI :
			TableMouvements = &MouvementsTour;
			TableDeplacements = &DeplacementsTour;
			TableValidation = &DeplacementsTourCorrects;
			break;
		case XDAME :
			TableMouvements = &MouvementsDame;
			TableDeplacements = &DeplacementsDame;
			TableValidation = &DeplacementsDameCorrects;
			break;
		case XROI :
			TableMouvements = &MouvementsRoi;
			TableDeplacements = &DeplacementsRoi;
			TableValidation = &DeplacementsRoiCorrects;
			break;
		default :
			break;
	}

	Verifier(TableValidation);

	if (!(*TableValidation)[De]) {
		if ((*TableValidation)[Vers])
			return (*TableDeplacements)[Vers][De];

		CalculDesDeplacementsPossibles(De, TableMouvements, TableDeplacements, TableValidation);
	}

	Verifier((*TableValidation)[De]);

	return (*TableDeplacements)[De][Vers];
}

/*************************************************************/

unsigned int CombienDeDeplacementsBis(pieces Piece, cases De, cases Vers)
{
	if (De == Vers)
		return 0;

	Verifier(De < MaxCases);
	Verifier(Vers < MaxCases);

	if (CasesMortes[De] || CasesMortes[Vers])
		return UINT_MAX;

	_tablebloqueurs *TableBloqueurs = NULL;
	_tabledeplacements *TableDeplacements = NULL;
	_tablevalidation *TableValidation = NULL;

	switch (Piece) {
		case CAVALIER :
			TableBloqueurs = &BloqueursCavalier;
			TableDeplacements = &DeplacementsCavalierBis;
			TableValidation = &DeplacementsCavalierCorrectsBis;
			break;
		case FOUBLANC :
		case FOUNOIR :
			TableBloqueurs = &BloqueursFou;
			TableDeplacements = &DeplacementsFouBis;
			TableValidation = &DeplacementsFouCorrectsBis;
			break;
		case TOUR :
			TableBloqueurs = &BloqueursTour;
			TableDeplacements = &DeplacementsTourBis;
			TableValidation = &DeplacementsTourCorrectsBis;
			break;
		case DAME :
			TableBloqueurs = &BloqueursDame;
			TableDeplacements = &DeplacementsDameBis;
			TableValidation = &DeplacementsDameCorrectsBis;
			break;
		case ROI :
			TableBloqueurs = &BloqueursRoi;
			TableDeplacements = &DeplacementsRoiBis;
			TableValidation = &DeplacementsRoiCorrectsBis;
			break;
		default :
			break;
	}

	Verifier(TableValidation);

	if (!(*TableValidation)[De]) {
		if ((*TableValidation)[Vers])
			return (*TableDeplacements)[Vers][De];

		CalculDesDeplacementsPossiblesBis(De, TableBloqueurs, TableDeplacements, TableValidation);
	}

	Verifier((*TableValidation)[De]);

	return (*TableDeplacements)[De][Vers];
}

/*************************************************************/

bool AvancePossibleSansCapture(cases De, cases Vers)
{
	if (De == Vers)
		return true;

	Verifier(QuelleColonne(De) == QuelleColonne(Vers));
		
	return MouvementsTour[De][Vers];
}

/*************************************************************/

void CalculDesTrajetsDePionPossibles(cases De, cases Vers, trajetpion *Trajets, couleurs Couleur)
{
	colonnes ColonneDe = QuelleColonne(De);
	colonnes ColonneVers = QuelleColonne(Vers);
	rangees RangeeDe = QuelleRangee(De);
	rangees RangeeVers = QuelleRangee(Vers);

	Verifier((RangeeDe == DEUX) || (Couleur != BLANCS));
	Verifier((RangeeDe == SEPT) || (Couleur != NOIRS));

	unsigned int MaxPas = abs((int)RangeeDe - (int)RangeeVers);
	unsigned int Trajectoires = 0;

	int Compteurs[MaxRangees];
	for (unsigned int i = 0; i < MaxRangees; i++)
		Compteurs[i] = -1;

	for ( ; ; ) {
		rangees Rangee = RangeeDe;
		colonnes Colonne = ColonneDe;

		trajetpion *Trajet = &Trajets[Trajectoires];
		Trajet->NombreDeCaptures = 0;
		Trajet->CaseDepart = De;
		Trajet->CaseFinale = Vers;
		Trajet->NombreDeCoupsMax = MaxPas;

        unsigned int Pas;
		for (Pas = 1; Pas <= MaxPas; Pas++) {

			if ((Colonne == A) && (Compteurs[Pas] == -1))
				break;

			if ((Colonne == H) && (Compteurs[Pas] == 1))
				break;

			Rangee += (Couleur == BLANCS) ? 1 : -1;
			Colonne += Compteurs[Pas];

			if (Compteurs[Pas])
				Trajet->Captures[Trajet->NombreDeCaptures++] = QuelleCase(Colonne, Rangee);

			if ((unsigned int)abs((int)Colonne - (int)ColonneVers) > (MaxPas - Pas))
				break;
		}

		if (Pas > MaxPas) {
			Verifier(Colonne == ColonneVers);
			Verifier(Rangee == RangeeVers);

			bool Possible = true;

			if (!Trajet->NombreDeCaptures && MaxPas) {
				cases FauxDepart = Trajet->CaseDepart + ((Couleur == BLANCS) ? 1 : -1);

				if (!AvancePossibleSansCapture(FauxDepart, Trajet->CaseFinale))
					Possible = false;
			}
			else if (MaxPas >= 5) {
				cases CaseCapture = Trajet->Captures[Trajet->NombreDeCaptures - 1];
				if (!AvancePossibleSansCapture(CaseCapture, Trajet->CaseFinale)) {
					Possible = false;
				}
				else if (QuelleRangee(CaseCapture) == (unsigned int)((Couleur == BLANCS) ? HUIT : UN)) {
					cases CaseSept = QuelleCase(ColonneDe, (Couleur == BLANCS) ? SEPT : DEUX);
					if (Trajet->NombreDeCaptures > 1)
						CaseSept = QuelleCase(QuelleColonne(Trajet->Captures[Trajet->NombreDeCaptures - 2]), QuelleRangee(CaseSept));

					if (CasesMortes[CaseSept])
						Possible = false;
				} 
			}

			if (Possible) {
				Trajet->NombreDeCoupsMin = Trajet->NombreDeCoupsMax;
				if (MaxPas > 1) {
					if (!Trajet->NombreDeCaptures) {
						Trajet->NombreDeCoupsMin--;
					}
					else if (Couleur == BLANCS) {
						if (QuelleRangee(Trajet->Captures[0]) > QUATRE)
							Trajet->NombreDeCoupsMin--;
					}
					else if (Couleur == NOIRS) {
						if (QuelleRangee(Trajet->Captures[0]) < CINQ)
							Trajet->NombreDeCoupsMin--;
					}
				}

				Trajectoires++;
			}

			Verifier(Trajectoires < MaxTrajectoiresDePion);
		}

		unsigned int k = Pas;
		if (k > MaxPas)
			k = MaxPas;
		
		for ( ; k >= 1; k--) {
			if (Compteurs[k]++ < 1)
				break;

			Compteurs[k] = -1;
		}
		
		if (!k)
			break;
	}

	Trajets[Trajectoires].NombreDeCaptures = UINT_MAX;
	qsort(Trajets, Trajectoires, sizeof(trajetpion), TriDesTrajets);
}

/*************************************************************/

int TriDesTrajets(const void *DataA, const void *DataB)
{
	const trajetpion *TrajetA = (const trajetpion *)DataA;
	const trajetpion *TrajetB = (const trajetpion *)DataB;

	if (TrajetA->NombreDeCaptures < TrajetB->NombreDeCaptures)
		return -1;

	if (TrajetA->NombreDeCaptures > TrajetB->NombreDeCaptures)
		return 1;

	return 0;
}

/*************************************************************/

void InitialisationDesTrajetsDePion()
{
	for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
		for (cases Case = A1; Case < MaxCases; Case++) {
			TrajetsBlancsValides[Colonne][Case] = false;
			TrajetsNoirsValides[Colonne][Case] = false;
		}
	}
}

/*************************************************************/

const trajetpion *ObtenirLesTrajetsDePion(colonnes De, cases Vers, couleurs Couleur)
{
	const trajetpion *Retour = NULL;

	Verifier(De < MaxColonnes);

	if (Couleur == BLANCS) {
		Retour = TrajetsPionsBlancs[De][Vers];

		if (!TrajetsBlancsValides[De][Vers]) {
			TrajetsBlancsValides[De][Vers] = true;
			CalculDesTrajetsDePionPossibles(QuelleCase(De, DEUX), Vers, TrajetsPionsBlancs[De][Vers], BLANCS);
		}
	}
	else if (Couleur == NOIRS) {
		Retour = TrajetsPionsNoirs[De][Vers];

		if (!TrajetsNoirsValides[De][Vers]) {
			TrajetsNoirsValides[De][Vers] = true;
			CalculDesTrajetsDePionPossibles(QuelleCase(De, SEPT), Vers, TrajetsPionsNoirs[De][Vers], NOIRS);
		}
	}

	return Retour;
}

/*************************************************************/

void AjouteUneCaseMorteTemporaire(cases Case)
{
	Verifier(!CasesMortes[Case]);
	Verifier(!NombreInterceptionsTemporaires);

	CasesMortes[Case] = true;

	for (cases De = A1; De <= H8; De++) {
		if (MouvementsTour[De][Case]) {
			MouvementsTour[De][Case] = false;
			MouvementsTour[Case][De] = false;
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsTour[De][Case];
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsTour[Case][De];
		}
		if (MouvementsDame[De][Case]) {
			MouvementsDame[De][Case] = false;
			MouvementsDame[Case][De] = false;
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsDame[De][Case];
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsDame[Case][De];
		}
		if (MouvementsFou[De][Case]) {
			MouvementsFou[De][Case] = false;
			MouvementsFou[Case][De] = false;
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsFou[De][Case];
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsFou[Case][De];
		}
		if (MouvementsCavalier[De][Case]) {
			MouvementsCavalier[De][Case] = false;
			MouvementsCavalier[Case][De] = false;
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsCavalier[De][Case];
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = &MouvementsCavalier[Case][De];
		}
	}

	Verifier(NombreInterceptionsTemporaires < MaxInterceptionsTemporaires / 2);

	bool **Interception = TablesInterception[Case];
	while (*Interception) {
		if (**Interception) {
			InterceptionsTemporaires[NombreInterceptionsTemporaires++] = *Interception;
			**Interception = false;
		}
		
		Interception++;
	}

	Verifier(NombreInterceptionsTemporaires < MaxInterceptionsTemporaires);
}

/*************************************************************/

void EnleveLaCaseMorteTemporaire(cases Case)
{
	Verifier(CasesMortes[Case]);
	CasesMortes[Case] = false;

	for (unsigned int k = 0; k < NombreInterceptionsTemporaires; k++)
		*(InterceptionsTemporaires[k]) = true;

	NombreInterceptionsTemporaires = 0;
}

/*************************************************************/

void InitialisationDesBloqueurs()
{
	for (cases De = A1; De < MaxCases; De++) {
		BloqueursPion[De] = CasesMortes[De] ? 1 : 0;

		for (cases Vers = A1; Vers < MaxCases; Vers++) {
			BloqueursCavalier[De][Vers] = TousLesMouvementsCavalier[De][Vers] ? 0 : 1;
			BloqueursTour[De][Vers] = TousLesMouvementsTour[De][Vers] ? 0 : 1;
			BloqueursDame[De][Vers] = TousLesMouvementsDame[De][Vers] ? 0 : 1;
			BloqueursFou[De][Vers] = TousLesMouvementsFou[De][Vers] ? 0 : 1;
			BloqueursRoi[De][Vers] = TousLesMouvementsRoi[De][Vers] ? 0 : 1;
		}
	}
}

/*************************************************************/

void DebloquerLaCase(cases Case, bool DebloquerDe, bool DebloquerVers)
{
	if (DebloquerVers) {
		for (cases De = A1; De <= H8; De++) {
			BloqueursCavalier[De][Case]--;
			BloqueursTour[De][Case]--;
			BloqueursDame[De][Case]--;
			BloqueursFou[De][Case]--;
			BloqueursRoi[De][Case]--;
		}
	}

	if (DebloquerDe) {
		for (cases Vers = A1; Vers <= H8; Vers++) {
			BloqueursCavalier[Case][Vers]--;
			BloqueursTour[Case][Vers]--;
			BloqueursDame[Case][Vers]--;
			BloqueursFou[Case][Vers]--;
			BloqueursRoi[Case][Vers]--;
		}
	}

	if (DebloquerDe && DebloquerVers)
		BloqueursPion[Case]--;

	unsigned int **Blocages = TablesBlocages[Case];
	while (*Blocages) {
		(**Blocages)--;
		Blocages++;
	}
}

/*************************************************************/

void BloquerLaCase(cases Case, bool BloquerDe, bool BloquerVers)
{
	if (BloquerVers) {
		for (cases De = A1; De <= H8; De++) {
			BloqueursCavalier[De][Case]++;
			BloqueursTour[De][Case]++;
			BloqueursDame[De][Case]++;
			BloqueursFou[De][Case]++;
			BloqueursRoi[De][Case]++;
		}
	}

	if (BloquerDe) {
		for (cases Vers = A1; Vers <= H8; Vers++) {
			BloqueursCavalier[Case][Vers]++;
			BloqueursTour[Case][Vers]++;
			BloqueursDame[Case][Vers]++;
			BloqueursFou[Case][Vers]++;
			BloqueursRoi[Case][Vers]++;
		}
	}

	if (BloquerDe && BloquerVers)
		BloqueursPion[Case]++;

	unsigned int **Blocages = TablesBlocages[Case];
	while (*Blocages) {
		(**Blocages)++;
		Blocages++;
	}
}

/*************************************************************/

void BloquerUneLigne(cases CaseX, cases CaseY, pieces Piece)
{
	colonnes ColonneX = QuelleColonne(CaseX);
	colonnes ColonneY = QuelleColonne(CaseY);
	rangees RangeeX = QuelleRangee(CaseX);
	rangees RangeeY = QuelleRangee(CaseY);

	Verifier((RangeeX != RangeeY) || (ColonneX != ColonneY));

	if (ColonneX == ColonneY) {
		bool EnBas[MaxRangees];
		bool EnHaut[MaxRangees];
		for (rangees Rangee = UN; Rangee < MaxRangees; Rangee++) {
			EnBas[Rangee] = false;
			EnHaut[Rangee] = false;
			if ((Rangee <= RangeeX) && (Rangee <= RangeeY))
				EnBas[Rangee] = true;
			if ((Rangee >= RangeeX) && (Rangee >= RangeeY))
				EnHaut[Rangee] = true;
		}
	
		for (rangees RangeeDe = UN; RangeeDe < MaxRangees; RangeeDe++) {
			if (!EnBas[RangeeDe])
				continue;
	
			cases CaseDe = QuelleCase(ColonneX, RangeeDe);
			
			for (rangees RangeeVers = UN; RangeeVers < MaxRangees; RangeeVers++) {
				if (!EnHaut[RangeeVers])
					continue;

				cases CaseVers = QuelleCase(ColonneX, RangeeVers);

				BloqueursTour[CaseDe][CaseVers]++;
				BloqueursTour[CaseVers][CaseDe]++;
				BloqueursDame[CaseDe][CaseVers]++;
				BloqueursDame[CaseVers][CaseDe]++;
				BloqueursRoi[CaseDe][CaseVers]++;
				BloqueursRoi[CaseVers][CaseDe]++;
			}
		}		
	}
	else if (RangeeX == RangeeY) {
		bool AGauche[MaxColonnes];
		bool ADroite[MaxColonnes];
		for (colonnes Colonne = A; Colonne < MaxColonnes; Colonne++) {
			AGauche[Colonne] = false;
			ADroite[Colonne] = false;
			if ((Colonne <= ColonneX) && (Colonne <= ColonneY))
				AGauche[Colonne] = true;
			if ((Colonne >= ColonneX) && (Colonne >= ColonneY))
				ADroite[Colonne] = true;
		}

		for (colonnes ColonneDe = A; ColonneDe < MaxColonnes; ColonneDe++) {
			if (!AGauche[ColonneDe])
				continue;
	
			cases CaseDe = QuelleCase(ColonneDe, RangeeX);
			
			for (colonnes ColonneVers = A; ColonneVers < MaxColonnes; ColonneVers++) {
				if (!ADroite[ColonneVers])
					continue;

				cases CaseVers = QuelleCase(ColonneVers, RangeeX);

				BloqueursTour[CaseDe][CaseVers]++;
				BloqueursTour[CaseVers][CaseDe]++;
				BloqueursDame[CaseDe][CaseVers]++;
				BloqueursDame[CaseVers][CaseDe]++;
				BloqueursRoi[CaseDe][CaseVers]++;
				BloqueursRoi[CaseVers][CaseDe]++;
			}
		}
	}
	else {
		Verifier(abs((int)RangeeX - (int)RangeeY) == abs((int)ColonneX - (int)ColonneY));

		colonnes ColonneGauche = (ColonneX < ColonneY) ? ColonneX : ColonneY;
		colonnes ColonneDroite = (ColonneX < ColonneY) ? ColonneY : ColonneX;
		rangees RangeeGauche = (ColonneX < ColonneY) ? RangeeX : RangeeY;
		rangees RangeeDroite = (ColonneX < ColonneY) ? RangeeY : RangeeX;

		for (colonnes ColonneDe = A; ColonneDe <= ColonneGauche; ColonneDe++) {
			for (colonnes ColonneVers = ColonneDroite; ColonneVers < MaxColonnes; ColonneVers++) {
	
				rangees RangeeDe = MaxRangees;
				rangees RangeeVers = MaxRangees;
	
				if (RangeeGauche > RangeeDroite) {
					RangeeDe = RangeeGauche + (ColonneGauche - ColonneDe);
					RangeeVers = RangeeDroite - (ColonneVers - ColonneDroite);  // Attention: UNDERFLOW
				}
				else if (RangeeGauche < RangeeDroite) {
					RangeeDe = RangeeGauche - (ColonneGauche - ColonneDe);
					RangeeVers = RangeeDroite + (ColonneVers - ColonneDroite);  // Attention: UNDERFLOW
				}
		
				if ((RangeeDe >= MaxRangees) || (RangeeVers >= MaxRangees))
					continue;
				
				cases CaseDe = QuelleCase(ColonneDe, RangeeDe);
				cases CaseVers = QuelleCase(ColonneVers, RangeeVers);
		
				BloqueursDame[CaseDe][CaseVers]++;
				BloqueursDame[CaseVers][CaseDe]++;
				BloqueursFou[CaseDe][CaseVers]++;
				BloqueursFou[CaseVers][CaseDe]++;
				BloqueursRoi[CaseDe][CaseVers]++;
				BloqueursRoi[CaseVers][CaseDe]++;
			}
		}
	}

	switch (Piece) {
		case ROI :
			Verifier(BloqueursRoi[CaseX][CaseY] > 0);
			BloqueursRoi[CaseX][CaseY]--;
			break;
		case DAME :
			Verifier(BloqueursDame[CaseX][CaseY] > 0);
			BloqueursDame[CaseX][CaseY]--;
			break;
		case TOUR :
			Verifier(BloqueursTour[CaseX][CaseY] > 0);
			BloqueursTour[CaseX][CaseY]--;
			break;
		case FOUBLANC :
		case FOUNOIR :
			Verifier(BloqueursFou[CaseX][CaseY] > 0);
			BloqueursFou[CaseX][CaseY]--;
			break;
		case CAVALIER :
			Verifier(Piece != CAVALIER);
			break;
		case PION :
		default :
			break;
	}
}

/*************************************************************/

bool CheminUniqueSiMinimumDeCoups(pieces Piece, cases De, cases Vers, cases Chemin[MaxCases])
{
	cases File[MaxCases];
	unsigned int Push = 0;
	unsigned int Pop = 0;

	if (De == Vers)
		return false;

	Verifier(De < MaxCases);
	Verifier(Vers < MaxCases);

	cases Sources[MaxCases];
	unsigned int Coups[MaxCases];
	memset(Coups, 0, MaxCases * sizeof(unsigned int));

	_tablebloqueurs *TableBloqueurs = NULL;

	switch (Piece) {
		case CAVALIER :
			TableBloqueurs = &BloqueursCavalier;
			break;
		case FOUNOIR :
		case FOUBLANC :
			TableBloqueurs = &BloqueursFou;
			break;
		case TOUR :
			TableBloqueurs = &BloqueursTour;
			break;
		case DAME :
			TableBloqueurs = &BloqueursDame;
			break;
		case ROI :
			TableBloqueurs = &BloqueursRoi;
			break;
		default :
			Verifier(TableBloqueurs);
			break;
	}

	File[Push++] = De;

	unsigned int Oui = UINT_MAX;
	while (Pop < Push) {
		cases Source = File[Pop++];
		unsigned int Longueur = 1 + Coups[Source];
		if (Longueur > Oui)
			break;

		for (cases Sink = A1; Sink <= H8; Sink++) {
			if ((*TableBloqueurs)[Source][Sink])
				continue;

			if (!Coups[Sink]) {
				Sources[Sink] = Source;
				Coups[Sink] = Longueur;
				File[Push++] = Sink;

				if (Sink == Vers)
					Oui = Longueur;
			}
			else {
				Verifier(Longueur >= Coups[Sink]);
				if (Longueur == Coups[Sink])
					Sources[Sink] = MaxCases;
			}
		}
	}

	Verifier(Oui < UINT_MAX);

	Chemin[0] = De;
	cases Case = Vers;
	while (Case != MaxCases) {
		Chemin[Coups[Case]] = Case;
		Case = Sources[Case];

		if (Case == De)
			return true;
	}

	return false;
}

/*************************************************************/

bool IsCoupPossible(pieces Piece, cases De, cases Vers)
{
	switch (Piece) {
		case ROI :
			if ((QuelleColonne(De) == E) && (QuelleColonne(Vers) == C))
				return (BloqueursRoi[De][Vers] <= 1) && !BloqueursTour[QuelleCase(A, QuelleRangee(De))][QuelleCase(D, QuelleRangee(Vers))];

			if ((QuelleColonne(De) == E) && (QuelleColonne(Vers) == G))
				return (BloqueursRoi[De][Vers] <= 1) && !BloqueursTour[QuelleCase(H, QuelleRangee(De))][QuelleCase(F, QuelleRangee(Vers))];

			return !BloqueursRoi[De][Vers];
		case DAME :
			return !BloqueursDame[De][Vers];
		case TOUR :
			return !BloqueursTour[De][Vers];
		case FOUNOIR :
		case FOUBLANC :
			return !BloqueursFou[De][Vers];
		case CAVALIER :
			return !BloqueursCavalier[De][Vers];
		case PION :
			if (BloqueursPion[De] || BloqueursPion[Vers])
				return false;

			if (QuelleColonne(De) == QuelleColonne(Vers))
				if (BloqueursPion[(De + Vers) / 2])
					return false;

			return true;
		default :
			Verifier(Piece < MaxPieces);
			break;
	}

	return false;
}

/*************************************************************/

unsigned int CombienDeCoups(pieces Piece, cases De, cases Vers, bool DeuxPourSwitchback)
{
	cases File[MaxCases + 1];
	unsigned int Push = 0;
	unsigned int Pop = 0;

	if (De == Vers)
		return (DeuxPourSwitchback ? 2 : 0);

	Verifier(De < MaxCases);
	Verifier(Vers < MaxCases);

	unsigned int Coups[MaxCases];
	memset(Coups, 0, MaxCases * sizeof(unsigned int));

	_tablebloqueurs *TableBloqueurs = NULL;

	switch (Piece) {
		case CAVALIER :
			TableBloqueurs = &BloqueursCavalier;
			break;
		case FOUNOIR :
		case FOUBLANC :
			TableBloqueurs = &BloqueursFou;
			break;
		case TOUR :
			TableBloqueurs = &BloqueursTour;
			break;
		case DAME :
			TableBloqueurs = &BloqueursDame;
			break;
		case ROI :
			TableBloqueurs = &BloqueursRoi;
			break;
		default :
			Verifier(TableBloqueurs);
			break;
	}

	File[Push++] = De;

	while (Pop < Push) {
		cases Source = File[Pop++];
		unsigned int Longueur = 1 + Coups[Source];

		for (cases Sink = A1; Sink <= H8; Sink++) {
			if ((*TableBloqueurs)[Source][Sink])
				continue;

			if (!Coups[Sink]) {
				Coups[Sink] = Longueur;
				File[Push++] = Sink;

				if (Sink == Vers)
					return Longueur;
			}
		}
	}

	return UINT_MAX;
}

/*************************************************************/

bool TrajetPionPossible(const trajetpion *Trajet)
{
	cases De = Trajet->CaseDepart;
	cases Vers;

	int Ajustement = (QuelleRangee(De) == DEUX) ? -1 : +1;

	for (unsigned int i = 0; i < Trajet->NombreDeCaptures; i++) {
		unsigned int ColonneVers = QuelleColonne(De);
		unsigned int RangeeVers = QuelleRangee(Trajet->Captures[i]) + Ajustement;
		Vers = QuelleCase(ColonneVers, RangeeVers);
		
		if (De != Vers)
			if (BloqueursTour[De][Vers])
				return false;

		De = Vers;
		Vers = Trajet->Captures[i];

		if (BloqueursFou[De][Vers])
			return false;

		De = Vers;
	}

	Vers = Trajet->CaseFinale;

	if (De != Vers)
		if (BloqueursTour[De][Vers])
			return false;

	return true;
}

/*************************************************************/

bool IsEnEchecImparable(cases CaseRoi, pieces Type, cases Case, couleurs Couleur)
{
	if ((CaseRoi >= MaxCases) || (Case >= MaxCases))
		return false;

	unsigned int ColonneRoi = QuelleColonne(CaseRoi);
	unsigned int RangeeRoi = QuelleRangee(CaseRoi);

	int DeltaC[MaxCases];
	int DeltaR[MaxCases];
	unsigned int NombreDeltas = 0;

	if (Type == PION) {
		DeltaC[NombreDeltas] = -1;
		DeltaR[NombreDeltas] = (Couleur == BLANCS) ? -1 : 1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 1;
		DeltaR[NombreDeltas] = (Couleur == BLANCS) ? -1 : 1;
		NombreDeltas++;
	}

	if ((Type == FOUBLANC) || (Type == FOUNOIR) || (Type == DAME) || (Type == ROI)) {
		DeltaC[NombreDeltas] = 1;
		DeltaR[NombreDeltas] = 1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 1;
		DeltaR[NombreDeltas] = -1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -1;
		DeltaR[NombreDeltas] = -1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -1;
		DeltaR[NombreDeltas] = 1;
		NombreDeltas++;
	}

	if ((Type == TOUR) || (Type == DAME) || (Type == ROI)) {
		DeltaC[NombreDeltas] = 1;
		DeltaR[NombreDeltas] = 0;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -1;
		DeltaR[NombreDeltas] = 0;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 0;
		DeltaR[NombreDeltas] = -1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 0;
		DeltaR[NombreDeltas] = 1;
		NombreDeltas++;
	}

	if (Type == CAVALIER) {
		DeltaC[NombreDeltas] = 2;
		DeltaR[NombreDeltas] = 1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 2;
		DeltaR[NombreDeltas] = -1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -1;
		DeltaR[NombreDeltas] = 2;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 1;
		DeltaR[NombreDeltas] = 2;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -2;
		DeltaR[NombreDeltas] = 1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -2;
		DeltaR[NombreDeltas] = -1;
		NombreDeltas++;
		DeltaC[NombreDeltas] = -1;
		DeltaR[NombreDeltas] = -2;
		NombreDeltas++;
		DeltaC[NombreDeltas] = 1;
		DeltaR[NombreDeltas] = -2;
		NombreDeltas++;
	}

	for (unsigned int i = 0; i < NombreDeltas; i++) {
		if ((DeltaC[i] < 0) && ((unsigned int)(-DeltaC[i]) > ColonneRoi))
			continue;

		if ((DeltaC[i] > 0) && ((ColonneRoi + DeltaC[i]) >= MaxColonnes))
			continue;

		if ((DeltaR[i] < 0) && ((unsigned int)(-DeltaR[i]) > RangeeRoi))
			continue;

		if ((DeltaR[i] > 0) && ((RangeeRoi + DeltaR[i]) >= MaxRangees))
			continue;

		cases Critique = QuelleCase(ColonneRoi + DeltaC[i], RangeeRoi + DeltaR[i]);

		if (Critique == Case)
			return true;
	}

	return false;
}

/*************************************************************/
