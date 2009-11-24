#include <string.h>
#include "Erreur.h"
#include "HashTables.h"
#include "Partie.h"

/*************************************************************/

static unsigned int BITSFORMOVES = 2;
static unsigned int HASHBITS = 2;
static unsigned int HASHSLOTS = 1 << HASHBITS;
static unsigned int HASHSIZE = (1 << 16) * (1 << BITSFORMOVES) * (1 << HASHBITS);

static hashentry *HashTables = NULL;

/*************************************************************/

void PositionToHashEntry(const etatdujeu *Position, hashentry *HashEntry);

/*************************************************************/

void CreateHashTables(unsigned int MemoireDisponible)
{
	if (HashTables)
		return;

	if (MemoireDisponible < 25)
		MemoireDisponible = 25;

	if (MemoireDisponible > 30)
		MemoireDisponible = 30;

	BITSFORMOVES = (MemoireDisponible - 21) / 2;
	HASHBITS = (MemoireDisponible - 20) / 2;
	HASHSLOTS = 1 << HASHBITS;
	HASHSIZE = 1 << (MemoireDisponible - 5);

	if (!HashTables)
		HashTables = new hashentry[HASHSIZE];

	if (!HashTables && (MemoireDisponible > 25))
		CreateHashTables(MemoireDisponible - 1);
}

/*************************************************************/

void InitHashTables()
{
	CreateHashTables();

	if (HashTables)
		memset(HashTables, 0, HASHSIZE * sizeof(hashentry));
}

/*************************************************************/

void InsertPosition(const etatdujeu *Position, unsigned int Index)
{
	if (!HashTables)
		return;

	if (Index >= UINT_MAX)
		Index = ComputeHashIndex(Position->Couleurs, Position->DemiCoups);

	for (unsigned int Slot = 0; Slot < HASHSLOTS - 1; Slot++, Index++)
		if (HashTables[Index].Entete.Strategie != Position->Strategie)
			break;

	PositionToHashEntry(Position, &HashTables[Index]);
}

/*************************************************************/

bool IsPositionIn(const etatdujeu *Position, unsigned int Index)
{
	hashentry HashEntry;

	if (!HashTables)
		return false;

	if (Index >= UINT_MAX)
		Index = ComputeHashIndex(Position->Couleurs, Position->DemiCoups);

	if (HashTables[Index].Entete.Strategie == Position->Strategie) {
		PositionToHashEntry(Position, &HashEntry);
	
		for (unsigned int Slot = 0; Slot < HASHSLOTS; Slot++) {
			if (HashTables[Index + Slot].Entete.Strategie != Position->Strategie)
				return false;

			if (memcmp(&HashTables[Index + Slot], &HashEntry, sizeof(hashentry)) == 0)
				return true;
		}
	}

	return false;
}

/*************************************************************/

void PositionToHashEntry(const etatdujeu *Position, hashentry *HashEntry)
{
	const pieces *Pieces = Position->Pieces;
	const couleurs *Couleurs = Position->Couleurs;
	unsigned int NombreDePieces = 0;

	HashEntry->Entete.Strategie = Position->Strategie;
	HashEntry->Entete.DemiCoups = Position->DemiCoups;
	HashEntry->Entete.GrandRoqueBlanc = Position->GrandRoqueBlancPossible;
	HashEntry->Entete.PetitRoqueBlanc = Position->PetitRoqueBlancPossible;
	HashEntry->Entete.GrandRoqueNoir = Position->GrandRoqueNoirPossible;
	HashEntry->Entete.PetitRoqueNoir = Position->PetitRoqueNoirPossible;
	HashEntry->Entete.Zero = 0;

	HashEntry->Position.CasesOccupees = 0;
	for (unsigned int i = 0; i < MaxCases; i++) {
		if (Pieces[i] != VIDE) {
			HashEntry->Position.CasesOccupees |= ((unsigned __int64)1 << i);

			if (NombreDePieces < MaxHommes)
				HashEntry->Position.Pieces[NombreDePieces++] = (Pieces[i] << 1) | (Couleurs[i] - 1);
			else
				HashEntry->Position.Pieces[NombreDePieces++ % MaxHommes] |= ((Pieces[i] << 1) | (Couleurs[i] - 1)) << 4;
		}
	}

	for (unsigned int i = NombreDePieces; i < MaxHommes; i++)
		HashEntry->Position.Pieces[i] = 0;
}

/*************************************************************/

