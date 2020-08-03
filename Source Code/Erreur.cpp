#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Erreur.h"
#include "Output.h"

/*************************************************************/

void ErreurFichierLangue(const char *Fichier)
{
	unsigned int L = strlen(Fichier);

	char *Tampon = new char[L + 8];
	memcpy(Tampon, Fichier, L);
	Tampon[L++] = ' ';
	Tampon[L++] = '?';
	Tampon[L++] = '?';
	Tampon[L++] = '?';
	Tampon[L++] = '\a';
	Tampon[L] = '\0';

	OutputMessageErreur(Tampon);
	OutputDestroy();

	fprintf(stderr, "%s\n", Tampon);

	delete[] Tampon;
	exit(-1);
}

/*************************************************************/

void FatalOutputError()
{
	fprintf(stderr, "\n\n\t\aOutput fatal error!!!\n\n");
	abort();
}

/*************************************************************/

void ErreurEnonce(texte Erreur)
{
	OutputMessageErreur(Erreur);
	throw Erreur;
}

/*************************************************************/

void ErreurInterne(const char *Fichier, unsigned int Ligne)
{
	unsigned int i = 0, j = 0;
	char FichierNu[1024];

	while (Fichier[i]) {
		FichierNu[j++] = Fichier[i];

		if ((Fichier[i] == '/') || (Fichier[i] == '\\'))
			j = 0;

		i++;
	}

	FichierNu[j] = '\0';

	char Tampon[128];
	sprintf(Tampon, "%s (%u) %s", FichierNu, Ligne, GetTexte(MESSAGE_ERREURINTERNE, 64, false));

	OutputMessageErreur(Tampon);

	texte Exception = MESSAGE_ERREURINTERNE;

	throw Exception;
}

/*************************************************************/
