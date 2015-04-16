#include "WorkerManager.h"


// Constructor
WorkerManager::WorkerManager() :
	depot(NULL),
	workers(),
	assignments(),
	employed()
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
		worker->exists() &&
		worker->getType().isWorker())
	{
		workers.insert(worker);
		assignments[worker] = TASK_IDLE;
		employed[TASK_IDLE].insert(worker);
	}
}


// Removes a worker from the pool.
void WorkerManager::removeWorker(BWAPI::Unit * worker)
{
	// Verify unit.
	if (contains(worker))
	{
		// Remove unit.
		workers.erase(worker);
		Task task = assignments[worker];
		assignments.erase(worker);
		employed[task].erase(worker);
	}
}


// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists() &&
		utilUnit::isOwned(depot) &&
		depot->getType().isResourceDepot())
		this->depot = depot;
}


// Assigns the unit a duty.
void WorkerManager::employWorker(BWAPI::Unit * worker, Task task)
{
	// Verify worker.
	if (contains(worker))
	{
		// Remove worker from current task.
		Task oldTask = assignments[worker];
		employed[oldTask].erase(worker);

		// Add worker to new task.
		assignments[worker] = task;
		employed[task].insert(worker);
	}
}


// Verifies worker integrity.
void WorkerManager::update()
{
	// Verify workers.
	//TODO Is this necessary?
	utilUnit::UnitSet::iterator it = workers.begin(), end = workers.end();
	while (it != end)
	{
		// Verify worker.
		BWAPI::Unit * worker = *it;
		if (worker->exists())
			it++;
		else
		{
			it = workers.erase(it);
			end = workers.end();
		}
	}
}


// Returns whether the worker is in the worker pool.
bool WorkerManager::contains(BWAPI::Unit * worker)
{
	return workers.count(worker) > 0;
}


// Returns the amount of workers.
unsigned int WorkerManager::workforce()
{
	return workers.size();
}


// Returns an idle worker.
BWAPI::Unit * WorkerManager::getIdleWorker()
{
	// Iterate through idle.
	BOOST_FOREACH(BWAPI::Unit * worker, employed[TASK_IDLE])
	{
		// Verify unit.
		if (worker->exists() &&
			!worker->isCarryingGas() &&
			!worker->isCarryingMinerals())
			return worker;
	}
	return NULL;
}


// Returns a copy of workers employed with the specified task.
utilUnit::UnitSet WorkerManager::getEmployed(Task task)
{
	return employed[task];
}