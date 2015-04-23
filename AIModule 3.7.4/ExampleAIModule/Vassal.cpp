#include "Vassal.h"


// Constructor
Vassal::Vassal(BWTA::Region * region) :
	depot(),
	region(region),
	taskmaster(),
	gatherer(&taskmaster)
{
	// Designate minerals.
	//TODO Check with getMinerals instead of static.
	BOOST_FOREACH(BWTA::BaseLocation * location, region->getBaseLocations())
		BOOST_FOREACH(BWAPI::Unit * mineral, location->getStaticMinerals())
			gatherer.addMineral(mineral);
}


// Destructor
Vassal::~Vassal()
{
}


// Commands idle workers to harvest.
void Vassal::gather()
{
	gatherer.gather();
}


/*
// Updates local managers.
void Vassal::update()
{
	taskmaster.update();
}
*/


// Designates the depot in the region.
void Vassal::setDepot(BWAPI::Unit * depot)
{
	// Verify depot.
	if (utilUnit::isOwned(depot) &&
		depot->exists() &&
		depot->getType().isResourceDepot())
		this->depot = depot;
}


// Adds a refinery to the harvester.
void Vassal::addRefinery(BWAPI::Unit * refinery)
{
	BWAPI::Broodwar->sendText("Refinery added.");
	gatherer.addRefinery(refinery);
}


// Removes a refinery from the harvester.
void Vassal::removeRefinery(BWAPI::Unit * refinery)
{
	gatherer.removeRefinery(refinery);
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
	gatherer.removeWorker(worker);
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


// Returns the amount of mineral fields in the region.
unsigned int Vassal::minerals()
{
	return gatherer.getMinerals().size();
}


// Returns the amount of refineries in the region.
unsigned int Vassal::refineries()
{
	return gatherer.getRefineries().size();
}

// Returns the amount of workers.
unsigned int Vassal::workforce()
{
	return taskmaster.workforce();
}


// Returns a zone containing the harvesting area.
utilMap::Zone Vassal::getHarvestingZone()
{
	// Verify depot.
	if (depot &&
		depot->exists())
	{
		// Calculate zone.
		BWAPI::TilePosition depotPos = depot->getTilePosition();
		BWAPI::UnitType depotType = depot->getType();
		int left = depotPos.x(),
			top = depotPos.y(),
			right = depotPos.x() + depotType.tileWidth(),
			bottom = depotPos.y() + depotType.tileHeight();
		BOOST_FOREACH(BWAPI::Unit * mineral, gatherer.getMinerals())
		{
			BWAPI::TilePosition mineralPos = mineral->getTilePosition();
			BWAPI::UnitType mineralType = mineral->getType();
			left = std::min(left, mineralPos.x() + mineralType.tileWidth());
			top = std::min(top, mineralPos.y() + mineralType.tileHeight());
			right = std::max(right, mineralPos.x());
			bottom = std::max(bottom, mineralPos.y());
		}
		return utilMap::Zone(left, top, right, bottom);
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