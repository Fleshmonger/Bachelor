#include "Archivist.h"

// Constructor
Archivist::Archivist() :
	homeRegion(NULL),
	units(std::set<BWAPI::Unit*>()),
	troops(std::set<BWAPI::Unit*>()),
	buildings(std::set<BWAPI::Unit*>()),
	depots(std::set<BWAPI::Unit*>()),
	refineries(std::set<BWAPI::Unit*>()),
	positions(std::map<BWAPI::Unit*, BWAPI::Position>())
{
}

// Deconstructor
Archivist::~Archivist()
{
}

// Removes a unit from the knowledge pool.
void Archivist::clearUnit(BWAPI::Unit * unit)
{
	BWAPI::UnitType unitType = unit->getType();
	units.erase(unit);
	positions.erase(unit);
	if (unitType.isBuilding())
	{
		buildings.erase(unit);
		if (unitType.isResourceDepot())
			depots.erase(unit);
		else if (unitType.isRefinery())
			refineries.erase(unit);
	}
	else
		troops.erase(unit);
}

// Inserts a unit to the knowledge pool.
void Archivist::recordUnit(BWAPI::Unit * unit)
{
	BWAPI::UnitType unitType = unit->getType();
	units.insert(unit);
	recordUnitPosition(unit);
	if (unitType.isBuilding())
	{
		buildings.insert(unit);
		if (unitType.isResourceDepot())
			depots.insert(unit);
		else if (unitType.isRefinery())
			refineries.insert(unit);
	}
	else
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

	// Ensure refineries haven't been destroyed.
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

// Returns whether or not the unit exists in the knowledge pool.
bool Archivist::isArchived(BWAPI::Unit * unit)
{
	return units.count(unit) > 0;
}

// Returns the position an enemy was last seen.
BWAPI::Position Archivist::getPosition(BWAPI::Unit * unit)
{
	if (isArchived(unit))
		return positions[unit];
	else
		return BWAPI::Positions::None;
}

// Finds and returns all invaders.
std::set<BWAPI::Unit*> Archivist::invaders()
{
	std::set<BWAPI::Unit*> invaders;
	BOOST_FOREACH(BWAPI::Unit * unit, units)
		if (BWTA::getRegion(positions[unit]) == homeRegion)
			invaders.insert(unit);
	return invaders;
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

// Returns a copy of recorded troops.
std::set<BWAPI::Unit*> Archivist::getTroops()
{
	return troops;
}