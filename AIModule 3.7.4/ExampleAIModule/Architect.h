#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"
#include "Accountant.h"
#include "WorkerManager.h"


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


// Manages building construction orders.
//TODO Cleanup.
class Architect
{
private:
	Accountant * accountant;
	WorkerManager * workerManager;

	Zone harvesting;
	std::multimap<BWAPI::UnitType, BWAPI::Unit*> constructSchedule;
	std::multimap<BWAPI::UnitType, std::pair<BWAPI::Unit*, BWAPI::TilePosition>>  buildSchedule;

public:
	Architect(Accountant * accountant, WorkerManager * workerManager);
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