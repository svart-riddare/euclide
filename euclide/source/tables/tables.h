#ifndef __EUCLIDE_TABLES_H
#define __EUCLIDE_TABLES_H

#include "../includes.h"

namespace Euclide
{
namespace Tables
{

/* -------------------------------------------------------------------------- */

void initializeLegalMoves(ArrayOfSquares *moves, Species species, Color color, Variant variant, tribool capture, bool promotion);

const ArrayOfSquares *getCaptureMoves(Species species, Color color, Variant variant);
const MatrixOfSquares *getMoveConstraints(Species species, Variant variant, bool capture, bool null = true);

const ArrayOfSquares *getUnstoppableChecks(Species species, Color color, Variant variant);

void initializeLineOfSights(const array<Species, NumGlyphs>& species, Variant variant, array<MatrixOfSquares, NumColors> *lines);

/* -------------------------------------------------------------------------- */

}}

#endif
