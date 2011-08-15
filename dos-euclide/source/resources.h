#ifndef __RESOURCES_H
#define __RESOURCES_H

/* -------------------------------------------------------------------------- */

static const string french[] =
{
	{ (1000 + EUCLIDE_STATUS_OUT_OF_MEMORY_ERROR),   L"Mémoire insuffisante" },
	{ (1000 + EUCLIDE_STATUS_INTERNAL_LOGIC_ERROR),  L"Erreur algorithmique interne - Contactez l'auteur" },
	{ (1000 + EUCLIDE_STATUS_INCORRECT_INPUT_ERROR), L"Problème invalide" },
	{ (1000 + EUCLIDE_STATUS_KO),                    L"Problème insoluble" },

	{ (2000),                                        L"Analyse préliminaire..." },

	{ (3000 + Forsythe),                             L"RDTFCP|KQRBNP|KDTLSB|RDTACP|KQRBSP" },
	{ (3000 + GlyphSymbols),                         L" RDTFCPRDTFCP" },
	{ (3000 + Moves),                                L"coups" },
	{ (3000 + HalfMove),                             L",5" },
	{ (3000 + NoHalfMove),                           L",0" },

	{ (4000 + NoArguments),                          L"Veuillez préciser un nom de fichier sur la ligne de commande" },
	{ (4000 + InvalidArguments),                     L"Le problème donné en ligne de commande est invalide" },
	{ (4000 + InvalidInputFile),                     L"Le fichier à analyser n'existe pas ou ne contient aucun problème" },
	{ (4000 + UserInterruption),                     L"Analyse interrompue par l'utilisateur" },

	{ (5000 + PressAnyKey),                          L"Appuyez sur une touche pour continuer..." },

	{ 0, L"" }
};

/* -------------------------------------------------------------------------- */

static const string english[] =
{
	{ (1000 + EUCLIDE_STATUS_OUT_OF_MEMORY_ERROR),   L"Out of memory" },
	{ (1000 + EUCLIDE_STATUS_INTERNAL_LOGIC_ERROR),  L"Internal error" },
	{ (1000 + EUCLIDE_STATUS_INCORRECT_INPUT_ERROR), L"Invalid problem" },
	{ (1000 + EUCLIDE_STATUS_KO),                    L"No solution" },

	{ (2000),                                        L"Static analysis..." },

	{ (3000 + Forsythe),                             L"KQRBNP|RDTFCP|KDTLSB|RDTACP|KQRBSP" },
	{ (3000 + GlyphSymbols),                         L" KQRBNPKQRBNP" },
	{ (3000 + Moves),                                L"moves" },
	{ (3000 + HalfMove),                             L".5" },
	{ (3000 + NoHalfMove),                           L".0" },

	{ (4000 + NoArguments),                          L"Please specify an input file name on the command line" },
	{ (4000 + InvalidArguments),                     L"Invalid problem given on command line" },
	{ (4000 + InvalidInputFile),                     L"Input file not found or empty" },
	{ (4000 + UserInterruption),                     L"Solving interrupted" },

	{ (5000 + PressAnyKey),                          L"Press a key to continue..." },

	{ 0, L"" }
};

/* -------------------------------------------------------------------------- */

#endif
