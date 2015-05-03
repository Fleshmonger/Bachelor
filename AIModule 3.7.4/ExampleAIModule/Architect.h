#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilMap.h"
#include "UtilUnit.h"
#include "Accountant.h"
#include "Landlord.h"


// Builds buildings and monitors their construction.
//TODO Cleanup.
class Architect
{
private:
	Accountant * accountant;
	Landlord * landlord;

	std::map<BWAPI::UnitType, int> schedule; //TODO Redundant.
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>  buildSchedule;

public:
	Architect(Accountant * accountant, Landlord * landlord);
	~Architect();

	void commandBuild(BWAPI::Unit * builder, BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType);
	void scheduleConstruct(BWAPI::Unit * building);
	void removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildLocation);
	void removeConstruct(BWAPI::Unit * building);
	void completeBuild(BWAPI::Unit * building);
	void completeConstruct(BWAPI::Unit * building);
	void addPylon(BWAPI::Unit * pylon);
	void removePylon(BWAPI::Unit * pylon);
	void update();

	bool scheduleBuilding(BWAPI::UnitType buildingType, Vassal * vassal);
	//bool scheduleBuilding(BWAPI::UnitType buildingType, BWTA::Region * region);
	bool scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation);
	bool scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation, BWAPI::Unit * builder);
	bool scheduleRefinery(BWAPI::UnitType refinery, BWAPI::TilePosition buildingLocation); //TODO Redundant?
	bool scheduleRefinery(BWAPI::UnitType refinery, BWAPI::Unit * geyser); //TODO Redundant?
	bool validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType);

	int scheduled(BWAPI::UnitType buildingType);

	BWAPI::TilePosition getBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition desiredLocation, BWAPI::UnitType buildingType);
};