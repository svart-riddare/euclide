#ifndef __EUCLIDE_PIECES_H
#define __EUCLIDE_PIECES_H

#include "includes.h"

namespace Euclide
{

class Problem;

/* -------------------------------------------------------------------------- */
/* -- Piece                                                                -- */
/* -------------------------------------------------------------------------- */

class Piece
{
	public :
		Piece(const Problem& problem, Square square);
		~Piece();

		void setCaptured(bool captured);

		void setAvailableMoves(int availableMoves);
		void setAvailableCaptures(int availableCaptures);

		void setPossibleSquares(const Squares& squares);
		void setPossibleCaptures(const Squares& captures);

		void bypassObstacles(const Squares& obstacles);
	
		bool update();

	public :
		inline Glyph glyph(bool initial = false) const
			{ return _glyph; }
		inline Color color() const
			{ return _color; }
		inline Species species() const
			{ return _species; }

		inline tribool captured() const
			{ return _captured; }
		inline tribool promoted() const
			{ return _promoted; }

		inline const Glyphs& glyphs() const
			{ return _glyphs; }

		inline Square square(bool initial = false) const
			{ return initial ? _initialSquare : _finalSquare; }
		inline const Squares& squares() const
			{ return _possibleSquares; }

		inline int requiredMoves() const
			{ return _requiredMoves; }
		inline int requiredCaptures() const
			{ return _requiredCaptures; }

		inline int moves() const
			{ return xstd::sum(_moves, 0, [](Squares squares) { return squares.count(); }); }

		inline const Squares& stops() const
			{ return _stops; }
		inline const Squares& route() const
			{ return _route; }

	public :
		inline bool operator==(const Piece& piece) const
			{ return this == &piece; }
		inline bool operator!=(const Piece& piece) const
			{ return this != &piece; }

	protected :
		void updateDeductions();
		void updatePossibleMoves();

		array<int, NumSquares> computeDistances(Square square, Square castling) const;
		array<int, NumSquares> computeDistancesTo(Squares destinations) const;

		array<int, NumSquares> computeCaptures(Square square, Square castling) const;
		array<int, NumSquares> computeCapturesTo(Squares destinations) const;

	private :
		Glyph _glyph;                                 /**< Piece's glyph. */
		Color _color;                                 /**< Piece's color, implicit from glyph. */
		Species _species;                             /**< Piece type. */

		bool _royal;                                  /**< A royal piece (the king) can not be captured and may not be left in check. */

		Square _initialSquare;                        /**< Piece's initial square. */
		Square _castlingSquare;                       /**< Piece's initial square, for rooks that have castled. */
		Square _finalSquare;                          /**< Piece's final square, if known. */

		tribool _captured;                            /**< Set if the piece has been captured. */
		tribool _promoted;                            /**< Set if the piece has been promoted. */

		Glyphs _glyphs;                               /**< Piece's possible glyphs after promotion. */

		Squares _possibleSquares;                     /**< Possible final squares of this piece. */
		Squares _possibleCaptures;                    /**< Possible captures made by this piece. */
		int _availableMoves;                          /**< Number of moves available for this piece. */
		int _availableCaptures;                       /**< Number of captures available for this piece. */
		int _requiredMoves;                           /**< Minimum number of moves required by this piece. */
		int _requiredCaptures;                        /**< Minimum number of captures performed by this piece. */

		array<int, NumSquares> _distances;            /**< Number of moves required to reach each square. */
		array<int, NumSquares> _captures;             /**< Number of captures required to reach each square. */

		ArrayOfSquares _moves;                        /**< Set of legal moves. */
		const ArrayOfSquares *_xmoves;                /**< Set of moves that must be captures, or null if there are no restrictions. */
		const MatrixOfSquares *_constraints;          /**< Move constraints, i.e. squares that must be empty for each possible move. */
		const MatrixOfSquares *_xconstraints;         /**< Capture move constraints, i.e. squares that must be empty for each possible capture. */

		Squares _stops;                               /**< Set of all squares on which the piece may have stopped. */
		Squares _route;                               /**< Set of all squares the piece may have crossed or stopped. */

		bool _update;                                 /**< Set when deductions must be updated and update() shall return true. */
};

/* -------------------------------------------------------------------------- */
/* -- Pieces                                                               -- */
/* -------------------------------------------------------------------------- */

typedef std::vector<Piece> Pieces;

/* -------------------------------------------------------------------------- */

}

#endif
