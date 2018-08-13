#include "output.h"

/* -------------------------------------------------------------------------- */

Output::Output(const Strings& strings, const char *inputFileName)
	: _strings(strings), _file(nullptr)
{
	reset();
	memset(&_callbacks, 0, sizeof(_callbacks));

	_callbacks.displayCopyright = displayCopyrightCallback;
	_callbacks.displayProblem = displayProblemCallback;
	_callbacks.displayMessage = displayMessageCallback;
	_callbacks.displayProgress = displayProgressCallback;
	_callbacks.displayDeductions = displayDeductionsCallback;
	_callbacks.displayThinking = displayThinkingCallback;
	_callbacks.displaySolution = displaySolutionCallback;
	_callbacks.handle = this;

	open(inputFileName);
}

/* -------------------------------------------------------------------------- */

Output::~Output()
{
	if (_file)
		fclose(_file);
}

/* -------------------------------------------------------------------------- */

void Output::open(const char *inputFileName)
{
	/* -- Close previously opened file -- */

	if (_file)
		fclose(_file);

	_file = nullptr;

	/* -- Early exit if no filename was provided -- */

	if (!inputFileName || !*inputFileName)
		return;

	/* -- Create output file name from input file name -- */

	char *outputFileName = new char[strlen(inputFileName) + strlen(".output.txt") + 1];
	strcpy(outputFileName, inputFileName);

	if (strrchr(inputFileName, '.'))
		sprintf(strrchr(outputFileName, '.'), ".output%s", strrchr(inputFileName, '.'));
	else
		strcat(outputFileName, ".output.txt");

	_file = fopen(outputFileName, "w");
	delete[] outputFileName;

}

/* -------------------------------------------------------------------------- */

void Output::reset()
{
	_timer = Timer();
	_complexity = 0.0;
	_positions = 0;
	_solutions = 0;
}

/* -------------------------------------------------------------------------- */

