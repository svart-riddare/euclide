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

const ArrayOfSquares *getUnstoppableChecks(Species species, Color color, Variant variant);

/* -------------------------------------------------------------------------- */

}}

#endif
