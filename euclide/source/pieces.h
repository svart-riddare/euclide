#ifndef __EUCLIDE_PIECES_H
#define __EUCLIDE_PIECES_H

#include "includes.h"
#include "moves.h"
#include "hashtables.h"

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
		Piece(Superman superman, Color color, int moves);
		~Piece();

		int distance(Square square) const;
		int captures(Square square) const;
		int rdistance(Square square) const;

		int distance(const Squares& squares) const;
		int captures(const Squares& squares) const;

		int distance(Square from, Square to) const;
		int captures(Square from, Square to) const;

		int getCaptures(Square from, Square to, vector<Squares>& captures) const;
		int getMandatoryMoves(Moves& moves, bool incomplete) /*const*/;
		int getMovesFrom(Square square, Moves& moves) const;
		int getMovesTo(Square square, Moves& moves) const;

		void block(Squares squares, Glyph glyph, bool definitive = true);
		void block(Square square, Glyph glyph, bool captured, bool definitive = false);
		void unblock(Square square, Glyph glyph, bool captured);

		void setPossibleSquares(const Squares& squares, tribool captured, int availableMoves, int availableCaptures);
		void setPossibleCaptures(const Squares& captures);
		void setMandatoryMoveConstraints(const Piece& piece, const Moves& moves);
		bool setMutualObstructions(Piece& piece, int availableMoves, int assignedMoves, int *requiredMoves = NULL, int *requiredMovesA = NULL, int *requiredMovesB = NULL, bool fast = true);
		
		void synchronizeCastling(Piece& krook, Piece& qrook);
		void optimizeCastling();

		void optimize();
		bool constrain();

	public :
		inline Man man() const
			{ return _superman.man(); }
		inline Superman superman() const
			{ return _superman; }
		inline Glyph glyph() const
			{ return _glyph; }
		inline Color color() const
			{ return _color; }

	public :
		inline int earliest() const
			{ return _earliest; }
		inline int latest() const
			{ return _latest; }

	public :
		inline int moves() const
			{ return (int)_moves.size(); }
		inline const Squares& squares() const
			{ return _squares; }

		inline const Move *move(Square from, Square to) const
			{ return from.isValid() ? to.isValid() ? &_movements[from][to] : &_imovements[from][0] : &_imovements[to][1]; }
		inline const Move *move(int move) const
			{ return _moves[move]; }

		tribool alive(bool final = true) const;

	protected :
		void computeInitialDistances();
		void computeInitialCaptures();

		void updateInitialDistances();
		void updateInitialCaptures();

		void updateInitialDistances(const int distances[NumSquares], bool reverse = false);
		void updateInitialCaptures(const int captures[NumSquares], bool reverse = false);

		void computeForwardDistances(Square square, int distances[NumSquares], bool castling = true) const;
		void computeForwardCaptures(Square square, int captures[NumSquares], bool castling = true) const;

		void computeReverseDistances(Square square, int distances[NumSquares]) const;
		void computeReverseCaptures(Square square, int captures[NumSquares]) const;

		void computeReverseDistances(const Squares& squares, int distances[NumSquares]) const;
		void computeReverseCaptures(const Squares& squares, int captures[NumSquares]) const;

		bool mayLeave(Square square) const;
		bool mayReach(Square square) const;

		void findMandatoryMoves();
		int findMutualObstructions(Piece *pieces[2], Square squares[2], int nmoves[2], Moves& moves, MiniHash& processed, int availableMoves, int assignedMoves, int maximumMoves, int rmoves[2]);

	private :
		static inline bool isMovePossible(const Move *move)
			{ return move->possible(); }
		static inline bool isMoveImpossible(const Move *move)
			{ return !move->possible(); }
		static inline bool isMoveEarlier(const Move *moveA, const Move *moveB, const Piece& piece)
			{ return piece.distance(moveA->from()) < piece.distance(moveB->from()); }

	private :
		Move _movements[NumSquares][NumSquares];               /**< All piece movements, one for each departure and arrival square. */
		Move _imovements[NumSquares][2];                       /**< Incomplete movemvents, one for each departure and arrival square. */
		Moves _moves;                                          /**< Possible piece movements. */

		Superman _superman;                                    /**< Piece type. */
		Glyph _glyph;                                          /**< Piece's figure. */
		Color _color;                                          /**< Piece's color. */

		Square _initial;                                       /**< Piece's initial square (promotion square for promoted pieces). */
		Square _xinitial;                                      /**< Piece's alternative initial square (for rooks which have performed castling). */

		Move *_kcastling;                                      /**< King side castling move (kings only). */
		Move *_qcastling;                                      /**< Queen side castling move (kings only). */

		tribool _teleported;                                   /**< Has piece been teleported (for rooks which have performed castling). */
		tribool _captured;                                     /**< Has piece been captured ? */
		tribool _promoted;                                     /**< Has piece been promoted ? */

		bool _hybrid;                                          /**< Hybrid pieces are pieces that captures differently than they move, like pawns. */

		int _distances[NumSquares];                            /**< Precomputed number of moves required to reach each square from the initial square (castling taken into account). */
		int _captures[NumSquares];                             /**< Precomputed number of captures required to reach each square from the initial square. */

		int _rdistances[NumSquares];                           /**< Precomputed number of moves required to reach any possible final destination square from given square (castling taken into account). */
		int _rcaptures[NumSquares];                            /**< Precomputed number of captures required to reach any possible final destination square from given square (castling taken into account). */

		Squares _squares;                                      /**< List of squares that can be reached by the moving piece. */

		Squares _possibleSquares;                              /**< List of possible final squares for this piece. */
		Squares _possibleCaptures;                             /**< List of possible squares for captures made by this piece. */
		int _availableMoves;                                   /**< Number of available moves for this piece. */
		int _availableCaptures;                                /**< Number of available captures for this piece. */

		int _earliest;                                         /**< Earliest move for this piece, counting from 1. May be undefined if piece has not moved. */
		int _latest;                                           /**< Latest move for this piece, counting from 1. May be undefined if piece has not moved. */

		bool _optimized;                                       /**< True if all movements, distances and other state variables are up to date. */

		Obstructions *_obstructions[NumSquares][NumGlyphs];    /**< Obstruction tables, one for each blocked square for each figure type. */
		Glyphs _validObstructions;                             /**< If true for a given figure, the obstrution table if unique for that figure; otherwise it points on the generic obstruction table. */
};

/* -------------------------------------------------------------------------- */

}

#endif
