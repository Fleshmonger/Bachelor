#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "ReserveMap.h"


const int BUILDING_GAP = 1; // Minimum tiles between buildings.


// Manages structures and structure positioning.
class BaseManager
{
private:
	ReserveMap * reserveMap;
	std::map<BWAPI::UnitType, utilUnit::UnitSet> buildings;

public:
	BaseManager();
	~BaseManager();

	void initialize();
	void addBuilding(BWAPI::Unit * building);
	void removeBuilding(BWAPI::Unit * building);

	bool contains(BWAPI::UnitType buildingType);
	bool isReserved(int x, int y);
	bool isReserved(BWAPI::TilePosition tile, BWAPI::UnitType buildingType);

	utilUnit::UnitSet getBuildings(BWAPI::UnitType unitType);
};

