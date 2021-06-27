#if !defined(GAMECODE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define GAMECODE_H
#endif


bool InitGameTiles();
bool IsCursorInTileRect(uint32 RectX, uint32 RectY, uint32 RectWidth, uint32 RectHeight);
int GetCursorTileIndex();
void DrawOSignToBitmap();
void DrawXSignToBitmap();
void DrawRectOnScreen(uint32 PosX, uint32 PosY, uint32 RectWidth, uint32 RectHeight);
void DrawVerticalTileMapLines();
void DrawHorizontalTileMapLines();
void DrawGameTileMap();
void DrawGameBackground();
GameTileSign *GameCheckIsThereAWinner();
GameTileSign *GameCheckHorizontalLinesForAWinner(GameTileSign *Winner);
GameTileSign *GameCheckVerticalLinesForAWinner(GameTileSign *Winner);
GameTileSign *GameCheckDiagonalLinesForAWinner(GameTileSign *Winner);
