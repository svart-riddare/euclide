#ifndef __EUCLIDE_FATE_H
#define __EUCLIDE_FATE_H

#include "includes.h"

namespace euclide 
{

/* -------------------------------------------------------------------------- */

class FinalSquare
{
	public :

	private :
		Square square;
		Man man;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

class Fate
{
	public :
		Fate();

	protected :	
		
	private :
		vector<FinalSquare> squares;
		tribool isCaptured;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

class Fates
{
	public :
		Fates(const Problem& problem, Color color);

	private :
		array<Fate, NumMen> fates;
		Color color;

		int requiredMoves;
		int requiredCaptures;
};

/* -------------------------------------------------------------------------- */

}

#endif
