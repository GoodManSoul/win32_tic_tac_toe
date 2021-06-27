/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "gamecode.h"

GameTileSign *
GameCheckIsThereAWinner()
{
    GameTileSign *Winner = 0;

    Winner = GameCheckHorizontalLinesForAWinner(Winner);
    Winner = GameCheckVerticalLinesForAWinner(Winner);
    Winner = GameCheckDiagonalLinesForAWinner(Winner);
    if(Winner == &X_Sign)
    {
        OutputDebugStringA("Winner is X\n");
    }
    else if(Winner == &O_Sign)
    {
        OutputDebugStringA("Winner is O\n");   
    }
    return  Winner;
}

GameTileSign *
GameCheckHorizontalLinesForAWinner(GameTileSign *Winner)
{
    uint32 WinXControlBits = 0x00010101; //X player goes as 1 value to 0, 8, 16 bit
    uint32 WinOControlBits = 0x00202020; //O player goes as 2 value to 4, 12, 20 bit
    uint32 CheckBits = 0x00000000;

    for(uint32 TileRow = 0; TileRow < GAME_TILE_COUNT / 3; TileRow++)
    {
        CheckBits = 0x00000000;
        uint32 BitPushMult = 8;
        for(uint32 TileCol = 0; TileCol < GAME_TILE_COUNT / 3; TileCol++)
        {
            if(Tiles[TileRow * (GAME_TILE_COUNT / 3) + TileCol].TileSign == &X_Sign)
            {
                CheckBits = (CheckBits) | (1 << TileCol * BitPushMult);
            }
            if(Tiles[TileRow * (GAME_TILE_COUNT / 3) + TileCol].TileSign == &O_Sign)
            {
                CheckBits = (CheckBits) | (2 << (TileCol * BitPushMult) + 4);
            }
            BitPushMult = 8;
        }
        if(CheckBits == (CheckBits | WinXControlBits))
        {
            Winner = &X_Sign;
        }
        if(CheckBits == (CheckBits | WinOControlBits))
        {
            Winner = &O_Sign;
        }
    }
    return Winner;
}

GameTileSign *
GameCheckVerticalLinesForAWinner(GameTileSign *Winner)
{
    uint32 WinXControlBits = 0x00010101; //X player goes as 1 value to 0, 8, 16 bit
    uint32 WinOControlBits = 0x00202020; //O player goes as 2 value to 4, 12, 20 bit
    uint32 CheckBits = 0x00000000;

    for(uint32 TileRow = 0; TileRow < GAME_TILE_COUNT / 3; TileRow++)
    {
        CheckBits = 0x00000000;
        uint32 BitPushMult = 8;
        for(uint32 TileCol = 0; TileCol < GAME_TILE_COUNT / 3; TileCol++)
        {
            if(Tiles[TileRow + (GAME_TILE_COUNT / 3) * TileCol].TileSign == &X_Sign)
            {
                CheckBits = (CheckBits) | (1 << TileCol * BitPushMult);
            }
            if(Tiles[TileRow + (GAME_TILE_COUNT / 3) * TileCol].TileSign == &O_Sign)
            {
                CheckBits = (CheckBits) | (2 << (TileCol * BitPushMult) + 4);
            }
            BitPushMult = 8;
        }
        if(CheckBits == (CheckBits | WinXControlBits))
        {
            Winner = &X_Sign;
        }
        if(CheckBits == (CheckBits | WinOControlBits))
        {
            Winner = &O_Sign;
        }
    }
    return Winner;
}

