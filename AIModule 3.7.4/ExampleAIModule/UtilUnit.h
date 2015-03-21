#pragma once
#include <BWAPI.h>

// Contains generally used functions for units.
// TODO Rename both files and namespace?
namespace utilUnit
{
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position position);
	void command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target, BWAPI::Position position);
	void commandBuild(BWAPI::Unit * unit, BWAPI::TilePosition, BWAPI::UnitType buildingType);

	bool isEnemy(BWAPI::Unit * unit);
	bool isOwned(BWAPI::Unit * unit);
}