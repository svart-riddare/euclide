#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <math.h>
#include "System.h"

/*************************************************************/

unsigned int MemoireDisponible()
{
	MEMORYSTATUSEX memory;
	memory.dwLength = (DWORD)sizeof(memory);
	if (!GlobalMemoryStatusEx(&memory))
		return 0;

	double MemoirePhysiqueDisponible = log((double)memory.ullAvailPhys) / log(2.0); 
	double MemoireVirtuelleDisponible = log((double)memory.ullAvailVirtual) / log(2.0);

	if ((MemoirePhysiqueDisponible < 1) || (MemoireVirtuelleDisponible < 1))
		return 0;

	unsigned int MemoirePhysique = (unsigned int)(MemoirePhysiqueDisponible - 0.5);
	unsigned int MemoireVirtuelle = (unsigned int)(MemoireVirtuelleDisponible - 0.5);

	return min(MemoirePhysique, MemoireVirtuelle);
}

/*************************************************************/

const char *CheminAlternatif(const char *Fichier)
{
	static char Chemin[MAX_PATH];
	ZeroMemory(Chemin, sizeof(Chemin));
	GetModuleFileName(NULL, Chemin, sizeof(Chemin) / sizeof(Chemin[0]) - 1);

	char *Executable = PathFindFileName(Chemin);
	if (!Executable)
		return Fichier;

	StringCchCopy(Executable, sizeof(Chemin) / sizeof(Chemin[0]) - (Executable - Chemin), Fichier);
	return Chemin;
}

/*************************************************************/
