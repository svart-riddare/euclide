#include "includes.h"
#include "forsythe.h"

#ifdef EUCLIDE_WINDOWS
	#include "console-win.h"
	typedef WinConsole DosConsole;
#else
	#include "console-linux.h"
	typedef LinuxConsole DosConsole;
#endif

/* -------------------------------------------------------------------------- */

static
bool solve(const Strings& strings, Console& console, const char *forsytheString, int numHalfMoves, bool wait) 
{
	/* -- Parse forsythe string -- */

	ForsytheString problem(strings, forsytheString, numHalfMoves);
	if (!problem)
		return false;

	/* -- Reset display -- */

	console.reset();

	/* -- Solve problem -- */

	const EUCLIDE_Status status = EUCLIDE_solve(nullptr, problem, console);
	if (status != EUCLIDE_STATUS_OK)
		console.displayError(strings[status]);

	/* -- Done -- */

	console.done(status);
	if (wait || (status != EUCLIDE_STATUS_OK))
		console.wait();

	return true;
}

/* -------------------------------------------------------------------------- */

static
bool solve(const Strings& strings, Console& console, const char *file, bool wait)
{
	/* -- Open input file -- */

	FILE *input = fopen(file, "r");
	if (!input)
		return false;

	/* -- Create output file -- */

	console.open(file);

	/* -- Read file, line by line, keeping two last lines in memory -- */

	const int bufferSize = 1024;
	char *bufferA = new char[bufferSize];
	char *bufferB = new char[bufferSize];

	int problems = 0;

	if (fgets(bufferA, bufferSize, input))
	{
		while (fgets(bufferB, bufferSize, input) && !!console)
		{
			/* -- Solve any problem found (forsythe string on first line, number of moves on second line) -- */

			int numHalfMoves;
			if (sscanf(bufferB, "%d", &numHalfMoves) == 1)
				if (solve(strings, console, bufferA, numHalfMoves, wait))
					problems++;

			/* -- Loop -- */

			std::swap(bufferA, bufferB);
		}
	}

	/* -- Done -- */

	fclose(input);

	delete[] bufferA;
	delete[] bufferB;

	/* -- Return number of problems found -- */

	return (problems > 0);
}

/* -------------------------------------------------------------------------- */

static
int euclide(int numArguments, char *arguments[], char * /*environment*/[])
{
	/* -- Load constant strings -- */

	Strings strings;

	/* -- Initialize console output -- */
	
	DosConsole console(strings);
	if (!console)
		return fprintf(stderr, "\n\t\bUnexpected console initialization failure. Aborting.\n\n"), -1;

	/* -- Parse arguments, either a file path or a forsythe string -- */

	Strings::Error error = (numArguments > 1) ? Strings::NumErrors : Strings::NoArguments;

	const char *problems = nullptr, *moves = nullptr;
	bool wait = false;

	for (int argument = 1; argument < numArguments; argument++)
	{
		if (arguments[argument][0] != '-')
		{
			if (!problems)
				problems = arguments[argument];
			else
			if (!moves)
				moves = arguments[argument];
			else
				error = Strings::InvalidArguments;
		}
		else
		if (strcmp(arguments[argument], "--wait") == 0)
			wait = true;
		else
			error = Strings::InvalidArguments;
	}

	/* -- Solve problems -- */

	if (error == Strings::NumErrors)
	{
		if (problems && moves)
		{
			if (!solve(strings, console, problems, atoi(moves), wait))
				error = Strings::InvalidProblem;
		}
		else
		if (problems)
		{
			if (!solve(strings, console, problems, wait))
				error = Strings::InvalidInputFile;
		}
	}

	/* -- Show error -- */

	const bool failed = (error < Strings::NumErrors);

	if (failed)
	{
		console.displayError(strings[error]);
		console.wait();
	}

	/* -- Done -- */

	return failed ? 1 : 0;
}

/* -------------------------------------------------------------------------- */

#undef main
int main(int numArguments, char *arguments[], char *environment[])
{
#ifdef DOS_EUCLIDE_CHECK_MEMORY_LEAKS
	_CrtMemState state;
	_CrtMemCheckpoint(&state);
#endif

	int result = euclide(numArguments, arguments, environment);

#ifdef DOS_EUCLIDE_CHECK_MEMORY_LEAKS
	_CrtMemDumpAllObjectsSince(&state);
#endif

	return result;
}

/* -------------------------------------------------------------------------- */
