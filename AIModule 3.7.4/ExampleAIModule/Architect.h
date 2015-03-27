#pragma once
#include <BWAPI.h>
#include <algorithm> //TODO Unused?
#include "UtilUnit.h"
#include "Accountant.h"
#include "WorkerManager.h"


static const int MIN_SUPPLY = 10;
static const BWAPI::UnitType BUILD_SUPPLY = BWAPI::UnitTypes::Protoss_Pylon;


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


class Architect
{
private:
	Accountant * accountant;
	WorkerManager * workerManager;

	bool harvestingDefined; // Temporary stupid solution, change when information manager is introduced.

	BWAPI::Unit * depot;
	utilUnit::UnitSet pylons;
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>  buildSchedule;

	Zone harvesting;

public:
	Architect(Accountant * accountant, WorkerManager * workerManager);
	~Architect();

	bool scheduleBuild(BWAPI::UnitType buildingType); // TODO Not symmetrical signature with scheduleConstruct
	void scheduleConstruct(BWAPI::Unit * building);
	void removeBuild(BWAPI::UnitType buildingType, BWAPI::TilePosition buildLocation);
	void removeConstruct(BWAPI::Unit * building);
	void completeBuild(BWAPI::Unit * building);
	void completeConstruct(BWAPI::Unit * building);
	void expandHarvesting(BWAPI::Unit * resource);
	void addPylon(BWAPI::Unit * pylon);
	void removePylon(BWAPI::Unit * pylon);
	void setDepot(BWAPI::Unit * depot);
	void update();

	bool validBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition location, BWAPI::UnitType buildingType);

	int scheduled(BWAPI::UnitType buildingType);

	BWAPI::TilePosition getBuildLocation(BWAPI::Unit * builder, BWAPI::TilePosition desiredLocation, BWAPI::UnitType buildingType);
};