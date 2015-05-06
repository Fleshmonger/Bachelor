#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilMap.h"
#include "UtilUnit.h"
#include "Accountant.h"
#include "BaseManager.h"
#include "Landlord.h"


// Builds buildings and monitors their construction.
//TODO Cleanup.
//TODO Split into constructor and builder?
class Architect
{
private:
	Accountant * accountant;
	BaseManager * baseManager;
	Landlord * landlord;

	utilUnit::UnitSet constructions;
	//std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>> buildSchedule;

public:
	Architect(Accountant * accountant, BaseManager * baseManager, Landlord * landlord);
	~Architect();

	void commandBuild(BWAPI::Unit * builder, BWAPI::TilePosition buildingLocation, BWAPI::UnitType buildingType);
	void scheduleConstruct(BWAPI::Unit * building);
	void removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildLocation);
	void removeConstruct(BWAPI::Unit * building);
	void completeBuild(BWAPI::Unit * building);
	void completeConstruct(BWAPI::Unit * building);
	void addPylon(BWAPI::Unit * pylon);	//TODO Redundant.
	void removePylon(BWAPI::Unit * pylon); //TODO Redundant.
	void update();

	bool contains(BWAPI::Unit * unit);
	bool scheduleBuilding(BWAPI::UnitType buildingType, Vassal * vassal);
	//bool scheduleBuilding(BWAPI::UnitType buildingType, BWTA::Region * region);
	bool scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation);
	bool scheduleBuilding(BWAPI::UnitType buildingType, BWAPI::TilePosition buildingLocation, BWAPI::Unit * builder);
	bool validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType);

	BWAPI::TilePosition getBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition desiredLocation, BWAPI::UnitType buildingType);
};