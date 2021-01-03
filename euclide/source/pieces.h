#ifndef __EUCLIDE_PIECES_H
#define __EUCLIDE_PIECES_H

#include "includes.h"

namespace Euclide
{

class Action;
class Pieces;
class Actions;
class Problem;
class Consequences;
class TwoPieceCache;
class PieceConditions;

/* -------------------------------------------------------------------------- */
/* -- Piece                                                                -- */
/* -------------------------------------------------------------------------- */

class Piece
{
	public:
		Piece(const Problem& problem, Square square, Man man, Glyph glyph = Empty, tribool promoted = unknown);
		~Piece();

		void initializeActions();

		void setCastling(CastlingSide side, bool castling);
		void setCaptured(bool captured);

		void setAvailableMoves(int availableMoves, int freeMoves);
		void setAvailableCaptures(int availableCaptures, int freeCaptures);

		void setPossibleGlyphs(Glyphs glyphs);
		void setPossibleSquares(Squares squares);
		void setPossibleCaptures(Squares captures);

		void bypassObstacles(const Piece& blocker);
		static int mutualInteractions(Piece& pieceA, Piece& pieceB, const array<int, NumColors>& freeMoves, bool fast);

		void findConsequences(const std::array<Pieces, NumColors>& pieces);

		bool update();

	public:
		inline Man man() const
			{ return m_man; }
		inline Glyph glyph() const
			{ return m_glyph ? m_glyph : m_child; }
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
		inline tribool captured(Square square) const
			{ return m_possibleSquares[square] ? m_captured : tribool(false); }
		inline tribool promoted() const
			{ return m_promoted; }

		inline const Glyphs& glyphs() const
			{ return m_glyphs; }

		inline Square initialSquare() const
			{ return m_initialSquare; }
		inline Square square() const
			{ return m_finalSquare; }
		inline Squares squares() const
			{ return m_possibleSquares; }
		inline Squares squares(Glyph glyph) const
			{ return m_pieces[glyph]->m_possibleSquares; }
		inline Squares captures() const
			{ return m_possibleCaptures; }
		inline Squares captures(Glyph glyph) const
			{ return m_pieces[glyph]->m_possibleCaptures; }
		inline Squares promotions() const
			{ return m_promotionSquares; }

		inline const Piece *piece(Glyph glyph) const
			{ return m_pieces[glyph]; }
		inline const Piece *piece() const
			{ return m_piece; }

		inline int availableMoves() const
			{ return m_availableMoves; }
		inline int availableCaptures() const
			{ return m_availableCaptures; }
		inline int requiredMoves() const
			{ return m_requiredMoves; }
		inline int requiredCaptures() const
			{ return m_requiredCaptures; }

		inline int requiredMovesTo(Square square, bool pawn = false) const
			{ return pawn ? m_pawn.distances[square] : m_distances[square]; }
		inline int requiredMovesTo(Squares squares, bool pawn = false) const
			{ return xstd::min(ValidSquares(squares), Infinity, [&](const Square square) { return pawn ? m_pawn.distances[square] : m_distances[square]; }); }
		inline int requiredMovesTo(Square square, Glyph glyph) const
			{ return m_pieces[glyph]->requiredMovesTo(square); }
		inline int requiredMovesTo(Squares squares, Glyph glyph) const
			{ return m_pieces[glyph]->requiredMovesTo(squares); }
		inline int requiredMovesFrom(Square square, bool pawn = false) const
			{ return pawn ? m_pawn.rdistances[square] : m_rdistances[square]; }
		inline int requiredMovesFrom(Square square, Glyph glyph) const
			{ return m_pieces[glyph]->requiredMovesFrom(square); }
		inline int requiredCapturesTo(Square square, bool pawn = false) const
			{ return pawn ? m_pawn.captures[square] : m_captures[square]; }
		inline int requiredCapturesTo(Squares squares, bool pawn = false) const
			{ return (pawn ? m_pawn.xmoves : m_xmoves) ? xstd::min(ValidSquares(squares), Infinity, [&](const Square square) { return pawn ? m_pawn.captures[square] : m_captures[square]; }) : 0; }
		inline int requiredCapturesTo(Square square, Glyph glyph) const
			{ return m_pieces[glyph]->requiredCapturesTo(square); }
		inline int requiredCapturesTo(Squares squares, Glyph glyph) const
			{ return m_pieces[glyph]->requiredCapturesTo(squares); }
		inline int requiresCapturesFrom(Square square, bool pawn = false) const
			{ return pawn ? m_pawn.rcaptures[square] : m_rcaptures[square]; }
		inline int requiredCapturesFrom(Square square, Glyph glyph) const
			{ return m_pieces[glyph]->m_rcaptures[square]; }

