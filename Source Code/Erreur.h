#ifndef __ERREUR_H
#define __ERREUR_H

#include "Output.h"

#define Verifier(Affirmation) { if (!(Affirmation)) ErreurInterne(__FILE__, __LINE__); }

/*************************************************************/

void ErreurFichierLangue(const char *Fichier);
void FatalOutputError();
void ErreurEnonce(texte Erreur);
void ErreurInterne(const char *Fichier, unsigned int Ligne);

/*************************************************************/

#endif
