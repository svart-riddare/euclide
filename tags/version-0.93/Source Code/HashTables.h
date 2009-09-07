#ifndef __HASHTABLES_H
#define __HASHTABLES_H

#include <limits.h>
#include "Constantes.h"

/*************************************************************/

typedef struct _hashdata {
	unsigned __int8 Position[MaxCases / 2];
} hashdata;

typedef struct _hashtag {
	unsigned int DemiCoups : 3;
	bool GrandRoqueBlanc : 1;
	bool PetitRoqueBlanc : 1;
	bool GrandRoqueNoir : 1;
	bool PetitRoqueNoir : 1;
	bool Valide : 1;
} hashtag;

/*************************************************************/

void InitHashTables();

void InsertPosition(const etatdujeu *Position, unsigned int Index = UINT_MAX);
bool IsPositionIn(const etatdujeu *Position, unsigned int Index = UINT_MAX);

unsigned int ComputeHashIndex(const couleurs *Couleurs, unsigned int DemiCoups);

/*************************************************************/

#endif


