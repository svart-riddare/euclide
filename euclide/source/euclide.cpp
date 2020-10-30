#include "includes.h"
#include "captures.h"
#include "problem.h"
#include "targets.h"
#include "pieces.h"
#include "game.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Euclide implementation                                               -- */
/* -------------------------------------------------------------------------- */

class Euclide
{
	public:
		Euclide(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks);
		~Euclide();

		void solve(const EUCLIDE_Problem& problem);
		int update(std::vector<Piece *>& pieces);

	protected:
		void reset();

		const EUCLIDE_Deductions& deductions() const;

	private:
		EUCLIDE_Configuration m_configuration;      /**< Global configuration. */
		EUCLIDE_Callbacks m_callbacks;              /**< User defined callbacks. */

		Problem m_problem;                          /**< Current problem to solve. */

		array<Pieces, NumColors> m_pieces;          /**< Deductions on pieces. */
		array<Targets, NumColors> m_targets;        /**< Targets that must be fullfilled. */
		array<Captures, NumColors> m_captures;      /**< Captures that must be realized. */

		array<int, NumColors> m_freeMoves;          /**< Unassigned moves. */
		array<int, NumColors> m_freeCaptures;       /**< Unassigned captures. */

		struct Tandem { const Piece& pieceA; const Piece& pieceB; int requiredMoves; Tandem(const Piece& pieceA, const Piece& pieceB, int requiredMoves) : pieceA(pieceA), pieceB(pieceB), requiredMoves(requiredMoves) {}};
		std::vector<Tandem> m_tandems;              /**< Required moves for pair of pieces. */

	private:
		mutable EUCLIDE_Deductions m_deductions;    /**< Temporary variable to hold deductions for corresponding user callback. */
};

/* -------------------------------------------------------------------------- */

Euclide::Euclide(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks)
	: m_configuration(configuration), m_callbacks(callbacks)
{
	/* -- Display copyright string -- */

	if (m_callbacks.displayCopyright)
		(*m_callbacks.displayCopyright)(m_callbacks.handle, Copyright);
}

/* -------------------------------------------------------------------------- */

Euclide::~Euclide()
{
}

/* -------------------------------------------------------------------------- */

