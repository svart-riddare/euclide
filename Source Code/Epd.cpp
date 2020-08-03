#include <ctype.h>
#include <string.h>
#include "Constantes.h"
#include "Erreur.h"
#include "Output.h"
#include "Position.h"
#include "Epd.h"

/*************************************************************
	Français: RDTFCP
	Anglais:  KQRBNP ou KQRBSP
	Allemand: KDTLSB
	Italien:  RDTACP
 *************************************************************/

/*************************************************************/

diagramme *EPDToDiagramme(const char *EPD, unsigned int DemiCoups, bool Afficher)
{
	diagramme *DiagrammeFrancais = new diagramme;
	diagramme *DiagrammeAllemand = new diagramme;
	diagramme *DiagrammeAnglais = new diagramme;
	diagramme *DiagrammeItalien = new diagramme;

	memset(DiagrammeFrancais, 0, sizeof(diagramme));
	memset(DiagrammeAllemand, 0, sizeof(diagramme));
	memset(DiagrammeAnglais, 0, sizeof(diagramme));
	memset(DiagrammeItalien, 0, sizeof(diagramme));

	DiagrammeFrancais->DemiCoups = DemiCoups;
	DiagrammeAllemand->DemiCoups = DemiCoups;
	DiagrammeAnglais->DemiCoups = DemiCoups;
	DiagrammeItalien->DemiCoups = DemiCoups;

	bool Francais = true;
	bool Allemand = true;
	bool Anglais = true;
	bool Italien = true;

	rangees Rangee = HUIT;
	colonnes Colonne = A;

	unsigned int RoisFrancais[MaxCouleurs];
	unsigned int RoisAllemand[MaxCouleurs];
	unsigned int RoisAnglais[MaxCouleurs];
	unsigned int RoisItalien[MaxCouleurs];

	memset(RoisFrancais, 0, sizeof(RoisFrancais));
	memset(RoisAllemand, 0, sizeof(RoisAllemand));
	memset(RoisAnglais, 0, sizeof(RoisAnglais));
	memset(RoisItalien, 0, sizeof(RoisItalien));

	bool EspacesOk = true;

	while (*EPD) {
		couleurs Couleur;

		if (isdigit(*EPD)) {
			Colonne += (*EPD - '0');
			EspacesOk = false;
		}
		else if (*EPD == '/') {
			EspacesOk = false;
			Colonne = A;
			if (Rangee-- == UN)
				goto GotoErreur;
		}
		else if (isalpha(*EPD)) {
			Couleur = isupper(*EPD) ? BLANCS : NOIRS;
			EspacesOk = false;

			if (Colonne++ > HUIT)
				goto GotoErreur;

			cases Case = QuelleCase(Colonne - 1, Rangee);

			DiagrammeFrancais->Couleurs[Case] = Couleur;
			DiagrammeAllemand->Couleurs[Case] = Couleur;
			DiagrammeAnglais->Couleurs[Case] = Couleur;
			DiagrammeItalien->Couleurs[Case] = Couleur;

			switch (toupper(*EPD)) {
				case 'A' :
					DiagrammeItalien->Pieces[Case] = QuelFou(Case);
					Francais = false;
					Allemand = false;
					Anglais = false;
					break;
				case 'B' :
					DiagrammeAllemand->Pieces[Case] = PION;
					DiagrammeAnglais->Pieces[Case] = QuelFou(Case);
					Francais = false;
					Italien = false;
					break;
				case 'C' :
					DiagrammeFrancais->Pieces[Case] = CAVALIER;
					DiagrammeItalien->Pieces[Case] = CAVALIER;
					Allemand = false;
					Anglais = false;
					break;
				case 'D' :
					DiagrammeFrancais->Pieces[Case] = DAME;
					DiagrammeAllemand->Pieces[Case] = DAME;
					DiagrammeItalien->Pieces[Case] = DAME;
					Anglais = false;
					break;
				case 'F' :
					DiagrammeFrancais->Pieces[Case] = QuelFou(Case);
					Allemand = false;
					Anglais = false;
					Italien = false;
					break;
				case 'K' :
					DiagrammeAllemand->Pieces[Case] = ROI;
					DiagrammeAnglais->Pieces[Case] = ROI;
					RoisAllemand[Couleur]++;
					RoisAnglais[Couleur]++;
					Francais = false;
					Italien = false;
					break;
				case 'L' :
					DiagrammeAllemand->Pieces[Case] = QuelFou(Case);
					Francais = false;
					Anglais = false;
					Italien = false;
					break;
				case 'N' :
					DiagrammeAnglais->Pieces[Case] = CAVALIER;
					Francais = false;
					Allemand = false;
					Italien = false;
					break;
				case 'P' :
					DiagrammeFrancais->Pieces[Case] = PION;
					DiagrammeAnglais->Pieces[Case] = PION;
					DiagrammeItalien->Pieces[Case] = PION;
					Allemand = false;
					break;
				case 'Q' :
					DiagrammeAnglais->Pieces[Case] = DAME;
					Francais = false;
					Allemand = false;
					Italien = false;
					break;
				case 'R' :
					DiagrammeFrancais->Pieces[Case] = ROI;
					DiagrammeAnglais->Pieces[Case] = TOUR;
					DiagrammeItalien->Pieces[Case] = ROI;
					RoisFrancais[Couleur]++;
					RoisItalien[Couleur]++;
					Allemand = false;
					break;
				case 'S' :
					DiagrammeAllemand->Pieces[Case] = CAVALIER;
					DiagrammeAnglais->Pieces[Case] = CAVALIER;
					Francais = false;
					Italien = false;
					break;
				case 'T' :
					DiagrammeFrancais->Pieces[Case] = TOUR;
					DiagrammeAllemand->Pieces[Case] = TOUR;
					DiagrammeItalien->Pieces[Case] = TOUR;
					Anglais = false;
					break;
				default :
					goto GotoErreur;
			}
		}
		else if (isspace(*EPD)) {
			if (!EspacesOk)
				break;
		}
		else {
			goto GotoErreur;
		}

		EPD++;
	}

	if ((RoisFrancais[BLANCS] != 1) || (RoisFrancais[NOIRS] != 1))
		Francais = false;
	if ((RoisAllemand[BLANCS] != 1) || (RoisAllemand[NOIRS] != 1))
		Allemand = false;
	if ((RoisAnglais[BLANCS] != 1) || (RoisAnglais[NOIRS] != 1))
		Anglais = false;
	if ((RoisItalien[BLANCS] != 1) || (RoisItalien[NOIRS] != 1))
		Italien = false;

	if (Francais && Italien && !Anglais && !Allemand)
		Italien = false;

	if (Francais && (Allemand || Anglais || Italien))
		ErreurEnonce(MESSAGE_CONFUSIONDELANGUE);
	if (Allemand && (Francais || Anglais || Italien))
		ErreurEnonce(MESSAGE_CONFUSIONDELANGUE);
	if (Anglais && (Francais || Allemand || Italien))
		ErreurEnonce(MESSAGE_CONFUSIONDELANGUE);
	if (Italien && (Francais || Allemand || Anglais))
		ErreurEnonce(MESSAGE_CONFUSIONDELANGUE);

	if (!Francais)
		delete DiagrammeFrancais;
	if (!Allemand)
		delete DiagrammeAllemand;
	if (!Anglais)
		delete DiagrammeAnglais;
	if (!Italien)
		delete DiagrammeItalien;

	if (Francais)
		return DiagrammeFrancais;

	if (Allemand)
		return DiagrammeAllemand;

	if (Anglais)
		return DiagrammeAnglais;

	if (Italien)
		return DiagrammeItalien;

	DiagrammeFrancais = NULL;
	DiagrammeAllemand = NULL;
	DiagrammeAnglais = NULL;
	DiagrammeItalien = NULL;

GotoErreur:
	delete DiagrammeFrancais;
	delete DiagrammeAllemand;
	delete DiagrammeAnglais;
	delete DiagrammeItalien;

	if (Afficher)
		ErreurEnonce(MESSAGE_ERREURCONVERSIONEPD);

	return NULL;
}

/*************************************************************/

void Delete(diagramme *Diagramme)
{
	delete Diagramme;
}

/*************************************************************/
