#ifndef __RESOURCES_H
#define __RESOURCES_H

/* -------------------------------------------------------------------------- */

static const string french[] =
{
	{ (1000 + EUCLIDE_STATUS_OUT_OF_MEMORY_ERROR),   L"M\xE9moire insuffisante" },
	{ (1000 + EUCLIDE_STATUS_INTERNAL_LOGIC_ERROR),  L"Erreur algorithmique interne - Contactez l'auteur" },
	{ (1000 + EUCLIDE_STATUS_INCORRECT_INPUT_ERROR), L"Probl\xE8me invalide" },
	{ (1000 + EUCLIDE_STATUS_KO),                    L"Probl\xE8me insoluble" },

	{ (2000),                                        L"Analyse pr\xE9liminaire..." },

	{ (3000 + Forsythe),                             L"RDTFCP|KQRBNP|KDTLSB|RDTACP|KQRBSP" },
	{ (3000 + GlyphSymbols),                         L" RDTFCPrdtcfp" },
	{ (3000 + Moves),                                L"coups" },
	{ (3000 + HalfMove),                             L",5" },
	{ (3000 + NoHalfMove),                           L",0" },
	{ (3000 + Input),                                L"Probl\xE8me analys\xE9 :" },
	{ (3000 + Output),                               L"Verdict : " },
	{ (3000 + Score),                                L"\xC9" L"chelle de difficulté =" },


	{ (4000 + NoArguments),                          L"Veuillez pr\xE9" L"ciser un nom de fichier sur la ligne de commande" },
	{ (4000 + InvalidArguments),                     L"Le probl\xE8me donn\xE9 en ligne de commande est invalide" },
	{ (4000 + InvalidInputFile),                     L"Le fichier \xE0 analyser n'existe pas ou ne contient aucun probl\xE8me" },
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
	{ (3000 + GlyphSymbols),                         L" KQRBNPkqrbnp" },
	{ (3000 + Moves),                                L"moves" },
	{ (3000 + HalfMove),                             L".5" },
	{ (3000 + NoHalfMove),                           L".0" },
	{ (3000 + Input),                                L"Input:" },
	{ (3000 + Output),                               L"Result:" },
	{ (3000 + Score),                                L"Score of" },

	{ (4000 + NoArguments),                          L"Please specify an input file name on the command line" },
	{ (4000 + InvalidArguments),                     L"Invalid problem given on command line" },
	{ (4000 + InvalidInputFile),                     L"Input file not found or empty" },
	{ (4000 + UserInterruption),                     L"Solving interrupted" },

	{ (5000 + PressAnyKey),                          L"Press a key to continue..." },

	{ 0, L"" }
};

/* -------------------------------------------------------------------------- */

#endif
