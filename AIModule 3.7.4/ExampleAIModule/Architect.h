#pragma once
#include <BWAPI.h>
#include <algorithm>
#include "WorkerManager.h"
#include "Accountant.h"

struct Zone {
	BWAPI::TilePosition origin;
	int width, height;

	Zone(BWAPI::TilePosition origin, int width, int height) :
		origin(origin),
		width(width),
		height(height)
	{
	}

	bool Zone::contains(BWAPI::TilePosition tile)
	{
		return
			origin.x() <= tile.x() &&
			origin.x() + width > tile.x() &&
			origin.y() <= tile.y() &&
			origin.y() + height > tile.y();
	}
};

static const BWAPI::UnitType BUILD_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;

class Architect
{
private:
	// Managers
	WorkerManager * workerManager;
	Accountant * accountant;

	// Local
	BWAPI::Unit * depot;
	std::set<BWAPI::Unit*> pylons;
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>  buildSchedule;

public:
	Zone resources;

	Architect(WorkerManager * workerManager, Accountant * accountant);
	~Architect();
	BWAPI::TilePosition getBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition desiredLocation, BWAPI::UnitType buildingType);
	bool validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType);
	bool scheduleBuild(BWAPI::UnitType buildingType);
	void scheduleConstruct(BWAPI::Unit * building);
	void removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildLocation);
	void removeConstruct(BWAPI::Unit * building);
	void completeBuild(BWAPI::Unit * building);
	void completeConstruct(BWAPI::Unit * building);
	int scheduled(BWAPI::UnitType buildingType);
	void includeResource(BWAPI::Unit * resource);
	void addPylon(BWAPI::Unit * pylon);
	void removePylon(BWAPI::Unit * pylon);
	void setDepot(BWAPI::Unit * depot);
	void update();
};