GameTileSign *
GameCheckDiagonalLinesForAWinner(GameTileSign *Winner)
{
    uint32 WinXControlBits = 0x00010101; //X player goes as 1 value to 0, 8,16 bit
    uint32 WinOControlBits = 0x00202020; //O player goes as 2 value to 4, 12, 20 bit
    uint32 CheckBits = 0x00000000;
    uint32 BitPushMult = 0;
    //NOTE: Checking \ line
    for(uint32 TileRow = 0; TileRow < GAME_TILE_COUNT / 3; TileRow++)
    {             
        if(Tiles[TileRow * (GAME_TILE_COUNT / 3) + TileRow].TileSign == &X_Sign)
        {
            CheckBits = (CheckBits) | (1 << TileRow * BitPushMult);
        }
        if(Tiles[TileRow * (GAME_TILE_COUNT / 3) + TileRow].TileSign == &O_Sign)
        {
            CheckBits = (CheckBits) | (2 << (TileRow * BitPushMult) + 4);
        }

        BitPushMult = 8;
    }

    //NOTE: Write check function to get rid of mess
    if(CheckBits == (CheckBits | WinXControlBits))
    {
        Winner = &X_Sign;
    }
    if(CheckBits == (CheckBits | WinOControlBits))
    {
        Winner = &O_Sign;        
    }
    
    CheckBits = 0x00000000;
    BitPushMult = 8;
    //NOTE: Checking / line
    for(uint32 TileRow = (GAME_TILE_COUNT / 3) - 1; TileRow <= GAME_TILE_COUNT - GAME_TILE_COUNT / 3; TileRow += 2)
    {     
        if(Tiles[TileRow].TileSign == &X_Sign)
        {
            CheckBits = (CheckBits) | (1 << TileRow * BitPushMult);
        }
        if(Tiles[TileRow].TileSign == &O_Sign)
        {
            CheckBits = (CheckBits) | (2 << (TileRow * BitPushMult) + 4);
        }
        BitPushMult = BitPushMult / 4; 

    }
    if(CheckBits == (CheckBits | WinXControlBits))
    {
        Winner = &X_Sign;
    }
    if(CheckBits == (CheckBits | WinOControlBits))
    {
        Winner = &O_Sign;
    }
    return Winner;
}

bool
InitGameTiles()
{
    bool TilesInit = false;
    uint32 TileWidth = GAME_TILE_MAP_LINE_INDENTATION;
    uint32 TileHeight = GAME_TILE_MAP_LINE_INDENTATION;
    uint32 TileIndex = 0;
    
    for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y += TileHeight + GAME_TILE_MAP_LINE_THICKNESS)
    {
        for(uint32 X = 0; X < GAME_FIELD_WIDTH; X += TileWidth + GAME_TILE_MAP_LINE_THICKNESS)
        {
            Tiles[TileIndex].TileX = X;
            Tiles[TileIndex].TileY = Y;
            Tiles[TileIndex].TileWidth = TileWidth;
            Tiles[TileIndex].TileHeight = TileHeight;
            Tiles[TileIndex].TileIndex = TileIndex;          
            TileIndex++;
        }
        if(Tiles[TileIndex].TileX && Tiles[TileIndex].TileY && Tiles[TileIndex].TileWidth &&
           Tiles[TileIndex].TileHeight && Tiles[TileIndex].TileIndex)
        {
            TilesInit = true;
        }
    }
    return TilesInit;
}

bool
IsCursorInTileRect(uint32 RectX, uint32 RectY, uint32 RectWidth, uint32 RectHeight)
{
    bool CursorIntersection = false;
    if((GameMouseCursor.X >= RectX) && (GameMouseCursor.X <= RectX + RectWidth - GAME_TILE_MAP_LINE_THICKNESS) &&
       (GameMouseCursor.Y >= RectY)&&(GameMouseCursor.Y <= RectY + RectHeight - GAME_TILE_MAP_LINE_THICKNESS))
    {
        CursorIntersection = true;
    
        OutputDebugStringA("\nCursor does intersect\n");
    }
    return CursorIntersection;
}


int
GetCursorTileIndex()
{
    int CursorTileIndex = -1;
    for(uint32 Index = 0; Index < GAME_TILE_COUNT; Index++)
    {
        if(IsCursorInTileRect(Tiles[Index].TileX, Tiles[Index].TileY,
                              Tiles[Index].TileWidth, Tiles[Index].TileHeight))
        {
            CursorTileIndex = Index;
        }
    }
    
    return CursorTileIndex;
}


//NOTE: Get reading done on drawing circle articles

void
DrawOSignToBitmap()
{
    OutputDebugStringA("DrawingO\n");
    float CX = 45;
    float CY = 45;
    float Radius = 45;

    float ThetaAngle = 0;
    float Step = 0.1;

    float DestX = 0;
    float DestY = 0;

    uint32 PixelRGB = (0 << 16) | (255 << 8) | (0 << 0);
    for(uint32 LineThicknessPX = 0; LineThicknessPX < GAME_TILE_MAP_LINE_THICKNESS / 2; LineThicknessPX++)
    {
        while(ThetaAngle <= 360)
        {
            DestX = CX + (Radius - LineThicknessPX) * cos(ThetaAngle);
            DestY = CY + (Radius - LineThicknessPX) * sin(ThetaAngle);
        
            Win32_DrawPixelToBitmapXY(O_Sign.SignBitmapMemory, O_Sign.BitmapWidth, O_Sign.BitmapHeight,
                                      DestX, DestY, &PixelRGB);
            ThetaAngle += Step;
        }    
        ThetaAngle = 0;
    }
}   

