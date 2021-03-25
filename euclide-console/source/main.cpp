#include "includes.h"
#include "forsythe.h"
#include "console.h"
#include "console-background.h"

/* -------------------------------------------------------------------------- */

struct Options
{
	bool quiet = false;      /**< If set, print to stdout rather than using console output. */
	bool wait = false;       /**< If set, wait for user input after each problem solved. */

	int timeout = 0;         /**< Timeout, in seconds, before aborting solving for a problem. */
	int solutions = 8;       /**< Maximum number of solutions, before aborting solving for a problem. */
	bool contest = false;    /**< Solving contest mode. */

	int threads = 0;         /**< Number of threads used for batch solving. */
};

/* -------------------------------------------------------------------------- */

bool solve(Console& console, const ForsytheString& problem, const Options& options, bool background = false)
{
	assert(problem);

	/* -- Reset display -- */

	console.reset(std::chrono::seconds(options.timeout));

	/* -- Solve problem -- */

	EUCLIDE_Options configuration = {};
	configuration.maxSolutions = options.solutions;
	configuration.solvingContest = options.contest;

	const EUCLIDE_Status status = EUCLIDE_solve(&configuration, problem, console);

	/* -- Done -- */

	console.done(status);

	/* -- Wait for user input -- */

	if (options.wait && !background)
		if (!console.wait())
			return false;

	/* -- Done -- */

	return true;
}

/* -------------------------------------------------------------------------- */

static
bool solve(Console& console, const std::list<ForsytheString>& problems, const Options& options)
{
	const size_t concurrency = std::min<size_t>(problems.size(), options.threads ? options.threads : std::thread::hardware_concurrency());

	/* -- Solve each problem -- */

	if (concurrency > 1)
	{
		std::list<BackgroundConsole> consoles;
		std::list<std::thread> threads;
		std::mutex mutex;

		/* -- Create background consoles -- */

		while (consoles.size() < problems.size())
			consoles.emplace_back(console, consoles.empty());

		/* -- Threads solve problems using a dedicated background console -- */

		auto nextConsole = consoles.begin();
		auto nextProblem = problems.begin();

		const auto main = [&]() -> void {

			/* -- Lower solving thread priority if we intend to use all cpu power -- */

			if (concurrency >= std::thread::hardware_concurrency())
			{
#ifdef EUCLIDE_WINDOWS
				SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
#else
				int policy;
				struct sched_param parameters;
				if (pthread_getschedparam(pthread_self(), &policy, &parameters) == 0)
				{
					parameters.sched_priority += 1;
					pthread_setschedparam(pthread_self(), policy, &parameters);
				}
#endif
			}

			/* -- Solve next available problem -- */

			std::unique_lock<std::mutex> lock(mutex);
			while ((nextProblem != problems.end()) && console)
			{
				BackgroundConsole& background = *nextConsole++;
				const ForsytheString& problem = *nextProblem++;

				lock.unlock();
				solve(background, problem, options, true);
				lock.lock();
			}
		};

		/* -- Create threads -- */

		while (threads.size() < concurrency)
			threads.emplace_back(std::thread(main));

		/* -- Main thread displays background consoles one after the other -- */

		for (auto& background : consoles)
			if (!background.foreground(options.wait))
				break;

		/* -- Threads should be terminated, unless we aborted solving -- */

		for (auto& thread : threads)
			thread.join();
	}
	else
	{
		/* -- Single threaded implementation is straightforward -- */

		for (const auto& problem : problems)
			if (!solve(console, problem, options))
				break;
	}

	/* -- Done -- */

	return true;
}

/* -------------------------------------------------------------------------- */

