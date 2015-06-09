#include "UtilUnit.h"


// Commands a unit with a given command type.
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType)
{
	command(unit, commandType, NULL, BWAPI::Positions::None);
}


// Commands a unit with a given command type and position.
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Position position)
{
	command(unit, commandType, NULL, position);
}


// Commands a unit with a given command type and target.
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target)
{
	command(unit, commandType, target, BWAPI::Positions::None);
}


// Commands a unit with a given command type with either target or position or both.
// TODO What is the last argument in unitCommand used for?
// TODO This does not properly work with attack, movement or when low-ai auto commands units (for example, worker retreat).
void utilUnit::command(BWAPI::Unit * unit, BWAPI::UnitCommandType commandType, BWAPI::Unit * target, BWAPI::Position position)
{
	// Validate the unit.
	if (unit &&
		unit->exists() &&
		isOwned(unit) &&
		!isCommanded(unit))
	{
		// Ensure the current unit command is not identical to the new one.
		BWAPI::UnitCommand lastCommand = unit->getLastCommand();
		if (unit->isIdle() ||
			lastCommand.getType() != commandType ||
			lastCommand.getTargetPosition() != position ||
			lastCommand.getTarget() != target)
			unit->issueCommand(BWAPI::UnitCommand(unit, commandType, target, position.x(), position.y(), 0));
	}
}


// Commands a unit to build at a location.
// TODO Merge with command somehow.
void utilUnit::commandBuild(BWAPI::Unit * unit, BWAPI::TilePosition location, BWAPI::UnitType buildingType)
{
	// Validate the unit.
	if (unit &&
		unit->exists() &&
		isOwned(unit) &&
		!isCommanded(unit))
	{
		// Ensure the current unit command is not identical to the new one.
		BWAPI::UnitCommand lastCommand = unit->getLastCommand();
		if (unit->isIdle() ||
			lastCommand.getType() != BWAPI::UnitCommandTypes::Build ||
			lastCommand.getTargetPosition() != location ||
			lastCommand.getUnitType() != buildingType)
			unit->build(location, buildingType);
	}
}


// Returns true if the unit has been commanded this frame.
bool utilUnit::isCommanded(BWAPI::Unit * unit)
{
	return unit && unit->getLastCommandFrame() == BWAPI::Broodwar->getFrameCount();
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


// Determines if a unit-type is non-building and combat based.
bool utilUnit::isFighter(BWAPI::UnitType unitType)
{
	return !unitType.isBuilding() && !isMisc(unitType) && !isSupport(unitType);
}


// Determines if a unit-type is a vespene geyser.
bool utilUnit::isGeyser(BWAPI::UnitType unitType)
{
	return unitType == BWAPI::UnitTypes::Resource_Vespene_Geyser;
}


// Determines if a unit-type is a special case.
bool utilUnit::isMisc(BWAPI::UnitType unitType)
{
	return
		unitType == BWAPI::UnitTypes::Zerg_Larva ||
		unitType == BWAPI::UnitTypes::Terran_Nuclear_Missile ||
		unitType == BWAPI::UnitTypes::Protoss_Scarab ||
		unitType == BWAPI::UnitTypes::Spell_Scanner_Sweep ||
		unitType == BWAPI::UnitTypes::Spell_Dark_Swarm ||
		unitType == BWAPI::UnitTypes::Spell_Disruption_Web;
}


// Determines if a unit-type is economy based.
//TODO Should eggs be in this or in combateer?
bool utilUnit::isSupport(BWAPI::UnitType unitType)
{
	return
		unitType.isWorker() ||
		unitType == BWAPI::UnitTypes::Zerg_Overlord ||
		unitType == BWAPI::UnitTypes::Zerg_Egg ||
		unitType == BWAPI::UnitTypes::Zerg_Lurker_Egg;
}


// Determines if a unit-type is a defensive structure.
// TODO Excludes bunkers - is this behavior wished?
bool utilUnit::isTurret(BWAPI::UnitType unitType)
{
	return
		unitType.isBuilding() && unitType.canAttack();
}