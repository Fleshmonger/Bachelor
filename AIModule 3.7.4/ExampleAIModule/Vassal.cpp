#include "Vassal.h"


// Constructor
Vassal::Vassal(BWAPI::Unit * depot, Accountant * accountant, Producer * producer) :
	accountant(accountant),
	producer(producer),
	depot(depot),
	region(BWTA::getRegion(depot->getPosition())),
	workerManager(),
	harvester(&workerManager),
	architect(accountant, &workerManager)
{
	// Designate depot.
	architect.expandHarvesting(depot);

	// Designate resources.
	BOOST_FOREACH(BWAPI::Unit * mineral, BWAPI::Broodwar->getStaticMinerals())
	{
		if (utilUnit::inRegion(mineral->getPosition(), region))
		{
			harvester.addMineral(mineral);
			architect.expandHarvesting(mineral);
		}
	}
	BOOST_FOREACH(BWAPI::Unit * geyser, BWAPI::Broodwar->getStaticGeysers())
	{
		if (utilUnit::inRegion(geyser->getPosition(), region))
			architect.expandHarvesting(geyser);
	}
}


// Destructor
Vassal::~Vassal()
{
}


// Adds a unit to the vassalage.
void Vassal::unitCompleted(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		utilUnit::isOwned(unit))
	{
		// Identify type.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
			architect.completeConstruct(unit);
		else if (unitType.isWorker())
			workerManager.addWorker(unit);
	}
}


// Assigns a unit under construction to the vassalage.
void Vassal::unitCreated(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit &&
		unit->exists() &&
		utilUnit::isOwned(unit))
	{
		// Identify type.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding())
		{
			architect.completeBuild(unit);
			architect.scheduleConstruct(unit);
		}
	}
}


// Removes a unit from the vassalage.
void Vassal::unitDestroyed(BWAPI::Unit * unit)
{
	// Verify unit.
	if (unit)
	{
		// Identify type.
		BWAPI::UnitType unitType = unit->getType();
		if (unitType.isBuilding() && !unit->isCompleted())
			architect.removeConstruct(unit);
		else if (unitType.isWorker() && unit->isCompleted())
		{
			workerManager.removeWorker(unit);
			harvester.removeMiner(unit);
		}
	}
}


// Updates local managers.
void Vassal::update()
{
	// Update managers.
	workerManager.update();
	architect.update();
	harvester.update();
}


// Constructs a building if able.
bool Vassal::build(BWAPI::UnitType buildingType)
{
	if (depot)
		return architect.scheduleBuild(buildingType, depot->getTilePosition());
	else
		return false;
}


// Returns the amount of minerals in the region.
unsigned int Vassal::mineralFields()
{
	return harvester.mineralFields();
}


// Returns the amount of workers.
unsigned int Vassal::workforce()
{
	return workerManager.workforce();
}


// Returns the amount of scheduled units of the specified type.
unsigned int Vassal::scheduled(BWAPI::UnitType unitType)
{
	return architect.scheduled(unitType);
}


// Returns the designated depot.
BWAPI::Unit * Vassal::getDepot()
{
	return depot;
}