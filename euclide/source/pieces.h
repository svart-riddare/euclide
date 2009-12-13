#ifndef __EUCLIDE_PIECES_H
#define __EUCLIDE_PIECES_H

#include "includes.h"

namespace euclide
{

class Obstructions;

/* -------------------------------------------------------------------------- */

/**
 * \class Piece
 * Representation of the legal/valid moves of a given piece. The list of valid
 * moves may not contain all legal moves if the solving engine finds that the
 * problem contraints prevent some legal moves from being played during the game.
 */

/* -------------------------------------------------------------------------- */

class Piece
{
	public :
		Piece(Superman superman, Color color);
		~Piece();

		int distance(Square square) const;
		int captures(Square square) const;

		int distance(const Squares& squares) const;
		int captures(const Squares& squares) const;

		int distance(Square from, Square to) const;
		int captures(Square from, Square to) const;

		int getCaptures(Square from, Square to, vector<Squares>& captures) const;
		bool getUniquePath(Square from, Square to, vector<Square>& squares) const;

		void block(Squares squares, Glyph glyph);
		void block(Square square, Glyph glyph, bool captured);
		void unblock(Square square, Glyph glyph, bool captured);

		void reduce(Square square, int availableMoves, int availableCaptures);
		void reduce(const Squares& squares, int availableMoves, int availableCaptures);
		
		void setCaptureSquares(const Squares& squares);
		void synchronizeCastling(Piece& krook, Piece& qrook);

		void optimize();
		void optimize(const vector<tuple<Man, Color, vector<Square> > >& paths);

		bool obstrusive(const vector<Square>& squares, Glyph glyph) const;

	public :
		int moves() const;
		const Squares& squares() const;

	protected :
		void computeInitialDistances();
		void computeInitialCaptures();

		void updateInitialDistances();
		void updateInitialCaptures();

		void updateInitialDistances(const int distances[NumSquares]);
		void updateInitialCaptures(const int captures[NumSquares]);

		void computeForwardDistances(Square square, int distances[NumSquares]) const;
		void computeForwardCaptures(Square square, int captures[NumSquares]) const;

		void computeForwardDistances(Square square, const vector<Square>& obstructions, Glyph glyph, int distances[NumSquares]);

		void computeReverseDistances(Square square, int distances[NumSquares]) const;
		void computeReverseCaptures(Square square, int captures[NumSquares]) const;

		bool mayLeave(Square square) const;
		bool mayReach(Square square) const;

	private :
		int _movements[NumSquares][NumSquares];                /**< Valid movements. Zero when a move is valid, positive if the move is not possible (blocked), infinity if the move is not legal or possible given the problem to solve. */
		int _possibilities;                                    /**< Number of valid movements in legal/valid movements table. */

		Superman _superman;                                    /**< Piece type. */
		Glyph _glyph;                                          /**< Piece's figure. */
		Color _color;                                          /**< Piece's color. */

		Square _initial;                                       /**< Piece's initial square (promotion square for promoted pieces). */
		Square _ksquare;                                       /**< Piece's initial square after performing king side castling. */
		Square _qsquare;                                       /**< Piece's initial square after performing queen side castling. */
		int _castling;                                         /**< Number of moves required for castling (1 for king, 0 for rooks). */

		tribool _kcastling;                                    /**< Has piece performed king side castling ? */
		tribool _qcastling;                                    /**< Has piece performed queen side castling ? */

		bool _hybrid;                                          /**< Hybrid pieces are pieces that captures differently than they move, like pawns. */

		int _distances[NumSquares];                            /**< Precomputed number of moves required to reach each square from the initial square (castling taken into account). */
		int _captures[NumSquares];                             /**< Precomputed number of captures required to reach each square from the initial square. */

		Squares _squares;                                      /**< List of squares that can be reached by the moving piece. */

		Obstructions *_obstructions[NumSquares][NumGlyphs];    /**< Obstruction tables, one for each blocked square for each figure type. */
		Glyphs _validObstructions;                             /**< If true for a given figure, the obstrution table if unique for that figure; otherwise it points on the generic obstruction table. */
};

/* -------------------------------------------------------------------------- */

}

#endif
