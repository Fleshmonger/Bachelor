#pragma once
#include <BWAPI.h>

// Contains generally used functions for units.
// TODO Rename both files and namespace?
namespace utilUnit
{
	void orderUnit(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position location);

	bool isEnemy(BWAPI::Unit * unit);
	bool isOwned(BWAPI::Unit * unit);
}