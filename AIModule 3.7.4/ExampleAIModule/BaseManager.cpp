#include "BaseManager.h"


// Constructor.
BaseManager::BaseManager()
{
}


// Deconstructor.
BaseManager::~BaseManager()
{
}


// Called after BWTA analysis. Instantiates reserve map.
void BaseManager::initialize()
{
	reserveMap = new ReserveMap(BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight());
}


// Adds a building to the pool.
void BaseManager::addBuilding(BWAPI::Unit * building)
{
	// Verify building.
	if (utilUnit::isOwned(building) &&
		building->exists() &&
		building->getType().isBuilding())
	{
		// Check type.
		BWAPI::UnitType buildingType = building->getType();
		if (!contains(buildingType))
			buildings[buildingType] = utilUnit::UnitSet();

		// Add building.
		buildings[buildingType].insert(building);

		// Reserve location.
		BWAPI::TilePosition tile = building->getTilePosition();
		for (int i = -BUILDING_GAP; i < buildingType.tileWidth() + BUILDING_GAP; i++)
			for (int j = -BUILDING_GAP; j < buildingType.tileHeight() + BUILDING_GAP; j++)
				reserveMap->reserve(tile.x() + i, tile.y() + j);
	}
}


// Removes a building from the pool.
void BaseManager::removeBuilding(BWAPI::Unit * building)
{
	// Verify building.
	if (building &&
		contains(building->getType()))
	{
		// Remove building.
		BWAPI::UnitType buildingType = building->getType();
		buildings[buildingType].erase(building);

		// Free reservation.
		BWAPI::TilePosition tile = building->getTilePosition();
		for (int i = -BUILDING_GAP; i < buildingType.tileWidth() + BUILDING_GAP; i++)
			for (int j = -BUILDING_GAP; j < buildingType.tileHeight() + BUILDING_GAP; j++)
				reserveMap->free(tile.x() + i, tile.y() + j);
	}
}


// Returns whether the building type has been recorded.
bool BaseManager::contains(BWAPI::UnitType buildingType)
{
	return buildings.count(buildingType) > 0 && buildings[buildingType].size() > 0;
}


// Returns whether the position is reserved.
bool BaseManager::isReserved(int x, int y)
{
	return reserveMap->isReserved(x, y);
}


// Returns whether the building type at the tile would intrude on reserved space.
bool BaseManager::isReserved(BWAPI::TilePosition tile, BWAPI::UnitType buildingType)
{
	// Check building location.
	for (int i = 0; i < buildingType.tileWidth(); i++)
		for (int j = 0; j < buildingType.tileHeight(); j++)
			if (reserveMap->isReserved(tile.x() + i, tile.y() + j))
				return true;
	return false;
}


// Returns all recorded buildings of the type.
utilUnit::UnitSet BaseManager::getBuildings(BWAPI::UnitType unitType)
{
	if (contains(unitType))
		return buildings[unitType];
	else
		return utilUnit::UnitSet();
}