void Euclide::solve(const EUCLIDE_Problem& problem)
{
	/* -- Display problem -- */

	if (m_callbacks.displayProblem)
		(*m_callbacks.displayProblem)(m_callbacks.handle, &problem);

	/* -- Reset solving state -- */

	m_problem = problem;
	reset();

	/* -- Display analysis message -- */

	if (m_callbacks.displayMessage)
		(*m_callbacks.displayMessage)(m_callbacks.handle, EUCLIDE_MESSAGE_ANALYZING);

	/* -- Initialize pieces -- */

	for (Color color : AllColors())
		m_pieces[color].reserve(m_problem.initialPieces(color));

	for (Glyph glyph : MostGlyphs())
		for (Square square : AllSquares())
			if (m_problem.initialPosition(square) == glyph)
				m_pieces[color(glyph)].emplace_back(m_problem, square);

	/* -- Initialize targets and captures -- */

	array<Men, NumColors> masks;
	for (Color color : AllColors())
		masks[color] = Men::mask(m_problem.initialPieces(color));

	for (Color color : AllColors())
		m_targets[color].reserve(m_problem.diagramPieces(color));

	for (Glyph glyph : MostGlyphs())
		for (Square square : AllSquares())
			if (m_problem.diagramPosition(square) == glyph)
				m_targets[color(glyph)].emplace_back(glyph, square, masks[color(glyph)]);

	for (Color color : AllColors())
		m_captures[color].assign(m_problem.capturedPieces(color), Capture(color, masks[color], masks[!color]));

	/* -- Local array to store pieces -- */

	std::vector<Piece *> pieces;
	pieces.reserve(2 * MaxPieces);
	for (Color color : AllColors())
		for (Piece& piece : m_pieces[color])
			pieces.push_back(&piece);

	/* -- Castling pieces -- */

	typedef struct { Piece *king, *rook; } CastlingPieces;
	CastlingPieces castlingPieces[NumColors][NumCastlingSides] = {
		{ { nullptr, nullptr }, { nullptr, nullptr } } , { { nullptr, nullptr }, { nullptr, nullptr } } };

	for (Color color : AllColors())
		for (CastlingSide side : AllCastlingSides())
			for (Piece& piece : m_pieces[color])
				if (maybe(piece.castling(side)))
					if (piece.species() == King)
						castlingPieces[color][side].king = &piece;
					else
						castlingPieces[color][side].rook = &piece;

	/* -- Repeat the following deductions until there is no improvements -- */

	bool conditions = false;

	for (bool loop = true; loop; )
	{
		/* -- Compute free moves and captures -- */

		for (Color color : AllColors())
		{
			m_freeMoves[color] = m_problem.moves(color) - xstd::sum(m_pieces[color], [](const Piece& piece) { return piece.requiredMoves(); });
			m_freeCaptures[color] = m_problem.capturedPieces(!color) - xstd::sum(m_pieces[color], [](const Piece& piece) { return piece.requiredCaptures(); });

			if ((m_freeMoves[color] < 0) || (m_freeCaptures[color] < 0))
				throw NoSolution;
		}

		/* -- Display current deductions -- */

		if (m_callbacks.displayDeductions)
			(*m_callbacks.displayDeductions)(m_callbacks.handle, &deductions());

		/* -- Assign free moves, free captures and possible squares -- */

		for (Color color : AllColors())
		{
			bool analyse = true;
			while (analyse)
			{
				Captures& captures = m_captures[color];
				Targets& targets = m_targets[color];
				Pieces& pieces = m_pieces[color];
				Pieces& xpieces = m_pieces[!color];

				/* -- Assign moves and captures to targets -- */

				do
				{
					for (Target& target : targets)
					{
						const Men men([&](Man man) { return pieces[man].glyphs()[target.glyph()] && pieces[man].squares(target.glyph())[target.square()] && maybe(!pieces[man].captured()); }, target.men().range());
						target.updatePossibleMen(men);

						target.updateRequiredMoves(xstd::min(men.in(pieces), 0, [&](const Piece& piece) { return std::max(piece.requiredMovesTo(target.square(), target.glyph()), piece.requiredMoves()); }));
						target.updateRequiredCaptures(xstd::min(men.in(pieces), 0, [&](const Piece& piece) { return std::max(piece.requiredCapturesTo(target.square(), target.glyph()), piece.requiredCaptures()); }));
					}

					for (Capture& capture : captures)
					{
						for (bool update = true; update; )
						{
							const Men men([&](Man man) { return (pieces[man].glyphs() & capture.glyphs()) && (pieces[man].squares() & capture.squares()) && maybe(pieces[man].captured()); }, capture.men().range());
							const Men xmen([&](Man xman) { return xpieces[xman].availableMoves() && (xpieces[xman].stops() & capture.squares()); }, capture.xmen().range());
							capture.updatePossibleMen(men, xmen);

							const Squares squares = xstd::merge(men.in(pieces), Squares(), [](const Piece& piece) { return piece.squares(); });
							const Squares xsquares = xstd::merge(xmen.in(xpieces), Squares(), [](const Piece& xpiece) { return xpiece.stops(); });
							update = capture.updatePossibleSquares(squares & xsquares);

						}

						capture.updateRequiredMoves(xstd::min(capture.men().in(pieces), 0, [&](const Piece& piece) { return std::max(piece.requiredMovesTo(capture.squares()), piece.requiredMoves()); }));
						capture.updateRequiredCaptures(xstd::min(capture.men().in(pieces), 0, [&](const Piece& piece) { return std::max(piece.requiredCapturesTo(capture.squares()), piece.requiredCaptures()); }));
					}

				} while (targets.update());

				/* -- Merge targets into partitions -- */

				TargetPartitions partitions(pieces, m_targets[color]);

				/* -- Update possible glyphs and squares -- */

				for (const TargetPartition& partition : partitions)
					for (Man man : ValidMen(partition.men()))
						if (!maybe(pieces[man].captured()))
							pieces[man].setPossibleGlyphs(partition.glyphs());

				for (const TargetPartition& partition : partitions)
					if (partition.men().count() <= partition.squares().count())
						for (Man man : ValidMen(partition.men()))
							pieces[man].setCaptured(false);

				for (const TargetPartition& partition : partitions)
					for (Man man : ValidMen(partition.men()))
						if (!maybe(pieces[man].captured()))
							pieces[man].setPossibleSquares(partition.squares());

				for (const Capture& capture : captures)
					if (capture.man() >= 0)
						pieces[capture.man()].setCaptured(true);

				for (const Capture& capture : captures)
					if (capture.man() >= 0)
						pieces[capture.man()].setPossibleSquares(capture.squares());

				/* -- Assign moves and captures -- */

				int freeMoves = m_freeMoves[color] - partitions.unassignedRequiredMoves();
				array<int, MaxPieces> unassignedRequiredMoves;
				for (Man man = 0; man < int(pieces.size()); man++)
					unassignedRequiredMoves[man] = partitions.unassignedRequiredMoves(man);

				int freeCaptures = m_freeCaptures[color] - partitions.unassignedRequiredCaptures();
				array<int, MaxPieces> unassignedRequiredCaptures;
				for (Man man = 0; man < int(pieces.size()); man++)
					unassignedRequiredCaptures[man] = partitions.unassignedRequiredCaptures(man);

				for (const Tandem& tandem : m_tandems)
				{
					if ((tandem.pieceA.color() == color) && (tandem.pieceB.color() == color))
					{
						const Man manA = std::distance(pieces.data(), const_cast<Piece *>(&tandem.pieceA));
						const Man manB = std::distance(pieces.data(), const_cast<Piece *>(&tandem.pieceB));
						const int moves = tandem.requiredMoves - tandem.pieceA.requiredMoves() - tandem.pieceB.requiredMoves();

						if (!unassignedRequiredMoves[manA] && !unassignedRequiredMoves[manB])
							freeMoves -= moves;

						xstd::maximize(unassignedRequiredMoves[manA], moves);
						xstd::maximize(unassignedRequiredMoves[manB], moves);
					}
				}

				/* -- Update available moves and captures for all pieces -- */

				for (Piece& piece : pieces)
				{
					const Man man = std::distance(pieces.data(), &piece);

					piece.setAvailableMoves(piece.requiredMoves() + unassignedRequiredMoves[man] + freeMoves, freeMoves);
					piece.setAvailableCaptures(piece.requiredCaptures() + unassignedRequiredCaptures[man] + freeCaptures, freeCaptures);
				}

				/* -- Split partitions if possible and loop if we did -- */

				analyse = false;
				for (const TargetPartition& partition : partitions)
					if (partition.disjoint(pieces, freeMoves, freeCaptures, targets))
						analyse = true;
			}
		}

		/* -- Ensure castling is coherent -- */

		for (Color color : AllColors())
		{
			for (CastlingSide side : AllCastlingSides())
			{
				Piece *king = castlingPieces[color][side].king;
				Piece *rook = castlingPieces[color][side].rook;

				if (king && rook)
				{
					if (!unknown(king->castling(side)))
						rook->setCastling(side, king->castling(side));
					if (!unknown(rook->castling(side)))
						king->setCastling(side, rook->castling(side));
				}
				else
				{
					if (king)
						king->setCastling(side, false);
					if (rook)
						rook->setCastling(side, false);
				}
			}
		}

		/* -- Update pieces -- */

		update(pieces);

		/* -- Sort pieces by number of moves -- */

		xstd::sort(pieces, [](const Piece *pieceA, const Piece *pieceB) { return pieceA->nmoves() < pieceB->nmoves(); });

		/* -- Apply basic obstructions -- */

		const int stopThreshold = 8;

		for (Piece *blocker : pieces)
			if (!maybe(blocker->captured()))
				if (blocker->stops().count() < stopThreshold)
					for (Piece *piece : pieces)
						if (piece != blocker)
							piece->bypassObstacles(*blocker);

		/* -- Update pieces -- */

		if (update(pieces))
			continue;

		/* -- Mutual obstructions between two pieces -- */

		m_tandems.clear();
		for (unsigned pieceA = 0; pieceA < pieces.size(); pieceA++)
		{
			for (unsigned pieceB = pieceA + 1; pieceB < pieces.size(); pieceB++)
			{
				const int requiredMoves = Piece::mutualInteractions(*pieces[pieceA], *pieces[pieceB], m_freeMoves, false);
				if (requiredMoves > pieces[pieceA]->requiredMoves() + pieces[pieceB]->requiredMoves())
					m_tandems.emplace_back(*pieces[pieceA], *pieces[pieceB], requiredMoves);
			}
		}

		/* -- Update pieces -- */

		if (update(pieces))
			continue;

		/* -- Create list of moves and related conditions -- */

		if (!conditions)
		{
			for (Color color : AllColors())
				for (Piece& piece : m_pieces[color])
					piece.initializeConditions();

			if (!m_problem.capturedPieces(White) && !m_problem.capturedPieces(Black))
				for (Color color : AllColors())
					for (Piece& piece : m_pieces[color])
						piece.basicConditions(m_pieces);

			conditions = true;
			//continue;
		}

		/* -- Done -- */

		loop = false;
	}

	/* -- Display final deductions -- */

	if (m_callbacks.displayDeductions)
		(*m_callbacks.displayDeductions)(m_callbacks.handle, &deductions());

	/* -- Display playing message -- */

	if (m_callbacks.displayMessage)
		(*m_callbacks.displayMessage)(m_callbacks.handle, EUCLIDE_MESSAGE_SEARCHING);

	/* -- Play all possible games -- */

	std::unique_ptr<Game> game(new Game(m_configuration, m_callbacks, m_problem, m_pieces, m_freeMoves));
	game->play();
}

