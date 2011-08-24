#include "includes.h"
#include "forsythe.h"

#ifndef EUCLIDE_LINUX
	#include "console-win.h"
	typedef WinConsole DosConsole;
#else
	#include "console-linux.h"
	typedef LinuxConsole DosConsole;
#endif

/* -------------------------------------------------------------------------- */

static
bool solveProblem(Console& console, const char *forsytheString, int numHalfMoves) 
{
	ForsytheString problem(forsytheString, numHalfMoves);
	if (!problem)
		return false;

	console.reset();

	EUCLIDE_Status status = EUCLIDE_solve(NULL, problem, console);
	if (status != EUCLIDE_STATUS_OK)
		console.displayError(strings::load(status));

	console.done();
	return true;
}

/* -------------------------------------------------------------------------- */

static
bool solveProblems(Console& console, const char *file)
{
	FILE *input = fopen(file, "r");
	if (!input)
		return false;

	console << file;

	const int bufferSize = 1024;
	char *bufferA = new char[bufferSize];
	char *bufferB = new char[bufferSize];

	int problems = 0;

	fgets(bufferA, bufferSize, input);
	while (fgets(bufferB, bufferSize, input) && !!console)
	{
		int numHalfMoves;

		if (sscanf(bufferB, "%d", &numHalfMoves) == 1)
			if (solveProblem(console, bufferA, numHalfMoves))
				problems++;
		
		std::swap(bufferA, bufferB);
	}

	fclose(input);

	delete[] bufferA;
	delete[] bufferB;

	return (problems > 0);
}

/* -------------------------------------------------------------------------- */

static
int _main(int numArguments, char *arguments[], char * /*environment*/[])
{
	DosConsole console;
	if (!console)
		return fprintf(stderr, "\n\t\bUnexpected console initialization failure. Aborting.\n\n"), -1;

	switch (numArguments)
	{
		case 2 : 
			if (!solveProblems(console, arguments[1]))
			{
				console.displayError(strings::load(strings::InvalidInputFile));
				console.wait();
			}

			break;

		case 3 :
			if (!solveProblem(console, arguments[1], atoi(arguments[2])))
			{
				console.displayError(strings::load(strings::InvalidArguments));
				console.wait();
			}

			break;

		default :
			console.displayError(strings::load(strings::NoArguments));
			console.wait();
			break;
	}

	return 0;
}

/* -------------------------------------------------------------------------- */

int main(int numArguments, char *arguments[], char *environment[])
{
#ifdef DOS_EUCLIDE_CHECK_MEMORY_LEAKS
	_CrtMemState state;
	_CrtMemCheckpoint(&state);
#endif

	int result = _main(numArguments, arguments, environment);

#ifdef DOS_EUCLIDE_CHECK_MEMORY_LEAKS
	_CrtMemDumpAllObjectsSince(&state);
#endif

	return result;
}

/* -------------------------------------------------------------------------- */
