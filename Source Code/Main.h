#ifndef __MAIN_H
#define __MAIN_H

/*************************************************************/

void MainStart(unsigned int MemoireDisponible = 0);
bool Main(const char *PositionEPD, unsigned int DemiCoups, bool Continuer, unsigned int ContinuerDe = 0, bool ModeExpress = false);
void MainEnd();

/*************************************************************/

#endif
