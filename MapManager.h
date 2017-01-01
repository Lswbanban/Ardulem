#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	extern void InitMap(int mapId);
	extern void Update(int frameNumber);
	extern void Modify8Pixels(int x, int lineY, char pixels);
	extern void ModifyAPixel(int x, int y, bool remove);
}

#endif