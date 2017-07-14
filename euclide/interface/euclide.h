#ifndef __EUCLIDE_H
#define __EUCLIDE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_STATUS_OK,         /**< One or more solutions found. */
	EUCLIDE_STATUS_KO,         /**< No solutions found. */
	EUCLIDE_STATUS_BUG,        /**< Unexpected bug. May only happen in debug builds. */
	EUCLIDE_STATUS_NULL,       /**< Null pointer error. */
	EUCLIDE_STATUS_ERROR,      /**< Unlikely system error. */
	EUCLIDE_STATUS_MEMORY,     /**< Out of memory error. */
	EUCLIDE_STATUS_INVALID,    /**< Invalid problem. */

	EUCLIDE_NUM_STATUSES       /**< Number of different enumerated values. */

} EUCLIDE_Status;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_GLYPH_NONE,

	EUCLIDE_GLYPH_WHITE_KING,
	EUCLIDE_GLYPH_BLACK_KING,
	EUCLIDE_GLYPH_WHITE_QUEEN,
	EUCLIDE_GLYPH_BLACK_QUEEN,
	EUCLIDE_GLYPH_WHITE_ROOK,
	EUCLIDE_GLYPH_BLACK_ROOK,
	EUCLIDE_GLYPH_WHITE_BISHOP,
	EUCLIDE_GLYPH_BLACK_BISHOP,
	EUCLIDE_GLYPH_WHITE_KNIGHT,
	EUCLIDE_GLYPH_BLACK_KNIGHT,
	EUCLIDE_GLYPH_WHITE_PAWN,
	EUCLIDE_GLYPH_BLACK_PAWN,

	EUCLIDE_NUM_GLYPHS

} EUCLIDE_Glyph;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_PIECE_NONE,

	/* -- Standard pieces -- */

	EUCLIDE_PIECE_KING,
	EUCLIDE_PIECE_QUEEN,
	EUCLIDE_PIECE_ROOK,
	EUCLIDE_PIECE_BISHOP,
	EUCLIDE_PIECE_KNIGHT,
	EUCLIDE_PIECE_PAWN,

	/* -- Fairy pieces -- */

	EUCLIDE_PIECE_GRASSHOPPER,
	EUCLIDE_PIECE_NIGHTRIDER,

	/* -- Knighted pieces -- */

	EUCLIDE_PIECE_AMAZON,         /**< Knighted queen. */
	EUCLIDE_PIECE_EMPRESS,        /**< Knighted rook. */
	EUCLIDE_PIECE_PRINCESS,       /**< Knighted bishop. */

	/* -- Leapers -- */

	EUCLIDE_PIECE_ALFIL,          /**< (2,2) leaper. */
	EUCLIDE_PIECE_CAMEL,          /**< (1,3) leaper. */
	EUCLIDE_PIECE_ZEBRA,          /**< (2,3) leaper. */

	/* -- Chinese pieces -- */

	EUCLIDE_PIECE_LEO,            /**< Chinese queen. */
	EUCLIDE_PIECE_PAO,            /**< Chinese rook (cannon). */
	EUCLIDE_PIECE_VAO,            /**< Chinese bishop. */
	EUCLIDE_PIECE_MAO,            /**< Chinese knight (horse). */

	/* -- Number of different pieces -- */
	
	EUCLIDE_NUM_PIECES

} EUCLIDE_Piece;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_VARIANT_NONE,              /**< Orthodoc chess. */

	/* -- Move restriction variants -- */

	EUCLIDE_VARIANT_MONOCHROMATIC,     /**< All moves that between dark and light squares are forbidden. Implies that knignts can not move. */
	EUCLIDE_VARIANT_BICHROMATIC,       /**< All moves should be performed between dark and light squares. Implies that bishop can not move. */

	EUCLIDE_VARIANT_GRID_CHESS,        /**< Grid chess. Board is divided in 2x2 zones, every move should go from one zone to another. */

	/* -- Board shape variants -- */

	EUCLIDE_VARIANT_CYLINDER_CHESS,    /**< Vertical cylindric board. Null moves are allowed, as well as extra castling moves. */

	/* -- Number of variants -- */

	EUCLIDE_NUM_VARIANTS

} EUCLIDE_Variant;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph initial[64];                   /**< Initial position. Standard initial position if board is empty. Squares are ordered as follow: A1, A2, ..., H7, H8. */
	bool blackToMove;                            /**< If set, black should move first. */

	EUCLIDE_Glyph diagram[64];                   /**< Target diagram position. Squares are ordered as follow: A1, A2, ..., H7, H8. */
	int numHalfMoves;                            /**< Number of half moves to reach diagram position from initial one. Zero is interpreted as unspecified. */

	bool forbidWhiteKingSideCastling;            /**< White king side castling should be forbidden. */
	bool forbidBlackKingSideCastling;            /**< Black king side castling should be forbidden. */
	bool forbidWhiteQueenSideCastling;           /**< White queen side castling should be forbidden. */
	bool forbidBlackQueenSideCastling;           /**< Black king side castling should be forbidden. */

	EUCLIDE_Piece pieces[EUCLIDE_NUM_GLYPHS];    /**< Fairy pieces. EUCLIDE_PIECE_NONE is replaced by the orthodox piece. Kings shall not be fairy. Promotion is restricted to these pieces. */

} EUCLIDE_Problem;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_MESSAGE_ANALYZING,                   /**< Analyzing problem. */

	EUCLIDE_NUM_MESSAGES                         /**< Number of different enumerated values. */

} EUCLIDE_Message;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph initialGlyph;
	EUCLIDE_Glyph promotionGlyph;

	int initialSquare;
	int finalSquare;
	
	int requiredMoves;
	int numSquares;
	int numMoves;

	bool captured;

} EUCLIDE_Deduction;

