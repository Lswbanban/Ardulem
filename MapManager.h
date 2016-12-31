#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H
/*
 * This file is in charge of instantiating the current map from the Map Sprite and Map descriptions
 */
namespace MapManager
{
	void InitMap(int mapId);
	void Update(int frameNumber);
	void Modify8Pixels(int x, int lineY, char pixels);
}

#endif