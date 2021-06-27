
#if 1

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define UPDATE_WINDOW_STATUS_MESSAGE 0

#define Assert(Expression)

#define global_variable static

#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t

#define int8  int8_t
#define int16 int16_t
#define int32 int32_t
#define int64 int64_t

#define GAME_FIELD_WIDTH 280 //270px + (5px x 2 - screen space for lines)
#define GAME_FIELD_HEIGHT 280 //270px + (5px x 2 - screen space for lines)
#define GAME_TILE_MAP_LINE_THICKNESS 5
#define GAME_TILE_MAP_LINE_INDENTATION 90
#define GAME_TILE_COUNT 9

global_variable int GlobalPerfCountFrequency;

global_variable uint32 WindowClientAreaWidth = 0;
global_variable uint32 WindowClientAreaHeight = 0;

global_variable uint8 RED_GameBackgroundColor = 100;
global_variable uint8 GREEN_GameBackgroundColor = 100;
global_variable uint8 BLUE_GameBackgroundColor = 100;

global_variable uint8 RED_GameTileMapColor = 255;
global_variable uint8 GREEN_GameTileMapColor = 255;
global_variable uint8 BLUE_GameTileMapColor = 255;

global_variable bool GameIsRunning = false;
global_variable HWND WindowHandle = {};
global_variable HDC GameDeviceContextHandle = {};
global_variable BITMAPINFO BitmapInfo = {};
global_variable void *GameScreenBackbufferMemory = 0;
global_variable uint32 BytesPerPixel = 4;
global_variable RECT GameClientRect = {};


global_variable int CurrentMouseClickTile = -1;
global_variable uint32 GameTurnCount = 0;

enum GameTileMapIndexEnum {ONE = 0, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE} GameTileMapIndex;
enum GameCurrentPlayerTurn {PlayerX = 0, PlayerO = 1} GamePlayerTurn;

struct GameMouseXYPosition{
    uint32 X = 0;
    uint32 Y = 0;
}GameMouseCursor;

struct GameTileSign{
    BITMAPINFO BitmapInfo = {};
    void *SignBitmapMemory = 0;
    uint32 BitmapWidth = GAME_TILE_MAP_LINE_INDENTATION;
    uint32 BitmapHeight = GAME_TILE_MAP_LINE_INDENTATION;
}X_Sign, O_Sign;

struct GameTile{
    uint32 TileX = 0;
    uint32 TileY = 0;
    uint32 TileWidth = 0;
    uint32 TileHeight = 0;
    int32 TileIndex = 0;
    struct GameTileSign *TileSign = 0;
}Tiles[GAME_TILE_COUNT];

#include "win32_new_playground.h"
#include "gamecode.h"
#include "gamecode.cpp"


//TODO: Handle memory cleanup 
 
void
Win32_DrawXOSignToGameBitmap(GameTileSign *TileSign, int TileIndex)
{
    uint32 DestX = Tiles[TileIndex].TileX;
    uint32 DestY = Tiles[TileIndex].TileY;
    uint32 TileWidth = Tiles[TileIndex].TileWidth;
    uint32 TileHeight = Tiles[TileIndex].TileHeight;
    uint32 SourcePixelRGB = 0;
    uint32 DestPixelRGB = 0;
    
    uint32 SourceSignBitmapX = 0;
    uint32 SourceSignBitmapY = 0;
    
    for(uint32 Y = DestY; Y < DestY + TileHeight; Y++, SourceSignBitmapY++)
    {    
        for(uint32 X = DestX; X < DestX + TileWidth; X++, SourceSignBitmapX++)
        {
            SourcePixelRGB = Win32_GetPixelRGBFromDestBitmap(TileSign->SignBitmapMemory, TileSign->BitmapWidth,
                                                             TileSign->BitmapHeight, SourceSignBitmapX, SourceSignBitmapY); 
            if(SourcePixelRGB)
            {
                DestPixelRGB = SourcePixelRGB;
                Win32_DrawPixelToBitmapXY(GameScreenBackbufferMemory, GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                                          X    , Y,&DestPixelRGB);
                uint32 *TestBackbuffer = (uint32 *)GameScreenBackbufferMemory;
                //utputDebugStringA("\nPlug\n");
                
            }
            SourcePixelRGB = 0;
        }
        SourceSignBitmapX = 0;
    }
}