/* -------------------------------------------------------------------------- */

int Euclide::update(std::vector<Piece *>& pieces)
{
	unsigned updated = 0;

	for (unsigned piece = 0; piece < pieces.size(); piece++)
		if (pieces[piece]->update())
			std::swap(pieces[piece], pieces[updated++]);

	return updated;
}

/* -------------------------------------------------------------------------- */

void Euclide::reset()
{
	for (Color color : AllColors())
		m_pieces[color].clear();
}

/* -------------------------------------------------------------------------- */

const EUCLIDE_Deductions& Euclide::deductions() const
{
	memset(&m_deductions, 0, sizeof(m_deductions));

	const int pieces[NumColors] = {
		m_deductions.numWhitePieces = std::min<int>(m_pieces[White].size(), countof(m_deductions.whitePieces)),
		m_deductions.numBlackPieces = std::min<int>(m_pieces[Black].size(), countof(m_deductions.blackPieces))
	};

	m_deductions.freeWhiteMoves = m_freeMoves[White];
	m_deductions.freeBlackMoves = m_freeMoves[Black];

	if ((m_deductions.freeWhiteMoves < 0) || (m_deductions.freeBlackMoves < 0))
		throw NoSolution;

	m_deductions.complexity += std::log(1.0 + m_deductions.freeWhiteMoves);
	m_deductions.complexity += std::log(1.0 + m_deductions.freeBlackMoves);

	for (Color color : AllColors())
	{
		for (int k = 0; k < pieces[color]; k++)
		{
			EUCLIDE_Deduction& deduction = color ? m_deductions.blackPieces[k] : m_deductions.whitePieces[k];
			const Piece& piece = m_pieces[color][k];

			deduction.initial.glyph = static_cast<EUCLIDE_Glyph>(m_problem.initialPosition(piece.initialSquare()));
			deduction.initial.square = static_cast<int>(piece.initialSquare());

			deduction.final.glyph = static_cast<EUCLIDE_Glyph>(piece.glyph());
			deduction.final.square = static_cast<int>(piece.square());

			deduction.promoted = is(piece.promoted());
			deduction.promotion.glyph = deduction.final.glyph;
			deduction.promotion.square = static_cast<int>((piece.promotions().count() == 1) ? piece.promotions().first() : Nowhere);

			deduction.captured = is(piece.captured());
			const auto capture = xstd::find_if(m_captures[color], [=](const Capture& capture) { return (capture.man() == k) && (capture.xman() >= 0); });
			deduction.capturer.glyph = static_cast<EUCLIDE_Glyph>((capture != m_captures[color].end()) ? m_problem.initialPosition(m_pieces[!color][capture->xman()].initialSquare()) : Empty);
			deduction.capturer.square = static_cast<int>((capture != m_captures[color].end()) ? m_pieces[!color][capture->xman()].initialSquare() : Nowhere);

			deduction.requiredMoves = piece.requiredMoves();
			deduction.numSquares = piece.squares().count();
			deduction.numMoves = piece.nmoves();

			m_deductions.complexity += std::log(0.0 + deduction.numSquares);
			m_deductions.complexity += std::log(1.0 + std::max(0, deduction.numMoves - deduction.requiredMoves));
		}
	}

	m_deductions.complexity *= (1.0 / std::log(2.0));
	return m_deductions;
}

