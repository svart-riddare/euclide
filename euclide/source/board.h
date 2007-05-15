#ifndef __EUCLIDE_BOARD_H
#define __EUCLIDE_BOARD_H

#include "includes.h"

namespace euclide
{

class Pieces;

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

	private :
		int numHardObstructions;
		int numSoftObstructions;

		int **obstructions;
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

		void block(Squares squares, Glyph glyph);
		void block(const vector<Squares>& xsquares);
		void block(Square square, Glyph glyph, bool captured);
		void unblock(Square square, Glyph glyph, bool captured);

		void reduce(Square square, int availableMoves, int availableCaptures);
		void reduce(const Squares& squares, int availableMoves, int availableCaptures);
		void reduce(const vector<Squares>& xsquares, Square destination, int availableMoves, int availableCaptures);
		void reduceCaptures(const Squares& captures);
		void reduceCastling(Movements& krook, Movements& qrook);

		void optimize();

	public :
		int moves() const;
		const Squares& squares() const;

	protected :
		void computeInitialDistances();
		void computeInitialCaptures();

		void computeForwardDistances(Square square, int distances[NumSquares]) const;
		void computeForwardCaptures(Square square, int captures[NumSquares]) const;

		void computeReverseDistances(Square square, int distances[NumSquares]) const;
		void computeReverseCaptures(Square square, int captures[NumSquares]) const;

		bool mayLeave(Square square) const;
		bool mayReach(Square square) const;

	private :
		int movements[NumSquares][NumSquares];
		int possibilities;

		Superman superman;
		Glyph glyph;
		Color color;

		Square initial;
		Square ksquare;
		Square qsquare;
		int castling;

		tribool kcastling;
		tribool qcastling;

		bool hybrid;

		int distances[NumSquares];
		int _captures[NumSquares];

		Squares _squares;

		Obstructions *obstructions[NumSquares][NumGlyphs];
		Glyphs validObstructions;
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

		void block(Superman superman, Color color, Square square, bool captured = false);
		void unblock(Superman superman, Color color, Square square, bool captured = false);
		void transblock(Superman superman, Color color, Square from, Square to, bool captured = false);
		void block(Man man, Superman superman, Color color);
		void block(Man man, Superman superman, Color color, const Squares& squares);

		void reduce(Man man, Superman superman, Color color, Square square, int availableMoves, int availableCaptures);
		void reduce(Man man, Superman superman, Color color, const Squares& squares, int availableMoves, int availableCaptures);
		void reduce(Man man, const Supermen& supermen, Color color, const Squares& squares, int availableMoves, int availableCaptures);
		void reduceCaptures(Man man, Superman superman, Color color, const Squares& captures);
		
		void optimize(const Pieces& pieces, Color color, int availableMoves, int availableCaptures, const vector<Squares> *xsquares = NULL);
		void optimize(const Pieces& whitePieces, const Pieces& blackPieces, int availableWhiteMoves, int availableBlackMoves, int availableWhiteCaptures, int availableBlackCaptures);
		void optimize();

	private :
		Movements *movements[NumColors][NumSupermen];
		bool optimized;
};

/* -------------------------------------------------------------------------- */

}

#endif
