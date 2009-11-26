#ifndef __HASHTABLES_H
#define __HASHTABLES_H

#include <limits.h>
#include "Constantes.h"
#include "Entiers.h"

/*************************************************************/

typedef struct _hashheader {
	unsigned int Strategie : 32;
	unsigned int DemiCoups : 8;

	unsigned int GrandRoqueBlanc : 1;
	unsigned int PetitRoqueBlanc : 1;
	unsigned int GrandRoqueNoir : 1;
	unsigned int PetitRoqueNoir : 1;

	unsigned int Zero : 20;
} hashheader;

typedef struct _hashdata {
	uint64_t CasesOccupees;
	uint8_t Pieces[MaxHommes];
} hashdata;

typedef struct _hashentry {
	hashheader Entete;
	hashdata Position;
} hashentry;

/*************************************************************/

void CreateHashTables(unsigned int MemoireDisponible = 0);
void InitHashTables();

void InsertPosition(const etatdujeu *Position, unsigned int Index = UINT_MAX);
bool IsPositionIn(const etatdujeu *Position, unsigned int Index = UINT_MAX);

unsigned int ComputeHashIndex(const couleurs *Couleurs, unsigned int DemiCoups);

/*************************************************************/

#endif


