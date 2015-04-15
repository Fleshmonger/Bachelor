#include "Vassal.h"


// Constructor
Vassal::Vassal(BWAPI::Unit * depot, Accountant * accountant) :
	accountant(accountant),
	depot(depot),
	region(BWTA::getRegion(depot->getPosition())),
	workerManager(),
	producer(accountant),
	harvester(&workerManager),
	architect(accountant, &workerManager)
{
	// Designate depot.
	architect.expandHarvesting(depot);
	producer.addFactory(depot);

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
		{
			architect.completeConstruct(unit);
			if (unitType.canProduce())
				producer.addFactory(unit);
		}
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
		else
			producer.addProduction(unit);
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
		if (unitType.isBuilding())
		{
			if (!unit->isCompleted())
				architect.removeConstruct(unit);
			else if (unitType.canProduce())
				producer.removeFactory(unit);
		}
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


// Returns whether the vassal needs more miners.
// TODO Rename and/or restrutucture.
bool Vassal::needMiners()
{
	return workerManager.workforce() < harvester.minersMax();
}


// Constructs a building if able.
bool Vassal::build(BWAPI::UnitType buildingType)
{
	if (depot)
		return architect.scheduleBuild(buildingType, depot->getTilePosition());
	else
		return false;
}


// Trains a unit if able.
bool Vassal::train(BWAPI::UnitType unitType)
{
	return producer.scheduleTraining(unitType);
}


// Returns the amount of scheduled units of the specified type.
//TODO Add producer scheduling support.
unsigned int Vassal::scheduled(BWAPI::UnitType unitType)
{
	if (unitType.isBuilding())
		return architect.scheduled(unitType);
	else
		return 0;
}