#include "output.h"

/* -------------------------------------------------------------------------- */

Output::Output()
{
	file = NULL;

	reset();
	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.displayCopyright = _displayCopyright;
	callbacks.displayProblem = _displayProblem;
	callbacks.displayMessage = _displayMessage;
	callbacks.displayProgress = _displayProgress;
	callbacks.displayDeductions = _displayDeductions;

	callbacks.handle = static_cast<EUCLIDE_Handle>(this);
}

/* -------------------------------------------------------------------------- */

Output::Output(const char *inputName)
{
	file = NULL;
	open(inputName);

	reset();
	memset(&callbacks, 0, sizeof(callbacks));

	callbacks.displayCopyright = _displayCopyright;
	callbacks.displayProblem = _displayProblem;
	callbacks.displayMessage = _displayMessage;
	callbacks.displayProgress = _displayProgress;
	callbacks.displayDeductions = _displayDeductions;

	callbacks.handle = static_cast<EUCLIDE_Handle>(this);
}

/* -------------------------------------------------------------------------- */

Output::~Output()
{
	if (file)
		fclose(file);
}

/* -------------------------------------------------------------------------- */

void Output::open(const char *inputName)
{
	/* -- Close previously opened file -- */

	if (file)
		fclose(file);

	/* -- Create output file name from input file name -- */

	char *outputName = new char[strlen(inputName) + 11];
	strcpy(outputName, inputName);

	if (strrchr(inputName, '.'))
		sprintf(strrchr(outputName, '.'), ".output%s", strrchr(inputName, '.'));
	else
		strcat(outputName, ".output.txt");

	file = fopen(outputName, "w");
	delete[] outputName;

}

/* -------------------------------------------------------------------------- */

void Output::reset()
{
	timer = Timer();
	complexity = 0.0;
}

/* -------------------------------------------------------------------------- */

void Output::done()
{
	if (file)
	{
		fwprintf(file, L"%ls\n", strings::load(strings::Output));
		fwprintf(file, L"\t%ls %.2f\n", strings::load(strings::Score), complexity);
		fwprintf(file, L"\n\n");
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayCopyright(const wchar_t *copyright)
{
	wchar_t *string = new wchar_t[wcslen(copyright) + 7];
	std::fill_n(string, wcslen(copyright) + 6, '-');
	string[wcslen(copyright) + 6] = '\0';

	if (file)
	{
		fwprintf(file, L"%ls\n", string);
		fwprintf(file, L"-- %ls --\n", copyright);
		fwprintf(file, L"%ls\n", string);
		fwprintf(file, L"\n");
		fflush(file);
	}

	delete[] string;
}

/* -------------------------------------------------------------------------- */

void Output::displayMessage(EUCLIDE_Message /*message*/)
{
}

/* -------------------------------------------------------------------------- */

void Output::displayProblem(const EUCLIDE_Problem *problem)
{
	const wchar_t *glyphs = strings::load(strings::GlyphSymbols);

	if (file)
	{
		int white = 0;
		int black = 0;

		char forsythe[80];		
		for (int y = 8, n = 0, k = 0; y-- > 0; k = 0)
		{
			for (int x = 0; x < 8; x++)
			{
				if (problem->glyphs[8 * x + y] != EUCLIDE_GLYPH_NONE)
				{
					if (k)
						forsythe[n++] = (char)('0' + k);

					forsythe[n++] = (char)glyphs[problem->glyphs[8 * x + y]];
					white += (problem->glyphs[8 * x + y] <= EUCLIDE_GLYPH_WHITE_PAWN) ? 1 : 0;
					black += (problem->glyphs[8 * x + y] >= EUCLIDE_GLYPH_BLACK_KING) ? 1 : 0;
					k = 0;
				}
				else
				{
					k++;
				}
			}

			if (k)
				forsythe[n++] = (char)('0' + k);

			forsythe[n++] = y ? '/' : '\0';
		}

		fwprintf(file, L"%ls\n\t%hs\n\t%d\n\n", strings::load(strings::Input), forsythe, problem->numHalfMoves);

		fprintf(file, "\t+---+---+---+---+---+---+---+---+\n\t");
		for (int y = 8; y-- > 0; )
		{
			for (int x = 0; x < 8; x++)
				fprintf(file, "| %c ",  glyphs[problem->glyphs[8 * x + y]]);

			fprintf(file, "|\n\t+---+---+---+---+---+---+---+---+\n\t");
		}

		wchar_t buffer[48];
		swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%d%ls %ls", problem->numHalfMoves / 2, strings::load((problem->numHalfMoves & 1) ? strings::HalfMove : strings::NoHalfMove), strings::load(strings::Moves));
		while ((int)wcslen(buffer) < 26 + ((white < 10) ? 1 : 0) + ((black < 10) ? 1 : 0)) wcscat(buffer, L" ");
		swprintf(buffer + wcslen(buffer), 8, L"(%d+%d)", white, black);
		fwprintf(file, L"%ls\n\n", buffer);
	}
}

/* -------------------------------------------------------------------------- */

void Output::displayProgress(int /*whiteFreeMoves*/, int /*blackFreeMoves*/, double complexity)
{
	this->complexity = complexity;	
}

/* -------------------------------------------------------------------------- */

void Output::displayDeductions(const EUCLIDE_Deductions * /*deductions*/)
{
}

/* -------------------------------------------------------------------------- */

Output::operator const EUCLIDE_Callbacks *() const
{
	return &callbacks;
}

/* -------------------------------------------------------------------------- */

bool Output::operator!() const
{
	return file ? false : true;
}

/* -------------------------------------------------------------------------- */

