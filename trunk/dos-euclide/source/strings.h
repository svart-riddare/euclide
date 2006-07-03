#ifndef __STRINGS_H
#define __STRINGS_H

#include "includes.h"

namespace strings
{

/* -------------------------------------------------------------------------- */

typedef enum
{
	GlyphSymbols,
	Moves,
	HalfMove,
	NoHalfMove,
	
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

LPCTSTR load(int stringId);
LPCTSTR load(EUCLIDE_Status status);
LPCTSTR load(EUCLIDE_Message message);
LPCTSTR load(Text text);
LPCTSTR load(Error error);
LPCTSTR load(Message message);

/* -------------------------------------------------------------------------- */

}

/* -------------------------------------------------------------------------- */

#endif
