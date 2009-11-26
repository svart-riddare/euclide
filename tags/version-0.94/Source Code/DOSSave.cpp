#include <windows.h>
#include "Position.h"

/*************************************************************/

static bool SauvegardeOk = false;

/*************************************************************/

HKEY EuclideKey() 
{
	HKEY Key;
	DWORD Operation;
	LONG Result = RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Euclide", NULL, "REG_SZ", REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_QUERY_VALUE, NULL, &Key, &Operation);
	
	if (Result != ERROR_SUCCESS)
		return NULL;

	return Key;
}

/*************************************************************/

void Sauvegarde(const diagramme *Diagramme, unsigned int ContinuerDe)
{
	SauvegardeOk = false;

	HKEY Key = EuclideKey();
	if (!Key)
		return;

	if (ContinuerDe)
		ContinuerDe--;

	LONG ReturnA = RegSetValueEx(Key, "Diagramme", NULL, REG_BINARY, (const BYTE *)Diagramme, sizeof(diagramme));
	LONG ReturnB = RegSetValueEx(Key, "Stratégie", NULL, REG_DWORD, (const BYTE *)&ContinuerDe, sizeof(unsigned int));
	RegCloseKey(Key);
	
	if ((ReturnA != ERROR_SUCCESS) || (ReturnB != ERROR_SUCCESS))
		return;

	SauvegardeOk = true;
}

/*************************************************************/

void SetSauvegarde(unsigned int Strategie)
{
	if (!SauvegardeOk)
		return;

	HKEY Key = EuclideKey();
	if (!Key)
		return;

	RegSetValueEx(Key, "Stratégie", NULL, REG_DWORD, (const BYTE *)&Strategie, sizeof(unsigned int));
	RegCloseKey(Key);
}

/*************************************************************/

unsigned int GetSauvegarde(const diagramme *Diagramme) 
{
	HKEY Key = EuclideKey();
	if (!Key)
		return 0;

	unsigned int Strategie = 0;	
	diagramme Sauvegarde;
	
	DWORD SizeofDiagramme = sizeof(diagramme);
	DWORD SizeofUnsignedInt = sizeof(unsigned int);
	LONG ResultA = RegQueryValueEx(Key, "Diagramme", NULL, NULL, (BYTE *)&Sauvegarde, &SizeofDiagramme);
	LONG ResultB = RegQueryValueEx(Key, "Stratégie", NULL, NULL, (BYTE *)&Strategie, &SizeofUnsignedInt);
	RegCloseKey(Key);

	if ((ResultA != ERROR_SUCCESS) || (ResultB != ERROR_SUCCESS))
		return 0;

	if ((SizeofDiagramme != sizeof(diagramme)) || (SizeofUnsignedInt != sizeof(unsigned int)))
		return 0;

	if (memcmp(Diagramme, &Sauvegarde, sizeof(diagramme)) != 0)
		return 0;

	return Strategie + 1;
}

/*************************************************************/

void ClearSauvegarde()
{
	SetSauvegarde(0);
}

/*************************************************************/