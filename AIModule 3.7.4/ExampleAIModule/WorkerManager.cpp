#include "WorkerManager.h"


// Constructor
WorkerManager::WorkerManager() :
	harvester(Harvester()),
	depot(NULL),
	idle(utilUnit::UnitSet()),
	miners(utilUnit::UnitSet())
{
}


// Deconstructor
WorkerManager::~WorkerManager()
{
}


// Fired when the map is analyzed. Calls analyzed on sub-managers.
void WorkerManager::analyzed()
{
	harvester.analyzed();
}

// Adds a worker to the pool.
void WorkerManager::addWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (worker &&
		worker->exists())
	{
		// Add worker to relevant container.
		if (miners.size() < harvester.maxMiners())
			miners.insert(worker);
		else
			idle.insert(worker);
	}
}


// Removes a worker from the pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	// Erase worker from relevant containers.
	if (idle.count(worker) > 0)
		idle.erase(worker);
	else
		miners.erase(worker);
}


// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists())
		this->depot = depot;
}


// Updates mining operations.
void WorkerManager::update()
{
	// Update miners.
	utilUnit::UnitSet::iterator it = idle.begin();
	while (it != idle.end() && miners.size() < harvester.maxMiners())
	{
		// Assign more miners.
		BWAPI::Unit * worker = *it;
		if (worker &&
			worker->exists())
		{
			it = idle.erase(it);
			miners.insert(worker);
		}
		else
			it++;
	}
	it = miners.begin();
	while (it != miners.end() && miners.size() > harvester.maxMiners())
	{
		// Unassign excess miners.
		BWAPI::Unit * miner = *it;
		if (miner &&
			miner->exists() &&
			!miner->isCarryingGas() &&
			!miner->isCarryingMinerals())
		{
			it = miners.erase(it);
			idle.insert(miner);
		}
		else
			it++;
	}

	// Command miners.
	harvester.harvest(miners);
}

// Returns whether harvesting requires more workers.
bool WorkerManager::needWorkers()
{
	return harvester.maxMiners() > idle.size() + miners.size();
}


// Returns the amount of workers.
unsigned int WorkerManager::workers()
{
	//return idle.size() + mining.size();
	return idle.size() + miners.size();
}


// Returns a valid worker or a null pointer if no valid workers are in the pool.
// TODO: Remove code repetition.
BWAPI::Unit * WorkerManager::takeWorker()
{
	// Find the worker in the idle pool.
	utilUnit::UnitSet::iterator it = idle.begin();
	while (it != idle.end())
	{
		BWAPI::Unit * worker = *it;
		if (worker &&
			worker->exists() &&
			!worker->isCarryingMinerals() &&
			!worker->isCarryingGas())
		{
			idle.erase(it);
			return worker;
		}
		else
			++it;
	}

	// Find the worker in the harvester pool.
	it = miners.begin();
	while (it != miners.end())
	{
		BWAPI::Unit * miner = *it;
		if (miner &&
			miner->exists() &&
			!miner->isCarryingGas() &&
			!miner->isCarryingMinerals())
		{
			miners.erase(it);
			return miner;
		}
		else
			it++;
	}
	// No available workers was found.
	return NULL;
}