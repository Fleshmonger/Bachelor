#include "WorkerManager.h"

WorkerManager::WorkerManager()
{
	workers = new BWAPI::Unitset();
}

//Unused deconstructor
WorkerManager::~WorkerManager()
{
}

// Adds a worker to the worker pool.
void WorkerManager::addWorker(BWAPI::Unit worker)
{
	workers->insert(worker);
}

// Removes a worker from the worker pool.
void WorkerManager::removeWorker(BWAPI::Unit worker)
{
	workers->erase(worker);
}

// Returns an undesignated worker. If no workers are in the pool, returns a null pointer.
BWAPI::Unit WorkerManager::takeWorker()
{
	auto it = workers->begin();
	while (it != workers->end())
	{
		if ((*it)->exists() &&
			(*it)->isCompleted())
		{
			BWAPI::Unit worker = *it;
			workers->erase(it);
			return worker;
		}

	}
	return nullptr;
}

// Returns the entire worker pool, without designating them.
BWAPI::Unitset * WorkerManager::getWorkers()
{
	return workers;
}