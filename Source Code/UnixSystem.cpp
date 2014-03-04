#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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
	static char Chemin[512];
	char Process[64];

	memset(Chemin, 0, sizeof(Chemin));
	sprintf(Process, "/proc/%d/exe", (int)getpid());
	if (readlink(Process, Chemin, sizeof(Chemin) - 1) < 0)
		return Fichier;

	char *Executable = strrchr(Chemin, '/');
	if (!Executable)
		return Fichier;

	strncpy(Executable + 1, Fichier, sizeof(Chemin) - (Executable - Chemin) - 2);
	return Chemin;
}

/*************************************************************/
