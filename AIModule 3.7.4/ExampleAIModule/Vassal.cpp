#include "Vassal.h"


// Constructor.
Vassal::Vassal(BWTA::Region * region) :
	depot(),
	region(region),
	taskmaster()
{
}


// Destructor.
Vassal::~Vassal()
{
}


// Clears the designated depot.
void Vassal::clearDepot()
{
	depot = NULL;
}


// Designates the depot in the region.
void Vassal::setDepot(BWAPI::Unit * depot)
{
	// Verify depot.
	if (utilUnit::isOwned(depot) &&
		depot->exists() &&
		depot->getType().isResourceDepot())
		this->depot = depot;
}


// Adds a worker to the taskmaster.
void Vassal::addWorker(BWAPI::Unit * worker)
{
	taskmaster.addWorker(worker);
}


// Removes a worker from the taskmaster.
void Vassal::removeWorker(BWAPI::Unit * worker)
{
	taskmaster.removeWorker(worker);
}


// Employs the worker in the taskmaster.
void Vassal::employWorker(BWAPI::Unit * worker, Task task)
{
	taskmaster.employWorker(worker, task);
}


// Employs workers in the taskmaster.
void Vassal::employWorkers(utilUnit::UnitSet workers, Task task)
{
	BOOST_FOREACH(BWAPI::Unit * worker, workers)
		taskmaster.employWorker(worker, task);
}


// Returns the amount of workers.
unsigned int Vassal::workforce()
{
	return taskmaster.workforce();
}


// Returns the harvesting area.
//TODO Make dynamic, i.e. disregard exhausted mineral fields. 
//TODO Remove code duplication.
utilMap::Zone Vassal::getHarvestingZone()
{
	// Verify depot.
	if (depot &&
		depot->exists())
	{
		// Initialize zone.
		BWAPI::TilePosition depotPos = depot->getTilePosition();
		BWAPI::UnitType depotType = depot->getType();
		utilMap::Zone zone (
			depotPos.x(),
			depotPos.y(),
			depotPos.x() + depotType.tileWidth(),
			depotPos.y() + depotType.tileHeight());

		// Include resources.
		BOOST_FOREACH(BWTA::BaseLocation * baseLocation, region->getBaseLocations())
		{
			// Include minerals.
			BOOST_FOREACH(BWAPI::Unit * mineral, baseLocation->getStaticMinerals())
				zone.expand(mineral->getInitialTilePosition(), mineral->getInitialType());

			// Include geysers.
			BOOST_FOREACH(BWAPI::Unit * geyser, baseLocation->getGeysers())
				zone.expand(geyser->getInitialTilePosition(), geyser->getInitialType());
		}
		return zone;
	}
	else
		return utilMap::Zone();
}


// Returns the designated depot.
BWAPI::Unit * Vassal::getDepot()
{
	return depot;
}


// Returns an idle worker in the region if one exists.
// TODO Simplify.
BWAPI::Unit * Vassal::getIdleWorker()
{
	// Search through idle.
	BOOST_FOREACH(BWAPI::Unit * worker, taskmaster.getEmployed(TASK_IDLE))
	{
		// Verify worker.
		if (worker->exists() &&
			!worker->isCarryingGas() &&
			!worker->isCarryingMinerals())
			return worker;
	}

	// Search through miners.
	BOOST_FOREACH(BWAPI::Unit * miner, taskmaster.getEmployed(TASK_MINE))
	{
		// Verify miner.
		if (miner->exists() &&
			!miner->isCarryingGas() &&
			!miner->isCarryingMinerals())
			return miner;
	}

	// Search through harvesters.
	BOOST_FOREACH(BWAPI::Unit * harvester, taskmaster.getEmployed(TASK_HARVEST))
	{
		// Verify harvester.
		if (harvester->exists() &&
			!harvester->isCarryingGas() &&
			!harvester->isCarryingMinerals())
			return harvester;
	}

	// No idle worker found.
	return NULL;
}


// Returns a copy of pointers of workers employed with the specified task in the region.
utilUnit::UnitSet Vassal::getEmployed(Task task)
{
	return taskmaster.getEmployed(task);
}


// Returns the region the Vassal is situated in.
BWTA::Region * Vassal::getRegion()
{
	return region;
}