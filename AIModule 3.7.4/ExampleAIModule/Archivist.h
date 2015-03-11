#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>

// Manages battlefield information.
class Archivist
{
private:
	BWTA::Region * homeRegion;
	std::set<BWAPI::Unit*> units, troops, buildings, invaders, depots;
	std::map<BWAPI::Unit*, BWAPI::Position> positions;

public:
	Archivist();
	~Archivist();

	void recordUnit(BWAPI::Unit * unit);
	void clearUnit(BWAPI::Unit * unit);
	void recordUnitPosition(BWAPI::Unit * unit);
	void setHomeRegion(BWTA::Region * region);
	void update();

	bool isArchived(BWAPI::Unit * unit);

	BWAPI::Position getPosition(BWAPI::Unit * unit);
	std::set<BWAPI::Unit*> getBuildings();
	std::set<BWAPI::Unit*> getInvaders();
	std::set<BWAPI::Unit*> getDepots();
};