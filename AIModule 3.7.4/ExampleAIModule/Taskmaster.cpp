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
	// Verify worker.
	if (contains(worker))
	{
		// Remove worker.
		workers.erase(worker);
		Task task = assignments[worker];
		employed[task].erase(worker);
		assignments.erase(worker);
	}
}


// Assigns the worker a task.
void Taskmaster::employWorker(BWAPI::Unit * worker, Task task)
{
	// Verify worker.
	if (contains(worker))
	{
		// Remove worker from current task.
		employed[assignments[worker]].erase(worker);

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


// Returns pointers to workers employed with the specified task.
utilUnit::UnitSet Taskmaster::getEmployed(Task task)
{
	return employed[task];
}