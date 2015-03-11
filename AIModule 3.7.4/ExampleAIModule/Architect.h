#pragma once
#include <BWAPI.h>
#include <algorithm>
#include "WorkerManager.h"
#include "Accountant.h"

struct Zone {
	int left, top, right, bottom;

	Zone(int left, int top, int right, int bottom) :
		left(left), top(top), right(right), bottom(bottom)
	{
	}

	bool Zone::contains(int x, int y)
	{
		return
			left <= x &&
			top <= y &&
			right > x &&
			bottom > y;
	}

	bool Zone::contains(BWAPI::TilePosition pos, BWAPI::UnitType unitType)
	{
		for (int x = pos.x(); x < pos.x() + unitType.tileWidth(); x++)
			for (int y = pos.y(); y < pos.y() + unitType.tileHeight(); y++)
				if (contains(x, y))
					return true;
		return false;
	}
};

static const int MIN_SUPPLY = 10;
static const BWAPI::UnitType BUILD_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;

class Architect
{
private:
	// Managers
	WorkerManager * workerManager;
	Accountant * accountant;

	// Local
	bool harvestingDefined; // Temporary stupid solution, change when information manager is introduced.
	BWAPI::Unit * depot;
	std::set<BWAPI::Unit*> pylons;
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>  buildSchedule;

public:
	Zone harvesting; //TODO Move to private.

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
	void expandHarvesting(BWAPI::Unit * resource);
	void addPylon(BWAPI::Unit * pylon);
	void removePylon(BWAPI::Unit * pylon);
	void setDepot(BWAPI::Unit * depot);
	void update();
};