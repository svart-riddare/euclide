#ifndef __OUTPUT_H
#define __OUTPUT_H

#include "Partie.h"
#include "Permutation.h"
#include "Position.h"

/*************************************************************/

typedef enum { EUCLIDE, FRANCAIS, ANGLAIS, MaxLangues } langue;

typedef enum {
	MESSAGE_SYMBOLES,
	MESSAGE_VIRGULE,
	MESSAGE_COUPS,
	MESSAGE_APPUYEZSURUNETOUCHE,
	MESSAGE_ERREURINTERNE,
	MESSAGE_FIN,
	MESSAGE_ZEROSOLUTION,
	MESSAGE_UNESOLUTION,
	MESSAGE_UNESOLUTIONUNIQUE,
	MESSAGE_NSOLUTIONS,
	MESSAGE_NSOLUTIONSDISTINCTES,
	MESSAGE_COOKED,
	NUL_1,
// Messages marquant le progrès des déductions
	MESSAGE_ANALYSEPIONS,
	MESSAGE_DECOMPTE,
	MESSAGE_CASESMORTES,
	MESSAGE_RESTRICTIONS,
	MESSAGE_MEILLEURESTRAJECTOIRES,
	MESSAGE_ANALYSEFOUS,
	MESSAGE_REJETTRAJECTOIRES,
	MESSAGE_ANALYSESOMMAIRECAPTURES,
	MESSAGE_TRAJECTOIRESDESMORTS,
	MESSAGE_ANALYSEROQUES,
	MESSAGE_PROMOTIONS,
	MESSAGE_REJETABSURDES,
	MESSAGE_PREPARATION,
	MESSAGE_RECHERCHE,
	MESSAGE_ANALYSE,
	MESSAGE_SUITE,
	NUL_2,
// Erreurs
	MESSAGE_TROPDEPIONS,
	MESSAGE_TROPDEPIECES,
	MESSAGE_TROPDEPIECESDEPROMOTION,
	MESSAGE_TROPDEROIS,
	MESSAGE_STRUCTUREDEPIONSILLEGALE,
	MESSAGE_CONFUSIONDELANGUE,
	MESSAGE_ERREURCONVERSIONEPD,
	MESSAGE_PASASSEZDECOUPS,	
	MESSAGE_AUCUNEPOSSIBILITE,
	MESSAGE_AUCUNECAPTUREPOSSIBLE,
	MESSAGE_AUCUNEPROMOTIONPOSSIBLE,
	MESSAGE_AUCUNESOLUTION,
	MESSAGE_UTILISATEUR,
	MESSAGE_ERREURLECTURE,
	MESSAGE_MAUVAISARGUMENTS,
	MESSAGE_AUCUNPROBLEME,
	MESSAGE_TROPDECOUPS,
	NUL_3,
// Fichier de sortie
	MESSAGE_PROBLEMEANALYSE,
	MESSAGE_SOLUTION,
	MESSAGE_VERDICT,
	MESSAGE_SECONDES,
	MESSAGE_ANALYSEINTERROMPUE,
	MESSAGE_STRATEGIEOMISE,
	MESSAGE_REPRISE,
	MaxTextes
} texte;

/*************************************************************/

void ChoixDeLangue(langue Langue = EUCLIDE);
void DestructionDesTextes();

const char *GetTexte(texte Texte, unsigned int LongueurMaximale, bool TailleExacte);
char PieceToChar(pieces Piece);
char HommeToChar(hommes Homme);
char ColonneToChar(colonnes Colonne);
char RangeeToChar(rangees Rangee);
const char *CaseToString(cases Case);

void OutputMessage(texte Message, unsigned int Compte = 0);
void OutputNombreSolutions(unsigned int NombreSolutions, bool Duals, bool Final);
void OutputMessageErreur(texte Message);

void OutputFile(const char *Fichier);
void OutputEntete(const char *PositionEPD, unsigned int DemiCoups);
void OutputSolution(const solution *Solution, unsigned int Numero, bool Dual);
void OutputPiedDePage(unsigned int NombreSolutions, bool Duals, bool Escape);
void OutputStrategieOmise(const strategie *Strategie);
void OutputContinuerDe(unsigned int ContinuerDe);

/*************************************************************/
/* Platform dependent functions...                           */
/*************************************************************/

void OutputCreate();
void OutputDestroy();

void OutputDiagramme(const diagramme *Diagramme);
void OutputNombreDePieces(unsigned int Blancs, unsigned int Noirs);
void OutputResultat(const char *Resultat);
void OutputMessage(const char *Message);
void OutputMessageErreur(const char *Message);
void OutputChrono(const char *Chrono);
void OutputClear();
void OutputDeplacementsMinimaux(const bonhomme PiecesBlanches[MaxHommes], const bonhomme PiecesNoires[MaxHommes]);
void OutputCoupsLibres(unsigned int CoupsBlancs, unsigned int CoupsNoirs);
void OutputStrategie(const strategie *Strategie, const pseudopartie *Partie);
void OutputDebutPartie(deplacement **Deplacements);

bool WaitForInput();
int IsEscape();

#define ESCAPE_NO     0
#define ESCAPE_ESCAPE 1
#define ESCAPE_SKIP   2

/*************************************************************/

#endif
