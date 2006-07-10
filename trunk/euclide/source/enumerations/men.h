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
	
	NumMen, UndefinedMan = -1,
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
	FirstSuperBishop = ABishop, LastSuperBishop = HBishop,
	FirstSuperKnight = AKnight, LastSuperKnight = HKnight,
	
} man_t;

/* -------------------------------------------------------------------------- */

class Man 
{
	public :
		inline Man() {}
		inline Man(man_t man)
			{ this->man = man; }

		/* ---------------------------------- */

		inline operator man_t() const
			{ return man; }		
		inline operator man_t&()
			{ return man; }

		/* ---------------------------------- */

		inline Man operator++(int)
			{ return (man_t)((int&)man)++; }
		inline Man operator--(int)
			{ return (man_t)((int&)man)--; }

		inline Man& operator++()
			{ ++(int&)man; return *this; }
		inline Man& operator--()
			{ --(int&)man; return *this; }
		
		/* ---------------------------------- */

		bool isValid() const
		{
			return ((man >= FirstMan) && (man <= LastSuperman));
		}

		/* ---------------------------------- */

		bool isKing() const
		{
			return (man == King);
		}

		bool isQueen()
		{
			return (man == Queen);
		}

		bool isRook()
		{
			return ((man == QueenRook) || (man == KingRook));
		}

		bool isBishop()
		{
			return ((man == QueenBishop) || (man == KingBishop));
		}

		bool isKnight()
		{
			return ((man == QueenKnight) || (man == QueenKnight));
		}

		bool isPawn()
		{
			return ((man >= FirstPawn) || (man >= LastPawn));
		}

		/* ---------------------------------- */

	private :
		man_t man;
};

/* -------------------------------------------------------------------------- */

}

#endif
