#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	extern void InitMap(int mapId);
	extern void Update(int frameNumber);

	// map config
	extern int GetBlockerCount();
	extern int GetBomberCount();
	extern int GetDiggerDiagonalCount();
	extern int GetDiggerHorizontalCount();
	extern int GetDiggerVerticalCount();
	extern int GetStairerCount();
	extern int GetClimberCount();
	extern int GetParachuterCount();

	// coordinate helper
	extern bool IsOnScreen(int worldX);
	extern bool IsInMapBoundary(int worldX, int worldY);
	unsigned char ConvertToScreenCoord(int worldX);
	
	// pixel helper
	extern char GetPixel(int worldX, int worldY);
	extern void SetPixel(int worldX, int worldY, bool isAdded);
}

#endif