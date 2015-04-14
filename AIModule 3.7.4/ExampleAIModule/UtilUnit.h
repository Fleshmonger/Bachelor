#pragma once
#include <BWAPI.h>
#include <BWTA.h>


// Contains common functions for units.
namespace utilUnit
{
	typedef std::list<BWAPI::Unit*> UnitList;
	typedef std::set<BWAPI::Unit*> UnitSet;

	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position position);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target, BWAPI::Position position);
	void commandBuild(BWAPI::Unit * unit, BWAPI::TilePosition, BWAPI::UnitType buildingType);

	bool inRegion(BWAPI::Position position, BWTA::Region * region);
	bool isEnemy(BWAPI::Unit * unit);
	bool isOwned(BWAPI::Unit * unit);
}