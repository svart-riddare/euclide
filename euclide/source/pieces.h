#ifndef __EUCLIDE_PIECES_H
#define __EUCLIDE_PIECES_H

#include "includes.h"

namespace Euclide
{

class Pieces;
class Problem;
class Conditions;
class TwoPieceCache;
class PieceConditions;

/* -------------------------------------------------------------------------- */
/* -- Piece                                                                -- */
/* -------------------------------------------------------------------------- */

class Piece
{
	public:
		Piece(const Problem& problem, Square square);
		~Piece();

		void initializeConditions();

		void setCastling(CastlingSide side, bool castling);
		void setCaptured(bool captured);

		void setAvailableMoves(int availableMoves);
		void setAvailableCaptures(int availableCaptures);

		void setPossibleSquares(const Squares& squares);
		void setPossibleCaptures(const Squares& captures);

		void bypassObstacles(const Piece& blocker);
		static int mutualInteractions(Piece& pieceA, Piece& pieceB, const array<int, NumColors>& freeMoves, bool fast);

		void basicConditions(const std::array<Pieces, NumColors>& pieces);

		bool update();

	public:
		inline Glyph initialGlyph() const
			{ return m_glyph; }
		inline Glyph glyph() const
			{ return m_glyph; }
		inline Color color() const
			{ return m_color; }
		inline Species species() const
			{ return m_species; }

		inline bool royal() const
			{ return m_royal; }

		inline tribool castling(CastlingSide side) const
			{ return m_castling[side]; }
		inline tribool captured() const
			{ return m_captured; }
		inline tribool promoted() const
			{ return m_promoted; }

		inline const Glyphs& glyphs() const
			{ return m_glyphs; }

		inline Square initialSquare() const
			{ return m_initialSquare; }
		inline Square square() const
			{ return m_finalSquare; }
		inline const Squares& squares() const
			{ return m_possibleSquares; }

		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline int requiredMovesTo(Square square) const
			{ return m_distances[square]; }
		inline int requiredMovesFrom(Square square) const
			{ return m_rdistances[square]; }
		inline int requiredCapturesTo(Square square) const
			{ return m_captures[square]; }
		inline int requiresCapturesFrom(Square square) const
			{ return m_rcaptures[square]; }

		inline int nmoves() const
			{ return xstd::sum(m_moves, 0, [](Squares squares) { return squares.count(); }); }

		inline const Squares& moves(Square from) const
			{ return m_moves[from]; }
		inline Squares captures(Square from) const
			{ return m_xmoves ? (*m_xmoves)[from] : Squares(); }
		inline Squares constraints(Square from, Square to, bool capture) const
			{ return (capture && m_xconstraints) ? (*m_xconstraints)[from][to] : (*m_constraints)[from][to]; }

		inline const Squares& stops() const
			{ return m_stops; }
		inline const Squares& route() const
			{ return m_route; }

		inline const PieceConditions& conditions() const
			{ return *m_conditions; }
		const Conditions& conditions(Square from, Square to) const;

	public:
		inline bool operator==(const Piece& piece) const
			{ return this == &piece; }
		inline bool operator!=(const Piece& piece) const
			{ return this != &piece; }

	protected:
		void updateDeductions();
		void updateDistances(bool castling);

		array<int, NumSquares> computeDistances(Square square, Square castling) const;
		array<int, NumSquares> computeDistancesTo(Squares destinations) const;
		array<int, NumSquares> computeDistancesTo(Squares destinations, const Piece& blocker, Square obstruction) const;

		array<int, NumSquares> computeCaptures(Square square, Square castling) const;
		array<int, NumSquares> computeCapturesTo(Squares destinations) const;

	protected:
		struct State
		{
			Piece& piece;                        /**< Piece referenced by this state. */
			bool teleportation;                  /**< Piece can be teleported (castling rook). */

			int availableMoves;                  /**< Number of available moves for this piece. */
			int requiredMoves;                   /**< Number of moves required for this piece. */
			int playedMoves;                     /**< Number of moves currently played. */

			ArrayOfSquares moves;                /**< All moves that leads to the possible final squares in time. */
			ArrayOfSquares squares;              /**< Occupied pair of squares. */
			Square square;                       /**< Current square. */

