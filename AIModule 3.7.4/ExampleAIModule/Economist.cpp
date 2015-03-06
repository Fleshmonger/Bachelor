#include "Economist.h"

//In the future needs access to Architect for refinery construction.
Economist::Economist(WorkerManager * workerManager, Producer * producer, Architect * architect)
{
	// Managers
	this->workerManager = workerManager;
	this->producer = producer;
	this->architect = architect;
}

//Unused deconstructor.
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
	if (workerManager->workers() < workerManager->harvesterMax())
		producer->trainUnit(PROTOSS_WORKER);
}