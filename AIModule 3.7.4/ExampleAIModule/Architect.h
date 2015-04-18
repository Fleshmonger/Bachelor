#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilMap.h"
#include "UtilUnit.h"
#include "Accountant.h"
#include "Landlord.h"


// Manages building construction orders.
//TODO Cleanup.
class Architect
{
private:
	Accountant * accountant;
	Landlord * landlord;

	utilMap::Zone harvesting;
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>  buildSchedule;

public:
	Architect(Accountant * accountant, Landlord * landlord);
	~Architect();

	void scheduleConstruct(BWAPI::Unit * building);
	void removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildLocation);
	void removeConstruct(BWAPI::Unit * building);
	void completeBuild(BWAPI::Unit * building);
	void completeConstruct(BWAPI::Unit * building);
	void expandHarvesting(BWAPI::Unit * unit);
	void addPylon(BWAPI::Unit * pylon);
	void removePylon(BWAPI::Unit * pylon);
	void update();

	bool scheduleBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition desiredLocation);
	bool validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType);

	int scheduled(BWAPI::UnitType buildingType);

	BWAPI::TilePosition getBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition desiredLocation, BWAPI::UnitType buildingType);
};