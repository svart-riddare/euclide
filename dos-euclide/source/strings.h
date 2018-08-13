#ifndef __STRINGS_H
#define __STRINGS_H

#include "includes.h"

/* -------------------------------------------------------------------------- */

class Strings
{
	public:
		typedef enum { PressAnyKey, NumTexts } Text;
		typedef enum { NoArguments, InvalidArguments, InvalidProblem, InvalidInputFile, UserInterruption, NumErrors } Error;
		typedef enum { ForsytheSymbols, GlyphSymbols, Moves, Dot, Input, Output, Score, Positions, Solution, Colon, NoSolution, UniqueSolution, OneSolution, TwoSolutions, ThreeSolutions, FourSolutions, Cooked, NumStrings } String;

	public:
		Strings();
		
		inline const wchar_t *operator[](Text text) const
			{ return _texts[text]; }
		inline const wchar_t *operator[](Error error) const
			{ return _errors[error]; }
		inline const wchar_t *operator[](String string) const
			{ return _strings[string]; } 

		inline const wchar_t *operator[](EUCLIDE_Status status) const
			{ return _statuses[status]; }
		inline const wchar_t *operator[](EUCLIDE_Message message) const
			{ return _messages[message]; }

	private:
		const wchar_t **_texts;           /**< Strings for each text defined above. */
		const wchar_t **_errors;          /**< Strings for each error defined above. */
		const wchar_t **_strings;         /**< Strings for each string defined above. */
		const wchar_t **_statuses;        /**< Strings for Euclide statuses. */
		const wchar_t **_messages;        /**< Strings for Euclide messages. */
};

/* -------------------------------------------------------------------------- */

#endif
