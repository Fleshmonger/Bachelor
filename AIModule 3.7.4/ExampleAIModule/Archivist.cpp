#include "Archivist.h"


// Constructor
Archivist::Archivist() :
	homeRegion(NULL),
	units(std::set<BWAPI::Unit*>()),
	buildings(std::set<BWAPI::Unit*>()),
	depots(std::set<BWAPI::Unit*>()),
	refineries(std::set<BWAPI::Unit*>()),
	turrets(std::set<BWAPI::Unit*>()),
	workers(std::set<BWAPI::Unit*>()),
	troops(std::set<BWAPI::Unit*>()),
	positions(std::map<BWAPI::Unit*, BWAPI::Position>()),
	types(std::map<BWAPI::Unit*, BWAPI::UnitType>())
{
}


// Deconstructor
Archivist::~Archivist()
{
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


// Designates the home region.
void Archivist::setHomeRegion(BWTA::Region * region)
{
	homeRegion = region;
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
	return region && BWTA::getRegion(getPosition(unit)) == region;
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
		if (isArchived(unit))
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
		if (isArchived(unit))
			return types[unit];
		else
			return BWAPI::UnitTypes::None;
	}
}


// Finds and returns all invaders.
std::set<BWAPI::Unit*> Archivist::invaders()
{
	std::set<BWAPI::Unit*> invaders;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
		if (utilUnit::isEnemy(unit) && inRegion(unit, homeRegion))
			invaders.insert(unit);
	return invaders;
}


// Returns the recorded home region.
BWTA::Region * Archivist::getHomeRegion()
{
	return homeRegion;
}


// Returns a copy of recorded buildings.
std::set<BWAPI::Unit*> Archivist::getBuildings()
{
	return buildings;
}


// Returns a copy of recorded depots.
std::set<BWAPI::Unit*> Archivist::getDepots()
{
	return depots;
}


// Returns a copy of recorded turrets.
std::set<BWAPI::Unit*> Archivist::getTurrets()
{
	return turrets;
}


// Returns a copy of recorded workers.
std::set<BWAPI::Unit*> Archivist::getWorkers()
{
	return workers;
}


// Returns a copy of recorded troops.
std::set<BWAPI::Unit*> Archivist::getTroops()
{
	return troops;
}