void
Win32_PlaceXOSignToGameScreen(GameTileSign *TileSign, int TileIndex)
{
    StretchDIBits(GameDeviceContextHandle,
                  Tiles[TileIndex].TileX, Tiles[TileIndex].TileY,
                  Tiles[TileIndex].TileWidth, Tiles[TileIndex].TileHeight,
                  0, 0,
                  GAME_TILE_MAP_LINE_INDENTATION, GAME_TILE_MAP_LINE_INDENTATION,
                  TileSign->SignBitmapMemory,
                  &(TileSign->BitmapInfo),
                  DIB_RGB_COLORS,
                  SRCCOPY);
 
    
}

void
Win32_DrawPixelToBitmapXY(void* DestBitmapMemory, uint32 BitmapWidth, uint32 BitmapHeight,
                          uint32 DestX, uint32 DestY, uint32* PixelRGB)
{    
    uint8 *Row = (uint8 *)DestBitmapMemory;
    uint32 Pitch = BitmapWidth * BytesPerPixel;
      
    uint32 *DestBitmapRow = (uint32 *)Row + (Pitch * DestY / 4); //NOTE: why dividing by 4 works?

    uint32 *DestBitmapColumn = DestBitmapRow + DestX;
    *DestBitmapColumn = *PixelRGB;      
}

uint32
Win32_GetPixelRGBFromDestBitmap(void* DestBitmapMemory, uint32 BitmapWidth, uint32 BitmapHeight,
                          uint32 DestX, uint32 DestY)
{
    uint8 *Row = (uint8 *)DestBitmapMemory;
    uint32 Pitch = BitmapWidth * BytesPerPixel;
      
    uint32 *DestBitmapRow = (uint32 *)Row + (Pitch * DestY / 4); //NOTE: why dividing by 4 works?

    uint32 *DestBitmapPixel = DestBitmapRow + DestX;
    return *DestBitmapPixel;
}

void
Win32_UpdateGameWindow()
{
    StretchDIBits(GameDeviceContextHandle,
                  0, 0,
                  GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                  0, 0,
                  GAME_FIELD_WIDTH, GAME_FIELD_HEIGHT,
                  GameScreenBackbufferMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

bool
Win32_GetGameDeviceContext()
{
    if(GameDeviceContextHandle)
    {
        ReleaseDC(WindowHandle, GameDeviceContextHandle);
    }
    GameDeviceContextHandle = GetDC(WindowHandle);
    bool HandleRetrieveStatus = false;
    if(GameDeviceContextHandle)
    {
        HandleRetrieveStatus = true;
    }
    return HandleRetrieveStatus;
}

bool
Win32_GetSignGameBitmapMemory(GameTileSign *GameSign)
{
    if(GameSign->SignBitmapMemory)
    {
        VirtualFree(GameSign->SignBitmapMemory, 0, MEM_RELEASE);
        
    }
    
    GameSign->BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo);
    GameSign->BitmapInfo.bmiHeader.biWidth = GameSign->BitmapWidth;               
    GameSign->BitmapInfo.bmiHeader.biHeight = -GameSign->BitmapHeight;
    GameSign->BitmapInfo.bmiHeader.biPlanes = 1;
    GameSign->BitmapInfo.bmiHeader.biBitCount = 32;
    GameSign->BitmapInfo.bmiHeader.biCompression = BI_RGB;
    GameSign->BitmapInfo.bmiHeader.biSizeImage = 0;
    GameSign->BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    GameSign->BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    GameSign->BitmapInfo.bmiHeader.biClrUsed = 0;
    GameSign->BitmapInfo.bmiHeader.biClrImportant = 0;

    int GameScreenBackbufferMemorySize = (GameSign->BitmapWidth * GameSign->BitmapHeight) * BytesPerPixel;
    GameSign->SignBitmapMemory = VirtualAlloc(0, GameScreenBackbufferMemorySize, MEM_COMMIT, PAGE_READWRITE);

    bool MemoryAllocStatus = false;
    if(GameSign->SignBitmapMemory)
    {
        MemoryAllocStatus = true;
    }
    return MemoryAllocStatus;    
}

bool
Win32_GetGameBitmapMemory()
{
    if(GameScreenBackbufferMemory)
    {
        VirtualFree(GameScreenBackbufferMemory, 0, MEM_RELEASE);
    }
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo);
    BitmapInfo.bmiHeader.biWidth = GAME_FIELD_WIDTH;
    BitmapInfo.bmiHeader.biHeight = -GAME_FIELD_HEIGHT;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    BitmapInfo.bmiHeader.biSizeImage = 0;
    BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    BitmapInfo.bmiHeader.biClrUsed = 0;
    BitmapInfo.bmiHeader.biClrImportant = 0;

    int GameScreenBackbufferMemorySize = (GAME_FIELD_WIDTH * GAME_FIELD_HEIGHT) * BytesPerPixel;
    GameScreenBackbufferMemory = VirtualAlloc(0, GameScreenBackbufferMemorySize, MEM_COMMIT, PAGE_READWRITE);

    bool MemoryAllocStatus = false;
    if(GameScreenBackbufferMemory)
    {
        MemoryAllocStatus = true;
    }
    return MemoryAllocStatus;
}

void
Win32_GetWindowRectSize(uint32 *Widht, uint32* Height)
{
    RECT WindowRect = {};
    GetWindowRect(WindowHandle, &WindowRect);
    *Widht = WindowRect.right - WindowRect.left;
    *Height = WindowRect.bottom - WindowRect.top;
}

void
Win32_GetClientRectSize(uint32 *Widht, uint32* Height)
{
    RECT ClientRect = {};
    GetClientRect(WindowHandle, &ClientRect);
    *Widht = ClientRect.right - ClientRect.left;
    *Height = ClientRect.bottom - ClientRect.top;
}

bool
Win32_PrePositionGameWindowOnScreen()
{
    bool DefineRequestedWindowClientArea = false;
    uint32 ClientRectWidth = 0;
    uint32 ClientRectHeight = 0;
    uint32 WindowRectWidth = 0;
    uint32 WindowRectHeight = 0;

    Win32_GetClientRectSize(&ClientRectWidth, &ClientRectHeight);
    Win32_GetWindowRectSize(&WindowRectWidth, &WindowRectHeight);

    uint32 TargetWindowWidth = WindowRectWidth + (WindowRectWidth - ClientRectWidth);
    uint32 TargetWindowHeight = WindowRectHeight + (WindowRectHeight - ClientRectHeight);
    
    //NOTE: GAME_FIELD_HEIGHT and GAME_FIELD_WIDTH == to TargetWindowHeight and TargetWindowWidth
    if(MoveWindow(WindowHandle, 200, 200, TargetWindowWidth, TargetWindowHeight, TRUE))
    {
        DefineRequestedWindowClientArea = true;
    }
    return DefineRequestedWindowClientArea;
}

bool
Win32_InitGame()
{
    OutputDebugStringA("\n---------INIT_STAGE---------\n");
    
    bool InitStatus = false;

    if(Win32_PrePositionGameWindowOnScreen())
    {
        OutputDebugStringA("Window Positioned\n");
        InitStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Position Window\n");
        InitStatus = false;
    }

    if(Win32_GetGameBitmapMemory())
    {
        OutputDebugStringA("Got Bitmap Memory\n");
        InitStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Get Bitmap Memory\n");
        InitStatus = false;
    }

    if(Win32_GetSignGameBitmapMemory(&X_Sign))
    {
        OutputDebugStringA("Got X_Sign Memory\n");
        InitStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Get X_Sign Memory\n");
        InitStatus = false;
    }
    
    if(Win32_GetSignGameBitmapMemory(&O_Sign))
    {
        OutputDebugStringA("Got O_Sign Memory\n");
        InitStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Get O_Sign Memory\n");
        InitStatus = false;
    }

    if(InitGameTiles())
    {
        OutputDebugStringA("Tiles Initialized\n");
        InitStatus = true;
    }
    else
    {
        OutputDebugStringA("Tiles Failed To be Initialized\n");
        InitStatus = false;
    }
    
    if(Win32_GetGameDeviceContext())
    {
        OutputDebugStringA("Got Device Context\n");
        InitStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Get Device Context\n");
        InitStatus = false;
    }

    
    OutputDebugStringA("---------INIT_STAGE---------\n");
    
    return InitStatus;
}
   
bool
Win32_ReleaseGameDeviceContext()
{
    bool DCReleaseStatus = false;
    if(GameDeviceContextHandle)
    {
        if(ReleaseDC(WindowHandle, GameDeviceContextHandle))
        {
            DCReleaseStatus = true;
        }
    }
    else
    {
        DCReleaseStatus = true;
    }
    
    return DCReleaseStatus;   
}

bool
Win32_ReleaseGameSignXOBitmapMemory(GameTileSign *GameSign)
{
    bool MemoryReleaseStatus = false;
    if(GameSign->SignBitmapMemory)
    {
        if(VirtualFree(GameSign->SignBitmapMemory, 0, MEM_RELEASE))
        {
            MemoryReleaseStatus = true;
        }
    }
    else
    {
        MemoryReleaseStatus = true;
    }
 
    return MemoryReleaseStatus;    
}

bool
Win32_ReleaseGameBitmapMemory()
{
    bool MemoryReleaseStatus = false;
    if(GameScreenBackbufferMemory)
    {
        if(VirtualFree(GameScreenBackbufferMemory, 0, MEM_RELEASE))
        {
            MemoryReleaseStatus = true;
        }
    }
    else
    {
        MemoryReleaseStatus = true;
    }
 
    return MemoryReleaseStatus;
}

bool
Win32_FreeOccupiedMemory()
{
    OutputDebugStringA("\n---------FREE_STAGE---------\n");
    
    bool FreeStatus = false;
    if(Win32_ReleaseGameBitmapMemory())
    {
        OutputDebugStringA("Bitmap Memory Released\n");
        FreeStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Release Bitmap Memory\n");
        FreeStatus = false;
    }

    if(Win32_ReleaseGameSignXOBitmapMemory(&X_Sign))
    {
        OutputDebugStringA("X_Sign Bitmap Memory Released\n");
        FreeStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Release X_Sign Bitmap Memory\n");
        FreeStatus = false;
    }

    if(Win32_ReleaseGameSignXOBitmapMemory(&O_Sign))
    {
        OutputDebugStringA("O_Sign Bitmap Memory Released\n");
        FreeStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Release O_Sign Bitmap Memory\n");
        FreeStatus = false;
    }

    if(Win32_ReleaseGameDeviceContext())
    {
        OutputDebugStringA("Device Context Released\n");
        FreeStatus = true;
    }
    else
    {
        OutputDebugStringA("Failed To Release Device Context\n");
        FreeStatus = false;
    }

    OutputDebugStringA("---------FREE_STAGE---------\n");
    return FreeStatus;
}

LRESULT CALLBACK
WindowProcedureMessageHandler(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_LBUTTONDOWN:
        {
            OutputDebugStringA("\nMESSAGE: WM_LBUTTONDOWN\n");
            
            GameMouseCursor.X = LOWORD(lParam);
            GameMouseCursor.Y = HIWORD(lParam);
            CurrentMouseClickTile = GetCursorTileIndex();
            char Buffer[256];
            sprintf(Buffer, "\n%d\n", CurrentMouseClickTile);
            OutputDebugStringA(Buffer);
        }break;
            
        case WM_PAINT:
        {   
            OutputDebugStringA("\nMESSAGE: WM_PAINT\n\n");
         
            PAINTSTRUCT PaintStruct;
            BeginPaint(WindowHandle, &PaintStruct);

           
            Win32_UpdateGameWindow();
            
            EndPaint(WindowHandle, &PaintStruct);            
        }break;

        case WM_SIZE:
        {
            GetClientRect(WindowHandle, &GameClientRect);
            WindowClientAreaWidth = GameClientRect.right - GameClientRect.left;
            WindowClientAreaHeight = GameClientRect.bottom - GameClientRect.top;
        }break;
        
        case WM_CLOSE:
        {
            OutputDebugStringA("\nMESSAGE: WM_CLOSE\n\n");
            GameIsRunning = false;
        }break;

        default:
        {
            Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
        }break;
    }

    return Result;
}


inline LARGE_INTEGER
Win32GetWallClock(void)
{    
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return(Result);
}

inline float
Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    float  Result = ((float)(End.QuadPart - Start.QuadPart) /
                     (float)GlobalPerfCountFrequency);
    return(Result);
}

