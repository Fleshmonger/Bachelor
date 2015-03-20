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

// Orders a unit with a given order type with either target or position or both.
// TODO What is the last argument in unitCommand used for?
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
			if (lastCommand.getType() != commandType || lastCommand.getTargetPosition() != position || lastCommand.getTarget() != target)
				unit->issueCommand(BWAPI::UnitCommand(unit, commandType, target, position.x(), position.y(), 0));
		} // Closure: Already commanded.
	} // Closure: Invalid unit.
}

// Returns true if the unit is owned by an enemy and false otherwise.
// TODO Can this be done more effectively?
bool utilUnit::isEnemy(BWAPI::Unit * unit)
{
	return BWAPI::Broodwar->self()->isEnemy(unit->getPlayer());
}

// Returns true if the unit is owned and false otherwise.
// TODO Can this be done more effectively?
bool utilUnit::isOwned(BWAPI::Unit * unit)
{
	return unit->getPlayer() == BWAPI::Broodwar->self();
}