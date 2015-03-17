#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>

// Manages battlefield information.
class Archivist
{
private:
	BWTA::Region * homeRegion;
	std::set<BWAPI::Unit*> units, buildings, depots, refineries, troops;
	std::map<BWAPI::Unit*, BWAPI::Position> positions;

public:
	Archivist();
	~Archivist();

	void clearUnit(BWAPI::Unit * unit);
	void recordUnit(BWAPI::Unit * unit);
	void recordUnitPosition(BWAPI::Unit * unit);
	void setHomeRegion(BWTA::Region * region);
	void update();

	bool isArchived(BWAPI::Unit * unit);

	BWAPI::Position getPosition(BWAPI::Unit * unit);

	std::set<BWAPI::Unit*> invaders();
	std::set<BWAPI::Unit*> getBuildings();
	std::set<BWAPI::Unit*> getDepots();
	std::set<BWAPI::Unit*> getTroops();
};