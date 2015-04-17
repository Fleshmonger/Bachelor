#include "Vassal.h"


// Constructor
Vassal::Vassal(BWAPI::Unit * depot) :
	depot(depot),
	workerManager(),
	harvester(&workerManager)
{
	// Designate resources.
	BWTA::Region * region = BWTA::getRegion(depot->getPosition());
	BOOST_FOREACH(BWAPI::Unit * mineral, BWAPI::Broodwar->getStaticMinerals())
		if (utilUnit::inRegion(mineral->getPosition(), region))
			harvester.addMineral(mineral);
}


// Destructor
Vassal::~Vassal()
{
}


// Adds a worker to the workerManager.
void Vassal::addWorker(BWAPI::Unit * worker)
{
	workerManager.addWorker(worker);
}


// Employs a worker in the workerManager.
void Vassal::employWorker(BWAPI::Unit * worker, Task task)
{
	workerManager.employWorker(worker, task);
}


// Removes a worker from the workerManager.
void Vassal::removeWorker(BWAPI::Unit * worker)
{
	workerManager.removeWorker(worker);
	harvester.removeMiner(worker);
}


// Commands idle workers to harvest.
void Vassal::harvest()
{
	harvester.harvest();
}


// Updates local managers.
void Vassal::update()
{
	workerManager.update();
}


// Returns the amount of minerals in the region.
unsigned int Vassal::mineralFields()
{
	return harvester.mineralFields();
}


// Returns the amount of workers.
unsigned int Vassal::workforce()
{
	return workerManager.workforce();
}


// Returns the designated depot.
BWAPI::Unit * Vassal::getDepot()
{
	return depot;
}


// Returns an idle worker if one exists.
BWAPI::Unit * Vassal::getIdleWorker()
{
	// Search through idle.
	BOOST_FOREACH(BWAPI::Unit * worker, workerManager.getEmployed(TASK_IDLE))
	{
		// Verify worker.
		if (worker->exists() &&
			!worker->isCarryingGas() &&
			!worker->isCarryingMinerals())
			return worker;
	}

	// No idle worker found.
	return NULL;
}