#include "assignations.h"
#include "implications.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Assignation::Assignation(Man man, Color color, int assigned)
{
	_men[man] = true;
	_color = color;
	_assigned = assigned;
}

/* -------------------------------------------------------------------------- */

Assignation::Assignation(Men men, Color color, int assigned)
{
	_men = men;
	_color = color;
	_assigned = assigned;
}

/* -------------------------------------------------------------------------- */

void Assignations::cleanup(const Implications& implications, int (Implications::*assigned)(Man) const)
{
	iterator assignation = begin();
	while (assignation != end())
	{
		int assignedOk = 0;		

		Men men = assignation->men();
		for (Man man = FirstMan; man <= LastMan; man++)
			if (men[man])
				assignedOk += (implications.*assigned)(man);

		if (assignedOk >= assignation->assigned())
			assignation = erase(assignation);
		else
			assignation++;
	}
}

/* -------------------------------------------------------------------------- */

}
