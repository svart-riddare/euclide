#ifndef __EUCLIDE_TARGETS_H
#define __EUCLIDE_TARGETS_H

#include "includes.h"
#include "destinations.h"

namespace euclide
{

class Pieces;
class Capture;

/* -------------------------------------------------------------------------- */

/**
 * \class Target
 * A target is a square that must be attained by a piece to reach the 
 * diagram position. At initialization, the destination square is known for
 * all figures on the diagram and unknown for missing (captured) figures.
 * Likewise, at initialization, the piece that fullfills a target is not
 * necessarily known. The target is hence a list of \link Destination
 * destinations\endlink that is refined as solving is performed.
 * The ultimate goal is to reduce this list to a single destination.
 */

/**
 * \class Targets
 * Target list.
 */

/* -------------------------------------------------------------------------- */

class Target : public Destinations
{
	public :
		Target(Glyph glyph, Square square);
		Target(Color color, const Squares& squares);

		int computeRequiredMoves(const Board& board);
		int computeRequiredCaptures(const Board& board);

		int updateRequiredMoves();
		int updateRequiredCaptures();
		const Men& updatePossibleMen();
		const Squares& updatePossibleSquares();
		const Supermen& updatePossibleSupermen();

		bool setPossibleMen(const Men& men);
		bool setPossibleSquares(const Squares& squares);
		bool setAvailableMoves(int availableMoves);
		bool setAvailableCaptures(int availableCaptures);
		bool setAvailableMoves(const array<int, NumMen>& availableMoves);
		bool setAvailableCaptures(const array<int, NumMen>& availableCaptures);

	public :
		bool operator==(const Target& target) const;

	public :
		inline bool alive() const
			{ return (_glyph != NoGlyph); }

	public :
		inline Man man() const
			{ return _man; }
		inline Glyph glyph() const
			{ return _glyph; }
		inline Color color() const
			{ return _color; }
		inline Square square() const
			{ return _square; }
		inline Superman superman() const
			{ return _superman; }

		inline const Squares& squares() const
			{ return _squares; }
		inline const Men& men() const
			{ return _men; }
		inline const Supermen& supermen() const
			{ return _supermen; }
		inline int candidates() const
			{ return (int)_men.count(); }

		inline bool isMan(Man man) const
			{ return _men[man]; }
		inline bool isSquare(Square square) const
			{ return _squares[square]; }
		inline bool isSuperman(Superman superman) const
			{ return _supermen[superman]; }

	public :
		inline int getRequiredMoves() const
			{ return requiredMoves; }
		inline int getRequiredCaptures() const
			{ return requiredCaptures; }

		inline int getRequiredMoves(Man man) const
			{ return menRequiredMoves[man]; }
		inline int getRequiredCaptures(Man man) const
			{ return menRequiredCaptures[man]; }

	private :
		Man _man;                                  /**< Piece that fullfills the target, if known. */
		Glyph _glyph;                              /**< Target figure, if not captured. */
		Color _color;                              /**< Color to which the target belongs. */
		Square _square;                            /**< Target destination square, if known. */
		Superman _superman;                        /**< Promotion type of piece that fullfills the target, if known. */

		Men _men;                                  /**< Pieces that may fullfill this target. */
		Squares _squares;                          /**< Possible destination squares for this target. */
		Supermen _supermen;                        /**< Promotion pieces involved by pieces that may fullfill this target. */

	private :
		int requiredMoves;                         /**< Required moves for this target. */
		int requiredCaptures;                      /**< Required captures for this target. */

		array<int, NumMen> menRequiredMoves;       /**< Required moves, by piece type, to fullfill this target. */
		array<int, NumMen> menRequiredCaptures;    /**< Required captures, by piece type, to fullfill this target. */

	private :
		vector_ptr<Capture> captures;
};

/* -------------------------------------------------------------------------- */

typedef vector_ptr<Target> Targets;

/* -------------------------------------------------------------------------- */

}

#endif
