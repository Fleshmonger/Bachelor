#include "Archivist.h"


// Constructor
Archivist::Archivist() :
	homeRegion(),
	units(),
	enemies(),
	buildings(),
	depots(),
	refineries(),
	turrets(),
	workers(),
	troops(),
	geysers(),
	positions(),
	types()
{
}


// Deconstructor
Archivist::~Archivist()
{
}


// Called after BWTA analysis. Sets home region.
void Archivist::initialize()
{
	// Set home region.
	homeRegion = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
}


// Removes a unit from the knowledge pool.
// TODO Code duplication with recordUnit.
void Archivist::clearUnit(BWAPI::Unit * unit)
{
	// Verify unit.
	if (isArchived(unit))
	{
		// Player check.
		BWAPI::UnitType unitType = unit->getType();
		units.erase(unit);
		if (utilUnit::isEnemy(unit))
		{
			// Type check.
			enemies.erase(unit);
			if (unitType.isBuilding())
			{
				// Sub-type check.
				buildings.erase(unit);
				if (unitType.isResourceDepot())
					depots.erase(unit);
				else if (unitType.isRefinery())
					refineries.erase(unit);
				else if (utilUnit::isTurret(unitType))
					turrets.erase(unit);
			}
			else if (unitType.isWorker())
				workers.erase(unit);
			else if (!utilUnit::isMisc(unitType) && !utilUnit::isSupport(unitType))
				troops.erase(unit);
		}
		else if (unitType == BWAPI::UnitTypes::Resource_Vespene_Geyser)
			geysers.erase(unit);
		positions.erase(unit);
		types.erase(unit);
	}
}


// Inserts a unit to the knowledge pool.
void Archivist::recordUnit(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		!utilUnit::isOwned(unit))
	{
		// Add entry.
		BWAPI::UnitType unitType = unit->getType();
		units.insert(unit);

		// Player check.
		if (utilUnit::isEnemy(unit))
		{
			// Add enemy.
			enemies.insert(unit);

			// Type check.
			if (unitType.isBuilding())
			{
				// Add building.
				buildings.insert(unit);

				// Sub-type check.
				if (unitType.isResourceDepot())
					depots.insert(unit);
				else if (unitType.isRefinery())
					refineries.insert(unit);
				else if (utilUnit::isTurret(unitType))
					turrets.insert(unit);
			}
			else if (unitType.isWorker())
				workers.insert(unit);
			else if (!utilUnit::isMisc(unitType) && !utilUnit::isSupport(unitType))
				troops.insert(unit);
		}
		else if (unitType == BWAPI::UnitTypes::Resource_Vespene_Geyser)
			geysers.insert(unit);
		recordUnitStatus(unit);
	}
}


// Updates the status of a unit if it is visible.
void Archivist::recordUnitStatus(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit)
	{
		// Visibility check.
		if (unit->isVisible())
		{
			positions[unit] = unit->getPosition();
			types[unit] = unit->getType();
		}
		else if (!isArchived(unit))
		{
			positions[unit] = BWAPI::Positions::None;
			types[unit] = BWAPI::UnitTypes::None;
		}
		else if (BWAPI::Broodwar->isVisible(BWAPI::TilePosition(positions[unit])))
			positions[unit] = BWAPI::Positions::None;
	}
}


// Revises records to match new game state.
void Archivist::update()
{
	// Iterate through units.
	utilUnit::UnitSet::iterator
		it = units.begin(),
		end = units.end();
	while (it != end)
	{
		// Update unit state.
		BWAPI::Unit * unit = *it;
		recordUnitStatus(unit);
		it++;

		// Remove unknown immobile units.
		BWAPI::UnitType unitType = types[unit];
		if (!positions[unit] &&
			unitType &&
			!unitType.canMove() &&
			!unitType.isFlyingBuilding())
		{
			clearUnit(unit);
			end = units.end();
		}
	}

	// Verify geysers.
	utilUnit::UnitSet::iterator
		geysersIt = geysers.begin(),
		geysersEnd = geysers.end();
	while (geysersIt != geysersEnd)
	{
		// Verify geyser.
		BWAPI::Unit * geyser = *geysersIt;
		if (utilUnit::isEnemy(geyser))
		{
			// Move geyser to enemy refineries.
			geysersIt = geysers.erase(geysersIt);
			refineries.insert(geyser);
			buildings.insert(geyser);
			enemies.insert(geyser);
			geysersEnd = geysers.end();
		}
		else
			geysersIt++;
	}

	// Verify refineries.
	std::set<BWAPI::Unit*>::iterator
		refineriesIt = refineries.begin(),
		refineriesEnd = refineries.end();
	while (refineriesIt != refineriesEnd)
	{
		// Verify refinery.
		BWAPI::Unit * refinery = *refineriesIt;
		if (!types[refinery].isRefinery())
		{
			// Move refinery to geyser.
			refineriesIt = refineries.erase(refineriesIt);
			buildings.erase(refinery);
			enemies.erase(refinery);
			geysers.insert(refinery);
			refineriesEnd = refineries.end();
		}
		else
			refineriesIt++;
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


// Returns a copy of recorded enemies.
utilUnit::UnitSet Archivist::getEnemies()
{
	return enemies;
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