		inline int nmoves() const
			{ return m_nmoves; }

		inline const bool move(Square from, Square to, bool pawn) const
			{ return (pawn ?  m_pawn.moves : m_moves)[from][to]; }
		inline const Squares& moves(Square from, bool pawn) const
			{ return (pawn ? m_pawn.moves : m_moves)[from]; }
		inline Squares captures(Square from, bool pawn) const
			{ return (pawn ? m_pawn.xmoves : m_xmoves) ? (*(pawn ? m_pawn.xmoves : m_xmoves))[from] : Squares(); }
		inline Squares constraints(Square from, Square to, bool capture, bool pawn) const
			{ return pawn ? (capture ? (*m_pawn.xconstraints)[from][to] : (*m_pawn.constraints)[from][to]) : (capture ? (*m_xconstraints)[from][to] : (*m_constraints)[from][to]); }

		inline const Squares& stops() const
			{ return m_stops; }
		inline const Squares& route() const
			{ return m_route; }

		inline const Actions& actions() const
			{ return *m_actions; }
		const Action& action(Square from, Square to) const;
		const Consequences& consequences(Square from, Square to) const;

	public:
		inline bool operator==(const Piece& piece) const
			{ return this == &piece; }
		inline bool operator!=(const Piece& piece) const
			{ return this != &piece; }

	protected:
		void unfold();
		void summarize();
		void updateDistances();
		void updateDistancesTo();
		void updateCaptures();
		void updateCapturesTo();
		void updateConsequences();

		array<int, NumSquares> computeDistances(Square initial, Square castling, bool pawn) const;
		array<int, NumSquares> computeDistances(Squares promotions, const array<int, NumSquares>& initial) const;
		array<int, NumSquares> computeDistancesTo(Squares destinations, bool pawn) const;
		array<int, NumSquares> computeDistancesTo(Squares promotions, const array<int, NumSquares>& initial) const;
		array<int, NumSquares> computeDistancesTo(Squares destinations, const Piece& blocker, Square obstruction) const;

		array<int, NumSquares> computeCaptures(Square initial, Square castling, bool pawn) const;
		array<int, NumSquares> computeCaptures(Squares promotions, const array<int, NumSquares>& initial) const;
		array<int, NumSquares> computeCapturesTo(Squares destinations, bool pawn) const;

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

			State(Piece& piece, int availableMoves) : piece(piece), teleportation((piece.m_castlingSquare != Nowhere) && !piece.m_distances[piece.m_castlingSquare]), availableMoves(availableMoves), requiredMoves(Infinity), playedMoves(0), square(piece.m_initialSquare)
			{
				distances.fill(Infinity);
				distances[piece.m_initialSquare] = 0;
			}
		};

		static int fastplay(array<State, 2>& states, int availableMoves, TwoPieceCache& cache);
		static int fullplay(array<State, 2>& states, int availableMoves, int maximumMoves, TwoPieceCache& cache, bool *invalidate = nullptr);

	private:
		Man m_man;                                     /**< Piece's man. */
		Glyph m_child;                                 /**< Piece's initial glyph, for promoted pawns. */
		Glyph m_glyph;                                 /**< Piece's final glyph, if known. */
		Color m_color;                                 /**< Piece's color, implicit from glyph. */
		Species m_species;                             /**< Piece type. */

		bool m_royal;                                  /**< A royal piece (the king) can not be captured and may not be left in check. */

		Square m_initialSquare;                        /**< Piece's initial square, if known. */
		Squares m_promotionSquares;                    /**< Piece's possible promotion squares, if any. */
		Square m_castlingSquare;                       /**< Piece's initial square, for rooks that have castled. */
		Square m_finalSquare;                          /**< Piece's final square, if known. */

