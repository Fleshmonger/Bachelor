#include "UtilMap.h"


// Zone constructor.
utilMap::Zone::Zone(int left, int top, int right, int bottom) :
	left(left),
	top(top),
	right(right),
	bottom(bottom)
{
}


// Returns whether the tile coordinates are within the zone.
bool utilMap::Zone::contains(int x, int y)
{
	return
		left <= x &&
		top <= y &&
		right > x &&
		bottom > y;
}


// Returns whether the unit type at the position is within the zone.
bool utilMap::Zone::contains(BWAPI::TilePosition pos, BWAPI::UnitType unitType)
{
	for (int x = pos.x(); x < pos.x() + unitType.tileWidth(); x++)
		for (int y = pos.y(); y < pos.y() + unitType.tileHeight(); y++)
			if (contains(x, y))
				return true;
	return false;
}