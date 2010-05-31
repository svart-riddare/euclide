#include <windows.h>
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
