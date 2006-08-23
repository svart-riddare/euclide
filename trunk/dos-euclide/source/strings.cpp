#include "strings.h"

namespace strings
{

static const size_t BufferSize = 256;
static const int  NumBuffers = 5;

/* -------------------------------------------------------------------------- */

LPCTSTR load(int stringId)
{
	static TCHAR buffers[4][256];
	static int index = 0;

	TCHAR *buffer = buffers[index++];
	if (index >= (int)(sizeof(buffers) / sizeof(buffers[0])))
		index = 0;

	buffer[0] = '\0';
	LoadString(NULL, stringId, buffer, sizeof(buffers[0]) / sizeof(buffers[0][0]));
	return buffer;
}

/* -------------------------------------------------------------------------- */

LPCTSTR load(EUCLIDE_Status status)
{
	return load(1000 + status);
}

LPCTSTR load(EUCLIDE_Message message)
{
	return load(2000 + message);
}

LPCTSTR load(Text text)
{
	return load(3000 + text);
}

LPCTSTR load(Error error)
{
	return load(4000 + error);
}

LPCTSTR load(Message message)
{
	return load(5000 + message);
}

/* -------------------------------------------------------------------------- */

}
