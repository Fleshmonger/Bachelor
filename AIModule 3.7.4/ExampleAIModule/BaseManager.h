#pragma once
#include <BWAPI.h>
#include "utilUnit.h"


//TODO Rename to something non-manager.
class BaseManager
{
private:
	utilUnit::UnitSet minerals;
	std::map<BWAPI::UnitType, utilUnit::UnitSet> buildings;

public:
	BaseManager();
	~BaseManager();

	utilUnit::UnitSet getBuildings(BWAPI::UnitType buildingType);
};

