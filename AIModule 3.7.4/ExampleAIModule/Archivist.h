#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>
#include "UtilUnit.h"


// Collects battlefield information, mostly about opponents units.
// TODO Only collects info about enemy - rename or restructure?
// TODO Does not account for morphed creatures (important with drones for example!)
// TODO Rename to avoid confusion with architect? Sage perhaps?
// TODO only have 1 entry for each unit; use "master sets" to contain sub-sets.
class Archivist
{
private:
	BWTA::Region * homeRegion;
	utilUnit::UnitSet
		units,
		enemies,
		buildings,
		depots,
		refineries,
		turrets,
		workers,
		troops,
		geysers;
	std::map<BWAPI::Unit*, BWAPI::Position> positions;
	std::map<BWAPI::Unit*, BWAPI::UnitType> types;

public:
	Archivist();
	~Archivist();

	void analyzed();
	void clearUnit(BWAPI::Unit * unit);
	void recordUnit(BWAPI::Unit * unit);
	void recordUnitStatus(BWAPI::Unit * unit);
	void update();

	bool inRegion(BWAPI::Unit * unit, BWTA::Region * region);
	bool isArchived(BWAPI::Unit * unit);

	BWAPI::Position getPosition(BWAPI::Unit * unit);
	BWAPI::UnitType getType(BWAPI::Unit * unit);

	BWTA::Region * getHomeRegion(); // TODO Move this to analyzer class

	utilUnit::UnitSet invaders();
	utilUnit::UnitSet getEnemies();
	utilUnit::UnitSet getBuildings();
	utilUnit::UnitSet getDepots();
	utilUnit::UnitSet getTurrets();
	utilUnit::UnitSet getWorkers();
	utilUnit::UnitSet getTroops();
};