#include "Reconnoiter.h"

Reconnoiter::Reconnoiter(WorkerManager * workerManager)
{
	this->workerManager = workerManager;
}

// Unused deconstructor
Reconnoiter::~Reconnoiter()
{
}

// Simulate the reconnoiter AI.
// Retrieves, returns and commands the scout.
void Reconnoiter::update()
{
	// Retrieving Target
	auto points = Broodwar->getStartLocations();
	auto it = points.begin();
	while ((!target || Broodwar->isExplored(target)) && it != points.end())
	{
		BWAPI::TilePosition tile = *it;
		if (!Broodwar->isExplored(tile))
			target = tile;
		++it;
	}
	// Scouting
	if (target)
	{
		// Retrieving Scout
		if ((!scout || !scout->exists()) && workerManager->getWorkers()->size() >= MINIMUM_WORKERS)
			scout = workerManager->takeWorker();
		// Commanding Scout
		if (scout)
			scout->move(Position(target));
	}
	else
	{
		// Return Scout
		if (scout && scout->exists())
			workerManager->addWorker(scout);
		scout = nullptr;
	}
}