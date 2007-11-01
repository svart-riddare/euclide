#ifndef __EUCLIDE_BOARD_H
#define __EUCLIDE_BOARD_H

#include "includes.h"

namespace euclide
{

class Pieces;

/* -------------------------------------------------------------------------- */

/**
 * \class Obstructions
 * List of obstructions to movements of a given piece caused by the blocking
 * of a given square by a given figure. The figure causing the blocking does
 * not mather except when it is a king as unmaskable checks are taken into
 * accounts.
 */

/**
 * \class Movements
 * Representation of the legal/valid moves of a given piece. The list of valid
 * moves may not contain all legal moves if the solving engine finds that the
 * problem contraints prevent some legal moves from being played during the game.
 */

/**
 * \class Board
 * Set of \link Movements movements\endlink for each piece type that may be 
 * present on the board.
 */

/* -------------------------------------------------------------------------- */

class Obstructions
{
	public :
		Obstructions(Superman superman, Color color, Square square, Glyph glyph, int movements[NumSquares][NumSquares]);
		Obstructions(const Obstructions& obstructions);
		~Obstructions();

		Obstructions& operator&=(const Obstructions& obstructions);

		void block(bool soft) const;
		void unblock(bool soft) const;

		void optimize();

	public :
		int numObstructions(bool soft) const;

	private :
		int numHardObstructions;    /**< Number of obstructions, except moves by a piece of opposite color that ends on the obstruction square (thus allowing capture of the blocking figure). */
		int numSoftObstructions;    /**< Total number of obstructions. */

		int **obstructions;         /**< Obstruction list. An obstruction is represented as a pointer in the valid movements table of the blocked piece. */
};

/* -------------------------------------------------------------------------- */

class Movements
{
	public :
		Movements(Superman superman, Color color);
		~Movements();

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
		void synchronizeCastling(Movements& krook, Movements& qrook);

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
		int movements[NumSquares][NumSquares];                /**< Valid movements. Zero when a move is valid, positive if the move is not possible (blocked), infinity if the move is not legal or possible given the problem to solve. */
		int possibilities;                                    /**< Number of valid movements in legal/valid movements table. */

		Superman superman;                                    /**< Piece type. */
		Glyph glyph;                                          /**< Piece's figure. */
		Color color;                                          /**< Piece's color. */

		Square initial;                                       /**< Piece's initial square (promotion square for promoted pieces). */
		Square ksquare;                                       /**< Piece's initial square after performing king side castling. */
		Square qsquare;                                       /**< Piece's initial square after performing queen side castling. */
		int castling;                                         /**< Number of moves required for castling (1 for king, 0 for rooks). */

		tribool kcastling;                                    /**< Has piece performed king side castling? */
		tribool qcastling;                                    /**< Has piece performed queen side castling ? */

		bool hybrid;                                          /**< Hybrid pieces are pieces that captures differently than they move, like pawns. */

		int distances[NumSquares];                            /**< Precomputed number of moves required to reach each square from the initial square (castling taken into account). */
		int _captures[NumSquares];                            /**< Precomputed number of captures required to reach each square from the initial square. */

		Squares _squares;                                     /**< List of squares that can be reached by the moving piece. */

		Obstructions *obstructions[NumSquares][NumGlyphs];    /**< Obstruction tables, one for each blocked square for each figure type. */
		Glyphs validObstructions;                             /**< If true for a given figure, the obstrution table if unique for that figure; otherwise it points on the generic obstruction table. */
};

/* -------------------------------------------------------------------------- */

class Board
{
	public :
		Board();
		~Board();

		int moves() const;
		int moves(Color color) const;
		int moves(Superman superman, Color color) const;
		int moves(Man man, Superman superman, Color color) const;

		int distance(Man man, Superman superman, Color color, Square from, Square to) const;
		int distance(Man man, Superman superman, Color color, Square to) const;

		int captures(Man man, Superman superman, Color color, Square from, Square to) const;
		int captures(Man man, Superman superman, Color color, Square to) const;

		int getCaptures(Glyph glyph, Square from, Square to, vector<Squares>& captures) const;
		int getCaptures(Man man, Superman superman, Color color, Square from, Square to, vector<Squares>& captures) const;

		bool getUniquePath(Man man, Superman superman, Color color, Square to, vector<Square>& squares) const;

		void block(Superman superman, Color color, Square square, bool captured = false);
		void unblock(Superman superman, Color color, Square square, bool captured = false);
		void transblock(Superman superman, Color color, Square from, Square to, bool captured = false);
		void block(Man man, Superman superman, Color color);
		void block(Man man, Superman superman, Color color, const Squares& squares);

		void reduce(Man man, Superman superman, Color color, Square square, int availableMoves, int availableCaptures);
		void reduce(Man man, Superman superman, Color color, const Squares& squares, int availableMoves, int availableCaptures);
		void reduce(Man man, const Supermen& supermen, Color color, const Squares& squares, int availableMoves, int availableCaptures);
		
		void setCaptureSquares(Man man, Superman superman, Color color, const Squares& squares);
		
		void optimizeLevelOne(const Pieces& pieces, Color color, int availableMoves, int availableCaptures);
		void optimizeLevelTwo(const Pieces& whitePieces, const Pieces& blackPieces);
		void optimize(Color color);

	private :
		Movements *movements[NumColors][NumSupermen];    /**< Movement descriptors for each piece type. */
		bool optimized;                                  /**< True when all movement tables have been optimized and distances precomputed. */
};

/* -------------------------------------------------------------------------- */

}

#endif
