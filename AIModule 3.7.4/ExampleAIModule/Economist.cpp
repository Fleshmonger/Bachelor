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
	std::set<BWTA::Region*> regions = BWTA::getRegions();
	BWTA::Region * region = *regions.begin();
	std::set<BWTA::Region*> reach = region->getReachableRegions();
}


// Simulate the architect AI. Creates pylons and commands builders.
void Economist::update()
{
	// Verify workforce.
	if (workerManager->workforce() < harvester.minersMax())
		producer->train(PROTOSS_WORKER);

	// Harvest resources
	harvester.update();
}