static
bool solve(const Strings& strings, Console& console, const char *file, const Options& options)
{
   std::list<ForsytheString> problems;

	/* -- Open input file -- */

	FILE *input = fopen(file, "r");
	if (!input)
		return false;

	/* -- Read file, line by line, keeping two last lines in memory -- */

	const int bufferSize = 1024;
	char *bufferA = new char[bufferSize];
	char *bufferB = new char[bufferSize];

	if (fgets(bufferA, bufferSize, input))
	{
		while (fgets(bufferB, bufferSize, input) && !!console)
		{
			/* -- Solve any problem found (forsythe string on first line, number of moves on second line) -- */

			int numHalfMoves, characters;
			if (sscanf(bufferB, "%d%n", &numHalfMoves, &characters) >= 1)
			{
				ForsytheString problem(strings, bufferA, numHalfMoves, bufferB + characters);
				if (problem)
					problems.emplace_back(problem);
			}

			/* -- Loop -- */

			std::swap(bufferA, bufferB);
		}
	}

	delete[] bufferB;
	delete[] bufferA;

	fclose(input);

	/* -- Early exit if there are no problems to solve -- */

	if (problems.empty())
		return false;

	/* -- Create output file -- */

	console.open(file);

	/* -- Solve problems -- */

	return solve(console, problems, options);
}

/* -------------------------------------------------------------------------- */

static
int euclide(int numArguments, char *arguments[], char * /*environment*/[])
{
	/* -- Load constant strings -- */

	Strings strings;

	/* -- Parse arguments, either a file path or a forsythe string -- */

	Strings::Error error = (numArguments > 1) ? Strings::NumErrors : Strings::NoArguments;

	const char *problems = nullptr, *moves = nullptr;
	Options options;

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
		if (strcmp(arguments[argument], "--timeout") == 0)
		{
			if (++argument < numArguments)
				options.timeout = atoi(arguments[argument]);
			else
				error = Strings::InvalidArguments;
		}
		else
		if (strncmp(arguments[argument], "--timeout=", strlen("--timeout=")) == 0)
		{
			options.timeout = atoi(arguments[argument] + strlen("--timeout="));
		}
		else
		if (strcmp(arguments[argument], "--solutions") == 0)
		{
			if (++argument < numArguments)
				options.solutions = atoi(arguments[argument]);
			else
				error = Strings::InvalidArguments;
		}
		else
		if (strncmp(arguments[argument], "--solutions=", strlen("--solutions=")) == 0)
		{
			options.solutions = atoi(arguments[argument] + strlen("--solutions="));
		}
		else
		if (strcmp(arguments[argument], "--contest") == 0)
		{
			options.contest = true;
		}
		else
		if (strcmp(arguments[argument], "--threads") == 0)
		{
			if (++argument < numArguments)
				options.threads = atoi(arguments[argument]);
			else
				error = Strings::InvalidArguments;
		}
		else
		if (strncmp(arguments[argument], "--threads=", strlen("--threads=")) == 0)
		{
			options.threads = atoi(arguments[argument] + strlen("--threads="));
		}
		else
		if (strcmp(arguments[argument], "--quiet") == 0)
		{
			options.quiet = true;
		}
		else
		if (strcmp(arguments[argument], "--wait") == 0)
		{
			options.wait = true;
		}
		else
		{
			error = Strings::InvalidArguments;
		}
	}

	/* -- Initialize console output -- */

	std::unique_ptr<Console> console(Console::create(strings, options.quiet));
	if (!console)
		return fprintf(stderr, "\n\t\bUnexpected console initialization failure. Aborting.\n\n"), -1;

	/* -- Solve problems -- */

	if (error == Strings::NumErrors)
	{
		if (problems && moves)
		{
			if (!solve(*console, ForsytheString(strings, problems, atoi(moves), ""), options))
				error = Strings::InvalidProblem;
		}
		else
		if (problems)
		{
			if (!solve(strings, *console, problems, options))
				error = Strings::InvalidInputFile;
		}
	}

	/* -- Show error -- */

	const bool failed = (error < Strings::NumErrors);

	if (failed)
	{
		console->displayError(strings[error]);
		console->wait();
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
