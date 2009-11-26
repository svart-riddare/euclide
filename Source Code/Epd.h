#ifndef __EPD_H
#define __EPD_H

#include "Constantes.h"
#include "Position.h"

/*************************************************************/

diagramme *EPDToDiagramme(const char *EPD, unsigned int DemiCoups, bool Afficher = true);
void Delete(diagramme *Diagramme);

/*************************************************************/

#endif
