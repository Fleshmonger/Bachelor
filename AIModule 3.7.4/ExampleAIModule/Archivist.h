#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <boost/foreach.hpp>

const BWAPI::UnitType
	INFANTRY_FACTORY = BWAPI::UnitTypes::Protoss_Gateway,
	INFANTRY_UNIT = BWAPI::UnitTypes::Protoss_Zealot;

// Manages battlefield information.
class Archivist
{
private:
	BWTA::Region * homeRegion;
	std::set<BWAPI::Unit*> units, troops, buildings, invaders;
	std::map<BWAPI::Unit*, BWAPI::Position> positions;

public:
	Archivist();
	~Archivist();
	void recordUnit(BWAPI::Unit * unit);
	void clearUnit(BWAPI::Unit * unit);
	void recordUnitPosition(BWAPI::Unit * unit);
	void setHomeRegion(BWTA::Region * region);
	bool isArchived(BWAPI::Unit * unit);
	void update();
	BWAPI::Position getPosition(BWAPI::Unit * unit);
	std::set<BWAPI::Unit*> getBuildings();
	std::set<BWAPI::Unit*> getInvaders();
};