typedef struct
{
	EUCLIDE_Deduction whitePieces[16];
	EUCLIDE_Deduction blackPieces[16];

	int freeWhiteMoves;
	int freeBlackMoves;

	double complexity;

} EUCLIDE_Deductions;

/* -------------------------------------------------------------------------- */

typedef void *EUCLIDE_UserHandle;

typedef void (*EUCLIDE_DisplayCopyrightFunction)(EUCLIDE_UserHandle handle, const wchar_t *copyright);
typedef void (*EUCLIDE_DisplayProblemFunction)(EUCLIDE_UserHandle handle, const EUCLIDE_Problem *problem);
typedef void (*EUCLIDE_DisplayMessageFunction)(EUCLIDE_UserHandle handle, EUCLIDE_Message message);
typedef void (*EUCLIDE_DisplayProgressFunction)(EUCLIDE_UserHandle handle, int whiteFreeMoves, int blackFreeMoves, double complexity);
typedef void (*EUCLIDE_DisplayDeductionsFunction)(EUCLIDE_UserHandle handle, const EUCLIDE_Deductions *deductions);

typedef struct
{
	EUCLIDE_DisplayCopyrightFunction displayCopyright;
	EUCLIDE_DisplayProblemFunction displayProblem;
	EUCLIDE_DisplayMessageFunction displayMessage;
	EUCLIDE_DisplayProgressFunction displayProgress;
	EUCLIDE_DisplayDeductionsFunction displayDeductions;

	EUCLIDE_UserHandle handle;

} EUCLIDE_Callbacks;

/* -------------------------------------------------------------------------- */

typedef struct
{
	int reserved;

} EUCLIDE_Configuration;

/* -------------------------------------------------------------------------- */

typedef struct EUCLIDE_Structure *EUCLIDE_Handle;

EUCLIDE_Status EUCLIDE_initialize(EUCLIDE_Handle *euclide, const EUCLIDE_Configuration *configuration, const EUCLIDE_Callbacks *callbacks);
EUCLIDE_Status EUCLIDE_problem(EUCLIDE_Handle euclide, const EUCLIDE_Problem *problem);
EUCLIDE_Status EUCLIDE_done(EUCLIDE_Handle euclide);

EUCLIDE_Status EUCLIDE_solve(const EUCLIDE_Configuration *configuration, const EUCLIDE_Problem *problem, const EUCLIDE_Callbacks *callbacks);

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif
