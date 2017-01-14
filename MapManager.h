#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	extern void InitMap();
	extern void Update();
	extern void DrawStartAndHome();

	// view scroll
	bool ScrollView(int scrollMoveInPixel);
	
	// map config
	extern unsigned char CurrentMapId;
	extern unsigned char GetStartX();
	extern unsigned char GetStartY();
	extern unsigned char GetHomeX();
	extern unsigned char GetHomeY();
	extern unsigned char GetMinDropSpeed();
	extern unsigned char GetAvailableLemCount();
	extern int GetRequiredLemPercentage();
	extern int GetBlockerCount();
	extern int GetBomberCount();
	extern int GetDiggerDiagonalCount();
	extern int GetDiggerHorizontalCount();
	extern int GetDiggerVerticalCount();
	extern int GetStairerCount();
	extern int GetClimberCount();
	extern int GetParachuterCount();
	
	// decrease lem count
	extern void DecreaseBlockerCount();
	extern void DecreaseBomberCount();
	extern void DecreaseDiggerDiagonalCount();
	extern void DecreaseDiggerHorizontalCount();
	extern void DecreaseDiggerVerticalCount();
	extern void DecreaseStairerCount();
	extern void DecreaseClimberCount();
	extern void DecreaseParachuterCount();

	// coordinate helper
	extern bool IsOnScreen(int worldX);
	extern bool IsInMapBoundary(int worldX, int worldY);
	unsigned char ConvertToScreenCoord(int worldX);
	
	// pixel helper
	extern unsigned char GetPixelsColumn(int worldX, int worldY, int height, bool considerAddedPixels);
	extern char GetPixel(int worldX, int worldY, bool considerAddedPixels);
	extern void SetPixel(int worldX, int worldY, bool isAdded);
}

#endif