		tribool m_castling[NumCastlingSides];          /**< Set if the piece has performed castling. */
		tribool m_captured;                            /**< Set if the piece has been captured. */
		tribool m_promoted;                            /**< Set if the piece has been promoted. */

		Glyphs m_glyphs;                               /**< Piece's possible final glyphs. Includes piece's glyph. */
		std::list<Piece> m_personalities;              /**< Possible pieces. At most five entries for pawns. */
		array<Piece *, NumGlyphs> m_pieces;            /**< Pointer to pieces for each personalities. One entry must be non null. */
		Piece *m_piece;                                /**< Master piece for personalities, or 'this' if we are not a virtual piece. */
		bool m_virtual;                                /**< Set if this piece is a possible piece (a personality) which may not exist. */

		Squares m_possibleSquares;                     /**< Possible final squares of this piece. */
		Squares m_possibleCaptures;                    /**< Possible captures made by this piece. */
		int m_availableMoves;                          /**< Number of moves available for this piece. */
		int m_availableCaptures;                       /**< Number of captures available for this piece. */
		int m_requiredMoves;                           /**< Minimum number of moves required by this piece. */
		int m_requiredCaptures;                        /**< Minimum number of captures performed by this piece. */
		int m_freeMoves;                               /**< Number of free moves in available moves. */
		int m_freeCaptures;                            /**< Number of free captures in available captures. */

		array<int, NumSquares> m_distances;            /**< Number of moves required to reach each square. */
		array<int, NumSquares> m_rdistances;           /**< Number of moves required to reach one of the final squares. */
		array<int, NumSquares> m_captures;             /**< Number of captures required to reach each square. */
		array<int, NumSquares> m_rcaptures;            /**< Number of moves required to reach one of the final squares. */

		ArrayOfSquares m_moves;                        /**< Set of legal moves. */
		const ArrayOfSquares *m_xmoves;                /**< Set of moves that must be captures, or null if there are no restrictions. */
		const MatrixOfSquares *m_constraints;          /**< Move constraints, i.e. squares that must be empty for each possible move. */
		const MatrixOfSquares *m_xconstraints;         /**< Capture move constraints, i.e. squares that must be empty for each possible capture. */
		const ArrayOfSquares *m_checks;                /**< For each square, set of squares on which the enemy king is in check. */

		struct {
			array<int, NumSquares> distances;
			array<int, NumSquares> rdistances;
			array<int, NumSquares> captures;
			array<int, NumSquares> rcaptures;

			ArrayOfSquares moves;
			const ArrayOfSquares *xmoves;
			const MatrixOfSquares *constraints;
			const MatrixOfSquares *xconstraints;
			const ArrayOfSquares *checks;
		} m_pawn;                                      /**< Same as above, for initial pawn when the piece is promoted. */

		int m_nmoves;                                  /**< Total number of legal moves. */

		struct Occupied { Squares squares; array<Piece *, NumSquares> pieces; };
		array<Occupied, NumSquares> m_occupied;        /**< Occupied squares, for each square the piece may lie. */

		Squares m_stops;                               /**< Set of all squares on which the piece may have stopped. */
		Squares m_route;                               /**< Set of all squares the piece may have crossed or stopped. */
		Squares m_threats;                             /**< Set of all squares on which the enemy king is threatened. */

		Actions *m_actions;                            /**< Actions associated with possible piece moves and their consequences. */

		bool m_update;                                 /**< Set when deductions must be updated and update() shall return true. */

	public:
		mutable struct {
			Glyph glyph;                               /**< Piece current glyph, different than initial if promoted. */
			Square square;                             /**< Piece current square, NoWhere if captured. */
			int moves;                                 /**< Number of moves played. */

			int assignedMoves;                         /**< Assigned number of moves. */

		} state;                                       /**< State, used when playing possible games, stored here for performance reasons. */
};

/* -------------------------------------------------------------------------- */
/* -- Pieces                                                               -- */
/* -------------------------------------------------------------------------- */

class Pieces : public std::vector<Piece> {};

/* -------------------------------------------------------------------------- */

}

#endif