void
DrawXSignToBitmap()
{
    OutputDebugStringA("DrawingX\n");
    
    //NOTE: Drawing \ line
    uint32 PixelRGB = (0 << 16) | (255 << 8) | (0 << 0);

    for(uint32 LineThicknessPX = 0; LineThicknessPX < GAME_TILE_MAP_LINE_THICKNESS / 2; LineThicknessPX++)
    {
        for(uint32 X = LineThicknessPX, Y = 0; Y < X_Sign.BitmapWidth - LineThicknessPX; X++, Y++)
        {
            Win32_DrawPixelToBitmapXY(X_Sign.SignBitmapMemory, X_Sign.BitmapWidth, X_Sign.BitmapHeight,
                                      X, Y, &PixelRGB);
        }
    
    //NOTE: Drawing / line
    for(uint32 X = X_Sign.BitmapWidth - LineThicknessPX, Y = 0; Y < X_Sign.BitmapWidth; X--, Y++)
    {        
        Win32_DrawPixelToBitmapXY(X_Sign.SignBitmapMemory, X_Sign.BitmapWidth, X_Sign.BitmapHeight,
                                  X, Y, &PixelRGB);           
    }
    }
}

//NOTE: Test target coordinates
void
DrawRectOnScreen(uint32 PosX, uint32 PosY, uint32 RectWidth, uint32 RectHeight)
{
    uint8 *BitmapMemoryRow = (uint8 *)GameScreenBackbufferMemory;
    uint32 Pitch = GAME_FIELD_WIDTH * BytesPerPixel;

    for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y++)
    {
        uint32* Pixel = (uint32 *)BitmapMemoryRow;
        for(uint32 X = 0; X < GAME_FIELD_WIDTH; X++)
        {
            if((X >= PosX) && (X <= RectWidth) && (Y >= PosY) && (Y <= RectHeight))
            {
            *Pixel = (255 << 16) | (255 << 8) | (0 << 0);
            }
            Pixel++;
        }
        BitmapMemoryRow += Pitch;
    }
}

void
DrawVerticalTileMapLines()
{
    /*
    //NOTE: Vertical lines
    for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y++)
    {
        uint32* Pixel = (uint32 *)BitmapMemoryRow;
        for(uint32 X = 0; X < GAME_FIELD_WIDTH; X++)
        {
            //NOTE: Drawing 1 vertical line
            if(X == GAME_TILE_MAP_LINE_INDENTATION - 1)
            {
                for(;X < (GAME_TILE_MAP_LINE_INDENTATION + GAME_TILE_MAP_LINE_THICKNESS); X++)
                {
                    *Pixel =    (RED_GameTileMapColor << 16)
                        | (GREEN_GameTileMapColor << 8)
                        | (BLUE_GameTileMapColor << 0);

                    Pixel++;
                }
            }
            //NOTE: Drawing 2 vertical line
            if(X == (GAME_TILE_MAP_LINE_INDENTATION * 2) - 1)
            {
                for(;X < (GAME_TILE_MAP_LINE_INDENTATION * 2 + GAME_TILE_MAP_LINE_THICKNESS); X++)
                {
                    *Pixel =    (RED_GameTileMapColor << 16)
                        | (GREEN_GameTileMapColor << 8)
                        | (BLUE_GameTileMapColor << 0);

                    Pixel++; 
                }
            }            
            Pixel++;
            //Win32_UpdateGameWindow();
        }
        BitmapMemoryRow += Pitch;
    }
    */
    uint32 PixelRGB = (RED_GameTileMapColor << 16) |
        (GREEN_GameTileMapColor <<  8) |
        (BLUE_GameTileMapColor << 0);
    
    for(uint32 X = 0; X < GAME_FIELD_WIDTH; X++)
    {
        //NOTE:Drawing 1 vertical line
        if((X >= GAME_TILE_MAP_LINE_INDENTATION) &&
           (X <=  GAME_TILE_MAP_LINE_INDENTATION + GAME_TILE_MAP_LINE_THICKNESS - 1))
        {
            for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y++)
            {        
                Win32_DrawPixelToBitmapXY(GameScreenBackbufferMemory, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                                          X, Y, &PixelRGB);       
            }
        }
        //NOTE:Drawing 2 vertical line
        if((X >= GAME_TILE_MAP_LINE_INDENTATION * 2 + GAME_TILE_MAP_LINE_THICKNESS) &&
           (X <= GAME_TILE_MAP_LINE_INDENTATION * 2 + GAME_TILE_MAP_LINE_THICKNESS * 2 - 1))
        {
            for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y++)
            {        
                Win32_DrawPixelToBitmapXY(GameScreenBackbufferMemory, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                                          X, Y, &PixelRGB);       
            }
        }
    }
}

