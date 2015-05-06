#include "ReserveMap.h"


// Constructor.
ReserveMap::ReserveMap(int width, int height) :
	width(width),
	height(height)
{
	// Initialize map.
	map = new int*[width];
	for (int i = 0; i < width; i++)
		map[i] = new int[height]();
}


// Deconstructor.
//TODO Destruct map.
ReserveMap::~ReserveMap()
{
}


// Decreases the reserve value at the position.
void ReserveMap::free(int x, int y)
{
	if (contains(x, y))
		map[x][y]--;
}


// Increases the reserve value at the position.
void ReserveMap::reserve(int x, int y)
{
	if (contains(x, y))
		map[x][y]++;
}


// Returns whether the position is within the map.
bool ReserveMap::contains(int x, int y)
{
	return x >= 0 && x < width && y >= 0 && y < height;
}


// Returns whether the tile is reserved.
bool ReserveMap::isReserved(int x, int y)
{
	if (contains(x, y))
		return map[x][y] > 0;
	else
		return false;
}