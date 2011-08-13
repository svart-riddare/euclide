#include "assignations.h"
#include "implications.h"

namespace euclide
{

/* -------------------------------------------------------------------------- */

Assignation::Assignation()
{
	_color  = UndefinedColor;
	_assigned = infinity;
	_minimum = 0;
}

/* -------------------------------------------------------------------------- */

Assignation::Assignation(Man man, Color color, int assigned)
{
	_men[man] = true;
	_color = color;
	_assigned = assigned;
	_minimum = 0;
}

/* -------------------------------------------------------------------------- */

Assignation::Assignation(Men men, Color color, int assigned)
{
	_men = men;
	_color = color;
	_assigned = assigned;
	_minimum = 0;
}

/* -------------------------------------------------------------------------- */

void Assignation::merge(const Assignation& assignation)
{
	assert(_color == assignation.color());

	if (_men.to_ulong() & assignation.men().to_ulong())
		maximize(_assigned, assignation.assigned());
	else
		_assigned += assignation.assigned();

	_men |= assignation.men();
}

/* -------------------------------------------------------------------------- */

void Assignation::minimum(int minimum)
{
	_minimum = minimum;
	assert(_minimum <= _assigned);
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
