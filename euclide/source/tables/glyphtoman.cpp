#include <cstddef>

#include "enumerations.h"
#include "constants.h"
#include "tables.h"

namespace euclide
{
namespace tables
{

/* --------------------------------------------------------------------------
   -- Computer generated with make-tables on Sat Jul 08 23:40:04 2006
   -------------------------------------------------------------------------- */

const bool validGlyphManColor[NumGlyphs][NumMen][NumColors] =
{
	{  /* NoGlyph */
		{ false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, 
	},
	{  /* WhiteKing */
		{  true, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, 
	},
	{  /* WhiteQueen */
		{ false, false }, {  true, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, 
	},
	{  /* WhiteRook */
		{ false, false }, { false, false }, {  true, false }, {  true, false }, { false, false }, { false, false }, { false, false }, { false, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, 
	},
	{  /* WhiteBishop */
		{ false, false }, { false, false }, { false, false }, { false, false }, {  true, false }, {  true, false }, { false, false }, { false, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, 
	},
	{  /* WhiteKnight */
		{ false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, 
	},
	{  /* WhitePawn */
		{ false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, {  true, false }, 
	},
	{  /* BlackKing */
		{ false,  true }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, 
	},
	{  /* BlackQueen */
		{ false, false }, { false,  true }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, 
	},
	{  /* BlackRook */
		{ false, false }, { false, false }, { false,  true }, { false,  true }, { false, false }, { false, false }, { false, false }, { false, false }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, 
	},
	{  /* BlackBishop */
		{ false, false }, { false, false }, { false, false }, { false, false }, { false,  true }, { false,  true }, { false, false }, { false, false }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, 
	},
	{  /* BlackKnight */
		{ false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, 
	},
	{  /* BlackPawn */
		{ false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false, false }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, { false,  true }, 
	},

};

/* -------------------------------------------------------------------------- */

const Man glyphToMan[NumGlyphs][NumColumns] =
{
	{  /* NoGlyph */
		UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, 
	},
	{  /* WhiteKing */
		UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, 
	},
	{  /* WhiteQueen */
		AQueen, BQueen, CQueen, DQueen, EQueen, FQueen, GQueen, HQueen, 
	},
	{  /* WhiteRook */
		ARook, BRook, CRook, DRook, ERook, FRook, GRook, HRook, 
	},
	{  /* WhiteBishop */
		ABishop, BBishop, CBishop, DBishop, EBishop, FBishop, GBishop, HBishop, 
	},
	{  /* WhiteKnight */
		AKnight, BKnight, CKnight, DKnight, EKnight, FKnight, GKnight, HKnight, 
	},
	{  /* WhitePawn */
		UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, 
	},
	{  /* BlackKing */
		UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, 
	},
	{  /* BlackQueen */
		AQueen, BQueen, CQueen, DQueen, EQueen, FQueen, GQueen, HQueen, 
	},
	{  /* BlackRook */
		ARook, BRook, CRook, DRook, ERook, FRook, GRook, HRook, 
	},
	{  /* BlackBishop */
		ABishop, BBishop, CBishop, DBishop, EBishop, FBishop, GBishop, HBishop, 
	},
	{  /* BlackKnight */
		AKnight, BKnight, CKnight, DKnight, EKnight, FKnight, GKnight, HKnight, 
	},
	{  /* BlackPawn */
		UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, UndefinedSuperman, 
	},

};

/* -------------------------------------------------------------------------- */

}}
