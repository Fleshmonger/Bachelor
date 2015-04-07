#include "WorkerManager.h"


// Constructor
WorkerManager::WorkerManager() :
	minerQouta(0),
	depot(NULL),
	idle(),
	miners()
{
}


// Deconstructor
WorkerManager::~WorkerManager()
{
}


// Adds a worker to the pool.
void WorkerManager::addWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (worker &&
		worker->exists())
	{
		// Add worker to relevant container.
		if (miners.size() < minerQouta)
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


// Sets the amount of required miners.
void WorkerManager::setMinerQouta(unsigned int qouta)
{
	minerQouta = qouta;
}

// Updates mining operations.
void WorkerManager::update()
{
	// Update miners.
	utilUnit::UnitSet::iterator it = idle.begin();
	while (it != idle.end() && miners.size() < minerQouta)
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
	while (it != miners.end() && miners.size() > minerQouta)
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
}


// Returns the amount of workers.
unsigned int WorkerManager::workforce()
{
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


// Returns a copy of the miners.
utilUnit::UnitSet WorkerManager::getMiners()
{
	return miners;
}