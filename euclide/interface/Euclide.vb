Imports System.Runtime.InteropServices

REM ----------------------------------------------------------------------------
REM -- See 'euclide.h' for documentation
REM ----------------------------------------------------------------------------

Friend Class Euclide
    Public Enum Status As Integer
        Ok
        NoSolutionFound
        InternalBug
        NullPointer
        SystemError
        OutOfMemory
        SolvingAborted
        InvalidProblem
        UnimplementedFeature
        N
    End Enum

    Public Enum Glyph As Integer
        None
        WhiteKing
        BlackKing
        WhiteQueen
        BlackQueen
        WhiteRook
        BlackRook
        WhiteBishop
        BlackBishop
        WhiteKnight
        BlackKnight
        WhitePawn
        BlackPawn
        N
    End Enum

    Public Enum Piece As Integer
        None
        King
        Queen
        Rook
        Bishop
        Knight
        Pawn
        Grasshopper
        Nightrider
        Amazon
        Empress
        Princess
        Alfil
        Camel
        Zebra
        Leo
        Pao
        Vao
        Mao
        N
    End Enum

    Public Enum Variation As Integer
        None
        Monochromatic
        Bichromatic
        GridChess
        CylinderChess
        GlasgowChess
        N
    End Enum

    Public Structure Problem
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=64)> Public InitialPosition() As Glyph
        Public BlackToMove As Boolean

        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=64)> Public DiagramPosition() As Glyph
        Public NumHalfMoves As Integer

        <MarshalAs(UnmanagedType.I1)> Public ForbidWhiteKingSideCastling As Boolean
        <MarshalAs(UnmanagedType.I1)> Public ForbidBlackKingSideCastling As Boolean
        <MarshalAs(UnmanagedType.I1)> Public ForbidWhiteQueenSideCastling As Boolean
        <MarshalAs(UnmanagedType.I1)> Public ForbidBlackQueenSideCastling As Boolean

        Public Variation As Variation
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=13)> Public PieceTypes() As Piece

    End Structure

    Public Enum Message As Integer
        Analyzing
        Searching
        Exhausting
        N
    End Enum

    Public Structure Location
        Public Glyph As Glyph
        Public Square As Integer
    End Structure

    Public Structure Deduction
        Public InitialSquare As Location
        Public FinalSquare As Location

        Public Promoted As Boolean
        Public PromotionSquare As Location

        Public Captured As Boolean
        Public CaptureSquare As Location

        Public RequiredMoves As Integer
        Public NumSquares As Integer
        Public NumMoves As Integer
    End Structure

    Public Structure Deductions
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> Public WhitePieces() As Deduction
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> Public BlackPieces() As Deduction

        Public NumWhitePieces As Integer
        Public NumBlackPieces As Integer

        Public FreeWhiteMoves As Integer
        Public FreeBlackMoves As Integer

        Public Complexity As Double
    End Structure

    Public Structure Move
        Public Glyph As Glyph
        Public Promotion As Glyph
        Public Captured As Glyph

        Public Move As Integer
        Public From As Integer
        Public Square As Integer

        <MarshalAs(UnmanagedType.I1)> Public Capture As Boolean
        <MarshalAs(UnmanagedType.I1)> Public EnPassant As Boolean
        <MarshalAs(UnmanagedType.I1)> Public Check As Boolean
        <MarshalAs(UnmanagedType.I1)> Public Mate As Boolean
        <MarshalAs(UnmanagedType.I1)> Public KingSideCastling As Boolean
        <MarshalAs(UnmanagedType.I1)> Public QueenSideCastling As Boolean
    End Structure

    Public Structure Thinking
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> Public Moves() As Move
        Public NumHalfMoves As Integer

        Public Positions As Long
    End Structure

    Public Structure Solution
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=128)> Public Moves() As Move
        Public NumHalfMoves As Integer

        Public Solution As Integer
    End Structure

    Public Structure Options
        Public SolvingContest As Boolean
        Public MaxSolutions As Integer
    End Structure

    Public Structure Callbacks
        Public Delegate Sub DisplayCopyrightFunction(ByVal Id As ULong, <MarshalAs(UnmanagedType.LPWStr)> ByVal Copyright As String)
        Public DisplayCopyright As DisplayCopyrightFunction
        Public Delegate Sub DisplayOptionsFunction(ByVal Id As ULong, ByRef Options As Options)
        Public DisplayOptions As DisplayOptionsFunction
        Public Delegate Sub DisplayProblemFunction(ByVal Id As ULong, ByRef Problem As Problem)
        Public DisplayProblem As DisplayProblemFunction
        Public Delegate Sub DisplayMessageFunction(ByVal Id As ULong, Message As Message)
        Public DisplayMessage As DisplayMessageFunction
        Public Delegate Sub DisplayDeductionsFunction(ByVal Id As ULong, ByRef Deductions As Deductions)
        Public DisplayDeductions As DisplayDeductionsFunction
        Public Delegate Sub DisplayThinkingFunction(ByVal Id As ULong, ByRef Thinking As Thinking)
        Public DisplayThinking As DisplayThinkingFunction
        Public Delegate Sub DisplaySolutionFunction(ByVal Id As ULong, ByRef Solution As Solution)
        Public DisplaySolution As DisplaySolutionFunction

        Public Delegate Function AbortFunction(ByVal Id As ULong) As Boolean
        Public Abort As AbortFunction

        Public Id As ULong
    End Structure

    Friend Declare Auto Function Solve Lib "euclide.dll" Alias "EUCLIDE_solve" (
       ByRef Options As Options,
       ByRef Problem As Problem,
       ByRef Callbacks As Callbacks
    ) As Status
End Class
