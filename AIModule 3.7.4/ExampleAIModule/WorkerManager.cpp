#include "WorkerManager.h"

WorkerManager::WorkerManager()
{
	workers = new std::set<Unit*>();
}

//Unused deconstructor
WorkerManager::~WorkerManager()
{
}

// Adds a worker to the worker pool.
void WorkerManager::addWorker(BWAPI::Unit * worker)
{
	workers->insert(worker);
}

// Removes a worker from the worker pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	workers->erase(worker);
}

// Returns a valid worker or a null pointer if no valid workers are in the pool.
BWAPI::Unit * WorkerManager::takeWorker()
{
	std::set<Unit*>::iterator it = workers->begin();
	while (it != workers->end())
	{
		BWAPI::Unit * worker = *it;
		if (worker->exists() && // Should be unecessary
			worker->isCompleted() && // Should be unecessary
			!worker->isCarryingMinerals() &&
			!worker->isCarryingGas())
		{
			workers->erase(it);
			return worker;
		}
		++it;
	}
	return NULL;
}

// Returns the entire worker pool.
std::set<Unit*> * WorkerManager::getWorkers()
{
	return workers;
}