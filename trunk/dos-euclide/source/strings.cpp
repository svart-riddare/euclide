#include "strings.h"

namespace strings
{

typedef struct
{
	int stringId;
	const wchar_t *string;

} string;

/* -------------------------------------------------------------------------- */

#include "resources.h"

/* -------------------------------------------------------------------------- */

const wchar_t *load(int stringId)
{
	int s = 0;
	while (french[s].stringId && (french[s].stringId != stringId))
		s++;

	return french[s].string;
}

/* -------------------------------------------------------------------------- */

const wchar_t *load(EUCLIDE_Status status)
{
	return load(1000 + status);
}

const wchar_t *load(EUCLIDE_Message message)
{
	return load(2000 + message);
}

const wchar_t *load(Text text)
{
	return load(3000 + text);
}

const wchar_t *load(Error error)
{
	return load(4000 + error);
}

const wchar_t *load(Message message)
{
	return load(5000 + message);
}

/* -------------------------------------------------------------------------- */

}
