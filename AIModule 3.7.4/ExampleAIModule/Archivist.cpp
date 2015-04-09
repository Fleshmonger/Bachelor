#include "Archivist.h"


// Constructor
Archivist::Archivist() :
	homeRegion(NULL),
	units(),
	buildings(),
	depots(),
	refineries(),
	turrets(),
	workers(),
	troops(),
	positions(),
	types()
{
}


// Deconstructor
Archivist::~Archivist()
{
}


// Fired when the map is analyzed. Finds the home region.
void Archivist::analyzed()
{
	homeRegion = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
}

// Private
// Determines whether unit is a special case such as overlord or larvae
bool Archivist::isMisc(BWAPI::UnitType unitType)
{
	return
		unitType == BWAPI::UnitTypes::Zerg_Larva ||
		unitType == BWAPI::UnitTypes::Zerg_Overlord ||
		unitType == BWAPI::UnitTypes::Zerg_Egg ||
		unitType == BWAPI::UnitTypes::Zerg_Lurker_Egg ||
		unitType == BWAPI::UnitTypes::Terran_Nuclear_Missile ||
		unitType == BWAPI::UnitTypes::Protoss_Scarab ||
		unitType == BWAPI::UnitTypes::Spell_Scanner_Sweep ||
		unitType == BWAPI::UnitTypes::Spell_Dark_Swarm ||
		unitType == BWAPI::UnitTypes::Spell_Disruption_Web;
}


// Private
// Determines whether a unit is a defensive structure.
// TODO Unneeded?
// TODO Excludes bunkers - is this behavior wished?
bool Archivist::isTurret(BWAPI::UnitType unitType)
{
	return
		unitType.isBuilding() && unitType.canAttack();
}


// Removes a unit from the knowledge pool.
// TODO Code duplication with recordUnit.
void Archivist::clearUnit(BWAPI::Unit * unit)
{
	BWAPI::UnitType unitType = unit->getType();
	units.erase(unit);
	positions.erase(unit);
	types.erase(unit);
	if (unitType.isBuilding())
	{
		buildings.erase(unit);
		if (unitType.isResourceDepot())
			depots.erase(unit);
		else if (unitType.isRefinery())
			refineries.erase(unit);
		else if (isTurret(unitType))
			turrets.erase(unit);
	}
	else if (unitType.isWorker())
		workers.erase(unit);
	else if (!isMisc(unitType))
		troops.erase(unit);
}


// Inserts a unit to the knowledge pool.
void Archivist::recordUnit(BWAPI::Unit * unit)
{
	BWAPI::UnitType unitType = unit->getType();
	units.insert(unit);
	recordUnitPosition(unit);
	recordUnitType(unit);
	if (unitType.isBuilding())
	{
		buildings.insert(unit);
		if (unitType.isResourceDepot())
			depots.insert(unit);
		else if (unitType.isRefinery())
			refineries.insert(unit);
		else if (isTurret(unitType))
			turrets.insert(unit);
	}
	else if (unitType.isWorker())
		workers.erase(unit);
	else if (!isMisc(unitType))
		troops.insert(unit);
}


// Updates the position of a unit if it is visible.
void Archivist::recordUnitPosition(BWAPI::Unit * unit)
{
	if (unit->isVisible())
		positions[unit] = unit->getPosition();
	else if (!isArchived(unit) || BWAPI::Broodwar->isVisible(BWAPI::TilePosition(positions[unit])))
		positions[unit] = BWAPI::Positions::None;
}


// Updates the type of a unit if it is visible.
void Archivist::recordUnitType(BWAPI::Unit * unit)
{
	if (unit->isVisible())
		types[unit] = unit->getType();
	else
		types[unit] = BWAPI::UnitTypes::None;
}


// Revises records to match new game state.
void Archivist::update()
{
	// Update new unit positions.
	BOOST_FOREACH(BWAPI::Unit * unit, units)
		if (unit->isVisible())
			recordUnitPosition(unit);

	// Validate refineries haven't become geysers.
	{
		std::set<BWAPI::Unit*>::iterator it = refineries.begin();
		while (it != refineries.end())
		{
			BWAPI::Unit * refinery = *it;
			++it;
			if (!refinery->getType().isRefinery())
				clearUnit(refinery);
		}
	}
}


// Returns whether a unit is within a given region.
bool Archivist::inRegion(BWAPI::Unit * unit, BWTA::Region * region)
{
	return utilUnit::inRegion(getPosition(unit), region);
}


// Returns whether or not the unit exists in the knowledge pool.
bool Archivist::isArchived(BWAPI::Unit * unit)
{
	return units.count(unit) > 0;
}


// Returns the position an enemy was last seen.
BWAPI::Position Archivist::getPosition(BWAPI::Unit * unit)
{
	if (utilUnit::isOwned(unit))
		return unit->getPosition();
	else
	{
		if (unit && isArchived(unit))
			return positions[unit];
		else
			return BWAPI::Positions::None;
	}
}


// Returns the type an enemy was last seen as.
BWAPI::UnitType Archivist::getType(BWAPI::Unit * unit)
{
	if (utilUnit::isOwned(unit))
		return unit->getType();
	else
	{
		if (unit && isArchived(unit))
			return types[unit];
		else
			return BWAPI::UnitTypes::None;
	}
}


// Returns the recorded home region.
BWTA::Region * Archivist::getHomeRegion()
{
	return homeRegion;
}


// Finds and returns all invaders.
utilUnit::UnitSet Archivist::invaders()
{
	std::set<BWAPI::Unit*> invaders;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
		if (utilUnit::isEnemy(unit) && inRegion(unit, homeRegion))
			invaders.insert(unit);
	return invaders;
}


// Returns a copy of recorded units.
utilUnit::UnitSet Archivist::getUnits()
{
	return units;
}


// Returns a copy of recorded buildings.
utilUnit::UnitSet Archivist::getBuildings()
{
	return buildings;
}


// Returns a copy of recorded depots.
utilUnit::UnitSet Archivist::getDepots()
{
	return depots;
}


// Returns a copy of recorded turrets.
utilUnit::UnitSet Archivist::getTurrets()
{
	return turrets;
}


// Returns a copy of recorded workers.
utilUnit::UnitSet Archivist::getWorkers()
{
	return workers;
}


// Returns a copy of recorded troops.
utilUnit::UnitSet Archivist::getTroops()
{
	return troops;
}