void Output::done(EUCLIDE_Status status)
{
	if (_file)
	{
		fprintf(_file, "%ls\n", _strings[Strings::Output]);
		if (status == EUCLIDE_STATUS_OK) {
			fprintf(_file, "\t%ls %.2f\n", _strings[Strings::Score], _complexity);
			if (_positions)
				fprintf(_file, "\t%ls %" PRId64 "\n", _strings[Strings::Positions], _positions);
					
			const Strings::String verdicts[] = { Strings::NoSolution, Strings::UniqueSolution, Strings::TwoSolutions, Strings::ThreeSolutions, Strings::FourSolutions, Strings::Cooked };
			if (_positions)
				fprintf(_file, "\t%ls\n", _strings[verdicts[std::min<int>(_solutions, countof(verdicts) - 1)]]);
		}
		else {
			fprintf(_file, "\t%ls\n", _strings[status]);
		}
		fprintf(_file, "\n\n");
		fflush(_file);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayCopyright(const wchar_t *copyright) const
{
	std::wstring hyphens(wcslen(copyright) + 6, '-');

	if (_file)
	{
		fprintf(_file, "%ls\n", hyphens.c_str());
		fprintf(_file, "-- %ls --\n", copyright);
		fprintf(_file, "%ls\n", hyphens.c_str());
		fprintf(_file, "\n");
		fflush(_file);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayMessage(EUCLIDE_Message /*message*/) const
{
}

/* -------------------------------------------------------------------------- */

void Output::displayProblem(const EUCLIDE_Problem& problem) const
{
	const wchar_t *glyphs = _strings[Strings::GlyphSymbols];

	if (_file)
	{
		int white = 0;
		int black = 0;

		char forsythe[80];		
		for (int y = 8, n = 0, k = 0; y-- > 0; k = 0)
		{
			for (int x = 0; x < 8; x++)
			{
				if (problem.diagram[8 * x + y] != EUCLIDE_GLYPH_NONE)
				{
					if (k)
						forsythe[n++] = '0' + k;

					forsythe[n++] = glyphs[problem.diagram[8 * x + y]];
					white += (problem.diagram[8 * x + y] & 1) ? 1 : 0;
					black += (problem.diagram[8 * x + y] & 1) ? 0 : 1;
					k = 0;
				}
				else
				{
					k++;
				}
			}

			if (k)
				forsythe[n++] = '0' + k;

			forsythe[n++] = y ? '/' : '\0';
		}

		fprintf(_file, "%ls\n\t%s\n\t%d\n\n", _strings[Strings::Input], forsythe, problem.numHalfMoves);

		fprintf(_file, "\t+---+---+---+---+---+---+---+---+\n\t");
		for (int y = 8; y-- > 0; )
		{
			for (int x = 0; x < 8; x++)
				fprintf(_file, "| %c ",  glyphs[problem.diagram[8 * x + y]]);

			fprintf(_file, "|\n\t+---+---+---+---+---+---+---+---+\n\t");
		}

		wchar_t buffer[48];
		swprintf(buffer, countof(buffer), L"%d%ls%d %ls", problem.numHalfMoves / 2, _strings[Strings::Dot], (problem.numHalfMoves % 2) ? 5 : 0, _strings[Strings::Moves]);
		while (int(wcslen(buffer)) < 26 + ((white < 10) ? 1 : 0) + ((black < 10) ? 1 : 0)) wcscat(buffer, L" ");
		swprintf(buffer + wcslen(buffer), 8, L"(%d+%d)", white, black);
		fprintf(_file, "%ls\n\n", buffer);
		fflush(_file);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayProgress(int /*whiteFreeMoves*/, int /*blackFreeMoves*/, double complexity) const
{
	_complexity = complexity;	
}

/* -------------------------------------------------------------------------- */

void Output::displayDeductions(const EUCLIDE_Deductions& /*deductions*/) const
{
}

/* -------------------------------------------------------------------------- */

void Output::displayThinking(const EUCLIDE_Thinking& thinking) const
{
	_positions = thinking.positions;
}

/* -------------------------------------------------------------------------- */

void Output::displaySolution(const EUCLIDE_Solution& solution) const
{
	if (_file)
	{
		fprintf(_file, "%ls%d%ls\n", _strings[Strings::Solution], solution.solution, _strings[Strings::Colon]);
		fprintf(_file, "---------------------------------------------------------------------------\n");

		const int black = (solution.moves[0].glyph & 1) ^ 1;
		if (black)
			fprintf(_file, " 1.           ");

		for (int m = 0; m < solution.numHalfMoves; m++)
		{
			const EUCLIDE_Move& move = solution.moves[m];

			if ((m & 1) == black)
				fprintf(_file, "%2d. ", move.move);

			if (move.kingSideCastling)
				fprintf(_file, "0-0%c       ", move.check ? '+' : ' ');
			else
			if (move.queenSideCastling)
				fprintf(_file, "0-0-0%c     ", move.check ? '+' : ' ');
			else
			if (move.promotion != move.glyph)
				fprintf(_file, "%c%c%c%c%c%c=%c%c ", toupper(_strings[Strings::GlyphSymbols][move.glyph]), 'a' + move.from / 8, '1' + move.from % 8, move.capture ? 'x' : '-', 'a' + move.to / 8, '1' + move.to % 8, toupper(_strings[Strings::GlyphSymbols][move.promotion]), move.check ? '+' : ' ');
			else
			if (move.enpassant)
				fprintf(_file, "%c%c%c%c%c%cep%c ", toupper(_strings[Strings::GlyphSymbols][move.glyph]), 'a' + move.from / 8, '1' + move.from % 8, move.capture ? 'x' : '-', 'a' + move.to / 8, '1' + move.to % 8, move.check ? '+' : ' ');
			else
				fprintf(_file, "%c%c%c%c%c%c%c   ", toupper(_strings[Strings::GlyphSymbols][move.glyph]), 'a' + move.from / 8, '1' + move.from % 8, move.capture ? 'x' : '-', 'a' + move.to / 8, '1' + move.to % 8, move.check ? '+' : ' ');

			if ((m & 1) != black)
				fprintf(_file, " ");

			if ((m % 6) == (5 - black))
				if (m != (solution.numHalfMoves - 1))
					fprintf(_file, "\n");
		}

		fprintf(_file, "\n\n");
		fflush(_file);
	}

	_solutions += 1;
}

/* -------------------------------------------------------------------------- */

