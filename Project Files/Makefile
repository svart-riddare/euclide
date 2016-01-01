
CC = g++
DEBUG = -g
CFLAGS = -Wall -O3 -DNDEBUG -I/usr/include/ncursesw
LFLAGS = -lncursesw
TARGET = euclide

SOURCE = ../Source\ Code
OUTPUT = ..

FILES = \
	Constantes.cpp \
	Coups.cpp \
	Epd.cpp \
	Erreur.cpp \
	Euclide.cpp \
	HashTables.cpp \
	Main.cpp \
	Mouvements.cpp \
	Output.cpp \
	Partie.cpp \
	Permutation.cpp \
	Position.cpp \
	Timer.cpp \
	UnixOutput.cpp \
	UnixSave.cpp \
	UnixSystem.cpp

HEADERS = \
	Constantes.h \
	Coups.h \
	Entiers.h \
	Epd.h \
	Erreur.h \
	HashTables.h \
	Main.h \
	Mouvements.h \
	Output.h \
	Partie.h \
	Permutation.h \
	Position.h \
	Save.h \
	System.h \
	Timer.h \
	Version.h

OBJECTS = $(patsubst %.cpp, %.o, $(FILES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $(OUTPUT)/$(TARGET)

$(OBJECTS) : %.o : $(SOURCE)/%.cpp
	$(CC) $(CFLAGS) -c "$<" -o $@ 

clean:
	\rm -f *.o $(OUTPUT)/$(TARGET)

