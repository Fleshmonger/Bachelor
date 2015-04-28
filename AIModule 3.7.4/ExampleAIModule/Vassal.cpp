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