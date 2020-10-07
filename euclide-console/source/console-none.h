#ifndef __CONSOLE_NONE_H
#define __CONSOLE_NONE_H

#include "includes.h"
#include "console.h"

/* -------------------------------------------------------------------------- */

class NoConsole : public Console
{
	public:
		NoConsole(const Strings& strings);
		virtual ~NoConsole();

		virtual bool wait() override;

	private:
};

/* -------------------------------------------------------------------------- */

#endif
