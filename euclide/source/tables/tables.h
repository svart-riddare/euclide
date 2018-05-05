#ifndef __EUCLIDE_TABLES_H
#define __EUCLIDE_TABLES_H

#include "../includes.h"

namespace Euclide
{
namespace Tables
{

/* -------------------------------------------------------------------------- */

void initializeLegalMoves(ArrayOfSquares *moves, Species species, Color color, Variant variant, tribool capture);

const ArrayOfSquares *getCaptureMoves(Species species, Color color, Variant variant);
const MatrixOfSquares *getMoveConstraints(Species species, Variant variant, bool capture);

#if 0

/* -------------------------------------------------------------------------- */
/* -- The movements table lists all legal moves (source square, destination
      square) of a given glyph, without capture on the destination square.
      The captures table lists all legal moves of a given glyph such that
	  a piece is captured on the destination square.                       -- */
/* -------------------------------------------------------------------------- */

extern const bool movements[NumGlyphs][NumSquares][NumSquares];
extern const bool captures[NumGlyphs][NumSquares][NumSquares];

/* -------------------------------------------------------------------------- */
/* -- An obstruction list is the list of moves (source square, destination
      square) of a given glyph that are blocked if a given square is occupied.
	  Royal obstructions are valid only if the occupied square is occupied
	  by a king of the opposite color of the given glyph.
	  Check obstructions are valid only if the occupied square is occupied 
	  by an opponent that would give check to the king if placed on the 
	  source square. These obstructions apply only to kings.               -- */
/* -------------------------------------------------------------------------- */

typedef struct
{
	square_t from;
	square_t to;
	bool royal;
	bool check;

} Obstruction;

typedef struct
{
	const Obstruction *obstructions;
	int numObstructions;

} Obstructions;

extern const Obstructions obstructions[NumGlyphs][NumSquares];

/* -------------------------------------------------------------------------- */
/* -- A constraint list is the list of squares that must be free for a 
      given move (piece type, source square, destination square) to be 
      possible. The source and destination squares are not included in the 
	  list of constraints.                                                 -- */
/* -------------------------------------------------------------------------- */

typedef struct
{
	const Square *constraints;
	int numConstraints;

} Constraints;

extern const Constraints constraints[NumGlyphs][NumSquares][NumSquares];

/* -------------------------------------------------------------------------- */
/* -- A king on a given square is in check from a given glyph on a given
      square if the check may not be intercepted by another piece other than
	  by capturing the checking piece.                                     -- */
/* -------------------------------------------------------------------------- */

extern const bool checks[NumSquares][NumGlyphs][NumSquares];

/* -------------------------------------------------------------------------- */

#endif

}}

#endif
