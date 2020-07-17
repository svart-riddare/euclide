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
	EUCLIDE_STATUS_OK,               /**< One or more solutions found. */
	EUCLIDE_STATUS_KO,               /**< No solutions found. */
	EUCLIDE_STATUS_BUG,              /**< Unexpected bug. May only happen in debug builds. */
	EUCLIDE_STATUS_NULL,             /**< Null pointer error. */
	EUCLIDE_STATUS_ERROR,            /**< Unlikely system error. */
	EUCLIDE_STATUS_MEMORY,           /**< Out of memory error. */
	EUCLIDE_STATUS_ABORTED,          /**< Solving aborted. */
	EUCLIDE_STATUS_INVALID,          /**< Invalid problem. */
	EUCLIDE_STATUS_UNIMPLEMENTED,    /**< Unimplemented fairy piece or chess variant. */

	EUCLIDE_NUM_STATUSES             /**< Number of different enumerated values. */

} EUCLIDE_Status;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_GLYPH_NONE,            /**< Empty square. */

	EUCLIDE_GLYPH_WHITE_KING,      /**< White king symbol. Represents a standard king. */
	EUCLIDE_GLYPH_BLACK_KING,      /**< Black king symbol. Represents a standard king. */
	EUCLIDE_GLYPH_WHITE_QUEEN,     /**< White queen symbol. Represents a queen or a queen-like fairy piece. */
	EUCLIDE_GLYPH_BLACK_QUEEN,     /**< Black queen symbol. Represents a queen or a queen-like fairy piece. */
	EUCLIDE_GLYPH_WHITE_ROOK,      /**< White rook symbol. Represents a rook or a rook-like fairy piece. May castle with the king. */
	EUCLIDE_GLYPH_BLACK_ROOK,      /**< Black rook symbol. Represents a rook or a rook-like fairy piece. May castle with the king. */
	EUCLIDE_GLYPH_WHITE_BISHOP,    /**< White bishop symbol. Represents a bishop or a bishop-like fairy piece. */
	EUCLIDE_GLYPH_BLACK_BISHOP,    /**< Black bishop symbol. Represents a bishop or a bishop-like fairy piece. */
	EUCLIDE_GLYPH_WHITE_KNIGHT,    /**< White knight symbol. Represents a knight or a knight-like fairy piece. */
	EUCLIDE_GLYPH_BLACK_KNIGHT,    /**< Black knight symbol. Represents a knight or a knight-like fairy piece. */
	EUCLIDE_GLYPH_WHITE_PAWN,      /**< White pawn symbol. Represents a standard pawn. Promotes to any queen, rook, bishop or knight pieces upon reaching eight rank. */
	EUCLIDE_GLYPH_BLACK_PAWN,      /**< Black pawn symbol. Represents a standard pawn. Promotes to any queen, rook, bishop or knight pieces upon reaching first rank. */

	EUCLIDE_NUM_GLYPHS             /**< Number of different glyphs. */

} EUCLIDE_Glyph;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_PIECE_NONE,           /**< No piece. */

	/* -- Standard pieces -- */

	EUCLIDE_PIECE_KING,           /**< Standard king. */
	EUCLIDE_PIECE_QUEEN,          /**< Standard queen. */
	EUCLIDE_PIECE_ROOK,           /**< Standard rook. */
	EUCLIDE_PIECE_BISHOP,         /**< Standard bishop. */
	EUCLIDE_PIECE_KNIGHT,         /**< Standard knight. */
	EUCLIDE_PIECE_PAWN,           /**< Standard pawn. */

	/* -- Fairy pieces -- */

	EUCLIDE_PIECE_GRASSHOPPER,    /**< Grasshopper classic fairy piece. */
	EUCLIDE_PIECE_NIGHTRIDER,     /**< Nighrider classic fairy piece. */

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

	EUCLIDE_NUM_PIECES            /**< Number of different enumerated values. */

} EUCLIDE_Piece;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_VARIANT_NONE,              /**< Orthodox chess. */

	/* -- Move restriction variants -- */

	EUCLIDE_VARIANT_MONOCHROMATIC,     /**< All moves between dark and light squares are forbidden. Implies that knignts can not move. */
	EUCLIDE_VARIANT_BICHROMATIC,       /**< All moves should be performed between dark and light squares. Implies that bishop can not move. */

	EUCLIDE_VARIANT_GRID_CHESS,        /**< Grid chess. Board is divided in 2x2 zones, every move should go from one zone to another. */

	/* -- Board shape variants -- */

	EUCLIDE_VARIANT_CYLINDER_CHESS,    /**< Vertical cylindric board. Null moves are allowed, as well as extra castling moves. */

	/* -- Promotion variants -- */

	EUCLIDE_VARIANT_GLASGOW_CHESS,     /**< Promotions occur on the 7th (2nd) rank. */

	/* -- Number of variants -- */

	EUCLIDE_NUM_VARIANTS               /**< Number of different variants. */

} EUCLIDE_Variant;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph initial[64];                   /**< Initial position. Standard initial position if board is empty. Squares are ordered as follow: A1, A2, ..., H7, H8. */
	bool blackToMove;                            /**< If set, black should move first. */

	EUCLIDE_Glyph diagram[64];                   /**< Target diagram position. Squares are ordered as follow: A1, A2, ..., H7, H8. No more than thirty-two pieces of each color, including exactly one king. */
	int numHalfMoves;                            /**< Number of half moves to reach diagram position from initial one. Zero is interpreted as unspecified. */

	bool forbidWhiteKingSideCastling;            /**< White king side castling should be forbidden. */
	bool forbidBlackKingSideCastling;            /**< Black king side castling should be forbidden. */
	bool forbidWhiteQueenSideCastling;           /**< White queen side castling should be forbidden. */
	bool forbidBlackQueenSideCastling;           /**< Black king side castling should be forbidden. */

	EUCLIDE_Variant variant;                     /**< Chess variant. */
	EUCLIDE_Piece pieces[EUCLIDE_NUM_GLYPHS];    /**< Fairy pieces. EUCLIDE_PIECE_NONE is replaced by the orthodox piece. Kings shall not be fairy. Promotion is restricted to these pieces. */

} EUCLIDE_Problem;

