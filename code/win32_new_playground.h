#if !defined(WIN32_NEW_PLAYGROUND_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#define WIN32_NEW_PLAYGROUND_H
#endif


bool Win32_FreeOccupiedMemory();
bool Win32_ReleaseGameBitmapMemory();
bool Win32_ReleaseGameSignXOBitmapMemory(GameTileSign *GameSign);
bool Win32_ReleaseGameDeviceContext();
bool Win32_InitGame();
bool Win32_PrePositionGameWindowOnScreen();
void Win32_GetClientRectSize(uint32 *Widht, uint32* Height);
void Win32_GetWindowRectSize(uint32 *Widht, uint32* Height);
bool Win32_GetGameBitmapMemory();
bool Win32_GetSignGameBitmapMemory(GameTileSign *GameSign);
bool Win32_GetGameDeviceContext();
void Win32_UpdateGameWindow();
uint32 Win32_GetPixelRGBFromDestBitmap(void* DestBitmapMemory, uint32 BitmapWidth, uint32 BitmapHeight, uint32 DestX, uint32 DestY);
void Win32_DrawPixelToBitmapXY(void* DestBitmapMemory, uint32 BitmapWidth, uint32 BitmapHeight, uint32 DestX, uint32 DestY, uint32* PixelRGB);
void Win32_PlaceXOSignToGameScreen(GameTileSign *TileSign, int TileIndex);
void Win32_DrawXOSignToGameBitmap(GameTileSign *TileSign, int TileIndex);
