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
}

/* -------------------------------------------------------------------------- */

void Output::done()
{
	if (_file)
	{
		fwprintf(_file, L"%ls\n", _strings[Strings::Output]);
		fwprintf(_file, L"\t%ls %.2f\n", _strings[Strings::Score], _complexity);
		fwprintf(_file, L"\n\n");
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayCopyright(const wchar_t *copyright) const
{
	std::string hyphens(wcslen(copyright) + 6, '-');

	if (_file)
	{
		fwprintf(_file, L"%s\n", hyphens.c_str());
		fwprintf(_file, L"-- %ls --\n", copyright);
		fwprintf(_file, L"%s\n", hyphens.c_str());
		fwprintf(_file, L"\n");
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
					white += (problem.diagram[8 * x + y] <= EUCLIDE_GLYPH_WHITE_PAWN) ? 1 : 0;
					black += (problem.diagram[8 * x + y] >= EUCLIDE_GLYPH_BLACK_KING) ? 1 : 0;
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

		fwprintf(_file, L"%ls\n\t%hs\n\t%d\n\n", _strings[Strings::Input], forsythe, problem.numHalfMoves);

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
		fwprintf(_file, L"%ls\n\n", buffer);
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

