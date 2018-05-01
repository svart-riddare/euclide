#include "includes.h"
#include "problem.h"
#include "targets.h"
#include "pieces.h"

namespace Euclide
{

/* -------------------------------------------------------------------------- */
/* -- Euclide implementation                                               -- */
/* -------------------------------------------------------------------------- */

class Euclide
{
	public :
		Euclide(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks);
		~Euclide();

		void solve(const EUCLIDE_Problem& problem);

	protected :
		void reset();

		const EUCLIDE_Deductions& deductions() const;

	private :
		EUCLIDE_Configuration _configuration;      /**< Global configuration. */
		EUCLIDE_Callbacks _callbacks;              /**< User defined callbacks. */

		Problem _problem;                          /**< Current problem to solve. */

		array<Pieces, NumColors> _pieces;          /**< Deductions on pieces. */
		array<Targets, NumColors> _targets;        /**< Targets that must be fullfilled. */

		array<int, NumColors> _freeMoves;          /**< Unassigned moves. */
		array<int, NumColors> _freeCaptures;       /**< Unassigned captures. */

	private :
		mutable EUCLIDE_Deductions _deductions;    /**< Temporary variable to hold deductions for corresponding user callback. */
};

/* -------------------------------------------------------------------------- */

Euclide::Euclide(const EUCLIDE_Configuration& configuration, const EUCLIDE_Callbacks& callbacks)
	: _configuration(configuration), _callbacks(callbacks)
{
	/* -- Display copyright string -- */

	if (_callbacks.displayCopyright)
		(*_callbacks.displayCopyright)(_callbacks.handle, Copyright);
}

/* -------------------------------------------------------------------------- */

Euclide::~Euclide()
{
}

/* -------------------------------------------------------------------------- */

void Euclide::solve(const EUCLIDE_Problem& problem)
{
	/* -- Display problem -- */

	if (_callbacks.displayProblem)
		(*_callbacks.displayProblem)(_callbacks.handle, &problem);

	/* -- Reset solving state -- */

	_problem = problem;
	reset();

	/* -- Display analysis message -- */

	if (_callbacks.displayMessage)
		(*_callbacks.displayMessage)(_callbacks.handle, EUCLIDE_MESSAGE_ANALYZING);

	/* -- Initialize pieces -- */

	for (Glyph glyph : MostGlyphs())
		for (Square square : AllSquares())
			if (_problem.initialPosition(square) == glyph)
				_pieces[color(glyph)].emplace_back(_problem, square);

	/* -- Initialize targets -- */

	for (Glyph glyph : MostGlyphs())
		for (Square square : AllSquares())
			if (_problem.diagramPosition(square) == glyph)
				_targets[color(glyph)].emplace_back(glyph, square);

	/* -- Repeat the following deductions until there is no improvements -- */

	for (bool update = true; update; )
	{
		update = false;

		/* -- Assign moves and captures to targets -- */

		for (Color color : AllColors())
		{
			const Pieces& pieces = _pieces[color];
			Targets& targets = _targets[color];

			for (Target& target : targets)
			{
				Men men([&](Man man) { return (man < pieces.size()) && pieces[man].glyphs()[target.glyph()] && pieces[man].squares()[target.square()]; });
				target.updatePossibleMen(men);

				target.updateRequiredMoves(xstd::min(men.in(pieces), [](const Piece& piece) { return piece.requiredMoves(); }));
				target.updateRequiredCaptures(xstd::min(men.in(pieces), [](const Piece& piece) { return piece.requiredCaptures(); }));
			}

			targets.update();
		}

		/* -- Compute free moves and captures -- */

		for (Color color : AllColors())
		{
			_freeMoves[color] = _problem.moves(color) - xstd::sum(_pieces[color], [](const Piece& piece) { return piece.requiredMoves(); });
			_freeCaptures[color] = _problem.capturedPieces(color) - xstd::sum(_pieces[color], [](const Piece& piece) { return piece.requiredCaptures(); });
		}

		/* -- Display current deductions -- */

		if (_callbacks.displayDeductions)
			(*_callbacks.displayDeductions)(_callbacks.handle, &deductions());

		/* -- Assign free moves, free captures and possible squares -- */

		for (Color color : AllColors())
		{
			Pieces& pieces = _pieces[color];

			TargetPartitions partitions(_targets[color]);

			for (const TargetPartition& partition : partitions)
				if (partition.men().count() <= partition.squares().count())
					for (Man man : ValidMen(partition.men()))
						pieces[man].setCaptured(false);

			for (const TargetPartition& partition : partitions)
				for (Man man : ValidMen(partition.men()))
					if (!maybe(pieces[man].captured()))
						pieces[man].setPossibleSquares(partition.squares());

			for (Piece& piece : pieces)
			{
				piece.setAvailableMoves(piece.requiredMoves() + _freeMoves[color]);
				piece.setAvailableCaptures(piece.requiredCaptures() + _freeCaptures[color]);
			
				if (piece.update())
					update = true;
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

void Euclide::reset()
{
	_pieces[White].clear();
	_pieces[Black].clear();
}

/* -------------------------------------------------------------------------- */

const EUCLIDE_Deductions& Euclide::deductions() const
{
	memset(&_deductions, 0, sizeof(_deductions));

	const int pieces[NumColors] = {
		_deductions.numWhitePieces = std::min<int>(_pieces[White].size(), countof(_deductions.whitePieces)),
		_deductions.numBlackPieces = std::min<int>(_pieces[Black].size(), countof(_deductions.blackPieces))
	};

	_deductions.freeWhiteMoves = _freeMoves[White];
	_deductions.freeBlackMoves = _freeMoves[Black];

	if ((_deductions.freeWhiteMoves < 0) || (_deductions.freeBlackMoves < 0))
		throw NoSolution;

	_deductions.complexity += std::log(1.0 + _deductions.freeWhiteMoves);
	_deductions.complexity += std::log(1.0 + _deductions.freeBlackMoves);

	for (Color color : AllColors())
	{
		for (int k = 0; k < pieces[color]; k++)
		{
			EUCLIDE_Deduction& deduction = color ? _deductions.blackPieces[k] : _deductions.whitePieces[k];
			const Piece& piece = _pieces[color][k];

			deduction.initialGlyph = static_cast<EUCLIDE_Glyph>(piece.glyph(true));
			deduction.diagramGlyph = static_cast<EUCLIDE_Glyph>(piece.glyph());

			deduction.initialSquare = static_cast<int>(piece.square(true));
			deduction.finalSquare = static_cast<int>(piece.square());

			deduction.requiredMoves = piece.requiredMoves();
			deduction.numSquares = piece.squares().count();
			deduction.numMoves = piece.moves();

			deduction.captured = false;

			_deductions.complexity += std::log(0.0 + deduction.numSquares);
			_deductions.complexity += std::log(1.0 + std::max(0, deduction.numMoves - deduction.requiredMoves));
		}
	}

	_deductions.complexity *= (1.0 / std::log(2.0));
	return _deductions;
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
