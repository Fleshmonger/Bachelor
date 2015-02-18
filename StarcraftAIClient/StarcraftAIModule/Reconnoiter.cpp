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
	// TODO Remove the loop, make it simpler.
	if (!target || Broodwar->isExplored(target))
	{
		auto points = Broodwar->getStartLocations();
		auto it = points.begin();
		while (it != points.end() && !target)
		{
			BWAPI::TilePosition tile = *it;
			if (!Broodwar->isExplored(tile))
				target = tile; // TODO Does this ever deallocate?
		}
	}
	if (target)
	{
		// Retrieving Scout
		if ((!scout || !scout->exists()) && workerManager->getWorkers()->size() >= MINIMUM_WORKERS)
		{
			if (scout && !scout->exists())
				workerManager->removeWorker(scout);
			scout = workerManager->takeWorker();
		}
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
		workerManager->addWorker(scout);
		scout = nullptr;
	}
}