#include "make-tables-list.h"
#include "make-tables.h"
#include <cstring>
#include <ctime>

// ----------------------------------------------------------------------------

int main(void)
{
	const makefunction *MakeFunction = MakeFunctions;

	while (*MakeFunction)
		(*MakeFunction++)(true);

	return 0;
}

// ----------------------------------------------------------------------------

CodeFile::CodeFile(const char *fileName, const char *include)
{
	const char *pathName = "../../source/tables/";
	char *name = new char[strlen(fileName) + strlen(pathName) + 1];

	strcpy(name, pathName);
	strcat(name, fileName);

	file = fopen(name, "w");
	if (!file)
		file = stdout;

	delete[] name;
	header(include);
}

// ----------------------------------------------------------------------------

CodeFile::~CodeFile()
{
	footer();

	if (file != stdout)
		fclose(file);
}

// ----------------------------------------------------------------------------

CodeFile::operator FILE *()
{
	return file;
}

// ----------------------------------------------------------------------------

void CodeFile::header(const char *include)
{
	fprintf(file, "#include <cstddef>\n\n");
	fprintf(file, "#include \"../enumerations/enumerations.h\"\n");
	fprintf(file, "#include \"../constants/constants.h\"\n");
	fprintf(file, "#include \"../tables/tables.h\"\n");
	if (include)
		fprintf(file, "\n#include \"%s\"\n", include);

	fprintf(file, "\nusing euclide::constants::infinity;\n");
	fprintf(file, "\nnamespace euclide\n{\n");
	fprintf(file, "namespace tables\n{\n");

	time_t now = time(NULL);

	fprintf(file, "\n/* --------------------------------------------------------------------------\n");
	fprintf(file, "   -- Computer generated with make-tables on %s", ctime(&now));
	fprintf(file, "   -------------------------------------------------------------------------- */\n");
	fprintf(file, "\n");
}

// ----------------------------------------------------------------------------

void CodeFile::footer()
{
	comment();

	fprintf(file, "}}\n");
}

// ----------------------------------------------------------------------------

void CodeFile::comment()
{
	fprintf(file, "\n/* -------------------------------------------------------------------------- */\n\n");
}

// ----------------------------------------------------------------------------