			array<int, NumSquares> distances;    /**< Moves required to reach each square, assuming goals are reached. */

			State(Piece& piece, int availableMoves) : piece(piece), teleportation((piece.m_initialSquare != piece.m_castlingSquare) && !piece.m_distances[piece.m_castlingSquare]), availableMoves(availableMoves), requiredMoves(Infinity), playedMoves(0), square(piece.m_initialSquare)
			{
				distances.fill(Infinity);
				distances[piece.m_initialSquare] = 0;
			}
		};

		static int fastplay(array<State, 2>& states, int availableMoves, TwoPieceCache& cache);
		static int fullplay(array<State, 2>& states, int availableMoves, int maximumMoves, TwoPieceCache& cache, bool *invalidate = nullptr);

	private:
		Glyph m_glyph;                                 /**< Piece's glyph. */
		Color m_color;                                 /**< Piece's color, implicit from glyph. */
		Species m_species;                             /**< Piece type. */

		bool m_royal;                                  /**< A royal piece (the king) can not be captured and may not be left in check. */

		Square m_initialSquare;                        /**< Piece's initial square. */
		Square m_castlingSquare;                       /**< Piece's initial square, for rooks that have castled. */
		Square m_finalSquare;                          /**< Piece's final square, if known. */

		tribool m_castling[NumCastlingSides];          /**< Set if the piece has performed castling. */
		tribool m_captured;                            /**< Set if the piece has been captured. */
		tribool m_promoted;                            /**< Set if the piece has been promoted. */

		Glyphs m_glyphs;                               /**< Piece's possible glyphs after promotion. */

		Squares m_possibleSquares;                     /**< Possible final squares of this piece. */
		Squares m_possibleCaptures;                    /**< Possible captures made by this piece. */
		int m_availableMoves;                          /**< Number of moves available for this piece. */
		int m_availableCaptures;                       /**< Number of captures available for this piece. */
		int m_requiredMoves;                           /**< Minimum number of moves required by this piece. */
		int m_requiredCaptures;                        /**< Minimum number of captures performed by this piece. */

		array<int, NumSquares> m_distances;            /**< Number of moves required to reach each square. */
		array<int, NumSquares> m_rdistances;           /**< Number of moves required to reach one of the final squares. */
		array<int, NumSquares> m_captures;             /**< Number of captures required to reach each square. */
		array<int, NumSquares> m_rcaptures;            /**< Number of moves required to reach one of the final squares. */

		ArrayOfSquares m_moves;                        /**< Set of legal moves. */
		const ArrayOfSquares *m_xmoves;                /**< Set of moves that must be captures, or null if there are no restrictions. */
		const MatrixOfSquares *m_constraints;          /**< Move constraints, i.e. squares that must be empty for each possible move. */
		const MatrixOfSquares *m_xconstraints;         /**< Capture move constraints, i.e. squares that must be empty for each possible capture. */
		const ArrayOfSquares *m_checks;                /**< For each square, set of squares on which the enemy king is in check. */

		struct Occupied { Squares squares; array<Piece *, NumSquares> pieces; };
		array<Occupied, NumSquares> m_occupied;        /**< Occupied squares, for each square the piece may lie. */

		Squares m_stops;                               /**< Set of all squares on which the piece may have stopped. */
		Squares m_route;                               /**< Set of all squares the piece may have crossed or stopped. */
		Squares m_threats;                             /**< Set of all squares on which the enemy king is threatened. */

		PieceConditions *m_conditions;                 /**< Conditions associated with possible piece moves. */

		bool m_update;                                 /**< Set when deductions must be updated and update() shall return true. */

	public:
		mutable struct {
			Glyph glyph;                               /**< Piece current glyph, different than initial if promoted. */
			Square square;                             /**< Piece current square, NoWhere if captured. */
			int moves;                                 /**< Number of moves played. */

		} state;                                       /**< State, used when playing possible games, stored here for performance reasons. */
};

/* -------------------------------------------------------------------------- */
/* -- Pieces                                                               -- */
/* -------------------------------------------------------------------------- */

class Pieces : public std::vector<Piece> {};

/* -------------------------------------------------------------------------- */

}

#endif
