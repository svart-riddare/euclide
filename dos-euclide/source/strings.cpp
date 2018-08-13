#include "strings.h"

/* -------------------------------------------------------------------------- */
/* -- French strings                                                       -- */
/* -------------------------------------------------------------------------- */

static const wchar_t *frenchTexts[] = 
{
	L"Appuyez sur une touche pour continuer...",
};

static const wchar_t *frenchErrors[] =
{
	L"Veuillez pr\xE9" L"ciser un nom de fichier sur la ligne de commande",
	L"Ligne de commande incorrecte",
	L"Le probl\xE8me donn\xE9 en ligne de commande est invalide",
	L"Le fichier \xE0 analyser n'existe pas ou ne contient aucun probl\xE8me",
	L"Analyse interrompue par l'utilisateur"
};

static const wchar_t *frenchStrings[] =
{
	L"RDTFCP|KQRBNP|KDTLSB|RDTACP|KQRBSP",
	L" RrDdTtFfCcPp",
	L"coups",
	L",",
	L"Probl\xE8me analys\xE9 :",
	L"Verdict :",
	L"\xC9" L"chelle de difficult\xE9 =",
	L"Positions examin\xE9" L"es :",
	L"Solution n° ",
	L" :",
	L"Aucune solution",
	L"Solution unique",
	L"Une solution",
	L"Deux solutions",
	L"Trois solutions",
	L"Quatre solutions",
	L"Probl\xE8me d\xE9moli"
};

static const wchar_t *frenchStatuses[] = 
{
	L"Probl\xE8me r\xE9solu",
	L"Probl\xE8me insoluble",
	L"Erreur logicielle, contactez l'auteur",
	L"Pointeur nul, contacter l'auteur",
	L"Erreur syst\xE8me inattendue",
	L"M\xE9moire insuffisante",
	L"Probl\xE8me invalide",
	L"Variante f\xE9\xE9rique non disponible"
};

static const wchar_t *frenchMessages[] =
{
	L"Analyse pr\xE9liminaire...",
	L"Recherche de solutions..."
};

static_assert(countof(frenchTexts) == Strings::NumTexts);
static_assert(countof(frenchErrors) == Strings::NumErrors);
static_assert(countof(frenchStrings) == Strings::NumStrings);
static_assert(countof(frenchStatuses) == EUCLIDE_NUM_STATUSES);
static_assert(countof(frenchMessages) == EUCLIDE_NUM_MESSAGES);

/* -------------------------------------------------------------------------- */
/* -- English strings                                                      -- */
/* -------------------------------------------------------------------------- */

static const wchar_t *englishTexts[] =
{
	L"Press any key to continue..."
};

static const wchar_t *englishErrors[] =
{
	L"Please specify an input file name on the command line",
	L"Invalid command line",
	L"Invalid problem given on command line",
	L"Input file not found or empty",
	L"Solving interrupted"
};

static const wchar_t *englishStrings[] =
{
	L"KQRBNP|RDTFCP|KDTLSB|RDTACP|KQRBSP",
	L" KkQqRrBbNnPp",
	L"moves",
	L".",
	L"Input:",
	L"Result:",
	L"Complexity:",
	L"Positions:",
	L"Solution #",
	L":",
	L"No solution",
	L"Unique proofgame",
	L"One solution",
	L"Two solutions",
	L"Three solutions",
	L"Four solutions",
	L"Multiple solutions"
};

static const wchar_t *englishStatuses[] =
{
	L"Solved",
	L"No solution",
	L"Internal error",
	L"Null pointer",
	L"System error",
	L"Memory error",
	L"Invalid problem",
	L"Unimplemented feature"
};

static const wchar_t *englishMessages[] =
{
	L"Static analysis...",
	L"Searching solutions..."
};

static_assert(countof(englishTexts) == Strings::NumTexts);
static_assert(countof(englishErrors) == Strings::NumErrors);
static_assert(countof(englishStrings) == Strings::NumStrings);
static_assert(countof(englishStatuses) == EUCLIDE_NUM_STATUSES);
static_assert(countof(englishMessages) == EUCLIDE_NUM_MESSAGES);

/* -------------------------------------------------------------------------- */
/* -- Strings implementation                                               -- */
/* -------------------------------------------------------------------------- */

Strings::Strings()
{
	/* -- Select French language if suitable, English otherwise -- */

#ifdef EUCLIDE_WINDOWS
	const bool french = (PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_FRENCH);
#else
	const bool french = false;
#endif

	if (french)
	{
		_texts = frenchTexts;
		_errors = frenchErrors;
		_strings = frenchStrings;
		_statuses = frenchStatuses;
		_messages = frenchMessages;
	}
	else
	{
		_texts = englishTexts;
		_errors = englishErrors;
		_strings = englishStrings;
		_statuses = englishStatuses;
		_messages = englishMessages;
	}
}

/* -------------------------------------------------------------------------- */

