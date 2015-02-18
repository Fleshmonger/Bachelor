#include "Reconnoiter.h"

Reconnoiter::Reconnoiter(WorkerManager * workerManager)
{
	this->workerManager = workerManager;
}

// Unused deconstructor
Reconnoiter::~Reconnoiter()
{
}

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
		{
			if (scout && !scout->exists())
				workerManager->removeWorker(scout);
			scout = workerManager->takeWorker();
		}
		// Ordering Scout
		if (scout && scout->isCompleted())
		{
			if (scout->isCarryingGas() || scout->isCarryingMinerals())
				scout->returnCargo();
			else
				scout->move(Position(target));
		}
	}
	else if (scout)
	{
		// Return Scout
		workerManager->addWorker(scout);
		scout = nullptr;
	}
}