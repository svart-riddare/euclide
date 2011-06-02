#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "System.h"

/*************************************************************/

unsigned int MemoireDisponible()
{
	FILE *Fichier = fopen("/proc/meminfo", "r");

	unsigned int Memoire = 0;
	char Ligne[128];

	if (Fichier) {
		while (fgets(Ligne, 128, Fichier))
			if (sscanf(Ligne, "MemFree: %u kB", &Memoire) == 1)
				break;

		fclose(Fichier);
	}

	if (Memoire <= 0)
		return 0;

	return 10 + log(Memoire) / log(2);
}

/*************************************************************/

const char *CheminAlternatif(const char *Fichier)
{
	return Fichier;
}

/*************************************************************/
