#include "output.h"

/* -------------------------------------------------------------------------- */

const wchar_t *Output::Hyphens = L"---------------------------------------------------------------------------";

/* -------------------------------------------------------------------------- */

Output::Output(const Strings& strings, const char *inputFileName)
	: m_strings(strings), m_file(nullptr)
{
	reset();
	memset(&m_callbacks, 0, sizeof(m_callbacks));

	m_callbacks.displayCopyright = displayCopyrightCallback;
	m_callbacks.displayProblem = displayProblemCallback;
	m_callbacks.displayMessage = displayMessageCallback;
	m_callbacks.displayProgress = displayProgressCallback;
	m_callbacks.displayDeductions = displayDeductionsCallback;
	m_callbacks.displayThinking = displayThinkingCallback;
	m_callbacks.displaySolution = displaySolutionCallback;
	m_callbacks.handle = this;

	open(inputFileName);
}

/* -------------------------------------------------------------------------- */

Output::~Output()
{
	if (m_file)
		fclose(m_file);
}

/* -------------------------------------------------------------------------- */

void Output::open(const char *inputFileName)
{
	/* -- Close previously opened file -- */

	if (m_file)
		fclose(m_file);

	m_file = nullptr;

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

	m_file = fopen(outputFileName, "w");
	delete[] outputFileName;

}

/* -------------------------------------------------------------------------- */

void Output::reset()
{
	m_timer = Timer();
	m_complexity = 0.0;
	m_positions = 0;
	m_solutions = 0;
}

/* -------------------------------------------------------------------------- */

void Output::done(EUCLIDE_Status status)
{
	if (m_file)
	{
		fprintf(m_file, "%ls\n", m_strings[Strings::Output]);
		if (status == EUCLIDE_STATUS_OK)
      {
			fprintf(m_file, "\t%ls %.2f\n", m_strings[Strings::Score], m_complexity);
			if (m_positions)
				fprintf(m_file, "\t%ls %" PRId64 "\n", m_strings[Strings::Positions], m_positions);

			const Strings::String verdicts[] = { Strings::NoSolution, Strings::UniqueSolution, Strings::TwoSolutions, Strings::ThreeSolutions, Strings::FourSolutions, Strings::Cooked };
			if (m_positions)
				fprintf(m_file, "\t%ls\n", m_strings[verdicts[std::min<int>(m_solutions, countof(verdicts) - 1)]]);
		}
      else
      if (status == EUCLIDE_STATUS_ABORTED)
      {
         fprintf(m_file, "\t%ls %.2f\n", m_strings[Strings::Score], m_complexity);
         fprintf(m_file, "\t%ls\n", m_strings[status]);
      }
		else
      {
			fprintf(m_file, "\t%ls\n", m_strings[status]);
		}
		fprintf(m_file, "\n\n");
		fflush(m_file);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayCopyright(const wchar_t *copyright) const
{
	if (m_file)
	{
		fprintf(m_file, "%ls\n", Hyphens);
		fprintf(m_file, "-- %-*ls --\n", int(wcslen(Hyphens)) - 6, copyright);
		fprintf(m_file, "%ls\n", Hyphens);
		fprintf(m_file, "\n");
		fflush(m_file);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayMessage(EUCLIDE_Message /*message*/) const
{
}

/* -------------------------------------------------------------------------- */

void Output::displayProblem(const EUCLIDE_Problem& problem) const
{
	const wchar_t *glyphs = m_strings[Strings::GlyphSymbols];

	if (m_file)
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

		fprintf(m_file, "%ls\n\t%s\n\t%d\n\n", m_strings[Strings::Input], forsythe, problem.numHalfMoves);

		fprintf(m_file, "\t+---+---+---+---+---+---+---+---+\n\t");
		for (int y = 8; y-- > 0; )
		{
			for (int x = 0; x < 8; x++)
				fprintf(m_file, "| %c ",  glyphs[problem.diagram[8 * x + y]]);

			fprintf(m_file, "|\n\t+---+---+---+---+---+---+---+---+\n\t");
		}

		wchar_t buffer[48];
		swprintf(buffer, countof(buffer), L"%d%ls%d %ls", problem.numHalfMoves / 2, m_strings[Strings::Dot], (problem.numHalfMoves % 2) ? 5 : 0, m_strings[Strings::Moves]);
		while (int(wcslen(buffer)) < 26 + ((white < 10) ? 1 : 0) + ((black < 10) ? 1 : 0)) wcscat(buffer, L" ");
		swprintf(buffer + wcslen(buffer), 8, L"(%d+%d)", white, black);
		fprintf(m_file, "%ls\n\n", buffer);
		fflush(m_file);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayProgress(int /*whiteFreeMoves*/, int /*blackFreeMoves*/, double complexity) const
{
	m_complexity = complexity;
}

/* -------------------------------------------------------------------------- */

void Output::displayDeductions(const EUCLIDE_Deductions& /*deductions*/) const
{
}

/* -------------------------------------------------------------------------- */

void Output::displayThinking(const EUCLIDE_Thinking& thinking) const
{
	m_positions = thinking.positions;
}

/* -------------------------------------------------------------------------- */

void Output::displaySolution(const EUCLIDE_Solution& solution) const
{
	if (m_file)
	{
		fprintf(m_file, "%ls%d%ls\n", m_strings[Strings::Solution], solution.solution, m_strings[Strings::Colon]);
		fprintf(m_file, "%ls\n", Hyphens);

		const int black = (solution.moves[0].glyph & 1) ^ 1;
		if (black)
			fprintf(m_file, " 1.           ");

		for (int m = 0; m < solution.numHalfMoves; m++)
		{
			const EUCLIDE_Move& move = solution.moves[m];

			if ((m & 1) == black)
				fprintf(m_file, "%2d. ", move.move);

			if (move.kingSideCastling)
				fprintf(m_file, "0-0%c      ", move.check ? '+' : ' ');
			else
			if (move.queenSideCastling)
				fprintf(m_file, "0-0-0%c    ", move.check ? '+' : ' ');
			else
			if (move.promotion != move.glyph)
				fprintf(m_file, "%c%c%c%c%c%c=%c%c ", toupper(m_strings[Strings::GlyphSymbols][move.glyph]), 'a' + move.from / 8, '1' + move.from % 8, move.capture ? 'x' : '-', 'a' + move.to / 8, '1' + move.to % 8, toupper(m_strings[Strings::GlyphSymbols][move.promotion]), move.check ? '+' : ' ');
			else
			if (move.enpassant)
				fprintf(m_file, "%c%c%c%c%c%cep%c ", toupper(m_strings[Strings::GlyphSymbols][move.glyph]), 'a' + move.from / 8, '1' + move.from % 8, move.capture ? 'x' : '-', 'a' + move.to / 8, '1' + move.to % 8, move.check ? '+' : ' ');
			else
				fprintf(m_file, "%c%c%c%c%c%c%c   ", toupper(m_strings[Strings::GlyphSymbols][move.glyph]), 'a' + move.from / 8, '1' + move.from % 8, move.capture ? 'x' : '-', 'a' + move.to / 8, '1' + move.to % 8, move.check ? '+' : ' ');

			if ((m & 1) != black)
				fprintf(m_file, " ");

			if ((m % 6) == (5 - black))
				if (m != (solution.numHalfMoves - 1))
					fprintf(m_file, "\n");
		}

		fprintf(m_file, "\n\n");
		fflush(m_file);
	}

	m_solutions += 1;
}

/* -------------------------------------------------------------------------- */

