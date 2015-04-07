#include "UtilUnit.h"


// Commands a unit with a given command type and target.
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target)
{
	command(unit, commandType, target, BWAPI::Positions::None);
}


// Commands a unit with a given command type and position.
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position position)
{
	command(unit, commandType, NULL, position);
}


// Commands a unit with a given command type with either target or position or both.
// TODO What is the last argument in unitCommand used for?
// TODO This is broken! Too often units stand still, because lastCommand is only used in BWAPI, not actually representative of the units CURRENT action.
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target, BWAPI::Position position)
{
	// Validate the unit.
	if (unit && unit->exists() && isOwned(unit))
	{
		// Ensure the unit has not recieved a command this frame.
		if (unit->getLastCommandFrame() < BWAPI::Broodwar->getFrameCount())
		{
			// Ensure the current unit command is not identical to the new one.
			BWAPI::UnitCommand lastCommand = unit->getLastCommand();
			if (lastCommand.getType() != commandType ||
				lastCommand.getTargetPosition() != position ||
				lastCommand.getTarget() != target)
				unit->issueCommand(BWAPI::UnitCommand(unit, commandType, target, position.x(), position.y(), 0));
		} // Closure: Already commanded.
	} // Closure: Invalid unit.
}


// Commands a unit to build at a location.
// TODO Merge with command somehow.
void utilUnit::commandBuild(BWAPI::Unit * unit, BWAPI::TilePosition location, BWAPI::UnitType buildingType)
{
	// Validate the unit.
	if (unit && unit->exists() && isOwned(unit))
	{
		// Ensure the unit has not recieved a command this frame.
		if (unit->getLastCommandFrame() < BWAPI::Broodwar->getFrameCount())
		{
			// Ensure the current unit command is not identical to the new one.
			BWAPI::UnitCommand lastCommand = unit->getLastCommand();
			if (lastCommand.getType() != BWAPI::UnitCommandTypes::Build ||
				lastCommand.getTargetPosition() != location)
				unit->build(location, buildingType);
		} // Closure: Uniqueness
	} // Closure: Validity
}

bool utilUnit::inRegion(BWAPI::Position pos, BWTA::Region * region)
{
	return region && BWTA::getRegion(pos) == region;
}

// Returns true if the unit is owned by an enemy and false otherwise.
// TODO Can this be done more effectively?
bool utilUnit::isEnemy(BWAPI::Unit * unit)
{
	return unit && BWAPI::Broodwar->self()->isEnemy(unit->getPlayer());
}


// Returns true if the unit is owned and false otherwise.
// TODO Can this be done more effectively?
bool utilUnit::isOwned(BWAPI::Unit * unit)
{
	return unit && unit->getPlayer() == BWAPI::Broodwar->self();
}