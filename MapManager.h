#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	extern void InitMap(int mapId);
	extern void Update(int frameNumber);

	extern bool IsOnScreen(unsigned char x);
	extern char GetPixel(unsigned char x, unsigned char y);
	extern void SetPixel(unsigned char x, unsigned char y, bool isAdded);
}

#endif