/* -------------------------------------------------------------------------- */

}

/* -------------------------------------------------------------------------- */
/* -- API Functions                                                        -- */
/* -------------------------------------------------------------------------- */

extern "C"
EUCLIDE_Status EUCLIDE_initialize(EUCLIDE_Handle *euclide, const EUCLIDE_Configuration *configuration, const EUCLIDE_Callbacks *callbacks)
{
	EUCLIDE_Configuration nullconfiguration; memset(&nullconfiguration, 0, sizeof(nullconfiguration));
	EUCLIDE_Callbacks nullcallbacks; memset(&nullcallbacks, 0, sizeof(nullcallbacks));

	/* -- Start by clearing return value -- */

	if (!euclide)
		return EUCLIDE_STATUS_NULL;

	*euclide = nullptr;

	/* -- Create Euclide instance --  */

	try { *euclide = reinterpret_cast<EUCLIDE_Handle>(new Euclide::Euclide(configuration ? *configuration : nullconfiguration, callbacks ? *callbacks : nullcallbacks)); }
	catch (Euclide::Status status) { return static_cast<EUCLIDE_Status>(status); }
	catch (std::bad_alloc) { return EUCLIDE_STATUS_MEMORY; }
	catch (EUCLIDE_Status status) { return status; }

	/* -- Done -- */

	return EUCLIDE_STATUS_OK;
}

