#ifndef __EUCLIDE_MEN_H
#define __EUCLIDE_MEN_H

#include "errors.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	King, Queen, QueenRook, KingRook, QueenBishop, KingBishop, QueenKnight, KingKnight,
	APawn, BPawn, CPawn, DPawn, EPawn, FPawn, GPawn, HPawn,
	
	NumMen, UndefinedMen = -1,
	FirstMan = King, LastMan = HPawn,
	FirstRook = QueenRook, LastRook = KingRook,
	FirstBishop = QueenBishop, LastBishop = KingBishop,
	FirstKnight = QueenKnight, LastKnight = KingKnight,
	FirstPawn = APawn, LastPawn = HPawn,

	AQueen = NumMen, BQueen, CQueen, DQueen, EQueen, FQueen, GQueen, HQueen,
	ARook, BRook, CRook, DRook, ERook, FRook, GRook, HRook,
	ABishop, BBishop, CBishop, DBishop, EBishop, FBishop, GBishop, HBishop,
	AKnight, BKnight, CKnight, DKnight, EKnight, FKnight, GKnight, HKnight,

	NumSupermen, UndefinedSuperman = -1,
	FirstSuperman = AQueen, LastSuperman = HKnight,
	FirstSuperQueen = AQueen, LastSuperQueen = HQueen,
	FirstSuperRook = ARook, LastSuperRook = HRook,
	FirstSuperBisop = ABishop, LastSuperBishop = HBishop,
	FirstSuperKnight = AKnight, LastSuperKnight = HKnight,
	
} Man;

/* -------------------------------------------------------------------------- */

inline bool isValidMan(Man man)
{
	return ((man >= FirstMan) && (man <= LastMan));
}

/* -------------------------------------------------------------------------- */

inline bool isManKing(Man man)
{
	assert(isValidMan(man));
	return (man == King);
}

inline bool isManQueen(Man man)
{
	assert(isValidMan(man));
	return (man == Queen);
}

inline bool isManRook(Man man)
{
	assert(isValidMan(man));
	return ((man == KingRook) || (man == QueenRook));
}

inline bool isManBishop(Man man)
{
	assert(isValidMan(man));
	return ((man == KingBishop) || (man == QueenBishop));
}

inline bool isManKnight(Man man)
{
	assert(isValidMan(man));
	return ((man == KingKnight) || (man == QueenKnight));
}

inline bool isManPawn(Man man)
{
	assert(isValidMan(man));
	return ((man >= FirstPawn) && (man <= LastPawn));
}

/* -------------------------------------------------------------------------- */

inline Man operator++(Man& man, int)
{
	Man result = man;
	man = static_cast<Man>(man + 1);
	return result;
}

inline Man operator++(Man& man)
{
	return 
	man = static_cast<Man>(man + 1);
}

/* -------------------------------------------------------------------------- */

}

#endif
