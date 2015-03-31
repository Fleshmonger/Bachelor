#include "Economist.h"


// Constructor.
Economist::Economist(WorkerManager * workerManager, Producer * producer, Architect * architect, Harvester * harvester) :
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	harvester(harvester)
{
}


// Deconstructor.
Economist::~Economist()
{
}


/*
// Designates the current depot used for cargo return.
// TODO This is probably needed later on.
void Economist::setDepot(BWAPI::Unit * depot)
{
	this->depot = depot;
}
*/


// Simulate the architect AI. Creates pylons and commands builders.
void Economist::update()
{
	// Verify workforce.
	// TODO Make it count workers currently constructing or repairing?
	if (harvester->maxMiners() > workerManager->workforce())
		producer->trainUnit(PROTOSS_WORKER);
}