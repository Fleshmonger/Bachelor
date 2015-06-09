#pragma once
#include <BWAPI.h>
#include <BWTA.h>


// Contains common functions and definitions for units.
namespace utilUnit
{
	typedef std::set<BWAPI::Unit*> UnitSet;
	typedef std::list<BWAPI::Unit*> UnitList;

	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position position);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target, BWAPI::Position position);
	void commandBuild(BWAPI::Unit * unit, BWAPI::TilePosition, BWAPI::UnitType buildingType);

	bool isCommanded(BWAPI::Unit * unit);
	bool isEnemy(BWAPI::Unit * unit);
	bool isOwned(BWAPI::Unit * unit);
	bool isFighter(BWAPI::UnitType unitType);
	bool isGeyser(BWAPI::UnitType unitType);
	bool isMisc(BWAPI::UnitType unitType);
	bool isSupport(BWAPI::UnitType unitType);
	bool isTurret(BWAPI::UnitType unitType);
}