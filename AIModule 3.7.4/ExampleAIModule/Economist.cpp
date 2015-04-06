#include "Economist.h"


// Constructor.
Economist::Economist(WorkerManager * workerManager, Producer * producer, Architect * architect) :
	workerManager(workerManager),
	producer(producer),
	architect(architect),
	harvester(workerManager)
{
}


// Deconstructor.
Economist::~Economist()
{
}


// Fired when the map is analyzed.
void Economist::analyzed()
{
	harvester.analyzed();
}


// Simulate the architect AI. Creates pylons and commands builders.
void Economist::update()
{
	// Verify workforce.
	// TODO Make it count workers currently constructing or repairing?
	if (workerManager->workforce() < harvester.minersMax())
		producer->train(PROTOSS_WORKER);

	// Harvest resources
	harvester.update();
}