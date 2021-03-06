#include "UtilMap.h"


// Zone constructor.
utilMap::Zone::Zone() :
	left(0),
	top(0),
	right(0),
	bottom(0)
{
}


// Zone constructor.
utilMap::Zone::Zone(int left, int top, int right, int bottom) :
	left(left),
	top(top),
	right(right),
	bottom(bottom)
{
}


// Expands the zone to include the area.
void utilMap::Zone::expand(int left, int top, int right, int bottom)
{
	this->left = std::min(this->left, left);
	this->top = std::min(this->top, top);
	this->right = std::max(this->right, right);
	this->bottom = std::max(this->bottom, bottom);
}


// Expands the zone to include the unit-type.
void utilMap::Zone::expand(BWAPI::TilePosition pos, BWAPI::UnitType unitType)
{
	expand(pos.x(), pos.y(), pos.x() + unitType.tileWidth(), pos.y() + unitType.tileHeight());
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


// Returns true if the position is within the region.
bool utilMap::inRegion(BWAPI::Position pos, BWTA::Region * region)
{
	return region && BWTA::getRegion(pos) == region;
}


// Returns whether or not the entire location has been explored.
bool utilMap::isExplored(BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType)
{
	for (int i = 0; i < buildingType.tileWidth(); i++)
		for (int j = 0; j < buildingType.tileHeight(); j++)
			if (!BWAPI::Broodwar->isExplored(BWAPI::TilePosition(buildingLocation.x() + i, buildingLocation.y() + j)))
				return false;
	return true;
}


// Returns whether or not the entire location is visible.
bool utilMap::isVisible(BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType)
{
	for (int i = 0; i < buildingType.tileWidth(); i++)
		for (int j = 0; j < buildingType.tileHeight(); j++)
			if (!BWAPI::Broodwar->isVisible(BWAPI::TilePosition(buildingLocation.x() + i, buildingLocation.y() + j)))
				return false;
	return true;
}


// Returns neighboring regions.
std::set<BWTA::Region*> utilMap::neighbors(BWTA::Region * region)
{
	std::set<BWTA::Region*> regions;
	if (region)
	{
		BOOST_FOREACH(BWTA::Chokepoint * border, region->getChokepoints())
		{
			// Add neighbor.
			std::pair<BWTA::Region*, BWTA::Region*> neighbors = border->getRegions();
			if (neighbors.first == region)
				regions.insert(neighbors.second);
			else
				regions.insert(neighbors.first);
		}
	}
	return regions;
}