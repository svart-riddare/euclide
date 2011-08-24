#ifndef __STRINGS_H
#define __STRINGS_H

#include "includes.h"

namespace strings
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	Forsythe,
	GlyphSymbols,
	Moves,
	HalfMove,
	NoHalfMove,
	Input,
	Output,
	Score,
	
	NumTexts

} Text;

/* -------------------------------------------------------------------------- */

typedef enum
{
	NoArguments,
	InvalidArguments,
	InvalidInputFile,
	UserInterruption,

	NumErrors

} Error;

/* -------------------------------------------------------------------------- */

typedef enum
{
	PressAnyKey,

	NumMessages

} Message;

/* -------------------------------------------------------------------------- */

const wchar_t *load(int stringId);
const wchar_t *load(EUCLIDE_Status status);
const wchar_t *load(EUCLIDE_Message message);
const wchar_t *load(Text text);
const wchar_t *load(Error error);
const wchar_t *load(Message message);

/* -------------------------------------------------------------------------- */

}

/* -------------------------------------------------------------------------- */

#endif
