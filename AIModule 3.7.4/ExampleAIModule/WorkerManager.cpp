#include "WorkerManager.h"


// Constructor
WorkerManager::WorkerManager() :
	//minerQouta(0),
	depot(NULL),
	//idle(),
	//miners()
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
		/*
		// Add worker to relevant container.
		if (miners.size() < minerQouta)
			miners.insert(worker);
		else
			idle.insert(worker);
		*/
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
	/*
	// Erase worker from relevant containers.
	if (idle.count(worker) > 0)
		idle.erase(worker);
	else
		miners.erase(worker);
		*/
}


// Designates the current depot for returning cargo
void WorkerManager::setDepot(BWAPI::Unit * depot)
{
	if (depot &&
		depot->exists())
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


/*
// Sets the amount of required miners.
void WorkerManager::setMinerQouta(unsigned int qouta)
{
	minerQouta = qouta;
}
*/

// Updates mining operations.
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
	/*
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
	*/
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
	//return idle.size() + miners.size();
}


// Returns an idle worker.
BWAPI::Unit * WorkerManager::getIdle()
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

/*
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
*/

utilUnit::UnitSet WorkerManager::getEmployed(Task task)
{
	return employed[task];
}

/*
// Returns a copy of the miners.
utilUnit::UnitSet WorkerManager::getMiners()
{
	return miners;
}
*/