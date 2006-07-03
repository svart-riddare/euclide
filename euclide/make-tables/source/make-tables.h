#ifndef __MAKETABLES_H
#define __MAKETABLES_H

#include <cstddef>
#include <cstdio>

using namespace std;

// ----------------------------------------------------------------------------

class CodeFile
{
	public :
		CodeFile(const char *fileName, const char *include = NULL);
		~CodeFile();

		void header(const char *include = NULL);
		void comment();
		void footer();

		operator FILE *();

	private :
		FILE *file;
};

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#endif
