#include "Economist.h"


// Constructor.
//TODO In the future needs access to Architect for refinery construction.
Economist::Economist(WorkerManager * workerManager, Producer * producer, Architect * architect)
{
	// Managers
	this->workerManager = workerManager;
	this->producer = producer;
	this->architect = architect;
}


// Deconstructor.
Economist::~Economist()
{
}


/*
// TODO This is probably needed later on.
// Designates the current depot used for cargo return.
void Economist::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
}
*/


// Simulate the architect AI. Creates pylons and commands builders.
void Economist::update()
{
	// Order more workers.
	// TODO Make it count workers currently constructing or repairing.
	//if (workerManager->getWorkersSize() < workerManager->harvesterMax())
	//if (workerManager->workers() < workerManager->maxMiners())
	if (workerManager->needWorkers())
		producer->trainUnit(PROTOSS_WORKER);
}