unsigned int ComputeHashIndex(const couleurs *Couleurs, unsigned int DemiCoups)
{
	unsigned int HashIndex = (DemiCoups & ((1 << BITSFORMOVES) - 1)) << 16;
	unsigned int ValeurA, ValeurB, ValeurC, ValeurD, Valeur;

	ValeurA = Couleurs[A1] | (Couleurs[A1] >> 1);
	ValeurB = Couleurs[B3] | (Couleurs[B3] >> 1);
	ValeurC = Couleurs[D5] | (Couleurs[D5] >> 1);
	ValeurD = Couleurs[H7] | (Couleurs[H7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 0);

	ValeurA = Couleurs[A2] | (Couleurs[A2] >> 1);
	ValeurB = Couleurs[C4] | (Couleurs[C4] >> 1);
	ValeurC = Couleurs[A6] | (Couleurs[A6] >> 1);
	ValeurD = Couleurs[A7] | (Couleurs[A7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 1);

	ValeurA = Couleurs[C1] | (Couleurs[C1] >> 1);
	ValeurB = Couleurs[C3] | (Couleurs[C3] >> 1);
	ValeurC = Couleurs[H6] | (Couleurs[H6] >> 1);
	ValeurD = Couleurs[E7] | (Couleurs[E7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 2);

	ValeurA = Couleurs[D1] | (Couleurs[D1] >> 1);
	ValeurB = Couleurs[A3] | (Couleurs[A3] >> 1);
	ValeurC = Couleurs[A5] | (Couleurs[A5] >> 1);
	ValeurD = Couleurs[C7] | (Couleurs[C7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 3);

	ValeurA = Couleurs[E1] | (Couleurs[E1] >> 1);
	ValeurB = Couleurs[H3] | (Couleurs[H3] >> 1);
	ValeurC = Couleurs[B6] | (Couleurs[B6] >> 1);
	ValeurD = Couleurs[F7] | (Couleurs[F7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 4);

	ValeurA = Couleurs[F1] | (Couleurs[F1] >> 1);
	ValeurB = Couleurs[F3] | (Couleurs[F3] >> 1);
	ValeurC = Couleurs[E5] | (Couleurs[E5] >> 1);
	ValeurD = Couleurs[G7] | (Couleurs[G7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 5);

	ValeurA = Couleurs[G1] | (Couleurs[G1] >> 1);
	ValeurB = Couleurs[H4] | (Couleurs[H4] >> 1);
	ValeurC = Couleurs[B5] | (Couleurs[B5] >> 1);
	ValeurD = Couleurs[D7] | (Couleurs[D7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 6);

	ValeurA = Couleurs[H1] | (Couleurs[H1] >> 1);
	ValeurB = Couleurs[D3] | (Couleurs[D3] >> 1);
	ValeurC = Couleurs[C5] | (Couleurs[C5] >> 1);
	ValeurD = Couleurs[B7] | (Couleurs[B7] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 7);

	ValeurA = Couleurs[A2] | (Couleurs[A2] >> 1);
	ValeurB = Couleurs[B4] | (Couleurs[B4] >> 1);
	ValeurC = Couleurs[E6] | (Couleurs[E6] >> 1);
	ValeurD = Couleurs[A8] | (Couleurs[A8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 8);

	ValeurA = Couleurs[B2] | (Couleurs[B2] >> 1);
	ValeurB = Couleurs[E4] | (Couleurs[E4] >> 1);
	ValeurC = Couleurs[D6] | (Couleurs[D6] >> 1);
	ValeurD = Couleurs[B8] | (Couleurs[B8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 9);

	ValeurA = Couleurs[C2] | (Couleurs[C2] >> 1);
	ValeurB = Couleurs[E3] | (Couleurs[E3] >> 1);
	ValeurC = Couleurs[H5] | (Couleurs[H5] >> 1);
	ValeurD = Couleurs[C8] | (Couleurs[C8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 10);

	ValeurA = Couleurs[D2] | (Couleurs[D2] >> 1);
	ValeurB = Couleurs[F4] | (Couleurs[F4] >> 1);
	ValeurC = Couleurs[C6] | (Couleurs[C6] >> 1);
	ValeurD = Couleurs[D8] | (Couleurs[D8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 11);

	ValeurA = Couleurs[E2] | (Couleurs[E2] >> 1);
	ValeurB = Couleurs[D4] | (Couleurs[D4] >> 1);
	ValeurC = Couleurs[F6] | (Couleurs[F6] >> 1);
	ValeurD = Couleurs[E8] | (Couleurs[E8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 12);

	ValeurA = Couleurs[F2] | (Couleurs[F2] >> 1);
	ValeurB = Couleurs[G4] | (Couleurs[G4] >> 1);
	ValeurC = Couleurs[G6] | (Couleurs[G6] >> 1);
	ValeurD = Couleurs[F8] | (Couleurs[F8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 13);

	ValeurA = Couleurs[G2] | (Couleurs[G2] >> 1);
	ValeurB = Couleurs[A4] | (Couleurs[A4] >> 1);
	ValeurC = Couleurs[G5] | (Couleurs[G5] >> 1);
	ValeurD = Couleurs[G8] | (Couleurs[G8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 14);

	ValeurA = Couleurs[H2] | (Couleurs[H2] >> 1);
	ValeurB = Couleurs[G3] | (Couleurs[G3] >> 1);
	ValeurC = Couleurs[F5] | (Couleurs[F5] >> 1);
	ValeurD = Couleurs[H8] | (Couleurs[H8] >> 1);
	Valeur = ValeurA ^ ValeurB ^ ValeurC ^ ValeurD;
	HashIndex |= ((Valeur & 1) << 15);

	HashIndex = HashIndex << HASHBITS;
	Verifier(HashIndex < HASHSIZE);

	return HashIndex;
}

/*************************************************************/


