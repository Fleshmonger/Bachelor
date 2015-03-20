#include "UtilUnit.h"

// Orders a unit with a given order type and target.
// TODO Currently only supports move and attack_move
void utilUnit::orderUnit(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position location)
{
	// Validate the unit.
	if (unit && unit->exists() && isOwned(unit))
	{
		// Ensure the unit has not recieved a command this frame.
		if (unit->getLastCommandFrame() < BWAPI::Broodwar->getFrameCount())
		{
			// Ensure the current unit command is not identical to the new one.
			BWAPI::UnitCommand lastCommand = unit->getLastCommand();
			if (lastCommand.getType() != commandType || lastCommand.getTargetPosition() != location)
			{
				if (commandType == BWAPI::UnitCommandTypes::Attack_Move)
					unit->attack(location);
				else if (commandType == BWAPI::UnitCommandTypes::Move)
					unit->move(location);
			} // Closure: Identical command.
		} // Closure: Already commanded.
	} // Closure: Invalid unit.
}

// Returns true if the unit is owned by an enemy and false otherwise.
// TODO This can probably be done cheaper.
bool utilUnit::isEnemy(BWAPI::Unit * unit)
{
	return BWAPI::Broodwar->self()->isEnemy(unit->getPlayer());
}

// Returns true if the unit is owned and false otherwise.
// TODO This can probably be done cheaper.
bool utilUnit::isOwned(BWAPI::Unit * unit)
{
	return unit->getPlayer() == BWAPI::Broodwar->self();
}