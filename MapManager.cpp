#include "Ardulem.h"
#include "MapManager.h"

namespace MapManager
{
	/*
	 * The modification list will store all the modification made by the Lem to the original map.
	 * The modifications will be paint with the color INVERT, which means if a bit is set, it is
	 * either a solid pixel of the map that was dig up by a lem, or an empty pixel that was fill
	 * with a stair of a lem. Therefore, painting with the INVERT color will bring the correct
	 * Modification.
	 * The modification info contains two chars. The first one store the X pixel coord
	 * (so the map can be 256 pixels max), and the second char store the painting information
	 * that will be drawn in INVERT color.
	 * However, we need to know the Y pixel coord information for each modification. For this
	 * we don't store it, but instead we sort the modification list, from the first row of 8 pixels
	 * to the last row of 8 pixels. Then we have a arry of just 8 chars that store the index inside
	 * the ModificationList, where each row starts.
	 */
	struct Modification
	{
		unsigned char x;
		unsigned char pixels;
	};
	
	const unsigned int MODIFICATION_LIST_SIZE = 100;
	Modification ModificationList[MODIFICATION_LIST_SIZE];

	const unsigned int MODIFICATION_LIST_LINE_INDEX_SIZE = 8;
	unsigned char ModificationListLineIndex[MODIFICATION_LIST_LINE_INDEX_SIZE];
	
	// private functions
	void ClearModificationList();
	void DrawMap();
	void ApplyModifications();
	void InsertModification(int insertIndex, int x, char pixels);
}

void MapManager::InitMap(int mapId)
{
	ClearModificationList();
}

void MapManager::ClearModificationList()
{
	for (int i = 0; i < MODIFICATION_LIST_SIZE; ++i)
	{
		ModificationList[i].x = 0;
		ModificationList[i].pixels = 0;
	}
	
	for (int i = 0; i < MODIFICATION_LIST_LINE_INDEX_SIZE; ++i)
		ModificationListLineIndex[i] = 0;
}

void MapManager::Update(int frameNumber)
{
	DrawMap();
	ApplyModifications();
}

void MapManager::DrawMap()
{
}

void MapManager::ApplyModifications()
{
}

void MapManager::Modify8Pixels(int x, int lineY, char pixels)
{
	// first search in the where to start in the modification list according to y
	int lineIndex = ModificationListLineIndex[lineY];
	int nextLineIndex = lineY < 7 ? ModificationListLineIndex[lineY+1] : MODIFICATION_LIST_SIZE;
	
	// then iterate between the two line index to check if the modification doesn't already exist
	for (int i = lineIndex; i < nextLineIndex; ++i)
	{
		int currentX = ModificationList[i].x;
		if (x < currentX)
		{
			// try the next one
			continue;
		}
		else if (x == currentX)
		{
			//we found an existing modification, so merge the modifications
			ModificationList[i].pixels |= pixels;
			return;
		}
		else
		{
			// we need to insert the modification here
			InsertModification(i, x, pixels);
			return;
		}
	}
}

void MapManager::InsertModification(int insertIndex, int x, char pixels)
{
	// push all the modification (that keep the order)
	for (int i = MODIFICATION_LIST_SIZE-1; i > insertIndex; --i)
		ModificationList[i] = ModificationList[i-1];
	
	// and insert the new entry
	ModificationList[insertIndex].x = x;
	ModificationList[insertIndex].pixels = pixels;
}