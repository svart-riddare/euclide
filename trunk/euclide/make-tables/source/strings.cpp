#include "strings.h"

namespace strings
{

/* -------------------------------------------------------------------------- */

const char *glyphs[NumGlyphs] = 
{
	"NoGlyph",
	"WhiteKing",
	"WhiteQueen",
	"WhiteRook",
	"WhiteBishop",
	"WhiteKnight",
	"WhitePawn",
	"BlackKing",
	"BlackQueen",
	"BlackRook",
	"BlackBishop",
	"BlackKnight",
	"BlackPawn",
};

/* -------------------------------------------------------------------------- */

const char *squares[NumSquares] =
{
	"A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
	"B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8",
	"C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8",
	"D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8",
	"E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8",
	"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
	"G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8",
	"H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8",
};

const char *columns[NumColumns] = 
{
	"A", "B", "C", "D", "E", "F", "G", "H"
};

const char *rows[NumRows] =
{
	"1", "2", "3", "4", "5", "6", "7", "8"
};

/* -------------------------------------------------------------------------- */

const char *men[NumSupermen] = 
{
	"King", 
	"Queen", 
	"QueenRook", 
	"KingRook", 
	"QueenBishop", 
	"KingBishop", 
	"QueenKnight", 
	"KingKnight",
	"APawn", "BPawn", "CPawn", "DPawn", "EPawn", "FPawn", "GPawn", "HPawn",
	"AQueen", "BQueen", "CQueen", "DQueen", "EQueen", "FQueen", "GQueen", "HQueen",
	"ARook", "BRook", "CRook", "DRook", "ERook", "FRook", "GRook", "HRook",
	"ABishop", "BBishop", "CBishop", "DBishop", "EBishop", "FBishop", "GBishop", "HBishop",
	"AKnight", "BKnight", "CKnight", "DKnight", "EKnight", "FKnight", "GKnight", "HKnight",
};



/* -------------------------------------------------------------------------- */

}