/* -------------------------------------------------------------------------- */

typedef enum
{
	EUCLIDE_MESSAGE_ANALYZING,                   /**< Analyzing problem. */
	EUCLIDE_MESSAGE_SEARCHING,                   /**< Searching for solutions. */

	EUCLIDE_NUM_MESSAGES                         /**< Number of different enumerated values. */

} EUCLIDE_Message;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph initialGlyph;           /**< Glyph, as given by user. */
	EUCLIDE_Glyph diagramGlyph;           /**< Glyph after promotion, for pawns only, if known. */

	int initialSquare;                    /**< Initial square, as given by user. A1 = 0, A2 = 1, ... */
	int finalSquare;                      /**< Final square, if known, -1 otherwise. */

	int requiredMoves;                    /**< Minimum number of moves required to reach final square, if known. */
	int numSquares;                       /**< Number of different possible final squares for this piece. One if final square is known. */
	int numMoves;                         /**< Number of different legal moves this piece may have performed on the board. At most number of required moves if the full path is known. */

	bool captured;                        /**< Set if the piece has been captured and is no longer present on the board. */

} EUCLIDE_Deduction;

typedef struct
{
	EUCLIDE_Deduction whitePieces[32];    /**< White piece deductions. */
	EUCLIDE_Deduction blackPieces[32];    /**< Black piece deductions. */

	int numWhitePieces;                   /**< Number of white pieces in above array. */
	int numBlackPieces;                   /**< Number of black pieces in above array. */

	int freeWhiteMoves;                   /**< Number of white moves not yet assigned to specific pieces. */
	int freeBlackMoves;                   /**< Number of black moves not yet assigned to specific pieces. */

	double complexity;                    /**< Abstract problem complexity. */

} EUCLIDE_Deductions;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Glyph glyph;                  /**< Glyph performing the move. */
	EUCLIDE_Glyph promotion;              /**< Promotion glyph, otherwise equal to initial glyph. */
	EUCLIDE_Glyph captured;               /**< Captured glyph. */

	int move;                             /**< Move index, starting from 1, same index for black and white moves. */
	int from;                             /**< Move departure square, from 0 (A1) to 63 (H8). */
	int to;                               /**< Move arrival square, from 0 (A1) to 63 (H8). */

	bool capture;                         /**< Set if an opponent piece was captured. */
	bool enpassant;                       /**< Set if capture is "en passant". */
	bool check;                           /**< Set if move is a check. */
	bool mate;                            /**< Set if move gives mate. */
	bool kingSideCastling;                /**< Set if move is castling, king side. */
	bool queenSideCastling;               /**< Set if move is castling, queen side. */

} EUCLIDE_Move;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Move moves[8];                /**< Moves being explored (first eight move of game). */
	int numHalfMoves;                     /**< Number of moves in above array. */

	int64_t positions;                    /**< Number of positions examined. */

} EUCLIDE_Thinking;

/* -------------------------------------------------------------------------- */

typedef struct
{
	EUCLIDE_Move moves[128];              /**< Moves played. */
	int numHalfMoves;                     /**< Number of moves in above array. */

	int solution;                         /**< Number of solutions found so far, counting from 1. */

} EUCLIDE_Solution;

/* -------------------------------------------------------------------------- */

typedef void *EUCLIDE_UserHandle;

typedef void (*EUCLIDE_DisplayCopyrightFunction)(EUCLIDE_UserHandle handle, const wchar_t *copyright);
typedef void (*EUCLIDE_DisplayProblemFunction)(EUCLIDE_UserHandle handle, const EUCLIDE_Problem *problem);
typedef void (*EUCLIDE_DisplayMessageFunction)(EUCLIDE_UserHandle handle, EUCLIDE_Message message);
typedef void (*EUCLIDE_DisplayProgressFunction)(EUCLIDE_UserHandle handle, int whiteFreeMoves, int blackFreeMoves, double complexity);
typedef void (*EUCLIDE_DisplayDeductionsFunction)(EUCLIDE_UserHandle handle, const EUCLIDE_Deductions *deductions);
typedef void (*EUCLIDE_DisplayThinkingFunction)(EUCLIDE_UserHandle handle, const EUCLIDE_Thinking *thinking);
typedef void (*EUCLIDE_DisplaySolutionFunction)(EUCLIDE_UserHandle handle, const EUCLIDE_Solution *solution);

typedef bool (*EUCLIDE_AbortFunction)(EUCLIDE_UserHandle handle);

typedef struct
{
	EUCLIDE_DisplayCopyrightFunction displayCopyright;
	EUCLIDE_DisplayProblemFunction displayProblem;
	EUCLIDE_DisplayMessageFunction displayMessage;
	EUCLIDE_DisplayProgressFunction displayProgress;
	EUCLIDE_DisplayDeductionsFunction displayDeductions;
	EUCLIDE_DisplayThinkingFunction displayThinking;
	EUCLIDE_DisplaySolutionFunction displaySolution;

	EUCLIDE_AbortFunction abort;

	EUCLIDE_UserHandle handle;

} EUCLIDE_Callbacks;

/* -------------------------------------------------------------------------- */

typedef struct
{
	int maxSolutions;                     /**< Solving stops (returns OK) when reaching this number of solutions. Zero means unlimited searching. */

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
