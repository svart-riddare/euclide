#include <mach/mach_host.h>
#include <math.h>
#include "System.h"

/*************************************************************/

unsigned int MemoireDisponible()
{
	host_basic_info Info;
	mach_msg_type_number_t Taille = sizeof(Info);
	
	if (host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&Info, &Taille) != 0)
		return 0;
	
	return log(Info.memory_size / 2) / log(2);
}

/*************************************************************/

const char *CheminAlternatif(const char *Fichier)
{
	return Fichier;
}

/*************************************************************/
