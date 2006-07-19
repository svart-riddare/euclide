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
	FirstPawn = APawn, LastPawn = HPawn

} man_t;

typedef enum
{
	AQueen = NumMen, BQueen, CQueen, DQueen, EQueen, FQueen, GQueen, HQueen,
	ARook, BRook, CRook, DRook, ERook, FRook, GRook, HRook,
	ABishop, BBishop, CBishop, DBishop, EBishop, FBishop, GBishop, HBishop,
	AKnight, BKnight, CKnight, DKnight, EKnight, FKnight, GKnight, HKnight,

	NumSupermen, UndefinedSuperman = -1,
	FirstSuperman = King, LastSuperman = HKnight,
	
	FirstPromotedMan = AQueen, LastPromotedMan = HKnight,
	FirstPromotedQueen = AQueen, LastPromotedQueen = HQueen,
	FirstPromotedRook = ARook, LastPromotedRook = HRook,
	FirstPromotedBishop = ABishop, LastPromotedBishop = HBishop,
	FirstPromotedKnight = AKnight, LastPromotedKnight = HKnight,
	
} superman_t;

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
			return ((man >= FirstMan) && (man <= LastMan));
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
			return ((man >= FirstPawn) && (man <= LastPawn));
		}

		/* ---------------------------------- */

	private :
		man_t man;
};

/* -------------------------------------------------------------------------- */

class Superman
{
	public :
		inline Superman() {}
		inline Superman(man_t man)
			{ superman = (superman_t)man; }
		inline Superman(Man man)
			{ superman = (superman_t)(man_t)man; }
		inline Superman(superman_t superman)
			{ this->superman = superman; }

		/* ---------------------------------- */

		inline operator superman_t() const
			{ return superman; }		
		inline operator superman_t&()
			{ return superman; }

		/* ---------------------------------- */

		inline operator Man() const
		{ 
			assert(superman <= LastMan); 
			return (man_t)superman;
		}

		/* ---------------------------------- */

		inline Superman operator++(int)
			{ return (superman_t)((int&)superman)++; }
		inline Superman operator--(int)
			{ return (superman_t)((int&)superman)--; }

		inline Superman& operator++()
			{ ++(int&)superman; return *this; }
		inline Superman& operator--()
			{ --(int&)superman; return *this; }
		
		/* ---------------------------------- */

		bool isValid() const
		{
			return ((superman >= FirstSuperman) && (superman <= LastSuperman));
		}

		/* ---------------------------------- */

		bool isPromoted() const
		{
			return ((superman >= FirstPromotedMan) && (superman <= LastPromotedMan));
		}

		/* ---------------------------------- */

		bool isKing() const
		{
			return (superman == King);
		}

		bool isQueen()
		{
			return ((superman == Queen) || ((superman >= FirstPromotedQueen) && (superman <= LastPromotedQueen)));
		}

		bool isRook()
		{
			return (((superman >= FirstRook) && (superman <= LastRook)) || ((superman >= FirstPromotedRook) && (superman <= LastPromotedRook)));
		}

		bool isBishop()
		{
			return (((superman >= FirstBishop) && (superman <= LastBishop)) || ((superman >= FirstPromotedBishop) && (superman <= LastPromotedBishop)));
		}

		bool isKnight()
		{
			return (((superman >= FirstKnight) && (superman <= LastKnight)) || ((superman >= FirstPromotedKnight) && (superman <= LastPromotedKnight)));
		}

		bool isPawn()
		{
			return ((superman >= FirstPawn) && (superman <= LastPawn));
		}

		/* ---------------------------------- */

	private :
		superman_t superman;
};

/* -------------------------------------------------------------------------- */

}

#endif