int CALLBACK
WinMain(HINSTANCE WindowInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{

    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    GlobalPerfCountFrequency = PerfCountFrequencyResult.QuadPart;

    
    UINT DesiredSchedulerMS = 1;
    bool SleepIsGranular = (timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR);


    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProcedureMessageHandler;
    WindowClass.hInstance = WindowInstance;
    WindowClass.lpszClassName = "TicTacToeClass";

    int MonitorRefreshHz = 60;
    int GameUpdateHz = MonitorRefreshHz / 2;
    float TargetSecondsPerFrame = 1.0f / (float)GameUpdateHz;
    

    RegisterClassA(&WindowClass);

    WindowHandle = CreateWindowExA(0,
                                  WindowClass.lpszClassName,
                                  "Tic Tac Toe",
                                  WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  GAME_FIELD_WIDTH,
                                  GAME_FIELD_HEIGHT,
                                  0,
                                  0,
                                  WindowInstance,
                                  0);

    uint32 PixelRGB = 0;
    PixelRGB = (0 << 16) | (0  << 8) | (255 << 0);
    if(WindowHandle)
    {
        if(Win32_InitGame())
        {
            DrawGameBackground();
            DrawGameTileMap();
            DrawXSignToBitmap();
            DrawOSignToBitmap();
            
            OutputDebugStringA("\nGameInit succesful \n\n");

            GameIsRunning = true;
            GameTurnCount = 1;
            GamePlayerTurn = PlayerX;

            LARGE_INTEGER LastCounter = Win32GetWallClock();
            uint64 LastCycleCount = __rdtsc();
            
            while(GameIsRunning)
            {
                LARGE_INTEGER BeginPerformanceCounter;
                LARGE_INTEGER Frequency;

                QueryPerformanceCounter(&BeginPerformanceCounter);
                QueryPerformanceFrequency(&Frequency);
                
                MSG WindowMessage = { };
                while(PeekMessage(&WindowMessage, WindowHandle, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&WindowMessage);
                    DispatchMessageA(&WindowMessage);
                }                
                
                if(CurrentMouseClickTile >= 0)
                {
                    if(GamePlayerTurn == PlayerX)
                    {
                        if(Tiles[CurrentMouseClickTile].TileSign == 0)
                        {                            
                            Tiles[CurrentMouseClickTile].TileSign = &X_Sign;                     
                            Win32_DrawXOSignToGameBitmap(&X_Sign, CurrentMouseClickTile);
                            GamePlayerTurn = PlayerO;
                        }
                        else
                        {
                            OutputDebugStringA("This Tile is already occupied\n");
                        }
                        
                    }                    
                    else if(GamePlayerTurn == PlayerO)
                    {
                        if(Tiles[CurrentMouseClickTile].TileSign == 0)
                        {                            
                            Tiles[CurrentMouseClickTile].TileSign = &O_Sign;                     
                            Win32_DrawXOSignToGameBitmap(&O_Sign, CurrentMouseClickTile);
                            GamePlayerTurn = PlayerX;
                        }
                        else
                        {
                            OutputDebugStringA("This Tile is already occupied\n");
                        }                       
                    }
                }
                Win32_UpdateGameWindow();

                if(GameCheckIsThereAWinner())
                {
                    OutputDebugStringA("END_OF_THE_GAME\n");
                }
                GameTurnCount++;
                CurrentMouseClickTile = -1;
                
                            
                LARGE_INTEGER WorkCounter = Win32GetWallClock();
                float WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);

                float SecondsElapsedForFrame = WorkSecondsElapsed;
                    if(SecondsElapsedForFrame < TargetSecondsPerFrame)
                    {                        
                        if(SleepIsGranular)
                        {
                            DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsPerFrame -
                                                               SecondsElapsedForFrame));
                            if(SleepMS > 0)
                            {
                                Sleep(SleepMS);
                            }
                        }
                        
                        float TestSecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter,
                                                                                   Win32GetWallClock());
                        Assert(TestSecondsElapsedForFrame < TargetSecondsPerFrame);
                        
                        while(SecondsElapsedForFrame < TargetSecondsPerFrame)
                        {                            
                            SecondsElapsedForFrame = Win32GetSecondsElapsed(LastCounter,
                                                                            Win32GetWallClock());
                        }
                    }
                    else
                    {
                        
                    }

                    LARGE_INTEGER EndCounter = Win32GetWallClock();
                    float MSPerFrame = 1000.0f*Win32GetSecondsElapsed(LastCounter, EndCounter);                    
                    LastCounter = EndCounter; 

                    uint64 EndCycleCount = __rdtsc();
                    uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
                    LastCycleCount = EndCycleCount;
                    
                    float FPS = 0.0f;
                    float MCPF = ((float)CyclesElapsed / (1000.0f * 1000.0f));

                    char FPSBuffer[256];
                    _snprintf_s(FPSBuffer, sizeof(FPSBuffer),
                                "%.02fms/f,  %.02ff/s,  %.02fmc/f\n", MSPerFrame, FPS, MCPF);
                    OutputDebugStringA(FPSBuffer);
            }
        }
        Win32_FreeOccupiedMemory();
    }
    return 0;
}

#else ///////////////////////////////////TEST/////////////////////////////////AREA//////////////

#endif
