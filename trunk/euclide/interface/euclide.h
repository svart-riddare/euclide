#ifndef __EUCLIDE_H
#define __EUCLIDE_H

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
	#define _Bool unsigned char
#endif

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_STATUS_OK,

	EUCLIDE_STATUS_INCORRECT_INPUT_ERROR = -1,
	EUCLIDE_STATUS_INTERNAL_LOGIC_ERROR = -2,
	EUCLIDE_STATUS_OUT_OF_MEMORY_ERROR = -3,

	EUCLIDE_STATUS_UNKNOWN_ERROR = -99,

} EUCLIDE_Status;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_GLYPH_NONE,
	EUCLIDE_GLYPH_WHITE_KING,
	EUCLIDE_GLYPH_WHITE_QUEEN,
	EUCLIDE_GLYPH_WHITE_ROOK,
	EUCLIDE_GLYPH_WHITE_BISHOP,
	EUCLIDE_GLYPH_WHITE_KNIGHT,
	EUCLIDE_GLYPH_WHITE_PAWN,
	EUCLIDE_GLYPH_BLACK_KING,
	EUCLIDE_GLYPH_BLACK_QUEEN,
	EUCLIDE_GLYPH_BLACK_ROOK,
	EUCLIDE_GLYPH_BLACK_BISHOP,
	EUCLIDE_GLYPH_BLACK_KNIGHT,
	EUCLIDE_GLYPH_BLACK_PAWN,

	EUCLIDE_NUM_GLYPHS

} EUCLIDE_Glyph;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_NUM_MESSAGES

} EUCLIDE_Message;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph glyphs[64];
	int numHalfMoves;

} EUCLIDE_Problem;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph initialGlyph;
	EUCLIDE_Glyph promotionGlyph;

	int initialSquare;
	int finalSquare;
	
	int requiredMoves;
	int numSquares;

	_Bool captured;

} EUCLIDE_Deduction;

typedef struct
{
	EUCLIDE_Deduction whitePieces[16];
	EUCLIDE_Deduction blackPieces[16];

	int freeWhiteMoves;
	int freeBlackMoves;

} EUCLIDE_Deductions;

/* -------------------------------------------------------------------------- */

typedef void *EUCLIDE_Handle;

typedef void (*EUCLIDE_DisplayCopyrightFunction)(EUCLIDE_Handle handle, const wchar_t *copyright);
typedef void (*EUCLIDE_DisplayProblemFunction)(EUCLIDE_Handle handle, const EUCLIDE_Problem *problem);
typedef void (*EUCLIDE_DisplayMessageFunction)(EUCLIDE_Handle handle, EUCLIDE_Message message);
typedef void (*EUCLIDE_DisplayFreeMovesFunction)(EUCLIDE_Handle handle, int whiteFreeMoves, int blackFreeMoves);
typedef void (*EUCLIDE_DisplayDeductionsFunction)(EUCLIDE_Handle handle, const EUCLIDE_Deductions *deductions);

typedef struct
{
	EUCLIDE_DisplayCopyrightFunction displayCopyright;
	EUCLIDE_DisplayProblemFunction displayProblem;
	EUCLIDE_DisplayMessageFunction displayMessage;
	EUCLIDE_DisplayFreeMovesFunction displayFreeMoves;
	EUCLIDE_DisplayDeductionsFunction displayDeductions;

	EUCLIDE_Handle handle;

} EUCLIDE_Callbacks;

/* -------------------------------------------------------------------------- */

typedef struct
{
	int reserved;

} EUCLIDE_Configuration;

/* -------------------------------------------------------------------------- */

EUCLIDE_Status EUCLIDE_solve(const EUCLIDE_Configuration *pConfiguration, const EUCLIDE_Problem *problem, const EUCLIDE_Callbacks *pCallbacks);

/* -------------------------------------------------------------------------- */

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
	#undef _Bool
#endif

#ifdef __cplusplus
}
#endif

#endif
