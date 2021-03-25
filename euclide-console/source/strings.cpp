#include "strings.h"

/* -------------------------------------------------------------------------- */
/* -- Options                                                              -- */
/* -------------------------------------------------------------------------- */

static const char *options[] =
{
	"Grasshoppers", "Nightriders",
	"Knighted",
	"Alfils", "Camels", "Zebras",
	"Chinese",
	"Monochromatic", "Bichromatic",
	"Grid",
	"Cylinder",
	"Glasgow"
};

static_assert(countof(options) == Strings::NumOptions);

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
	L"RDTFCP|KQRBNP|KDTLSB|RDTACP|KQRBSP|",
	L" RrDdTtFfCcPp",
	L"coups",
	L",",
	L"Probl\xE8me analys\xE9 :",
	L"Verdict :",
	L"\xC9" L"chelle de difficult\xE9 =",
	L"Positions examin\xE9" L"es :",
	L"Solution n\xB0 ",
	L" :",
	L"Aucune solution",
	L"Solution unique",
	L"Une solution",
	L"Deux solutions",
	L"Trois solutions",
	L"Quatre solutions",
	L"Une solution ou plus",
	L"Deux solutions ou plus",
	L"Trois solutions ou plus",
	L"Quatre solutions ou plus",
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
	L"Recherche interrompue",
	L"Probl\xE8me invalide",
	L"Variante f\xE9\xE9rique non disponible"
};

static const wchar_t *frenchMessages[] =
{
	L"Analyse pr\xE9liminaire",
	L"Recherche de solutions",
	L"Recherche exhaustive"
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
	L"KQRBNP|RDTFCP|KDTLSB|RDTACP|KQRBSP|",
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
	L"At least one solution",
	L"At least two solutions",
	L"At least three solutions",
	L"At least four solutions",
	L"Cooked"
};

static const wchar_t *englishStatuses[] =
{
	L"Solved",
	L"No solution",
	L"Internal error",
	L"Null pointer",
	L"System error",
	L"Memory error",
	L"Solving aborted",
	L"Invalid problem",
	L"Unimplemented feature"
};

static const wchar_t *englishMessages[] =
{
	L"Static analysis",
	L"Looking for solutions",
	L"Listing all solutions"
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
		m_texts = frenchTexts;
		m_errors = frenchErrors;
		m_strings = frenchStrings;
		m_statuses = frenchStatuses;
		m_messages = frenchMessages;
	}
	else
	{
		m_texts = englishTexts;
		m_errors = englishErrors;
		m_strings = englishStrings;
		m_statuses = englishStatuses;
		m_messages = englishMessages;
	}

	m_options = options;
}

/* -------------------------------------------------------------------------- */