void
DrawHorizontalTileMapLines()
{
    /*
    //NOTE: Horizontal Lines  
    for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y++)
    {
        uint32* Pixel = (uint32 *)BitmapMemoryRow;
        for(uint32 X = 0; X < GAME_FIELD_WIDTH; X++)
        {
            //NOTE: Drawing 1 horizontal line
            if((Y >= GAME_TILE_MAP_LINE_INDENTATION - 1) &&
               (Y <= (GAME_TILE_MAP_LINE_INDENTATION + GAME_TILE_MAP_LINE_THICKNESS) - 1))
            {
                for(;X < GAME_FIELD_WIDTH; X++)
                {
                    *Pixel =    (RED_GameTileMapColor << 16)
                        | (GREEN_GameTileMapColor << 8)
                        | (BLUE_GameTileMapColor << 0);

                    Pixel++; 
                }
            }
            //NOTE: Drawing 1 horizontal line
            if((Y >= (GAME_TILE_MAP_LINE_INDENTATION * 2) - 1) &&
               (Y <= ((GAME_TILE_MAP_LINE_INDENTATION * 2) + GAME_TILE_MAP_LINE_THICKNESS) - 1))
            
            {
                for(;X < GAME_FIELD_WIDTH; X++)
                {
                    *Pixel =    (RED_GameTileMapColor << 16)
                        | (GREEN_GameTileMapColor << 8)
                        | (BLUE_GameTileMapColor << 0);

                    Pixel++; 
                }
            }            
            Pixel++;
            //Win32_UpdateGameWindow();
        }
        BitmapMemoryRow += Pitch;
    }
    */
    
    uint32 PixelRGB = (RED_GameTileMapColor << 16) | (GREEN_GameTileMapColor <<  8)|(BLUE_GameTileMapColor << 0);
    
    for(uint32 Y = 0; Y < GAME_FIELD_WIDTH; Y++)
    {
        //NOTE:Drawing 1 horizontal line
        if((Y >= GAME_TILE_MAP_LINE_INDENTATION) &&
           (Y <=  GAME_TILE_MAP_LINE_INDENTATION + GAME_TILE_MAP_LINE_THICKNESS - 1))
        {
            for(uint32 X = 0; X < GAME_FIELD_HEIGHT; X++)
            {        
                Win32_DrawPixelToBitmapXY(GameScreenBackbufferMemory, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                                          X, Y, &PixelRGB);       
            }
        }
        //NOTE:Drawing 2 horizontal line
        if((Y >= GAME_TILE_MAP_LINE_INDENTATION * 2 + GAME_TILE_MAP_LINE_THICKNESS) &&
           (Y <= GAME_TILE_MAP_LINE_INDENTATION * 2 + GAME_TILE_MAP_LINE_THICKNESS * 2 - 1))
        {
            for(uint32 X = 0; X < GAME_FIELD_HEIGHT; X++)
            {        
                Win32_DrawPixelToBitmapXY(GameScreenBackbufferMemory, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                                          X, Y, &PixelRGB);       
            }
        }
    }

  
}

void
DrawGameTileMap()
{
    DrawVerticalTileMapLines();
    
    DrawHorizontalTileMapLines();
}

void
DrawGameBackground()
{
    uint32 PixelRGB = (RED_GameBackgroundColor << 16) |
        (GREEN_GameBackgroundColor << 8) |
        (BLUE_GameBackgroundColor << 0);
    for(uint32 X = 0; X < GAME_FIELD_WIDTH; X++)
    {
        for(uint32 Y = 0; Y < GAME_FIELD_HEIGHT; Y++)
        {
            Win32_DrawPixelToBitmapXY(GameScreenBackbufferMemory, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                                      X, Y, &PixelRGB);
        }
    }
}

