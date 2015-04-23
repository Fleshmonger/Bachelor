#include "Taskmaster.h"


// Constructor
Taskmaster::Taskmaster() :
	workers(),
	assignments(),
	employed()
{
}


// Deconstructor
Taskmaster::~Taskmaster()
{
}


/*
// Verifies worker integrity.
void Taskmaster::update()
{
	// Verify workers.
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
			Task task = assignments[worker];
			assignments.erase(worker);
			employed[task].erase(worker);
			end = workers.end();
		}
	}
}
*/


// Adds a worker to the pool.
void Taskmaster::addWorker(BWAPI::Unit * worker)
{
	// Verify worker.
	if (utilUnit::isOwned(worker) &&
		worker->exists() &&
		worker->getType().isWorker())
	{
		workers.insert(worker);
		assignments[worker] = TASK_IDLE;
		employed[TASK_IDLE].insert(worker);
	}
}


// Removes a worker from the pool.
void Taskmaster::removeWorker(BWAPI::Unit * worker)
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


// Assigns the worker a task.
void Taskmaster::employWorker(BWAPI::Unit * worker, Task task)
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


// Returns whether the worker is in the worker pool.
bool Taskmaster::contains(BWAPI::Unit * worker)
{
	return workers.count(worker) > 0;
}


// Returns the amount of workers.
unsigned int Taskmaster::workforce()
{
	return workers.size();
}


// Returns a copy of workers employed with the specified task.
utilUnit::UnitSet Taskmaster::getEmployed(Task task)
{
	return employed[task];
}