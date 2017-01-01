#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	extern void InitMap(int mapId);
	extern void Update(int frameNumber);

	extern bool IsOnScreen(int x);
	extern bool IsInMapBoundary(int x, int y);
	extern char GetPixel(int x, int y);
	extern void SetPixel(int x, int y, bool isAdded);
}

#endif