#ifndef __EUCLIDE_SQUARES_H
#define __EUCLIDE_SQUARES_H

#include "errors.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	A1, A2, A3, A4, A5, A6, A7, A8,
	B1, B2, B3, B4, B5, B6, B7, B8,
	C1, C2, C3, C4, C5, C6, C7, C8,
	D1, D2, D3, D4, D5, D6, D7, D8,
	E1, E2, E3, E4, E5, E6, E7, E8,
	F1, F2, F3, F4, F5, F6, F7, F8,
	G1, G2, G3, G4, G5, G6, G7, G8,
	H1, H2, H3, H4, H5, H6, H7, H8,

	NumSquares, UndefinedSquare = -1,
	FirstSquare = A1, LastSquare = H8,

} Square;

typedef enum
{
	A, B, C, D, E, F, G, H,

	NumColumns, UndefinedColumn = -1,
	FirstColumn = A, LastColumn = H,

} Column;

typedef enum
{
	One, Two, Three, Four, Five, Fiw, Seven, Eight,

	NumRows, UndefinedRow = -1,
	FirstRow = One, LastRow = Eight,

} Row;

/* -------------------------------------------------------------------------- */

inline bool isValidSquare(Square square)
{
	return ((square >= FirstSquare) && (square <= LastSquare));
}

inline bool isValidColumn(Column column)
{
	return ((column >= FirstColumn) && (column <= LastColumn));
}

inline bool isValidRow(Row row)
{
	return ((row >= FirstRow) && (row <= LastRow));
}

/* -------------------------------------------------------------------------- */

inline Square operator++(Square& square, int)
{
	Square result = square;
	square = static_cast<Square>(square + 1);
	return result;
}

inline Square operator++(Square& square)
{
	return 
	square = static_cast<Square>(square + 1);
}

inline Column operator++(Column& column, int)
{
	Column result = column;
	column = static_cast<Column>(column + 1);
	return result;
}

inline Column operator++(Column& column)
{
	return
	column = static_cast<Column>(column + 1);
}

inline Row operator++(Row& row, int)
{
	Row result = row;
	row = static_cast<Row>(row + 1);
	return result;
}

inline Row operator++(Row& row)
{
	return
	row = static_cast<Row>(row + 1);
}

/* -------------------------------------------------------------------------- */

inline Column squareColumn(Square square)
{
	assert(isValidSquare(square));

	return (Column)(square / 8);
}

inline Row squareRow(Square square)
{
	assert(isValidSquare(square));

	return (Row)(square % 8);
}

inline Square mksquare(Column column, Row row)
{
	assert(isValidColumn(column));
	assert(isValidRow(row));

	return (Square)(column * 8 + row);
}

/* -------------------------------------------------------------------------- */

}

#endif
