#ifndef __EUCLIDE_MEN_H
#define __EUCLIDE_MEN_H

#include "errors.h"
#include "glyphs.h"
#include "squares.h"

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

		Glyph glyph(Color color) const
		{
			static const Glyph glyphs[NumMen][NumColors] =
			{
				{ WhiteKing, BlackKing },
				{ WhiteQueen, BlackQueen },
				{ WhiteRook, BlackRook }, { WhiteRook, BlackRook },
				{ WhiteBishop, BlackBishop },{ WhiteBishop, BlackBishop },
				{ WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight },
				{ WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }
			};

			assert(color.isValid());
			return glyphs[man][color];
		}

		/* ---------------------------------- */

		Square square(Color color) const
		{
			static const Square squares[NumMen][NumColors] =
			{
				{ E1, E8 }, { D1, D8 }, { A1, A8 }, { H1, H8 }, { C1, C8 }, { F1, F8 }, { B1, B8 }, { G1, G8 },
				{ A2, A7 }, { B2, B7 }, { C2, C7 }, { D2, D7 }, { E2, E7 }, { F2, F7 }, { G2, G7 }, { H2, H7 }
			};

			assert(color.isValid());
			return squares[man][color];
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
			assert(superman <= (superman_t)LastMan);
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

		Glyph glyph(Color color) const
		{
			static const Glyph glyphs[NumSupermen][NumColors] =
			{
				{ WhiteKing, BlackKing },
				{ WhiteQueen, BlackQueen },
				{ WhiteRook, BlackRook }, { WhiteRook, BlackRook },
				{ WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop },
				{ WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight },
				{ WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn }, { WhitePawn, BlackPawn },
				{ WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen }, { WhiteQueen, BlackQueen },
				{ WhiteRook, BlackRook }, { WhiteRook, BlackRook }, { WhiteRook, BlackRook }, { WhiteRook, BlackRook }, { WhiteRook, BlackRook }, { WhiteRook, BlackRook }, { WhiteRook, BlackRook }, { WhiteRook, BlackRook },
				{ WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop }, { WhiteBishop, BlackBishop },
				{ WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }, { WhiteKnight, BlackKnight }
			};

			assert(color.isValid());
			return glyphs[superman][color];
		}

		/* ---------------------------------- */

		Square square(Color color) const
		{
			static const Square squares[NumSupermen][NumColors] =
			{
				{ E1, E8 }, { D1, D8 }, { A1, A8 }, { H1, H8 }, { C1, C8 }, { F1, F8 }, { B1, B8 }, { G1, G8 },
				{ A2, A7 }, { B2, B7 }, { C2, C7 }, { D2, D7 }, { E2, E7 }, { F2, F7 }, { G2, G7 }, { H2, H7 },
				{ A8, A1 }, { B8, B1 }, { C8, C1 }, { D8, D1 }, { E8, E1 }, { F8, F1 }, { G8, G1 }, { H8, H1 },
				{ A8, A1 }, { B8, B1 }, { C8, C1 }, { D8, D1 }, { E8, E1 }, { F8, F1 }, { G8, G1 }, { H8, H1 },
				{ A8, A1 }, { B8, B1 }, { C8, C1 }, { D8, D1 }, { E8, E1 }, { F8, F1 }, { G8, G1 }, { H8, H1 },
				{ A8, A1 }, { B8, B1 }, { C8, C1 }, { D8, D1 }, { E8, E1 }, { F8, F1 }, { G8, G1 }, { H8, H1 }
			};

			assert(color.isValid());
			return squares[superman][color];
		}

		/* ---------------------------------- */

		bool isPromoted() const
		{
			return ((superman >= FirstPromotedMan) && (superman <= LastPromotedMan));
		}

		/* ---------------------------------- */

		bool isKing() const
		{
			return (superman == (superman_t)King);
		}

		bool isQueen()
		{
			return ((superman == (superman_t)Queen) || ((superman >= FirstPromotedQueen) && (superman <= LastPromotedQueen)));
		}

		bool isRook()
		{
			return (((superman >= (superman_t)FirstRook) && (superman <= (superman_t)LastRook)) || ((superman >= FirstPromotedRook) && (superman <= LastPromotedRook)));
		}

		bool isBishop()
		{
			return (((superman >= (superman_t)FirstBishop) && (superman <= (superman_t)LastBishop)) || ((superman >= FirstPromotedBishop) && (superman <= LastPromotedBishop)));
		}

		bool isKnight()
		{
			return (((superman >= (superman_t)FirstKnight) && (superman <= (superman_t)LastKnight)) || ((superman >= FirstPromotedKnight) && (superman <= LastPromotedKnight)));
		}

		bool isPawn()
		{
			return ((superman >= (superman_t)FirstPawn) && (superman <= (superman_t)LastPawn));
		}

		/* ---------------------------------- */

	private :
		superman_t superman;
};

/* -------------------------------------------------------------------------- */

}

#endif