/* -------------------------------------------------------------------------- */

extern "C"
EUCLIDE_Status EUCLIDE_problem(EUCLIDE_Handle euclide, const EUCLIDE_Problem *problem)
{
	if (!euclide || !problem)
		return EUCLIDE_STATUS_NULL;

	try { reinterpret_cast<Euclide::Euclide *>(euclide)->solve(*problem); }
	catch (Euclide::Status status) { return static_cast<EUCLIDE_Status>(status); }
	catch (std::bad_alloc) { return EUCLIDE_STATUS_MEMORY; }
	catch (EUCLIDE_Status status) { return status; }

	return EUCLIDE_STATUS_OK;
}

/* -------------------------------------------------------------------------- */

extern "C"
EUCLIDE_Status EUCLIDE_done(EUCLIDE_Handle euclide)
{
	if (euclide)
		delete reinterpret_cast<Euclide::Euclide *>(euclide);

	return EUCLIDE_STATUS_OK;
}

/* -------------------------------------------------------------------------- */

extern "C"
EUCLIDE_Status EUCLIDE_solve(const EUCLIDE_Configuration *configuration, const EUCLIDE_Problem *problem, const EUCLIDE_Callbacks *callbacks)
{
	EUCLIDE_Handle euclide = nullptr;

	EUCLIDE_Status status = EUCLIDE_initialize(&euclide, configuration, callbacks);
	if (status != EUCLIDE_STATUS_OK)
		return status;

	status = EUCLIDE_problem(euclide, problem);
	EUCLIDE_done(euclide);

	return status;
}

/* -------------------------------------------------------------------------- */
