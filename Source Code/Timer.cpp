#include <math.h>
#include <stdio.h>
#include <time.h>
#include "Timer.h"

static clock_t TimerStart = (clock_t)(-1);

/*************************************************************/

void StartTimer()
{
	TimerStart = clock();
}

/*************************************************************/

const char *GetElapsedTime()
{
	clock_t Timer = clock();

	double Seconds = 0.0;
	double Elapsed = (double)(Timer - TimerStart) / CLOCKS_PER_SEC;

	if (Elapsed < 0.0)
		return "?.??";

	if ((Timer == (clock_t)(-1)) || (TimerStart == (clock_t)(-1)))
		return "?.??";

	unsigned int Centiemes = (unsigned int)floor(100.0 * modf(Elapsed, &Seconds));
	unsigned int Secondes = (unsigned int)Seconds % 60;
	unsigned int Minutes = (unsigned int)floor(Seconds / 60) % 60;
	unsigned int Heures = (unsigned int)floor(Seconds / 3600);

	if (Heures > (24 * 366))
		return "-:--:--.--";

	static char Tampon[32];

	if (Heures > 0) {
		sprintf(Tampon, "%u:%02u:%02u.%02u", Heures, Minutes, Secondes, Centiemes);
	} 
	else if (Minutes > 0) {
		sprintf(Tampon, "%u:%02u.%02u", Minutes, Secondes, Centiemes);
	}
	else {
		sprintf(Tampon, "%u.%02u", Secondes, Centiemes);
	}

	return Tampon;
}

/*************************************************************/