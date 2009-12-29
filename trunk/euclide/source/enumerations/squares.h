#ifndef __EUCLIDE_SQUARES_H
#define __EUCLIDE_SQUARES_H

#include "errors.h"
#include "colors.h"

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

} square_t;

typedef enum
{
	A, B, C, D, E, F, G, H,

	NumColumns, UndefinedColumn = -1,
	FirstColumn = A, LastColumn = H,

} column_t;

typedef enum
{
	One, Two, Three, Four, Five, Fiw, Seven, Eight,

	NumRows, UndefinedRow = -1,
	FirstRow = One, LastRow = Eight,

} row_t;

/* -------------------------------------------------------------------------- */

class Column
{
	public :
		inline Column() {}
		inline Column(column_t column)
			{ this->column = column; }

		/* ---------------------------------- */

		inline operator column_t() const
			{ return column; }		
		inline operator column_t&()
			{ return column; }

		/* ---------------------------------- */

		inline Column operator++(int)
			{ return (column_t)((int&)column)++; }
		inline Column operator--(int)
			{ return (column_t)((int&)column)--; }

		inline Column& operator++()
			{ ++(int&)column; return *this; }
		inline Column& operator--()
			{ --(int&)column; return *this; }
		
		/* ---------------------------------- */

		bool isValid() const
		{
			return ((column >= FirstColumn) && (column <= LastColumn));
		}

	private :
		column_t column;
};

/* -------------------------------------------------------------------------- */

class Row
{
	public :
		inline Row() {}
		inline Row(row_t row)
			{ this->row = row; }

		/* ---------------------------------- */

		inline operator row_t() const
			{ return row; }		
		inline operator row_t&()
			{ return row; }

		/* ---------------------------------- */

		inline Row operator++(int)
			{ return (row_t)((int&)row)++; }
		inline Row operator--(int)
			{ return (row_t)((int&)row)--; }

		inline Row& operator++()
			{ ++(int&)row; return *this; }
		inline Row& operator--()
			{ --(int&)row; return *this; }
		
		/* ---------------------------------- */

		bool isValid() const
		{
			return ((row >= FirstRow) && (row <= LastRow));
		}

	private :
		row_t row;
};

/* -------------------------------------------------------------------------- */

class Square 
{
	public :
		inline Square() {}
		inline Square(square_t square)
			{ this->square = square; }
		inline Square(column_t column, row_t row)
			{ this->square = (square_t)(column * 8 + row); }

		/* ---------------------------------- */

		inline operator square_t() const
			{ return square; }		
		inline operator square_t&()
			{ return square; }

		/* ---------------------------------- */

		inline Square operator++(int)
			{ return (square_t)((int&)square)++; }
		inline Square operator--(int)
			{ return (square_t)((int&)square)--; }

		inline Square& operator++()
			{ ++(int&)square; return *this; }
		inline Square& operator--()
			{ --(int&)square; return *this; }
		
		/* ---------------------------------- */

		Square operator-() const
		{
			assert(isValid());
			return (square_t)(square ^ 7);
		}

		/* ---------------------------------- */

		bool isValid() const
		{
			return ((square >= FirstSquare) && (square <= LastSquare));
		}

		/* ---------------------------------- */

		inline column_t column() const
		{
			assert(isValid());
			return (column_t)(square / 8);
		}

		inline row_t row() const
		{
			assert(isValid());
			return (row_t)(square % 8);
		}

		/* ---------------------------------- */

		inline bool isPromotion(Color color)
		{
			assert(isValid());
			assert(color.isValid());
			
			return (row() == ((color == White) ? Eight : One));
		}

		/* ---------------------------------- */

	private :
		square_t square;
};

/* -------------------------------------------------------------------------- */

}

#endif
